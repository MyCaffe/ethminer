#include <map>
#include <boost/optional/optional_io.hpp>
#include <boost/algorithm/string.hpp>
//#include <network/uri/detail/decode.hpp>
#include "PoolURI.h"
#include "getwork/GetWorkClientEx.h"
#include "../libdevcore/Log.h"
#include <iostream>
#include <windows.h>
#include <excpt.h>

using namespace dev;

typedef struct {
	ProtocolFamily family;
	SecureLevel secure;
	unsigned version;
} SchemeAttributes;

static std::map<std::string, SchemeAttributes> s_schemes = {
	{"stratum+tcp",	  {ProtocolFamily::STRATUM, SecureLevel::NONE,  0}},
	{"stratum1+tcp",  {ProtocolFamily::STRATUM, SecureLevel::NONE,  1}},
	{"stratum2+tcp",  {ProtocolFamily::STRATUM, SecureLevel::NONE,  2}},
	{"stratum+tls",	  {ProtocolFamily::STRATUM, SecureLevel::TLS,   0}},
	{"stratum1+tls",  {ProtocolFamily::STRATUM, SecureLevel::TLS,   1}},
	{"stratum2+tls",  {ProtocolFamily::STRATUM, SecureLevel::TLS,   2}},
	{"stratum+tls12", {ProtocolFamily::STRATUM, SecureLevel::TLS12, 0}},
	{"stratum1+tls12",{ProtocolFamily::STRATUM, SecureLevel::TLS12, 1}},
	{"stratum2+tls12",{ProtocolFamily::STRATUM, SecureLevel::TLS12, 2}},
	{"stratum+ssl",	  {ProtocolFamily::STRATUM, SecureLevel::TLS12, 0}},
	{"stratum1+ssl",  {ProtocolFamily::STRATUM, SecureLevel::TLS12, 1}},
	{"stratum2+ssl",  {ProtocolFamily::STRATUM, SecureLevel::TLS12, 2}},
	{"http",		  {ProtocolFamily::GETWORK, SecureLevel::NONE,  0}}
};

std::vector<std::string> split(std::string str, std::string sep) {
	char* cstr = const_cast<char*>(str.c_str());
	char* current;
	std::vector<std::string> arr;
	current = strtok(cstr, sep.c_str());
	while (current != NULL) {
		arr.push_back(current);
		current = strtok(NULL, sep.c_str());
	}
	return arr;
}

URI::URI(const std::string uri)
{
	std::vector<std::string> arr = split(uri, ":");
	string strHost;
	string strPath;
	string strPort;
	string strUser;
	string strPw;

	cnote << "parsing url: " << uri;

	if (arr.size() == 2)
	{
		strHost = arr[0];
		strPort = arr[1];
	}

	cnote << "Host: " << strHost;
	cnote << "Path: " << strPath;
	cnote << "Port: " << strPort;

	m_host = strHost;
	m_path = strPath;
	m_user = strUser;
	m_password = strPw;
	m_port = stoi(strPort);
}

bool URI::KnownScheme()
{
	return false;
}

ProtocolFamily URI::ProtoFamily() const
{
	return ProtocolFamily::STRATUM;
}

unsigned URI::ProtoVersion() const
{
	return 0;
}

SecureLevel URI::ProtoSecureLevel() const
{
	return SecureLevel::NONE;
}

std::string URI::KnownSchemes(ProtocolFamily family)
{
	std::string schemes;
	for(const auto&s : s_schemes)
		if (s.second.family == family)
			schemes += s.first + " ";
	boost::trim(schemes);
	return schemes;
}

std::string URI::Scheme() const
{
	return "";
}

std::string URI::Host() const
{
	return m_host;
}

std::string URI::Path() const
{
	return m_path;
}

unsigned short URI::Port() const
{
	return m_port;
}

std::string URI::User() const
{
	return m_user;
}

std::string URI::Pswd() const
{
	return m_password;
}

