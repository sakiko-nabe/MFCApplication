
// MFCApplicationDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication.h"
#include "MFCApplicationDlg.h"
#include "TaskScheduler.h"
#include "TaskFactory.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplicationDlg 对话框



CMFCApplicationDlg::CMFCApplicationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION_DIALOG, pParent)
	, m_resMatrix(_T(""))
	, m_resHttp(_T(""))
	, m_resStats(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_RES_MATRIX, m_resMatrix);
	DDX_Text(pDX, IDC_STATIC_RES_HTTP, m_resHttp);
	DDX_Text(pDX, IDC_STATIC_RES_STATS, m_resStats);
}

BEGIN_MESSAGE_MAP(CMFCApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CMFCApplicationDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_MATRIX, &CMFCApplicationDlg::OnBnClickedBtnMatrix)
	ON_BN_CLICKED(IDC_BTN_REMIND, &CMFCApplicationDlg::OnBnClickedBtnRemind)
	ON_BN_CLICKED(IDC_BTN_STATS, &CMFCApplicationDlg::OnBnClickedBtnStats)
	ON_BN_CLICKED(IDC_BTN_HTTP, &CMFCApplicationDlg::OnBnClickedBtnHttp)
	ON_MESSAGE(WM_UPDATE_LOG, &CMFCApplicationDlg::OnUpdateLog)
	ON_BN_CLICKED(IDC_BTN_BACKUP, &CMFCApplicationDlg::OnBnClickedBtnBackup)
END_MESSAGE_MAP()


// CMFCApplicationDlg 消息处理程序

BOOL CMFCApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    TODO:
	TaskScheduler::GetInstance()->AttachObserver(this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CMFCApplicationDlg::OnLogUpdate(const std::string& message)
{
	// 注意：这里是在后台线程被调用的，不能直接更新 UI 控件！
	// 必须把消息复制一份，发送给主线程处理

	// 动态分配字符串，传给主线程（记得在主线程释放）
	std::string* pMsg = new std::string(message);
	PostMessage(WM_UPDATE_LOG, 0, (LPARAM)pMsg);
}
void CMFCApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplicationDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CMFCApplicationDlg::OnBnClickedBtnStart()
{
	// 获取单例并启动
	TaskScheduler::GetInstance()->Start();
	AfxMessageBox(_T("调度器已启动！(Scheduler Started)"));
}

void CMFCApplicationDlg::OnBnClickedBtnMatrix()
{
	// 使用工厂创建任务
	auto task = TaskFactory::CreateTask("Matrix");
	if (task) {
		// 添加到调度器：延迟 0ms 执行，非周期
		TaskScheduler::GetInstance()->AddTask(task, 0,true,5000);
		
	}
}

void CMFCApplicationDlg::OnBnClickedBtnRemind()
{
	auto task = TaskFactory::CreateTask("Reminder");
	if (task) {
		// 添加到调度器：延迟 1000ms 执行，周期性 true，间隔 5000ms (5秒)
		// 这样每隔 5 秒就会弹出一个窗口
		TaskScheduler::GetInstance()->AddTask(task, 1000, true, 2000);
		
	}
}
LRESULT CMFCApplicationDlg::OnUpdateLog(WPARAM wParam, LPARAM lParam)
{
	std::string* pMsg = (std::string*)lParam;
	if (!pMsg) return 0;
	std::string msg = *pMsg;

	// 1. 常规日志显示 (保留)
	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_LOG);
	if (pList) {
		CString strMsg = CA2T(msg.c_str());
		int nIndex = pList->AddString(strMsg);
		pList->SetCurSel(nIndex);
	}

	// 2. ★★★ 结果可视化逻辑 (解析数据并上屏) ★★★
	CString strFullMsg = CA2T(msg.c_str());

	// 检查是不是矩阵数据
	if (msg.find("[DATA-MATRIX]") != std::string::npos) {
		// 去掉前缀，只显示后面的内容
		m_resMatrix = strFullMsg.Mid(13); // 13 是 "[DATA-MATRIX] " 的长度
		UpdateData(FALSE); // 刷新界面
	}
	// 检查是不是HTTP数据
	else if (msg.find("[DATA-HTTP]") != std::string::npos) {
		m_resHttp = strFullMsg.Mid(11);
		UpdateData(FALSE);
	}
	// 检查是不是统计数据
	else if (msg.find("[DATA-STATS]") != std::string::npos) {
		m_resStats = strFullMsg.Mid(12);
		UpdateData(FALSE);
	}

	delete pMsg;
	return 0;
}

void CMFCApplicationDlg::OnBnClickedBtnStats()
{
	// 使用工厂创建 Stats 任务
	auto task = TaskFactory::CreateTask("Stats");

	if (task) {
		// 参数: task对象, 延迟10000ms (10秒后执行)
		TaskScheduler::GetInstance()->AddTask(task, 0);

		// 弹窗提示用户，避免用户以为程序没反应
		AfxMessageBox(_T("统计任务已添加"));
	}
}
void CMFCApplicationDlg::OnBnClickedBtnHttp()
{
	// 使用工厂创建 Http 任务
	auto task = TaskFactory::CreateTask("Http");

	// 如果创建成功
	if (task) {
		// 调用单例调度器添加任务
		// 参数: task对象, 延迟0ms (立即执行)
		TaskScheduler::GetInstance()->AddTask(task, 0);
		AfxMessageBox(_T("已添加！"));
	}
}



void CMFCApplicationDlg::OnBnClickedBtnBackup()
{
	// 1. 创建备份任务
	auto task = TaskFactory::CreateTask("Backup");

	if (task) {
		// 课件要求: "一次性延迟" (One-time delayed)
		// 我们设置延迟 3秒 (3000ms) 执行，体现 "Delayed" 的特性
		TaskScheduler::GetInstance()->AddTask(task, 0);

		AfxMessageBox(_T("备份任务已添加在E:\\请查看。\n请确保 C:\\Data 文件夹存在！"));
	}
}
