#pragma once
/** ==========================================================================
 * 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================
 * Filename:tstring.hpp  Framework for Logging and Design By Contract
 * Created: 2020 by sumof2primes
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */

#include "g3log/generated_definitions.hpp"

#include <string>

namespace g3 {
	typedef char ANSICHAR;
	typedef wchar_t WIDECHAR;
#if defined(G3_USING_WIDECHAR)
	typedef WIDECHAR TCHAR;
#else
	typedef ANSICHAR TCHAR;
#endif
}


#if defined(G3_USING_WIDECHAR)

#if ((defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__GNUC__))
#define _vsntprintf_s   _vsnwprintf_s
#define DBGHELP_TRANSLATE_TCHAR
#endif

#define __G3TEXT(x) L##x
#define TO_STRING(x) std::to_wstring(x)

#define tcout std::wcout
#define tcerr std::wcerr

#else

#if ((defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__GNUC__))
#define _vsntprintf_s   _vsnprintf_s
#endif

#define __G3TEXT(x) x
#define TO_STRING(x) std::to_string(x)
#define _vsntprintf_s   _vsnprintf_s
#define tcout std::cout
#define tcerr std::cerr

#endif

#define G3TEXT(x) __G3TEXT(x)

namespace g3 {
	using AString = std::basic_string<ANSICHAR>;
	using WString = std::basic_string<WIDECHAR>;
	using TString = std::basic_string<g3::TCHAR>;
	using OTStringStream = std::basic_ostringstream<g3::TCHAR, std::char_traits<g3::TCHAR>, std::allocator<g3::TCHAR>>;
	using OTFStream = std::basic_ofstream<g3::TCHAR>;
}
