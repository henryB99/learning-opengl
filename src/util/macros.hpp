/*
 * This file is part of "alloy".
 * Copyright (C) 2019-2020 sn0w <sn0w@sn0w.sh>. All rights reserved.
 */

#pragma once

/*
 * Defines various macros for querying and modifying the compilation.
 * Based on Qt's CompilerDetection and SystemDetection.
 *
 * LANG_CPP indicates if the current compiler is C++-capable,
 * and VER_CPP can be used to retrieve the version.
 *
 * LANG_C and VER_C work similarly for C.
 *
 * The compiler (CC_X) can be one of:
 *   MSVC
 *   GNU
 *   CLANG
 *
 * The OS (OS_X) can be one or multiple of:
 *   WIN{,32,64}
 *   DARWIN{,32,64}
 *   MACOS
 *   LINUX
 *   {FREE,NET,OPEN}BSD
 *   BSD4
 *   UNIX
 *   HURD
 *
 * Other macros that get defined here are:
 *   FUNC_INFO             - Get the "pretty" name of the current function
 *   IMPORT                - Imports this symbol from a shared library
 *   EXPORT                - Exports this symbol in a shared library
 *   NORETURN              - Declare that this function never returns
 *   DEPRECATED            - Declare that this function is deprecated
 *   DEPRECATED_X(text)    - see above, but with a reason
 *   WARNING_PUSH          - Push the diagnostic stack
 *   WARNING_POP           - Pop the diagnostic stack
 *   WARNING_DISABLE(text) - Ignore a warning
 *   UNUSED                - Mark variable or parameter as deliberately unused
 *   FALLTHROUGH           - Mark a switch fallthrough as intentional
 *   NODISCARD             - Declare that the return value of a function should not be ignored (C++17)
 *   NODISCARD_X(text)     - See above, but with a message (C++20). Falls back to plain nodiscard on C++17.
 */

#ifdef __cplusplus
#define LANG_CPP
#if __cplusplus < 199711L
#define VER_CPP 0L
#elif __cplusplus == 199711L
#define VER_CPP 1977L
#elif __cplusplus == 201103L
#define VER_CPP 2011L
#elif __cplusplus == 201402L
#define VER_CPP 2014L
#elif __cplusplus == 201703L
#define VER_CPP 2017L
#elif __cplusplus == 202002L
#define VER_CPP 2020L
#endif
#endif

#if defined(_ISOC11_SOURCE)
#define VER_C 2011L
#define LANG_C
#elif defined(_ISOC99_SOURCE)
#define VER_C 1999L
#define LANG_C
#elif defined(__STRICT_ANSI__)
#define VER_C 1989L
#define LANG_C
#endif

// region os-detection
#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#include <TargetConditionals.h>  // from OSX SDK
#if defined(TARGET_OS_MAC) && TARGET_OS_MAC
#define OS_DARWIN
#ifdef __LP64__
#define OS_DARWIN64
#else
#define OS_DARWIN32
#endif
#if (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || (defined(TARGET_OS_WATCH) && TARGET_OS_WATCH) || (defined(TARGET_OS_TV) && TARGET_OS_TV)
#error iPhone/iWatch/TvOS are not supported
#else
#define OS_MACOS
#endif
#else
#error this platform is not supported
#endif
#elif !defined(SAG_COM) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#define OS_WIN32
#define OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#if defined(WINAPI_FAMILY)
#ifndef WINAPI_FAMILY_PC_APP
#define WINAPI_FAMILY_PC_APP WINAPI_FAMILY_APP
#endif
#if defined(WINAPI_FAMILY_PHONE_APP) && WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#error WindowsRT Phones are not supported
#elif WINAPI_FAMILY == WINAPI_FAMILY_PC_APP
#error WindowsRT PCs/Tablets are not supported
#else
#define OS_WIN32
#endif
#else
#define OS_WIN32
#endif
#elif defined(__EMSCRIPTEN__)
#error Web browsers are not supported
#elif defined(__linux__) || defined(__linux)
#define OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
#ifndef __FreeBSD_kernel__
#define OS_FREEBSD
#endif
#define OS_FREEBSD_KERNEL
#define OS_BSD4
#elif defined(__NetBSD__)
#define OS_NETBSD
#define OS_BSD4
#elif defined(__OpenBSD__)
#define OS_OPENBSD
#define OS_BSD4
#elif defined(__GNU__)
#define OS_HURD
#else
#error this platform is not supported
#endif
#if defined(OS_WIN32) || defined(OS_WIN64) || defined(OS_WINRT)
#define OS_WIN
#endif
#if defined(OS_FREEBSD) || defined(OS_NETBSD) || defined(OS_OPENBSD)
#define OS_BSD
#endif
#if defined(OS_WIN)
#undef OS_UNIX
#elif !defined(OS_UNIX)
#define OS_UNIX
#endif
#ifdef OS_DARWIN
#define OS_MAC
#endif
#ifdef OS_DARWIN32
#define OS_MAC32
#endif
#ifdef OS_DARWIN64
#define OS_MAC64
#endif
#ifdef OS_MACOS
#define OS_MACX
#define OS_OSX
#endif
// endregion
// region macros
#if defined(_MSC_VER)
#if defined(LANG_CPP) && VER_CPP >= 2017L
#define FALLTHROUGH [[fallthrough]]
#else
#define FALLTHROUGH  // MSCV seemingly doesn't support this before C++17
#endif
#define FUNC_INFO          __FUNCSIG__
#define IMPORT             __declspec(dllimport)
#define EXPORT             __declspec(dllexport)
#define NORETURN           __declspec(noreturn)
#define DEPRECATED         __declspec(deprecated)
#define DEPRECATED_X(text) __declspec(deprecated(text))
#define CC_MSVC
#elif defined(__GNUC__) || defined(__clang__)
#define FALLTHROUGH __attribute__((fallthrough))
#define FUNC_INFO   __FUNCTION__
#ifdef OS_WIN
#define IMPORT __declspec(dllimport)
#define EXPORT __declspec(dllexport)
#else
#define IMPORT __attribute__((visibility("default")))
#define EXPORT __attribute__((visibility("default")))
#endif
#define NORETURN           __attribute__((__noreturn__))
#define DEPRECATED         __attribute__((__deprecated__))
#define DEPRECATED_X(text) __attribute__((__deprecated__(text)))
#ifdef __clang__
#define CC_CLANG
#else
#define CC_GNU
#endif
#endif
#define UNUSED(x) ((void)(x))

#if !defined(LANG_CPP) || VER_CPP < 2017L
#define NODISCARD
#define NODISCARD_X(_)
#else
#define NODISCARD [[nodiscard]]
#if VER_CPP >= 2020L
#define NODISCARD_X(text) [[nodiscard(text)]]
#else
#define NODISCARD_X(_) [[nodiscard]]
#endif
#endif

// endregion macros
