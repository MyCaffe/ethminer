#pragma once

#include <string>

#define STRATUM_PROTOCOL_STRATUM		 0
#define STRATUM_PROTOCOL_ETHPROXY		 1
#define STRATUM_PROTOCOL_ETHEREUMSTRATUM 2

using namespace std;

typedef struct {
	string host;
	string port;
	string user;
	string pass;
} cred_t;
