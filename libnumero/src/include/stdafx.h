#pragma once

#if defined(_WIN32)

// Unnamed struct/union
#pragma warning( disable : 4201)
#define _WIN32_LEAN_AND_MEAN

#ifndef STRICT
#define STRICT
#endif

#include <windows.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <mmsystem.h>
#include <stddef.h>
#include <time.h>
#include <stdint.h>
#include <io.h>
//#include <algorithm>
#include <map>
#include <list>
#include <vector>
#include <string.h>
#include "resource.h"
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

#include <csetjmp>
//#include <setjmpex.h>
// #define setjmp  _setjmp
// #define longjmp longjmp
#include <setjmp.h>


typedef void* LPVOID;
//typedef const char* LPCTSTR;
typedef int errno_t;
#define MAX_PATH 256
#define _T(z) z
#define _tprintf_s printf
#define ARRAYSIZE(z) (sizeof(z)/sizeof((z)[0]))
// #ifndef max
// #define max(a,b)            (((a) > (b)) ? (a) : (b))
// #endif
// #ifndef min
// #define min(a,b)            (((a) < (b)) ? (a) : (b))
// #endif

#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define _tcsrchr strrchr
#define _tcsicmp strcasecmp
#define _strnicmp _strnicmp
#define _putts puts
#define _tcscpy_s(dest, len, src) strcpy(dest, src)
#define _tcslen strlen
#define _tcscmp strcmp
#define _tcsncmp strncmp
#define _tcsnicmp _strnicmp
#define _stscanf_s sscanf_s
#define sscanf_s sscanf
#define vsprintf_s vsprintf
#define ZeroMemory(dest, size) memset(dest, 0, size)
#define memcpy_s(dest, num, source, size) memcpy(dest, source, size)
#define StringCbCopy(dest, size, source) strcpy(dest, source)
#define StringCchCopy(dest, size, source) strcpy(dest, source)
#define StringCbPrintf(dest, size, format, args) sprintf(dest, format, args)
#define fopen_s(pFile,filename,mode) (*(pFile)) = fopen((filename),(mode))
#define tmpfile_s(pFile) (*(pFile)) = tmpfile()
#define _tfopen_s fopen_s

#define TCHAR char
#define LPCTSTR const char*

#elif defined(_MACVER)
#include <assert.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <time.h>
#include <ctype.h>
#ifdef __cplusplus
#include <map>
#endif

typedef int errno_t;

#define MAX_PATH 256
#define _T(z) z
#define _tprintf_s printf
#define ARRAYSIZE(z) (sizeof(z)/sizeof((z)[0]))
#define _strnicmp strncasecmp
#define _tcsnicmp strncasecmp
#define _tcsicmp strcasecmp
#define _putts puts
#define _tcsrchr strrchr
#define _tcscpy_s strcpy
#define _tcslen strlen
#define _tcscmp strcmp



#else

#include <float.h>
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stddef.h>
#include <time.h>
#include <ctype.h>
#ifdef __cplusplus
#include <map>
#endif

typedef void *LPVOID;
typedef const char *LPCTSTR;
typedef int errno_t;
#define MAX_PATH 256
#define _T(z) z
#define _tprintf_s printf
#define ARRAYSIZE(z) (sizeof(z)/sizeof((z)[0]))
// #ifndef max
// #define max(a,b)            (((a) > (b)) ? (a) : (b))
// #endif
// #ifndef min
// #define min(a,b)            (((a) < (b)) ? (a) : (b))
// #endif
#define _strnicmp my_strncasecmp
#define _tcsicmp my_strcasecmp
#define _putts puts
#define _tcsrchr strrchr
#define _tcscpy_s(dest, len, src) strcpy(dest, src)
#define _tcslen strlen
#define _tcscmp strcmp
#define _tcsncmp strncmp
#define _tcsnicmp _strnicmp
#define _stscanf_s sscanf_s
#define sscanf_s sscanf
#define vsprintf_s vsprintf
#define ZeroMemory(dest, size) memset(dest, 0, size)
#define memcpy_s(dest, num, source, size) memcpy(dest, source, size)
#define StringCbCopy(dest, size, source) strcpy(dest, source)
#define StringCchCopy(dest, size, source) strcpy(dest, source)
#define StringCbPrintf(dest, size, format, args...) sprintf(dest, format, args)
#define fopen_s(pFile,filename,mode) (*(pFile)) = fopen((filename),(mode))
#define tmpfile_s(pFile) (*(pFile)) = tmpfile()
#define _tfopen_s fopen_s

#endif
