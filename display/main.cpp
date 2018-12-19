//	Copyright (C) 2010-2012 Vaptistis Anogeianakis <nomad@cornercase.gr>
/*
 *	This file is part of HID Enumeration.
 *
 *	HID Enumeration is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	HID Enumeration is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with HID Enumeration.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "stdafx.h"
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::clog;
using std::left;

#include <iomanip>
using std::setw;
using std::setprecision;

#include <cstdlib>
using std::system;
using std::exit;
#include <Vector>

#include <windows.h>
#include <atlconv.h>

const UINT w = 25;

//const TCHAR *deviceTypeToString(DWORD type)
//{
//	switch(type)
//	{
//	case RIM_TYPEHID:
//		return "HID";
//	case RIM_TYPEKEYBOARD:
//		return "keyboard";
//	case RIM_TYPEMOUSE:
//		return "mouse";
//	default:
//		return "?";
//	} // end switch
//} // end function deviceTypeToString
static std::vector<HMONITOR> g_hMonitorGroup;
int m_iMontorNumber=0;

typedef struct _tagVMonitorInfo_t
{
	HMONITOR hMonitor; //显示器句柄
	TCHAR szDevice[32]; //显示器名
	RECT rcVirtual; //虚拟显示屏坐标
	RECT rcMonitor; //物理显示坐标
	RECT rcWork; //工作显示坐标
	BOOL bPrimary; //主显示器？

	_tagVMonitorInfo_t()
	{
		memset(this, 0, sizeof(*this));
	}
}VMONITORINFO, *LPVMONITORINFO;
VMONITORINFO m_MonitorInfoExArray[64];
static int CALLBACK MonitorEnumProc(HMONITOR hMonitor,
	HDC hdc,
	LPRECT lpRMonitor,
	LPARAM dwData) {
	g_hMonitorGroup.push_back(hMonitor);


	if (lpRMonitor)
	{
		printf(("%p (l=%d t=%d r=%d b=%d)\n"),
			hMonitor,
			lpRMonitor->left, lpRMonitor->top,
			lpRMonitor->right, lpRMonitor->bottom);
	}

	//GetMonitorInfo 获取显示器信息
	MONITORINFOEX infoEx;
	memset(&infoEx, 0, sizeof(infoEx));
	infoEx.cbSize = sizeof(infoEx);
	if (GetMonitorInfo(hMonitor, &infoEx))
	{
		USES_CONVERSION;
		printf(("%p [%s] %s rcMonitor(l=%d t=%d r=%d b=%d) rcWork(l=%d t=%d r=%d b=%d) \n \n \n \n"),
			hMonitor,
			T2A(infoEx.szDevice),
			infoEx.dwFlags == MONITORINFOF_PRIMARY ? ("Primary") : ("Slave"),
			infoEx.rcMonitor.left, infoEx.rcMonitor.top, infoEx.rcMonitor.right, infoEx.rcMonitor.bottom,
			infoEx.rcWork.left, infoEx.rcWork.top, infoEx.rcWork.right, infoEx.rcWork.bottom);
	}

	//保存显示器信息
	if (m_iMontorNumber < _countof(m_MonitorInfoExArray) - 1)
	{
		VMONITORINFO *pInfo = &m_MonitorInfoExArray[m_iMontorNumber];
		pInfo->hMonitor = hMonitor;
		if (lpRMonitor)
		{
			pInfo->rcVirtual = *lpRMonitor;
		}
		pInfo->rcMonitor = infoEx.rcMonitor;
		pInfo->rcWork = infoEx.rcWork;
		pInfo->bPrimary = infoEx.dwFlags == MONITORINFOF_PRIMARY;
		_tcscpy_s(pInfo->szDevice, infoEx.szDevice);

		m_iMontorNumber++;

		return TRUE; //continue the enumeration
	}

	return FALSE; //stop the enumeration
	return 1;
}
void printDeviceInformation(const RID_DEVICE_INFO deviceInfo, UINT indent);

int main()
{

	//第一种方式
	USES_CONVERSION;

	int i; 
	int monitorNum;
	BOOL flag;
	DISPLAY_DEVICE adapter, dd1;
	i = 0;
	flag = true;
	DEVMODE dm;
	do {
		ZeroMemory(&dm, sizeof(dm));
		dm.dmSize = sizeof(dm);
		ZeroMemory(&adapter, sizeof(adapter));
		adapter.cb = sizeof(adapter);
		ZeroMemory(&dd1, sizeof(dd1));
		dd1.cb = sizeof(dd1);
		flag = EnumDisplayDevices(NULL, i, &adapter, 0);//整个机器有n个显卡接口，dd对应的是第i个显卡的硬件接口的属性
		if (!flag) 
			break; 
		printf("adapter name : %s\n", T2A(adapter.DeviceName)); //显卡接口名称
		printf("adapter DeviceString :  %s\n", T2A(adapter.DeviceString));//显卡接口所在的显卡名称
		EnumDisplayDevices(adapter.DeviceName, 0, &dd1, 0); 
		printf("Device is %s\n", T2A(dd1.DeviceString));//显卡接口对应的显示器名称
		printf("Device key is %s\n", T2A(dd1.DeviceKey));//注册表里面的值   如 \Registry\Machine\System\CurrentControlSet\Control\Class\{4d36e96e-e325-11ce-bfc1-08002be10318}\0001
		printf("Device id is %s\n", T2A(dd1.DeviceID)); //显示器的唯一标识符 MONITOR\DELD082\{4d36e96e-e325-11ce-bfc1-08002be10318}\0001

		if (dd1.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			printf("Device is DISPLAY_DEVICE_PRIMARY_DEVICE \n");//主显示屏
		}
		if (DISPLAY_DEVICE_ATTACHED_TO_DESKTOP & dd1.StateFlags){
			printf("Device is DISPLAY_DEVICE_ATTACHED_TO_DESKTOP \n"); //桌面的拼接屏
		}
		if (DISPLAY_DEVICE_MIRRORING_DRIVER&dd1.StateFlags) {
			printf("Device is DISPLAY_DEVICE_MIRRORING_DRIVER \n"); //桌面的镜像屏
		}

		//printf("DISPLAY_DEVICE_ATTACHED_TO_DESKTOP is %d \n", adapter.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP); 
		EnumDisplaySettings(adapter.DeviceName, ENUM_CURRENT_SETTINGS, &dm);
		printf("Device width is %d,height is %d \n", dm.dmPelsWidth, dm.dmPelsHeight); 
		printf("\n\n\n\n"); 
		//printf("find\n"); 
		if (flag) i += 1; 
	} while (flag); 
	monitorNum = i; 
	// 总数量
	printf("SM_CMONITORS is %d;monitorNum is %d\n \n \n \n", GetSystemMetrics(SM_CMONITORS) ,monitorNum); 










	//第二种方式（推荐）
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, NULL);

	system("pause");
	return 0;

	DISPLAYCONFIG_DEVICE_INFO_HEADER *requestPacke;
	DisplayConfigGetDeviceInfo(requestPacke);



	MONITORINFO monitorinfo;
	monitorinfo.cbSize = sizeof(MONITORINFO);

	//获得显示器信息，将信息保存到monitorinfo中
	HMONITOR hMonitor;
	GetMonitorInfo(hMonitor, &monitorinfo);


	UINT n = 0;
	UINT infoSize;	// in bytes
	GetRegisteredRawInputDevices(NULL,&n,sizeof(RAWINPUTDEVICE));
	cout << left << "number of registered devices = " << n << endl;

	GetRawInputDeviceList(NULL,&n,sizeof(RAWINPUTDEVICELIST));
	RAWINPUTDEVICELIST *devices = new RAWINPUTDEVICELIST[n];
	n = GetRawInputDeviceList(devices,&n,sizeof(RAWINPUTDEVICELIST));	// what if a device is added/removed in the meantime?!
	cout << "number of HIDs attached to the system = " << n << '\n' << endl;

	for(UINT i = 0 ; i < n ; ++i)
	{
		cout << "device " << i << ":\n";
		cout << setw(2) << ' ' << setw(w) << "handle: HID " << devices[i].hDevice << '\n';
		if(RIM_TYPEHID== devices[i].dwType)
		cout << setw(2) << ' ' << setw(w) << "type: RIM_TYPEHID" << '\n';

		GetRawInputDeviceInfo(devices[i].hDevice,RIDI_DEVICENAME,NULL,&infoSize);
		TCHAR *deviceName = new TCHAR[infoSize];
		GetRawInputDeviceInfo(devices[i].hDevice,RIDI_DEVICENAME,deviceName,&infoSize);
		cout << setw(2) << ' ' << setw(w) << "name: " << deviceName << '\n';
		delete[] deviceName;

		//GetRawInputDeviceInfo(devices[i].hDevice,RIDI_PREPARSEDDATA,NULL,&infoSize);
		//char *parsedData = new char[infoSize];
		//GetRawInputDeviceInfo(devices[i].hDevice,RIDI_PREPARSEDDATA,parsedData,&infoSize);
		//cout << setw(2) << ' ' << setw(10) << "parsed data: " << parsedData << '\n';
		//delete[] parsedData;

		RID_DEVICE_INFO deviceInfo;
		deviceInfo.cbSize = infoSize = sizeof(deviceInfo);
		GetRawInputDeviceInfo(devices[i].hDevice,RIDI_DEVICEINFO,&deviceInfo,&infoSize);	// can it fail? require more/less space?
		printDeviceInformation(deviceInfo,2);
	} // end for
	cout << endl;

	delete[] devices;

	system("pause");
	return 0;
} // end function main

void printDeviceInformation(const RID_DEVICE_INFO deviceInfo, UINT indent)
{
	switch(deviceInfo.dwType)
	{
	case RIM_TYPEHID:
		cout << setw(2) << ' ' << setw(w) << "vendor ID: " << deviceInfo.hid.dwVendorId << '\n';
		cout << setw(2) << ' ' << setw(w) << "product ID: " << deviceInfo.hid.dwProductId << '\n';
		cout << setw(2) << ' ' << setw(w) << "version number: " << deviceInfo.hid.dwVersionNumber << '\n';
		cout << setw(2) << ' ' << setw(w) << "usage page: " << deviceInfo.hid.usUsagePage << '\n';
		cout << setw(2) << ' ' << setw(w) << "usage: " << deviceInfo.hid.usUsage << '\n';
		break;
	case RIM_TYPEKEYBOARD:
		cout << setw(2) << ' ' << setw(w) << "type: " << deviceInfo.keyboard.dwType << '\n';
		cout << setw(2) << ' ' << setw(w) << "subtype: " << deviceInfo.keyboard.dwSubType << '\n';
		cout << setw(2) << ' ' << setw(w) << "scan code mode: " << deviceInfo.keyboard.dwKeyboardMode << '\n';
		cout << setw(2) << ' ' << setw(w) << "number of function keys: " << deviceInfo.keyboard.dwNumberOfFunctionKeys << '\n';
		cout << setw(2) << ' ' << setw(w) << "number of indicators: " << deviceInfo.keyboard.dwNumberOfIndicators << '\n';
		cout << setw(2) << ' ' << setw(w) << "total number of keys: " << deviceInfo.keyboard.dwNumberOfKeysTotal << '\n';
		break;
	case RIM_TYPEMOUSE:
		cout << setw(2) << ' ' << setw(w) << "ID: " << deviceInfo.mouse.dwId << '\n';
		cout << setw(2) << ' ' << setw(w) << "number of buttons: " << deviceInfo.mouse.dwNumberOfButtons << '\n';
		cout << setw(2) << ' ' << setw(w) << "sample rate: " << deviceInfo.mouse.dwSampleRate << '\n';
		cout << setw(2) << ' ' << setw(w) << "horizontal wheel: " << deviceInfo.mouse.fHasHorizontalWheel << '\n';
		break;
	default:
		cout << setw(2) << ' ' << "Unknown Device Type!\n";
		break;
	} // end switch
} // end function printDeviceInformation
