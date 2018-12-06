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
	pSpVoice->Speak(ptr, SPF_DEFAULT, NULL);     // �ʶ����ĺ�Ӣ�ĵĻ���ַ���
}


int main(int argc, char** argv)
{
	InitVoice();
	Speek("�ϳ��ı����ȱ���С��1024�ֽڣ�");

	Speek("������ĳ��Ƚϳ������Բ��ö������ķ�ʽ���м��ı����ȳ�������");
	Speek("�����ϳ� rest api�����Ƶ����� ");

	Speek("�����ǳ�ʼ��QPSΪ100�����Ĭ��������������ҵ������");
	ExitVoice();
}