#include "PckClassThread.h"
#include "PckModelStrip.h"

//在多线程运算中获取未压缩好的源数据
FETCHDATA_RET CPckClassThreadWorker::GetUncompressedDataFromFile(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex)
{

	while(1) {

		AcquireSRWLockExclusive(&g_mt_LockCompressedflag);
		if(lpDataFetchMethod->ciFilesList == lpDataFetchMethod->ciFilesListEnd) {
			ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);
			return FD_END;
		}
		vector<FILES_TO_COMPRESS>::const_pointer lpOneFile = &lpDataFetchMethod->ciFilesList[0];
		lpDataFetchMethod->ciFilesList++;
		ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);

#ifdef _DEBUG
		logOutput(__FUNCTION__, "lpfirstFile_id=%d\r\n", lpOneFile->id);
#endif
		LPBYTE lpCompressedBuffer = (BYTE*)MALLOCED_EMPTY_DATA;
		pckFileIndex.dwMallocSize = pThis->m_zlib.GetCompressBoundSizeByFileSize(pckFileIndex.cFileIndex.dwFileClearTextSize, pckFileIndex.cFileIndex.dwFileCipherTextSize, lpOneFile->dwFileSize);

		//构建文件名
		memcpy(mystrcpy(pckFileIndex.cFileIndex.szwFilename, lpDataFetchMethod->szCurrentNodeString), lpOneFile->szwFilename + lpOneFile->nFileTitleLen, lpOneFile->nBytesToCopy - lpDataFetchMethod->nCurrentNodeStringLen);
		//Unicode文件名转换为CP936的ANSI
		CPckClassCodepage::PckFilenameCode2Ansi(pckFileIndex.cFileIndex.szwFilename, pckFileIndex.cFileIndex.szFilename, sizeof(pckFileIndex.cFileIndex.szwFilename));

		//如果文件大小为0，则跳过打开文件步骤
		if(0 != pckFileIndex.cFileIndex.dwFileClearTextSize) {
			CMapViewFileRead		cFileRead;
			LPBYTE					lpBufferToRead;
			//文件不为0时的处理
			//打开要进行压缩的文件
			if(NULL == (lpBufferToRead = cFileRead.OpenMappingViewAllRead(lpOneFile->szwFilename))) {
				pThis->SetErrMsgFlag(PCK_ERR_OPENMAPVIEWR);
				return FD_ERR;
			}

			//判断使用的内存是否超过最大值
			FETCHDATA_RET rtn;
			if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpCompressedBuffer, pckFileIndex.dwMallocSize))) {
				return rtn;
			}

			if(PCK_BEGINCOMPRESS_SIZE < pckFileIndex.cFileIndex.dwFileClearTextSize) {
				pThis->m_zlib.compress(lpCompressedBuffer, &pckFileIndex.cFileIndex.dwFileCipherTextSize,
					lpBufferToRead, pckFileIndex.cFileIndex.dwFileClearTextSize);
			} else {
				memcpy(lpCompressedBuffer, lpBufferToRead, pckFileIndex.cFileIndex.dwFileClearTextSize);
			}

		}

		pckFileIndex.compressed_file_data = lpCompressedBuffer;
		return FD_OK;

	}
	return FD_END;
}

FETCHDATA_RET CPckClassThreadWorker::GetUncompressedDataFromPCK(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex)
{

	while(1) {

		AcquireSRWLockExclusive(&g_mt_LockCompressedflag);
		if(lpDataFetchMethod->dwProcessIndex >= lpDataFetchMethod->dwTotalIndexCount) {
			ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);
			return FD_END;
		}

		DATA_FETCH_METHOD cDataFetchMethod;
		memcpy(&cDataFetchMethod, lpDataFetchMethod, sizeof(DATA_FETCH_METHOD));
		++(lpDataFetchMethod->lpPckIndexTablePtrSrc);
		++(lpDataFetchMethod->dwProcessIndex);
		ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);

#ifdef _DEBUG
		logOutput(__FUNCTION__, "dwProcessIndex=%d\r\n", cDataFetchMethod.dwProcessIndex);
