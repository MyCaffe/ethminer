/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file main.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 * Ethereum client.
 */

#include <thread>
#include <fstream>
#include <iostream>
#include "../BuildInfo.h.in"
#include "MinerAux.h"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace boost::algorithm;

vector<string> g_rgInitialMessages;

void help()
{
	cout
		<< "Usage ethminer [OPTIONS]" << endl
		<< "Options:" << endl << endl;
	MinerCLI::streamHelp(cout);
	cout
		<< " General Options:" << endl
		<< "    -v,--verbosity <0 - 9>  Set the log verbosity from 0 to 9 (default: 5). Set to 9 for switch time logging." << endl
		<< "    -V,--version  Show the version and exit." << endl
		<< "    -h,--help  Show this help message and exit." << endl
		<< " Environment variables:" << endl
		<< "     NO_COLOR - set to any value to disable color output. Unset to re-enable color output." << endl
		;
	exit(0);
}

void version()
{
	cout << "ethminer version " << ETH_PROJECT_VERSION << endl;
	cout << "Build: " << ETH_BUILD_PLATFORM << "/" << ETH_BUILD_TYPE << endl;
	exit(0);
}

std::string GetLastErrorAsString(DWORD dwErr)
{
	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

bool handleError(string str, bool bAddToPreMessages)
{
	DWORD dwErr = GetLastError();

	if (dwErr == ERROR_SUCCESS)
		return false;

	string strErr = GetLastErrorAsString(dwErr);
	stringstream strmMsg;	
	strmMsg << " (" << dwErr << ") " << str << ": " << strErr;
	string strMsg = strmMsg.str();
	string strErr1 = "ERROR!";

	DWORD dwWrite;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
	WriteConsole(hStdout, strErr1.c_str(), strErr1.size(), &dwWrite, NULL);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
	WriteConsole(hStdout, strMsg.c_str(), strMsg.size(), &dwWrite, NULL);

	if (bAddToPreMessages)
		g_rgInitialMessages.push_back(strErr1 + " " + strMsg);

	return true;
}

void output(stringstream* strm, string& strName)
{
	DWORD dwWrite = 0;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	string strPipe = TEXT("\\\\.\\pipe\\");
	strPipe += strName;
	HANDLE hPipe = CreateNamedPipe(strPipe.c_str(), 
									PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
									PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 
									PIPE_UNLIMITED_INSTANCES, 
									1024 * 16, 
									0, 
									NMPWAIT_USE_DEFAULT_WAIT,
									NULL);
	OVERLAPPED ov = {};
	bool bDone = false;
	stringstream out;

	if (hPipe == NULL)
	{
		handleError("CreateNamedPipe", true);
		return;
	}

	ov.hEvent = CreateEvent(NULL, true, false, NULL);
	if (ov.hEvent == NULL)
	{
		handleError("CreateEvent for named pipe", true);
		return;
	}

	ConnectNamedPipe(hPipe, &ov);

	for (std::vector<string>::iterator it = g_rgInitialMessages.begin(); it != g_rgInitialMessages.end(); it++)
	{
		string str = *it;
		WriteConsole(hStdout, str.c_str(), str.size(), &dwWrite, NULL);

		int nWait = WaitForSingleObject(ov.hEvent, 0);
		if (nWait == WAIT_OBJECT_0)
		{
			WriteFile(hPipe, str.c_str(), str.size(), &dwWrite, NULL);
			FlushFileBuffers(hPipe);
		}
	}

	while (!bDone)
	{
		string str = strm->str();
		if (str.empty())
		{
			this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

		WriteConsole(hStdout, str.c_str(), str.size(), &dwWrite, NULL);

		int nWait = WaitForSingleObject(ov.hEvent, 0);
		if (nWait == WAIT_OBJECT_0)
		{
			WriteFile(hPipe, str.c_str(), str.size(), &dwWrite, NULL);
			FlushFileBuffers(hPipe);
		}

		strm->clear();
		strm->str(std::string());
	}

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
}

int interpretOption(int argc, char** argv, string* pstrStop, string* pstrOut, string* pstrThrottle, string* pstrFeeName)
{
	int nFound = 0;

	for (int i = 0; i < argc; i++)
	{
		string strArg = argv[i];

		if (strArg == "-$$STOP$$")
		{
			*pstrStop = argv[++i];
			nFound++;
		}
		else if (strArg == "-$$OUT$$")
		{
			*pstrOut = argv[++i];
			nFound++;
		}
		else if (strArg == "-$$THRTL$$")
		{
			*pstrThrottle = argv[++i];
			nFound++;
		}
		else if (strArg == "-$$FEENAME$$")
		{
			*pstrFeeName = argv[++i];
			nFound++;
		}
	}

	return nFound;
}

bool prework(int argc, char** argv, stringstream* pMyOut, string* pstrOut, string* pstrFeeName)
{
	string strStop;
	string strThrtl = "";
	int nFound = interpretOption(argc, argv, &strStop, pstrOut, &strThrtl, pstrFeeName);
	if (nFound != 2 && nFound != 3 && nFound != 4)
	{	
		handleError("Invalid arguments.", true);
		return false;
	}
	else
	{
		string strCancel = TEXT("Global\\");
		strCancel += strStop;
		g_hCancel = CreateEvent(NULL, TRUE, FALSE, strCancel.c_str());
		if (g_hCancel == NULL)
		{
			handleError("CreateEvent for cancel", true);
			return false;
		}

		if (strThrtl.length() > 0)
		{
			string strThrottle = TEXT("");
			strThrottle += strThrtl;
			int nSize = sizeof(int) * (MAX_GPU + 1);

			g_pnGpuThrottle = NULL;
			g_hGpuThrottle = OpenFileMapping(FILE_MAP_READ, FALSE, strThrottle.c_str());
			if (g_hGpuThrottle != NULL)
			{
				g_pnGpuThrottle = (int*)MapViewOfFile(g_hGpuThrottle, FILE_MAP_READ, 0, 0, nSize);
				if (g_pnGpuThrottle == NULL)
				{
					handleError("MapViewOfFile for gpu throttling", true);
					return false;
				}

				if ((g_pnGpuLastThrottle = (int*)malloc(sizeof(int) * MAX_GPU)) != NULL)
					memset(g_pnGpuLastThrottle, 0, sizeof(int) * MAX_GPU);

				g_rgInitialMessages.push_back("Throttling enabled.\n");
			}
			else
			{
				DWORD dwErr = GetLastError();
				string strErr = GetLastErrorAsString(dwErr);
				string str = "Throttling not enabled. " + strErr + "\n";
				g_rgInitialMessages.push_back(str);
			}
		}
	}

	return true;
}

void cleanup()
{
	if (g_hCancel != NULL)
	{
		CloseHandle(g_hCancel);
		g_hCancel = NULL;
	}

	if (g_pnGpuThrottle != NULL)
	{
		UnmapViewOfFile(g_pnGpuThrottle);
		g_pnGpuThrottle = NULL;
	}

	if (g_hGpuThrottle != NULL)
	{
		CloseHandle(g_hGpuThrottle);
		g_hGpuThrottle = NULL;
	}

	if (g_pnGpuLastThrottle != NULL)
	{
		free(g_pnGpuLastThrottle);
		g_pnGpuLastThrottle = NULL;
	}
}

int main(int argc, char** argv)
{
	stringstream myOut;
	cout.rdbuf(myOut.rdbuf());
	cerr.rdbuf(myOut.rdbuf());

	string strFeeName = "";
	string strOut;
	if (!prework(argc, argv, &myOut, &strOut, &strFeeName))
		return -1;

	thread t1(output, &myOut, strOut);

	// Set env vars controlling GPU driver behavior.
	setenv("GPU_MAX_HEAP_SIZE", "100");
	setenv("GPU_MAX_ALLOC_PERCENT", "100");
	setenv("GPU_SINGLE_ALLOC_PERCENT", "100");

	if (getenv("NO_COLOR"))
		g_useColor = false;
#if defined(_WIN32)
	if (g_useColor)
	{
		g_useColor = false;
		// Set output mode to handle virtual terminal sequences
		// Only works on Windows 10, but most users should use it anyway
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut != INVALID_HANDLE_VALUE)
		{
			DWORD dwMode = 0;
			if (GetConsoleMode(hOut, &dwMode))
			{
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				if (SetConsoleMode(hOut, dwMode))
					g_useColor = true;
			}
		}
	}
#endif

	MinerCLI m;

	try
	{
		for (int i = 1; i < argc; ++i)
		{
			// Mining options:
			if (m.interpretOption(i, argc, argv, strFeeName))
				continue;

			// Standard options:
			string arg = argv[i];
			if ((arg == "-v" || arg == "--verbosity") && i + 1 < argc)
				g_logVerbosity = atoi(argv[++i]);
			else if (arg == "-h" || arg == "--help")
				help();
			else if (arg == "-V" || arg == "--version")
				version();
			else
			{
				cerr << "Invalid argument: " << arg << endl;
				exit(-1);
			}
		}
	}
	catch (BadArgument ex)
	{
		std::cerr << "Error: " << ex.what() << "\n";
		cleanup();
		exit(-1);
	}

	try
	{
		m.execute();
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << "\n";
		cleanup();
		return 1;
	}

	cleanup();

	return 0;
}
