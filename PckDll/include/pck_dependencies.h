#pragma once
#include "PckDefines.h"

//显示列表的回调函数
typedef void(*SHOW_LIST_CALLBACK)(void*, int, const wchar_t *, int, unsigned __int64, unsigned __int64, void*);


//打开、关闭、复原等事件注册
#define PCK_FILE_OPEN_SUCESS		1
#define PCK_FILE_CLOSE				2
#define PCK_FILE_NEED_RESTORE		3

#define PCK_FEEDBACK_YES			1
#define PCK_FEEDBACK_NO				2
#define PCK_FEEDBACK_IGNORE			3

typedef int(*FeedbackCallback)(void* pTag, int eventId, WPARAM wParam, LPARAM lParam);

#if 0
typedef struct _RESTORE_INFOS
{
	BOOL			isValid;
	TCHAR			szFile[MAX_PATH];
	DWORD			dwMaxSinglePckSize;
	struct 
	{
		DWORD		dwHeadCheckHead;
		DWORD		dwPckSize;
		DWORD		dwHeadCheckTail;
	}Head;
	QWORD			dwAddressOfFilenameIndex;
	size_t			sizeOfIndexTailBuffer;
	LPBYTE			lpIndexTailBuffer;
}RESTORE_INFOS, *LPRESTORE_INFOS;
#endif

class CPckControlCenter;

typedef struct _PCK_VARIETY_PARAMS {

	DWORD		dwOldFileCount;
	DWORD		dwPrepareToAddFileCount;
	DWORD		dwChangedFileCount;
	DWORD		dwDuplicateFileCount;
	DWORD		dwFinalFileCount;

	//DWORD		dwUseNewDataAreaInDuplicateFileSize;
	DWORD		dwDataAreaSize;
	DWORD		dwRedundancyDataSize;

	LPCSTR		lpszAdditionalInfo;

	DWORD		dwUIProgress;
	DWORD		dwUIProgressUpper;

	DWORD		dwMTMemoryUsed;

	BOOL		bThreadRunning;
	BOOL		bForcedStopWorking;	//强制停止
	int			errMessageNo;			//0 - ok

}PCK_VARIETY_PARAMS;

typedef struct _PCK_RUNTIME_PARAMS {

	PCK_VARIETY_PARAMS	cVarParams;

	DWORD		dwMTMaxMemory;		//最大使用内存
	DWORD		dwMTThread;			//压缩线程数
	DWORD		dwCompressLevel;	//数据压缩率

	//int			code_page;			//pck文件使用编码

	CPckControlCenter	*lpPckControlCenter;

}PCK_RUNTIME_PARAMS, *LPPCK_RUNTIME_PARAMS;


typedef struct _PCK_UNIFIED_FILEENTRY {
	int				entryType;
	wchar_t			szName[MAX_PATH_PCK_260];
}PCK_UNIFIED_FILE_ENTRY, *LPPCK_UNIFIED_FILE_ENTRY;
