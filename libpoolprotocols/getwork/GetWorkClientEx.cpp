#include "GetWorkClientEx.h"
#include <Windows.h>
#include <objbase.h>
#if _DEBUG
#import "..\..\GetWorkClient\bin\Debug\GetWorkClient.tlb"
#else
#import "..\..\GetWorkClient\bin\Release\GetWorkClient.tlb"
#endif

using namespace GetWorkClient;

GetWorkClientEx::GetWorkClientEx()
{
}


GetWorkClientEx::~GetWorkClientEx()
{
	if (m_pDispatch != NULL)
	{
		m_pDispatch->Release();
		m_pDispatch = NULL;
	}

	CoUninitialize();
}

long GetWorkClientEx::Create()
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
		return hr;

	const CLSID CLSID_GetWorkClient = { 0x8657A7C3,0xE544,0x401C,{ 0x82,0x75,0x7D,0xA3,0xF7,0xBD,0x65,0xEC } };

	CLSID clsid;
	hr = CLSIDFromProgID(OLESTR("GetWorkClient.GetWorkmClient"), &clsid);
	if (FAILED(hr))
		clsid = CLSID_GetWorkClient;

	hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void **)&m_pDispatch);
	if (FAILED(hr))
		return hr;

	return 0;
}

bool GetWorkClientEx::Connect(string strUrl)
{
	IGetWorkClient* client = (IGetWorkClient*)m_pDispatch;

	_bstr_t bstrUrl(strUrl.c_str());
	VARIANT_BOOL bRes = client->Connect(bstrUrl);
	if (bRes == VARIANT_TRUE)
		return true;

	return false;
}

string GetWorkClientEx::eth_submitHashrate(string strRate, string strID)
{
	IGetWorkClient* client = (IGetWorkClient*)m_pDispatch;

	_bstr_t bstrParam1(strRate.c_str());
	_bstr_t bstrParam2(strID.c_str());
	_bstr_t bstrAccept("ACCEPTED");
	_bstr_t bstrResult = client->eth_submitHashrate(bstrParam1, bstrParam2);
	if (bstrResult == bstrAccept)
		return "OK";

	return "REJECTED";
}

bool GetWorkClientEx::ParseHost(string strUri, string* pstrHost, string* pstrPath, string* pstrPort, string* pstrUser, string* pstrPw)
{
	IGetWorkClient* client = (IGetWorkClient*)m_pDispatch;

	_bstr_t bstrUri(strUri.c_str());
	SAFEARRAY* rgVal = client->ParseHost(bstrUri);
	if (rgVal == NULL)
		return false;

	UINT uiDim = SafeArrayGetDim(rgVal);
	if (uiDim != 1)
		return false;

	LONG lBound;
	HRESULT hr = SafeArrayGetLBound(rgVal, 1, &lBound);
	if (FAILED(hr))
		return false;

	LONG uBound;
	hr = SafeArrayGetUBound(rgVal, 1, &uBound);
	if (FAILED(hr))
		return false;

	if (lBound != 0)
		return false;

	if (uBound != 4)
		return false;

	LONG lIdx = 0;
	BSTR bstrVal1;
	hr = SafeArrayGetElement(rgVal, &lIdx, &bstrVal1);
	if (FAILED(hr))
		return false;

	lIdx = 1;
	BSTR bstrVal2;
	hr = SafeArrayGetElement(rgVal, &lIdx, &bstrVal2);
	if (FAILED(hr))
		return false;

	lIdx = 2;
	BSTR bstrVal3;
	hr = SafeArrayGetElement(rgVal, &lIdx, &bstrVal3);
	if (FAILED(hr))
		return false;

	lIdx = 3;
	BSTR bstrVal4;
	hr = SafeArrayGetElement(rgVal, &lIdx, &bstrVal4);
	if (FAILED(hr))
		return false;

	lIdx = 4;
	BSTR bstrVal5;
	hr = SafeArrayGetElement(rgVal, &lIdx, &bstrVal5);
	if (FAILED(hr))
		return false;

	pstrHost->assign(_bstr_t(bstrVal1));
	pstrPath->assign(_bstr_t(bstrVal2));
	pstrPort->assign(_bstr_t(bstrVal3));
	pstrUser->assign(_bstr_t(bstrVal4));
	pstrPw->assign(_bstr_t(bstrVal5));

	return true;
}


bool GetWorkClientEx::eth_getWork(string* pstr1, string* pstr2, string* pstr3)
{
	IGetWorkClient* client = (IGetWorkClient*)m_pDispatch;

	SAFEARRAY* rgVal = client->eth_getWork();
	if (rgVal == NULL)
		return false;

	UINT uiDim = SafeArrayGetDim(rgVal);
	if (uiDim != 1)
		return false;

	LONG lBound;
	HRESULT hr = SafeArrayGetLBound(rgVal, 1, &lBound);
	if (FAILED(hr))
		return false;

	LONG uBound;
	hr = SafeArrayGetUBound(rgVal, 1, &uBound);
	if (FAILED(hr))
		return false;

	if (lBound != 0)
		return false;

	if (uBound != 2)
		return false;
		
	LONG lIdx = 0;
	BSTR bstrVal1;
	hr = SafeArrayGetElement(rgVal, &lIdx, &bstrVal1);
	if (FAILED(hr))
		return false;

	lIdx = 1;
	BSTR bstrVal2;
	hr = SafeArrayGetElement(rgVal, &lIdx, &bstrVal2);
	if (FAILED(hr))
		return false;

	lIdx = 2;
	BSTR bstrVal3;
	hr = SafeArrayGetElement(rgVal, &lIdx, &bstrVal3);
	if (FAILED(hr))
		return false;

	pstr1->assign(_bstr_t(bstrVal1));
	pstr2->assign(_bstr_t(bstrVal2));
	pstr3->assign(_bstr_t(bstrVal3));

	return true;
}

bool GetWorkClientEx::eth_submitWork(string str1, string str2, string str3)
{
	IGetWorkClient* client = (IGetWorkClient*)m_pDispatch;

	_bstr_t bstrParam1(str1.c_str());
	_bstr_t bstrParam2(str2.c_str());
	_bstr_t bstrParam3(str3.c_str());
	_bstr_t bstrAccept("ACCEPTED");
	_bstr_t bstrResult = client->eth_submitWork(bstrParam1, bstrParam2, bstrParam3);
	if (bstrResult == bstrAccept)
		return true;

	return false;
}

