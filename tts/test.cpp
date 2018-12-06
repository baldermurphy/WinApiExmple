// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include<math.h>
#include<iostream>

#include "sapi.h"
#include "sphelper.h"
#include <iostream>
#include <atlbase.h>
#include <atlcom.h>

using namespace std;

ISpVoice *pSpVoice = NULL;
IEnumSpObjectTokens *pSpEnumTokens = NULL;
wchar_t* localText;
void InitVoice() {
	localText = new wchar_t[1024];
	::CoInitialize(NULL); 
	CLSID CLSID_SpVoice; 
	CLSIDFromProgID(_T("SAPI.SpVoice"), &CLSID_SpVoice); 
	CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)&pSpVoice); 
	SpEnumTokens(SPCAT_VOICES, L"gender = female", NULL, &pSpEnumTokens);
	ULONG ulTokensNumber = 0; 
	pSpEnumTokens->GetCount(&ulTokensNumber); 
	ISpObjectToken *pSpToken = NULL; 
	pSpEnumTokens->Item(2, &pSpToken);
	pSpVoice->SetVoice(pSpToken);

}
void ExitVoice() {
	pSpVoice->Release();
	pSpEnumTokens->Release();
	::CoUninitialize();
}
void Speek(std::string  incText) {

	mbstowcs(localText, incText.c_str(), strlen(incText.c_str())+1);
	LPWSTR ptr = localText;
	pSpVoice->Speak(ptr, SPF_DEFAULT, NULL);     // 朗读中文和英文的混合字符串
}


int main(int argc, char** argv)
{
	InitVoice();
	Speek("合成文本长度必须小于1024字节，");

	Speek("如果本文长度较长，可以采用多次请求的方式。切忌文本长度超过限制");
	Speek("语音合成 rest api不限制调用量 ");

	Speek("，但是初始的QPS为100，如果默认配额不能满足您的业务需求");
	ExitVoice();
}