
// test4Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "test4.h"
#include "test4Dlg.h"
#include "afxdialogex.h"
#include "MidiDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int start_time = 0;
int end_time = 0;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Ctest4Dlg 对话框
CMidiDevice* m_midiDev;
Ctest4Dlg::Ctest4Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Ctest4Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctest4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Ctest4Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDOK, &Ctest4Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Ctest4Dlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &Ctest4Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Ctest4Dlg::OnBnClickedButton2)
END_MESSAGE_MAP()

void InitConsoleWindow()
{
	int nCrt = 0;
	FILE* fp;
	AllocConsole();
	nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	fp = _fdopen(nCrt, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}


// Ctest4Dlg 消息处理程序

BOOL Ctest4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 将“关于...”菜单项添加到系统菜单中。
	InitConsoleWindow();
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	 m_midiDev=new CMidiDevice(this);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	int i = 0;

	// enumerate all midi device
	m_midiDev->EnumMidiDev();

	// get the name of all midi device in

	for(i = 0; i < m_midiDev->GetNumOfMidiDevIn(); i++)
	{
		m_comboDevIn.AddString((LPCTSTR)(m_midiDev->m_midiDevIn[i].szPnameIn));
		printf("输入设备%d: %s\n",i,m_midiDev->m_midiDevIn[i].szPnameIn);
		//MessageBox((LPCTSTR)(m_midiDev.m_midiDevIn[i].szPnameIn),(LPCTSTR)("输入"));
	}

	// get the name of all midi device out
	for(i = 0; i < m_midiDev->GetNumOfMidiDevOut(); i++)
	{
		m_comboDevOut.AddString((LPCTSTR)(m_midiDev->m_midiDevOut[i].szPnameOut));
		printf("输出设备%d: %s\n",i,m_midiDev->m_midiDevOut[i].szPnameOut);
		//MessageBox((LPCTSTR)(m_midiDev.m_midiDevOut[i].szPnameOut),(LPCTSTR)("输出"));
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Ctest4Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Ctest4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect   rect;
		CPaintDC   dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect, RGB(255, 255, 255));   //设置为白色背景
		// TODO:  在此处为本机数据添加绘制代码
		int width = 1;
		int row = 20;
		int Line_height = 10;
		int row_height = 140;
		for (int s = 0; s <6; s++){
			CPen SOLID_Pen(PS_SOLID, 2, RGB(0, 0, 0));
			CPen DASH_Pen(PS_DASH, width,RGB(96,96,96));
			for (int i = 0; i < 4; i++){
				//画九线谱一条实线
				dc.SelectObject(&SOLID_Pen);
				dc.MoveTo(20, row + (2 * i + 0) *Line_height);
				dc.LineTo(600, row + (2 * i + 0)*Line_height);
				//画九线谱一条虚线
				dc.SelectObject(&DASH_Pen);
				dc.MoveTo(20, row + (2 * i + 1) *Line_height);
				dc.LineTo(600, row + (2 * i + 1)*Line_height);
			}

			//画九线谱第九条线
			dc.SelectObject(&SOLID_Pen);
			dc.MoveTo(20, row + 8 * Line_height);
			dc.LineTo(600, row + 8 * Line_height);

			row += row_height;

		}
		CDialogEx::OnPaint();

	}
}
void Ctest4Dlg::Draw(long msg_voice, long time){
	CClientDC   dc(this);
	CPen SOLID_Pen(PS_SOLID, 4, RGB(255, 0, 0));
	int score = (msg_voice >> 8) % 256;
	int strong = msg_voice >> 16;
	if (strong != 0){
		start_time = time-1000;
	}
	   
	else {
		end_time = time-1000;
		int big_line = start_time / 600;
		int draw_start =start_time % 600;
		int draw_end = end_time % 600;
		int small_line = 0;
		switch (score){
		case  68 : small_line = 1; break;
		case  67 : small_line = 2; break;
		case  66 : small_line = 3; break;
		case  65    : small_line = 4; break;
		case  64    : small_line = 5; break;
		case  63    : small_line = 6; break;
		case  62    : small_line = 7; break;
		case  61    : small_line = 8; break;
		case  60    : small_line = 9; break;
		default:  small_line = 1;
		}
		if (end_time < start_time){
			int line_height = 20 + big_line * 140 + (small_line - 1) * 10;
			dc.SelectObject(&SOLID_Pen);
			dc.MoveTo(20 + draw_start, line_height);
			dc.LineTo(20+600, line_height);
			line_height += big_line;
			dc.MoveTo(20 , line_height);
			dc.LineTo(20+draw_end, line_height);
		}
		else{
			int line_height = 20 + big_line * 140 + (small_line - 1) * 10;
			dc.SelectObject(&SOLID_Pen);
			dc.MoveTo(20 + draw_start, line_height);
			dc.LineTo(20 + draw_end, line_height);
		}
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Ctest4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void Ctest4Dlg::OnBnClickedOk()
{
	//Draw('3C', 100, 200);
}


void Ctest4Dlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();   
}
BOOL Ctest4Dlg::PreTranslateMessage(MSG* pMsg)
{

	//判断是否为键盘消息
	if (pMsg->message== WM_KEYDOWN)
	{
		for(int i=65;i<=90;i++)
		{
			if(pMsg->wParam==i)
			{
				int nSel = m_comboDevOut.GetCurSel();
				BOOL bOpenOut = m_midiDev->OpenDevOut(m_midiDev->m_midiDevOut[1].nDevID);

				// get the value of program, note, velocity, volume and pan

				// change the program
				m_midiDev->ProgramChange(0, 1);

				// set the master volume
				m_midiDev->SetVolume(0,125);

				// set the pan
				m_midiDev->SetPan(0, 2);

				//send note on message on channel 0
				DWORD dwEvent = 0x90 | ((DWORD)i << 8) | ((DWORD)90 << 16);
				printf("midi out sending message:0x90 %d %d\n",i,90);
				m_midiDev->SendShortMsg(dwEvent);

				return TRUE;
			}
		}
	}
	else if (pMsg->message == WM_KEYUP)
	{
		if(m_midiDev->IsMidiOutOpen())
		{
			m_midiDev->AllNotesOff(0);
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void Ctest4Dlg::OnBnClickedButton1()
{
	// open the selected midi device in
	int nSel = m_comboDevIn.GetCurSel();
	BOOL bOpenIn = m_midiDev->OpenDevIn(m_midiDev->m_midiDevIn[0].nDevID);
	if(!bOpenIn)
	{
		//MessageBox("Open Midi Device in failed");
		printf("Open Midi Device in failed\n");
	}
	else
	{
		//MessageBox("Open Midi Device successfully!");
		printf("Open Midi Device successfully!\n");
	}
	// start to record
	m_midiDev->StartRecording();
}


void Ctest4Dlg::OnBnClickedButton2()
{
	m_midiDev->StopRecording();
	printf("stop recording!!\n");
	TCHAR szFilter[] = _T("*.mid|*.mid|*.*|*.*||");    
   
	//CFileDialog fileDlg(FALSE, NULL, _T("*.mid"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);    

  
	//if (IDOK == fileDlg.DoModal())    
	//{    
		// 如果点击了文件对话框上的“保存”按钮，则将选择的文件路径显示到编辑框里    
		//m_midiDev.SaveRecEventToFile(fileDlg.GetPathName());
	m_midiDev->SaveRecEventToFile("D:\\Personal\\Desktop\\九线谱\\数字音频规范与程序设计光盘内容\\第4章\\示范MIDI文件\\abdtest1.mid");
		//SetDlgItemText(IDC_SAVE_EDIT, strFilePath);    
	//}    


}
