#include <Windows.h>
#include "PckClassBaseFeatures.h"
#include <tchar.h>

CPckClassBaseFeatures::CPckClassBaseFeatures():
	m_PckAllInfo({ 0 }),
	m_lpPckParams(NULL),
	m_NodeMemPool(10 * 1024 * 1024)
{
	m_PckAllInfo.cRootNode.entryType = PCK_ENTRY_TYPE_NODE | PCK_ENTRY_TYPE_FOLDER | PCK_ENTRY_TYPE_ROOT;
#ifdef _DEBUG
	InitializeSRWLock(&m_LockLogFile);
#endif
}

CPckClassBaseFeatures::~CPckClassBaseFeatures()
{}


#if _USELOGFILE
#pragma warning ( disable : 4996 )
SRWLOCK	CPckClassBaseFeatures::m_LockLogFile;

int CPckClassBaseFeatures::logOutput(const char *file, const char *format, ...)
{
	char szFile[MAX_PATH];
	const int BUFFER_SIZE = 4097;
	char strbuf[BUFFER_SIZE];
	memset(strbuf, 0, sizeof(strbuf));

	const char *lpszTitle = strstr(file, "::");
	if(0 != lpszTitle) {
		sprintf_s(szFile, "d:\\pcktest\\%s.log", lpszTitle + 2);
	} else {
		sprintf_s(szFile, "d:\\pcktest\\%s.log", file);
	}

	
	FILE *pFile = fopen(szFile, "ab");
	int ret = -1;
	if(pFile != NULL) {

		va_list ap;
		va_start(ap, format);
		int result = ::vsnprintf(strbuf, BUFFER_SIZE - 1, format, ap);
		va_end(ap);

		AcquireSRWLockExclusive(&m_LockLogFile);
		fseek(pFile, 0, SEEK_END);
		int ret = fwrite(strbuf, 1, strlen(strbuf), pFile);
		fclose(pFile);
		pFile = NULL;
		ReleaseSRWLockExclusive(&m_LockLogFile);
	}
	
	return ret;
}
#endif

void CPckClassBaseFeatures::ResetPckInfos()
{
	memset(&m_PckAllInfo, 0, sizeof(PCK_ALL_INFOS));
	m_PckAllInfo.dwAddressOfFilenameIndex = PCK_DATA_START_AT;
}

CONST	LPPCKINDEXTABLE CPckClassBaseFeatures::GetPckIndexTable()
{
	return m_PckAllInfo.lpPckIndexTable;
}

CONST	LPPCK_PATH_NODE CPckClassBaseFeatures::GetPckPathNode()
{
	return &m_PckAllInfo.cRootNode;
}

BOOL CPckClassBaseFeatures::isFileLoaded()
{
	return m_PckAllInfo.isPckFileLoaded;
}

QWORD CPckClassBaseFeatures::GetPckSize()
{
	return m_PckAllInfo.qwPckSize;
}

DWORD CPckClassBaseFeatures::GetPckFileCount()
{
	return m_PckAllInfo.dwFileCount;
}

QWORD CPckClassBaseFeatures::GetPckDataAreaSize()
{
	return m_PckAllInfo.dwAddressOfFilenameIndex - PCK_DATA_START_AT;
}

QWORD CPckClassBaseFeatures::GetPckRedundancyDataSize()
{
	return m_PckAllInfo.dwAddressOfFilenameIndex - PCK_DATA_START_AT - m_PckAllInfo.cRootNode.child->qdwDirCipherTextSize;
}


#pragma region UI_Params
void CPckClassBaseFeatures::SetParams_ProgressInc()
{
	++m_lpPckParams->cVarParams.dwUIProgress;
}
void CPckClassBaseFeatures::SetParams_Progress(DWORD dwUIProgress)
{
	m_lpPckParams->cVarParams.dwUIProgress = dwUIProgress;
}

void CPckClassBaseFeatures::SetParams_ProgressUpper(DWORD dwUIProgressUpper, BOOL bReset)
{
	if(bReset)
		m_lpPckParams->cVarParams.dwUIProgress = 0;
	m_lpPckParams->cVarParams.dwUIProgressUpper = dwUIProgressUpper;
}

void CPckClassBaseFeatures::AddParams_ProgressUpper(DWORD dwUIProgressUpperAdd)
{
	m_lpPckParams->cVarParams.dwUIProgressUpper += dwUIProgressUpperAdd;
}
#pragma endregion

#pragma region Thread Params
void CPckClassBaseFeatures::SetThreadFlag(BOOL isThreadWorking)
{
	//if (FALSE == isThreadWorking)
	//在开始和结束任务后，把强制退出标记置0
	//m_lpPckParams->cVarParams.bForcedStopWorking = FALSE;
	SetErrMsgFlag(PCK_OK);
	m_lpPckParams->cVarParams.bThreadRunning = isThreadWorking;
}

//BOOL CPckClassBaseFeatures::CheckIfNeedForcedStopWorking()
//{
//	return m_lpPckParams->cVarParams.bForcedStopWorking;
//}

void CPckClassBaseFeatures::SetErrMsgFlag(int errMsg)
{
	m_lpPckParams->cVarParams.bForcedStopWorking = (PCK_OK != errMsg);
	m_lpPckParams->cVarParams.errMessageNo = errMsg;
}
#pragma endregion




