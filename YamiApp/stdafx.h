//#pragma once

#define NOMINMAX
#define _USE_MATH_DEFINES

#if defined(YAMI_WINDOWS) //(defined (_WIN32) || defined (_WIN64)) // for windows builds __cdecl
#   ifdef YAMI_DLL
#		define YAMI_API __declspec(dllexport)
#		define YAMI_EXTERN
#		define YAMI_CALL 
#   else
#		define YAMI_API __declspec(dllimport)
#		define YAMI_EXTERN extern 
#		define YAMI_CALL 
#	endif
//	Set the proper SDK version before including boost/Asio
#   include <SDKDDKVer.h>
#	define WIN32_LEAN_AND_MEAN
#	define WIN64_LEAN_AND_MEAN
#	include <WinSock2.h>
#   include <Windows.h>
#	include <BaseTsd.h>
#	include <eh.h>
#	define JNIEXPORT __declspec(dllexport)
#	define JNIIMPORT __declspec(dllimport)
#	define JNICALL __stdcall
#elif defined(YAMI_LINUX) //_GCC               // for linux builds
#	ifdef YAMI_DLL //__attribute__(visibility("default")) // __attribute__((cdecl))
#		define YAMI_API 
#		define YAMI_EXTERN
#		define YAMI_CALL 
#   else
#		define YAMI_API 
#		define YAMI_EXTERN extern 
#		define YAMI_CALL 
#	endif
#	define _MAX_PATH 260
#	define INT_PTR __int64_t
#	define HANDLE void*
#	define INVALID_HANDLE_VALUE 0
#	define HINSTANCE void*
#	define ULONGLONG unsigned long long
#	define JNIEXPORT __attribute__(visibility("default"))
#	define JNIIMPORT 
#	define JNICALL __attribute__((stdcall))
// #include <termios.h> // POSIX terminal control definitions (struct termios)
// #include <asm/termios.h> // Terminal control definitions (struct termios)
#	include <asm/ioctls.h>
#	include <asm/termbits.h>
#	include <dirent.h>
#	include <stdlib.h>
#	include <unistd.h>
#elif defined(YAMI_MACOS) //_GCC               // for linux builds
#	ifdef YAMI_DLL //__attribute__(visibility("default")) // __attribute__((cdecl))
#		define YAMI_API 
#		define YAMI_EXTERN
#		define YAMI_CALL 
#   else
#		define YAMI_API 
#		define YAMI_EXTERN extern 
#		define YAMI_CALL 
#	endif
#	define _MAX_PATH 260
#	define INT_PTR __int64_t
#	define HANDLE void*
#	define INVALID_HANDLE_VALUE 0
#	define HINSTANCE void*
#	define ULONGLONG unsigned long long
#	define JNIEXPORT __attribute__(visibility("default"))
#	define JNIIMPORT 
#	define JNICALL __attribute__((stdcall))
#include <termios.h> // POSIX terminal control definitions (struct termios)
// #include <asm/termios.h> // Terminal control definitions (struct termios)
//#	include <asm/ioctls.h>
//#	include <asm/termbits.h>
#	include <dirent.h>
#	include <stdlib.h>
#	include <unistd.h>
#elif defined(YAMI_ANDROID)
#	ifdef YAMI_DLL //__attribute__(visibility("default")) // __attribute__((cdecl))
#		define YAMI_API 
#		define YAMI_EXTERN
#		define YAMI_CALL 
#   else
#		define YAMI_API 
#		define YAMI_EXTERN extern 
#		define YAMI_CALL 
#	endif
#	define _MAX_PATH 260
#	define INT_PTR __int64_t
#	define HANDLE void*
#	define INVALID_HANDLE_VALUE 0
#	define HINSTANCE void*
#	define ULONGLONG unsigned long long
#	define JNIEXPORT __attribute__(visibility("default"))
#	define JNIIMPORT 
#	define JNICALL __attribute__((stdcall))
#else 
#	ifdef YAMI_DLL //__attribute__(visibility("default")) // __attribute__((cdecl))
#		define YAMI_API 
#		define YAMI_EXTERN
#		define YAMI_CALL 
#   else
#		define YAMI_API 
#		define YAMI_EXTERN extern 
#		define YAMI_CALL 
#	endif
#	define _MAX_PATH 260
#	define INT_PTR __int64_t
#	define HANDLE void*
#	define INVALID_HANDLE_VALUE 0
#	define HINSTANCE void*
#	define ULONGLONG unsigned long long
#	define JNIEXPORT __attribute__(visibility("default"))
#	define JNIIMPORT 
#	define JNICALL __attribute__((stdcall))
#endif

#ifdef YAMI_DEBUG
#	define DEBUG_TRY try
#	define DEBUG_CATCH(v) catch(v)
#else
#	define DEBUG_TRY 
#	define DEBUG_CATCH(v) if(false)
#endif

#include <string>
#include <memory>
#include <functional>
#include <iostream>
#include <ctime>
#include <time.h>
#include <thread>
#include <stdio.h>
#include <mutex>
#include <exception>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include <limits>
#include <condition_variable>
#include <list>
#include <cstring>
#include <ostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <math.h>