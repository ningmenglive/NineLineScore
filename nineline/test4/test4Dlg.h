
// test4Dlg.h : 头文件
//
#include "Resource.h"
#pragma once


// Ctest4Dlg 对话框
class Ctest4Dlg : public CDialogEx
{
// 构造
public:
	Ctest4Dlg(CWnd* pParent = NULL);	// 标准构造函数
	BOOL PreTranslateMessage(MSG * msg);
// 对话框数据
	void Draw(long msg_voice,long time);
	enum { IDD = IDD_TEST4_DIALOG };
	CComboBox	m_comboDevOut;
	CComboBox	m_comboDevIn;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
		
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
