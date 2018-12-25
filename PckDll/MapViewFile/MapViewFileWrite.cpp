//////////////////////////////////////////////////////////////////////
// MapViewFileWrite.cpp: ����ӳ���ļ���ͼ��д��
// 
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2014.4.24
//////////////////////////////////////////////////////////////////////

#include "MapViewFile.h"


CMapViewFileWrite::CMapViewFileWrite()
{}

CMapViewFileWrite::~CMapViewFileWrite()
{}

BOOL CMapViewFileWrite::Open(LPCSTR lpszFilename, DWORD dwCreationDisposition)
{
	return CMapViewFile::Open(lpszFilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL);
}

BOOL CMapViewFileWrite::Open(LPCWSTR lpszFilename, DWORD dwCreationDisposition)
{
	return CMapViewFile::Open(lpszFilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL);
}

BOOL CMapViewFileWrite::Mapping(QWORD qwMaxSize)
{

	UNQWORD uqwMaxSize;
	uqwMaxSize.qwValue = qwMaxSize;

	if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, uqwMaxSize.dwValueHigh, uqwMaxSize.dwValue, GenerateMapName()))) {
#ifdef _DEBUG
		if(0 != qwMaxSize)
			assert(FALSE);
#endif
		return FALSE;
	}
	return TRUE;
}

LPBYTE CMapViewFileWrite::View(QWORD qdwAddress, DWORD dwSize)
{
	return ViewReal(qdwAddress, dwSize, FILE_MAP_WRITE);
}


LPBYTE CMapViewFileWrite::ReView(LPVOID lpMapAddressOld, QWORD dwAddress, DWORD dwSize)
{
	UnmapView(lpMapAddressOld);
	return View(dwAddress, dwSize);
}


BOOL CMapViewFileWrite::OpenMappingWrite(LPCSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap)
{
	if(!Open(lpFileName, dwCreationDisposition))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileWrite::OpenMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap)
{
	if(!Open(lpFileName, dwCreationDisposition))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileWrite::SetEndOfFile()
{
	return ::SetEndOfFile(hFile);
}

DWORD CMapViewFileWrite::Write(LPVOID buffer, DWORD dwBytesToWrite)
{
	DWORD	dwFileBytesWrote = 0;

	if(!WriteFile(hFile, buffer, dwBytesToWrite, &dwFileBytesWrote, NULL)) {
		return 0;
	}

	return dwFileBytesWrote;
}

BOOL CMapViewFileWrite::FlushFileBuffers()
{
	return ::FlushFileBuffers(hFile);
}

////ʹ��MapViewOfFile����д����
//BOOL CMapViewFileWrite::Write2(QWORD dwAddress, LPVOID buffer, DWORD dwBytesToWrite)
//{
//	return 1;
//}
