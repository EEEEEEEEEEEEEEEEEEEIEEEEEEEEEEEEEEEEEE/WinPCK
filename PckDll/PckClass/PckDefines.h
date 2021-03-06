//////////////////////////////////////////////////////////////////////
// PckDefines.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2015.5.13
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <vector>
using namespace std;

typedef unsigned __int64	QWORD, *LPQWORD;


#if !defined(_PCKDEFINES_H_)
#define _PCKDEFINES_H_

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#pragma warning ( disable : 4005 )
#pragma warning ( disable : 4995 )


typedef enum _FMTPCK
{
	FMTPCK_PCK = 0,
	FMTPCK_ZUP = 1,
	FMTPCK_CUP = 2,
	FMTPCK_UNKNOWN = 0x7fffffff
}FMTPCK;

typedef enum _PCKVER
{
	PCK_V2020,
	PCK_V2030,
	PCK_VXAJH
}PCK_CATEGORY;

typedef void(*ShowLogA)(const int log_level, const char *str);
typedef void(*ShowLogW)(const int log_level, const wchar_t *str);

#define MAX_PATH			260

//zlib
#define Z_OK				0

//params
#define	MT_MAX_MEMORY				0x80000000	//2048MB
#define MAX_COMPRESS_LEVEL			12
#define Z_DEFAULT_COMPRESS_LEVEL	9

#define PCK_OK					0   /* Successful result */
/* beginning-of-error-codes */
#define PCK_MSG_USERCANCELED	1   /* 用户取消 */
#define PCK_ERROR				2   /* Generic error */
#define PCK_ERR_DISKFULL		3	/* 磁盘已满 */	//磁盘空间不足，申请空间：%d，剩余空间：%d
#define PCK_ERR_VIEWMAP_FAIL	4	/* Mapping 失败 */
#define PCK_ERR_VIEW			5	/* View 失败 */
#define PCK_ERR_OPENMAPVIEWR	6	/* OpenMappingViewAllRead 失败 */
#define PCK_ERR_MALLOC			7	/* 内存申请失败 */

/* end-of-error-codes */

//日志

#define LOG_IMAGE_NOTICE				0
#define LOG_IMAGE_INFO					1
#define LOG_IMAGE_WARNING				2
#define LOG_IMAGE_ERROR					3
#define LOG_IMAGE_DEBUG					4
#define LOG_IMAGE_EMPTY					5
#define LOG_IMAGE_COUNT					6

//数字转字符时使用的字符串长度
#define CHAR_NUM_LEN 12

#define	MAX_INDEXTABLE_CLEARTEXT_LENGTH	0x120
#define MAX_TAIL_LENGTH					300

#define	PCK_BEGINCOMPRESS_SIZE			20
#define	MAX_PATH_PCK_256				256
#define	MAX_PATH_PCK_260				260
#define PCK_ADDITIONAL_INFO_SIZE		252
#define PCK_DATA_START_AT				12

#define PCK_ENTRY_TYPE_INDEX			1
#define PCK_ENTRY_TYPE_NODE				2
#define PCK_ENTRY_TYPE_FOLDER			4
#define PCK_ENTRY_TYPE_DOTDOT			8
#define PCK_ENTRY_TYPE_ROOT				16
#define PCK_ENTRY_TYPE_TAIL_INDEX		0x80000000

#define	PCK_ADDITIONAL_KEY				"Angelica File Package"
#define	PCK_ADDITIONAL_INFO				PCK_ADDITIONAL_KEY", Perfect World Co. Ltd. 2002~2008. All Rights Reserved."

//LOG INFO STRING
#define TEXT_LOG_OPENFILE				"打开文件 %s"
#define TEXT_LOG_CLOSEFILE				"关闭文件"
#define	TEXT_LOG_FLUSH_CACHE			"写入缓存中..."
#define	TEXT_LOG_RESTOR_OK				"数据恢复成功"

#define TEXT_LOG_WORKING_DONE			"操作完成"

#define	TEXT_LOG_LEVEL_THREAD			"压缩率=%d:线程=%d"

#define	TEXT_LOG_UPDATE_ADD				"更新模式"
#define	TEXT_LOG_UPDATE_NEW				"新建模式"

#define	TEXT_LOG_RENAME					"重命名(删除)包中文件..."
#define	TEXT_LOG_REBUILD				"重建PCK文件..."
#define	TEXT_LOG_RECOMPRESS				"重压缩PCK文件..."


#define	TEXT_LOG_CREATE					"新建PCK文件:%s..."

#define	TEXT_LOG_COMPRESSOK				"压缩完成，写入索引..."

#define	TEXT_LOG_EXTRACT				"解压文件..."



//ERROR STRING
#define	TEXT_ERROR						"错误"

#define TEXT_INVALID_VERSION			"无效的版本ID！"

#define	TEXT_MALLOC_FAIL				"申请内存失败！"
#define	TEXT_CREATEMAP_FAIL				"创建文件映射失败！"
#define	TEXT_CREATEMAPNAME_FAIL			"映射文件\"%s\"失败！"
#define	TEXT_OPENNAME_FAIL				"打开文件\"%s\"失败！"
#define	TEXT_VIEWMAP_FAIL				"创建映射视图失败！"
#define	TEXT_VIEWMAPNAME_FAIL			"文件\"%s\"创建映射视图失败！"

#define TEXT_READ_INDEX_FAIL			"文件索引表读取错误！"
#define TEXT_UNKNOWN_PCK_FILE			"无法识别的PCK文件！"

#define	TEXT_OPENWRITENAME_FAIL			"打开写入文件\"%s\"失败！"
#define	TEXT_READFILE_FAIL				"文件读取失败！"
#define	TEXT_WRITEFILE_FAIL				"文件写入失败！"
#define TEXT_WRITE_PCK_INDEX			"写入文件索引出错！"

#define	TEXT_USERCANCLE					"用户取消退出！"

#define	TEXT_COMPFILE_TOOBIG			"压缩文件过大！"
#define	TEXT_UNCOMP_FAIL				"解压文件失败！"

#define	TEXT_UNCOMPRESSDATA_FAIL		"文件 %s \r\n数据解压失败！"
#define	TEXT_UNCOMPRESSDATA_FAIL_REASON	"数据解压失败: %s"

#define	TEXT_ERROR_OPEN_AFTER_UPDATE	"打开失败，可能是上次的操作导致了文件的损坏。\r\n是否尝试恢复到上次打开状态？"
#define	TEXT_ERROR_GET_RESTORE_DATA		"获取恢复信息时出错"
#define	TEXT_ERROR_RESTORING			"恢复时出错"
#define	TEXT_ERROR_DUP_FOLDER_FILE		"存在（文件名=文件夹名），退出。"

#define TEXT_UPDATE_FILE_INFO			"预添加文件数=%d:预期文件大小=%lld, 开始进行作业..."

#define TEXT_PCK_SIZE_INVALID			"无效的PCK文件大小"
#define TEXT_VERSION_NOT_FOUND			"没找到有效的版本"


#define	TEXT_EVENT_WRITE_PCK_DATA_FINISH			"WPckDataF%d"

#endif