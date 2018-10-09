#pragma once

#include "GetWorkClientEx.h"
#include <iostream>
#include "../../libdevcore/Worker.h"
#include "../PoolClient.h"

using namespace std;
using namespace dev;
using namespace eth;

class EthGetworkClient : public PoolClient, Worker
{
public:
	EthGetworkClient(unsigned const & farmRecheckPeriod);
	~EthGetworkClient();

	void connect() override;
	void disconnect() override;

	bool isConnected() override { return m_connected; }
	string ActiveEndPoint() { return ""; };

	void submitHashrate(string const & rate) override;
	void submitSolution(Solution solution) override;

private:
	void workLoop() override;
	unsigned m_farmRecheckPeriod = 500;

	string m_currentHashrateToSubmit = "";
	Solution m_solutionToSubmit;
	bool m_justConnected = false;
	h256 m_client_id;
	GetWorkClientEx m_client;
	WorkPackage m_prevWorkPackage;
};