#endif

		if(cDataFetchMethod.lpPckIndexTablePtrSrc->isInvalid)
			continue;

		LPBYTE				lpBufferToRead;
		//保存重压缩数据的解压的数据
		LPBYTE				lpDecompressBuffer = NULL;
		//保存重压缩数据的源数据
		LPBYTE				lpSourceBuffer = NULL;

		LPPCKINDEXTABLE	lpPckIndexTablePtrSrc = cDataFetchMethod.lpPckIndexTablePtrSrc;
		DWORD dwNumberOfBytesToMap = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
		DWORD dwFileClearTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize;

		memcpy(&pckFileIndex.cFileIndex, &lpPckIndexTablePtrSrc->cFileIndex, sizeof(PCKFILEINDEX));

		if(PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
			pckFileIndex.cFileIndex.dwFileCipherTextSize = pThis->m_zlib.compressBound(dwFileClearTextSize);
		}
		else {
			pckFileIndex.cFileIndex.dwFileCipherTextSize = dwFileClearTextSize;
		}

		LPBYTE lpCompressedBuffer = (BYTE*)MALLOCED_EMPTY_DATA;
		pckFileIndex.dwMallocSize = pckFileIndex.cFileIndex.dwFileCipherTextSize;

		if(0 != dwFileClearTextSize) {

			//判断使用的内存是否超过最大值
			FETCHDATA_RET rtn;
			if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpCompressedBuffer, pckFileIndex.dwMallocSize))) {
				return rtn;
			}

			//文件数据需要重压缩
			if(PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
				//保存源数据的空间
				if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpSourceBuffer, dwNumberOfBytesToMap))) {
					return rtn;
				}

				if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpDecompressBuffer, dwFileClearTextSize))) {
					return rtn;
				}

				AcquireSRWLockExclusive(&g_mt_LockReadFileMap);
				if(NULL == (lpBufferToRead = cDataFetchMethod.lpFileReadPCK->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap))) {
					ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);
					pThis->freeMaxAndSubtractMemory(lpSourceBuffer, dwNumberOfBytesToMap);
					pThis->freeMaxAndSubtractMemory(lpDecompressBuffer, dwFileClearTextSize);

					return FD_ERR;
					break;
				}

				memcpy(lpSourceBuffer, lpBufferToRead, dwNumberOfBytesToMap);
				cDataFetchMethod.lpFileReadPCK->UnmapViewAll();
				ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);

				if (pThis->m_zlib.check_zlib_header(lpSourceBuffer)) {

					pThis->m_zlib.decompress(lpDecompressBuffer, &dwFileClearTextSize, lpSourceBuffer, dwNumberOfBytesToMap);
					
					if (dwFileClearTextSize == lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize) {

						/*
						在这里加入精简代码
						*/
						if (PCK_STRIP_NONE != cDataFetchMethod.iStripFlag) {
							CPckModelStrip cModelStrip;
							cModelStrip.StripContent(lpDecompressBuffer, &pckFileIndex.cFileIndex, cDataFetchMethod.iStripFlag);
						}

						pThis->m_zlib.compress(lpCompressedBuffer, &pckFileIndex.cFileIndex.dwFileCipherTextSize, lpDecompressBuffer, pckFileIndex.cFileIndex.dwFileClearTextSize);
					}
					else {
						memcpy(lpCompressedBuffer, lpSourceBuffer, dwNumberOfBytesToMap);
						pckFileIndex.cFileIndex.dwFileCipherTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
					}
				
				}
				else {
					memcpy(lpCompressedBuffer, lpSourceBuffer, dwNumberOfBytesToMap);
					pckFileIndex.cFileIndex.dwFileCipherTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
				}

				pThis->freeMaxAndSubtractMemory(lpSourceBuffer, dwNumberOfBytesToMap);
				pThis->freeMaxAndSubtractMemory(lpDecompressBuffer, dwFileClearTextSize);


			} else {
#pragma region 文件过小不需要压缩时
				AcquireSRWLockExclusive(&g_mt_LockReadFileMap);
				if(NULL == (lpBufferToRead = cDataFetchMethod.lpFileReadPCK->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap))) {
					ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);
					return FD_ERR;
					break;
				}
				memcpy(lpCompressedBuffer, lpBufferToRead, dwNumberOfBytesToMap);
				cDataFetchMethod.lpFileReadPCK->UnmapViewAll();
				ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);
#pragma endregion
			}

		}

		pckFileIndex.compressed_file_data = lpCompressedBuffer;
		return FD_OK;

	}
	return FD_END;
}
