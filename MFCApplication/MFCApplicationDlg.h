
// MFCApplicationDlg.h: 头文件
//

#pragma once
#include "IObserver.h"

// CMFCApplicationDlg 对话框
class CMFCApplicationDlg : public CDialogEx,public IObserver
{
// 构造
public:
	CMFCApplicationDlg(CWnd* pParent = nullptr);	// 标准构造函数
	virtual void OnLogUpdate(const std::string& message) override;
// 对话框数据
#define WM_UPDATE_LOG (WM_USER + 100)
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION_DIALOG };
#endif

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
	afx_msg LRESULT OnUpdateLog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnMatrix();
	afx_msg void OnBnClickedBtnRemind();
	afx_msg void OnBnClickedBtnStats();
	afx_msg void OnBnClickedBtnHttp();
	afx_msg void OnBnClickedBtnBackup();
	CString m_resMatrix;
	CString m_resHttp;
	CString m_resStats;
	afx_msg void OnBnClickedBtnTestBad();
	afx_msg void OnBnClickedBtnTestGood();
};
