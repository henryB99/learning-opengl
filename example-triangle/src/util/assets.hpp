/*
 * This file is part of "alloy".
 * Copyright (C) 2019-2020 sn0w <sn0w@sn0w.sh>. All rights reserved.
 */

#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <util/types.hpp>

#ifdef OS_DARWIN
#  define ASSET_SECTION ".const_data"
#else
#  define ASSET_SECTION ".section .rodata"
#endif

#define ASSET_DECL(name) \
    extern unsigned char const _ ## name ## _data[] __asm__("_" #name "_data"); \
    extern unsigned char const *const _ ## name ## _end __asm__("_" #name "_end"); \
    extern u32 const _ ## name ## _size __asm__("_" #name "_size"); \
    \
    void const* name(); \
    i32 name ## _size(); \
    void* name ## _copy(); \

#define ASSET_OBJ(name, path) \
    __asm__( \
        ASSET_SECTION "\n"\
        \
        ".global " "_" #name "_data\n" \
        ".balign 8\n" \
        "_" #name "_data:\n" \
        ".incbin " "\"" path "\"\n" \
        \
        ".global _" #name "_size\n" \
        ".balign 8\n" \
        "_" #name "_size:\n" \
        ".int _" #name "_size - _" #name "_data\n" \
        ".balign 8\n" \
        \
        ".text\n" \
    ); \
    \
    void const* name() { \
        return (void const*)_ ## name ## _data; \
    } \
    i32 name ## _size() { \
        return _ ## name ## _size; \
    } \
    \
    void* name ## _copy() { \
        void* memory = malloc(name ## _size()); \
        assert(memory); \
        \
        memcpy(memory, name(), name ## _size()); \
        return memory; \
    }
