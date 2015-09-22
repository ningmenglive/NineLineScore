
// test4.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"	
#include "test4Dlg.h"
// Ctest4App:
// 有关此类的实现，请参阅 test4.cpp
//

class Ctest4App : public CWinApp
{
public:
	
	Ctest4App();
	Ctest4Dlg pDlg;

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern Ctest4App theApp;