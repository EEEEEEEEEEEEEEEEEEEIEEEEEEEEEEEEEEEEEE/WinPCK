//////////////////////////////////////////////////////////////////////
// ZupClassFunction.cpp: ���ڽ����������繫˾��zup�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "ZupClass.h"
#include "..\base64\base64.h"
#include <intrin.h>
#include "CharsCodeConv.h"

_inline void CZupClass::DecodeDict(LPZUP_FILENAME_DICT lpZupDict)
{
	char			szUTF8str[MAX_PATH_PCK];

	//base64����
	DWORD	dwRealLen = lpZupDict->realstrlength;
	base64_decode(lpZupDict->realbase64str, dwRealLen, szUTF8str);

	lpZupDict->wrealstrlength = U8toW(szUTF8str, lpZupDict->wrealstr, MAX_PATH_PCK, -1) - 1;
	lpZupDict->realstrlength = CPckClassCodepage::PckFilenameCode2Ansi(lpZupDict->wrealstr, lpZupDict->realstr, MAX_PATH_PCK);


}

VOID CZupClass::AddDict(char *&lpszStringToAdd)
{

	LPZUP_FILENAME_DICT lpZupDict;

	char			*lpszFilename = lpszStringToAdd;
	char			*lpszToFind;


	if('\\' == *lpszFilename || '/' == *lpszFilename)lpszFilename++;

	lpszToFind = lpszFilename;

	BOOL bContinueWhile = TRUE;

	do {

		switch(*lpszFilename) {
		case '\\':

			*lpszFilename++ = 0;
			if(NULL != (lpZupDict = m_lpDictHash->add(lpszToFind))) {
				DecodeDict(lpZupDict);
			}
			lpszToFind = lpszFilename;

			break;

		case '/':

			*lpszFilename++ = 0;
			if(NULL != (lpZupDict = m_lpDictHash->add(lpszToFind))) {
				DecodeDict(lpZupDict);
			}
			lpszToFind = lpszFilename;

			break;

		case 0x0a:

			*lpszFilename++ = 0;
			if(NULL != (lpZupDict = m_lpDictHash->add(lpszToFind))) {
				DecodeDict(lpZupDict);
			}
			lpszToFind = lpszFilename;

			bContinueWhile = FALSE;
			break;

		default:
			lpszFilename++;

			break;
		}
	} while(bContinueWhile);

	lpszStringToAdd = lpszFilename;
}

void CZupClass::DecodeFilename(char *_dst, wchar_t *_wdst, char *_src)
{
	char	__srcbuf[MAX_PATH_PCK];
	char	*__srcbufptr;

	memset(_dst + 8, 0, MAX_PATH_PCK - 8);
	//����"element\"
	//memcpy(_dst, _src, 8);

	__m128i xmm0 = _mm_setzero_si128();
	__m128i xmm1 = _mm_loadl_epi64((__m128i*)_src);
	__m128i xmm2 = _mm_unpacklo_epi8(xmm1, xmm0);
	_mm_storeu_si128((__m128i*)_wdst, xmm2);

	_dst += 8, _src += 8, _wdst += 8;

	//char	*_srcptr = _src;
	//DWORD	dwBase64Len;

	while(0 != *_src) {
		__srcbufptr = __srcbuf;

		while(*_src && '\\' != *_src && '/' != *_src) {
			*__srcbufptr++ = *_src++;
		}

		*__srcbufptr = 0;

		//��dict�в���base64
		LPZUP_FILENAME_DICT	lpZupDict;

		if(NULL != (lpZupDict = m_lpDictHash->find(__srcbuf))) {
			memcpy(_dst, lpZupDict->realstr, lpZupDict->realstrlength);
			_dst += lpZupDict->realstrlength;

			memcpy(_wdst, lpZupDict->wrealstr, lpZupDict->wrealstrlength * sizeof(wchar_t));
			_wdst += lpZupDict->wrealstrlength;

			*_dst++ = *_src;
			*_wdst++ = *_src++;
		} else {
			*_dst++ = *_src;
			*_wdst++ = *_src++;
		}
	}
}


