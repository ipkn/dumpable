// Copyright (c) 2014 ipkn.
// Licensed under the MIT license.

#pragma once

// Use the same memory layout for both 32-bit and 64-bit architecture
// If DUMPABLE_COMPATIBLE_LAYOUT is defined, the result binary is slightly larger.
//#define DUMPABLE_COMPATIBLE_LAYOUT

#if defined(_MSC_VER) && !defined(noexcept)
#define noexcept throw()
#endif

namespace dumpable
{
#ifdef DUMPABLE_COMPATIBLE_LAYOUT
	typedef unsigned long long size_t;
	typedef long long ptrdiff_t;
#else
	typedef std::size_t size_t;
	typedef std::ptrdiff_t ptrdiff_t;
#endif
}

