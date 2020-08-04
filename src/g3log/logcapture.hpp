/** ==========================================================================
 * 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#pragma once

#include "g3log/loglevels.hpp"
#include "g3log/crashhandler.hpp"

#include <string>
#include <sstream>
#include <cstdarg>
#include <csignal>
#ifdef _MSC_VER
# include <sal.h>
#endif

/**
 * Simple struct for capturing log/fatal entries. At destruction the captured message is
 * forwarded to background worker.
 * As a safety precaution: No memory allocated here will be moved into the background
 * worker in case of dynamic loaded library reasons
*/
struct LogCapture {
   /// Called from crash handler when a fatal signal has occurred (SIGSEGV etc)
   LogCapture(const LEVELS &level, g3::SignalType fatal_signal, const g3::TCHAR *dump = nullptr);


   /**
    * @file, line, function are given in g3log.hpp from macros
    * @level INFO/DEBUG/WARNING/FATAL
    * @expression for CHECK calls
    * @fatal_signal for failed CHECK:SIGABRT or fatal signal caught in the signal handler
    */
   LogCapture(const g3::TCHAR *file, const int line, const g3::TCHAR *function, const LEVELS &level, const g3::TCHAR *expression = G3TEXT(""), g3::SignalType fatal_signal = SIGABRT, const g3::TCHAR *dump = nullptr);


   // At destruction the message will be forwarded to the g3log worker.
   // In the case of dynamically (at runtime) loaded libraries, the important thing to know is that
   // all strings are copied, so the original are not destroyed at the receiving end, only the copy
   virtual ~LogCapture() noexcept (false);




#ifdef _MSC_VER 
#	if _MSC_VER >= 1400
#		define G3LOG_FORMAT_STRING _Printf_format_string_
#	else
#		define G3LOG_FORMAT_STRING __format_string
#	endif
   
    void capturef(G3LOG_FORMAT_STRING const g3::TCHAR *printf_like_message, ...);
#else
#	define G3LOG_FORMAT_STRING

   // Use "-Wall" to generate warnings in case of illegal printf format.
   //      Ref:  http://www.unixwiz.net/techtips/gnu-c-attributes.html
   [[gnu::format(printf, 2, 3)]] void capturef(G3LOG_FORMAT_STRING const g3::TCHAR *printf_like_message, ...); // 2,3 ref:  http://www.codemaestro.com/reviews/18
#endif

   /// prettifying API for this completely open struct
   g3::OTStringStream &stream() {
      return _stream;
   }



   g3::OTStringStream _stream;
   g3::TString _stack_trace;
   const g3::TCHAR* _file;
   const int _line;
   const g3::TCHAR* _function;
   const LEVELS &_level;
   const g3::TCHAR* _expression;
   const g3::SignalType _fatal_signal;

};
//} // g3
