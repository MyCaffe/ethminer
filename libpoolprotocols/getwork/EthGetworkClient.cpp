#include "EthGetworkClient.h"
#include <chrono>

using namespace std;
using namespace dev;
using namespace eth;

EthGetworkClient::EthGetworkClient(unsigned const & farmRecheckPeriod) : PoolClient(), Worker("getwork")
{
	m_client.Create();

	m_farmRecheckPeriod = farmRecheckPeriod;
	m_authorized = true;
	m_connection_changed = true;
	m_solutionToSubmit.nonce = 0;
	startWorking();
}

EthGetworkClient::~EthGetworkClient()
{
}

void EthGetworkClient::connect()
{
	if (m_connection_changed) {
		m_client.Connect(m_conn.Host());
	}

//	cnote << "connect to " << m_host;

	m_client_id = h256::random();
	m_connection_changed = false;
	m_justConnected = true; // We set a fake flag, that we can check with workhandler if connection works
}

void EthGetworkClient::disconnect()
{
	m_connected = false;
	m_justConnected = false;

	// Since we do not have a real connected state with getwork, we just fake it.
	if (m_onDisconnected) {
		m_onDisconnected();
	}
}

void EthGetworkClient::submitHashrate(string const & rate)
{
	// Store the rate in temp var. Will be handled in workLoop
	m_currentHashrateToSubmit = rate;
}

void EthGetworkClient::submitSolution(Solution solution)
{
	// Store the solution in temp var. Will be handled in workLoop
	m_solutionToSubmit = solution;
}

// Handles all getwork communication.
void EthGetworkClient::workLoop()
{
	while (true)
	{
		if (m_connected || m_justConnected) {

			// Submit solution
			if (m_solutionToSubmit.nonce) {
				try
				{
					bool accepted = m_client.eth_submitWork("0x" + toHex(m_solutionToSubmit.nonce), "0x" + toString(m_solutionToSubmit.work.header), "0x" + toString(m_solutionToSubmit.mixHash));
					if (accepted) {
						if (m_onSolutionAccepted) {
							m_onSolutionAccepted(false);
						}
					}
					else {
						if (m_onSolutionRejected) {
							m_onSolutionRejected(false);
						}
					}

					m_solutionToSubmit.nonce = 0;
				}
				catch (...)
				{
					cwarn << "Failed to submit solution.";
				}
			}

			// Get Work
			try
			{
				string strHeader;
				string strSeed;
				string strTarget;

				if (m_client.eth_getWork(&strHeader, &strSeed, &strTarget))
				{
					WorkPackage newWorkPackage;
					newWorkPackage.header = h256(strHeader);
					newWorkPackage.seed = h256(strSeed);

					// Since we do not have a real connected state with getwork, we just fake it.
					// If getting work succeeds we know that the connection works
					if (m_justConnected && m_onConnected) {
						m_justConnected = false;
						m_connected = true;
						m_onConnected();
					}

					// Check if header changes so the new workpackage is really new
					if (newWorkPackage.header != m_prevWorkPackage.header) {
						m_prevWorkPackage.header = newWorkPackage.header;
						m_prevWorkPackage.seed = newWorkPackage.seed;
						m_prevWorkPackage.boundary = h256(fromHex(strTarget), h256::AlignRight);

						if (m_onWorkReceived) {
							m_onWorkReceived(m_prevWorkPackage);
						}
					}
				}
				else
				{
					cwarn << "Failed getting work!";
					disconnect();
				}
			}
			catch (...)
			{
				cwarn << "Failed getting work!";
				disconnect();
			}

			// Submit current hashrate if needed
			if (!m_currentHashrateToSubmit.empty()) {
				try
				{
					m_client.eth_submitHashrate(m_currentHashrateToSubmit, "0x" + m_client_id.hex());
				}
				catch (...)
				{
				}
				m_currentHashrateToSubmit = "";
			}
		}

		// Sleep
		this_thread::sleep_for(chrono::milliseconds(m_farmRecheckPeriod));
	}
}