BOOL CZupClass::BuildZupBaseDict()
{
	LPPCKINDEXTABLE	lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;

	for(unsigned long i = 0;i < m_PckAllInfo.dwFileCount;i++) {
		//��element\��ͷ�Ķ���Ҫ����
		//����ֱ�Ӹ���
		//"element\" = 0x6d656c65, 0x5c746e656d656c65
		//0x2D76 = "v-"
		if(0x2D76 == *(WORD*)lpPckIndexTable->cFileIndex.szFilename) {
			//wID = atoul(lpNode->szName + 2, NULL, 10);
			//��ȡinc�ļ�
			char	*_incbuf = (char*)malloc(lpPckIndexTable->cFileIndex.dwFileClearTextSize);

			if(NULL == _incbuf) {
				//lpNode = lpNode->next;
				continue;
			}

			if(GetSingleFileData(NULL, lpPckIndexTable, _incbuf)) {
				char	*__lpincbuf = _incbuf;
				char	*__lpincbufnextline = _incbuf;
				char	*__lpincbufEnd = _incbuf + lpPckIndexTable->cFileIndex.dwFileClearTextSize;
				//char	*lpszTemp;

				//�ֲ�
				//1��'#'://�汾
				//2��!+�ļ�
				//3��-ǩ��
				//1���汾
				while(0x0a != *__lpincbufnextline && __lpincbufnextline < __lpincbufEnd)
					__lpincbufnextline++;

				*__lpincbufnextline++ = 0;

				//�ڴ˴����봦���汾���µ���Ϣ


				//2��!+�ļ�
				__lpincbuf = __lpincbufnextline;
				while('!' == *__lpincbuf || '+' == *__lpincbuf) {
					//md5ֵ 
					//34���ֽڵ�md5��Ϣ
					//memcpy(szMD5, __lpincbuf, 33);
					//szMD5[33] = 0;
					__lpincbuf += 34;

					//�ļ�����base64
					AddDict(__lpincbuf);
				}

				//3��//ǩ��


				//while(__lpincbuf <= __lpincbufEnd)
				//{

				//	while(0x0d != *__lpincbuf && 0x0a != *__lpincbuf && __lpincbuf <= __lpincbufEnd)
				//		__lpincbuf++;


				//	while((0x0d == *__lpincbuf || 0x0a == *__lpincbuf) && __lpincbuf <= __lpincbufEnd)
				//		__lpincbuf++;


				//	switch(*__lpincbuf)
				//	{
				//	case '#'://�汾
				//		//while(32 == (*__lpincbuf++))
				//		//	;

				//		//wMinVersion = strtoul(__lpincbuf, &lpszTemp, 10);
				//		//__lpincbuf = lpszTemp;

				//		//while(32 == (*__lpincbuf++))
				//		//	;

				//		//wTargetVersion = strtoul(__lpincbuf, &lpszTemp, 10);
				//		//__lpincbuf = lpszTemp;


				//		//while(32 == (*__lpincbuf++))
				//		//	;

				//		//dwTotalSize = strtoul(__lpincbuf, &lpszTemp, 10);
				//		//__lpincbuf = lpszTemp;

				//		break;

				//	case '!':
				//	case '+':
				//		//34���ֽڵ�md5��Ϣ
				//		//memcpy(szMD5, __lpincbuf, 33);
				//		//szMD5[33] = 0;
				//		__lpincbuf += 34;

				//		//�ļ�����base64
				//		AddDict(__lpincbuf);

				//		break;

				//	case '-':
				//		//ǩ��
				//		

				//		break;

				//	default:

				//		break;
				//	}

				//}

			}

			free(_incbuf);
		}
		lpPckIndexTable++;
	}
	//if(NULL == m_lpZupDict)
	//	return FALSE;
	//else
	return TRUE;
}

const PCKINDEXTABLE* CZupClass::GetBaseFileIndex(const PCKINDEXTABLE* lpIndex, const PCKINDEXTABLE* lpZeroBaseIndex)
{
	return m_PckAllInfo.lpPckIndexTable + (lpIndex - lpZeroBaseIndex);
}