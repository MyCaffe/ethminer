#pragma once

#include <string>

using namespace std;

struct IDispatch;

class GetWorkClientEx
{
public:
	GetWorkClientEx();
	~GetWorkClientEx();

	long Create();
	bool Connect(string strUrl);

	string eth_submitHashrate(string strHash, string strID);
	bool eth_getWork(string* pstr1, string* pstr2, string* pstr3);
	bool eth_submitWork(string str1, string str2, string str3);
	bool ParseHost(string strUri, string* pstrHost, string* pstrPath, string* pstrPort, string* pstrUser, string* pstrPw);

private:
	IDispatch* m_pDispatch;
};

