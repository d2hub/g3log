/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include "g3log/logmessage.hpp"
#include "g3log/crashhandler.hpp"
#include "g3log/time.hpp"
#include <mutex>




namespace g3 {

   TString LogMessage::splitFileName(const TString& str) {
      size_t found;
      found = str.find_last_of(G3TEXT("(/\\"));
      return str.substr(found + 1);
   }


   // helper for fatal signal
   TString  LogMessage::fatalSignalToString(const LogMessage& msg) {
      TString out; // clear any previous text and formatting
      out.append(msg.timestamp()
                 + G3TEXT("\n\n***** FATAL SIGNAL RECEIVED ******* \n")
                 + msg.message() + G3TEXT('\n'));
      return out;
   }


   // helper for fatal exception (windows only)
   TString  LogMessage::fatalExceptionToString(const LogMessage& msg) {
      TString out; // clear any previous text and formatting
      out.append(msg.timestamp()
                 + G3TEXT("\n\n***** FATAL EXCEPTION RECEIVED ******* \n")
                 + msg.message() + G3TEXT('\n'));
      return out;
   }


   // helper for fatal LOG
   TString LogMessage::fatalLogToString(const LogMessage& msg) {
      auto out = msg._logDetailsToStringFunc(msg);
      static const TString fatalExitReason = {G3TEXT("EXIT trigger caused by LOG(FATAL) entry: ")};
      out.append(G3TEXT("\n\t*******\t ") + fatalExitReason + G3TEXT("\n\t") + G3TEXT('"') + msg.message() + G3TEXT('"'));
      return out;
   }

   // helper for fatal CHECK
   TString LogMessage::fatalCheckToString(const LogMessage& msg) {
      auto out = msg._logDetailsToStringFunc(msg);
      static const TString contractExitReason = {G3TEXT("EXIT trigger caused by broken Contract:")};
      out.append(G3TEXT("\n\t*******\t ") + contractExitReason + G3TEXT(" CHECK(") + msg.expression() + G3TEXT(")\n\t")
                 + G3TEXT('"') + msg. message() + G3TEXT('"'));
      return out;
   }

   // helper for setting the normal log details in an entry
   TString LogMessage::DefaultLogDetailsToString(const LogMessage& msg) {
      TString out;
      out.append(msg.timestamp() + G3TEXT("\t")
                 + msg.level() 
                 + G3TEXT(" [") 
                 + msg.file() 
                 + G3TEXT("->") 
                 + msg.function() 
                 + G3TEXT(":") + msg.line() + G3TEXT("]\t"));
      return out;
   }


   TString LogMessage::FullLogDetailsToString(const LogMessage& msg) {
      TString out;
      out.append(msg.timestamp() + G3TEXT("\t")
                 + msg.level() 
                 + G3TEXT(" [") 
                 + msg.threadID() 
                 + G3TEXT(" ")
                 + msg.file() 
                 + G3TEXT("->")+ msg.function() 
                 + G3TEXT(":") + msg.line() + G3TEXT("]\t"));
      return out;
   }


   // helper for normal
   TString LogMessage::normalToString(const LogMessage& msg) {
      auto out = msg._logDetailsToStringFunc(msg);
      out.append(msg.message() + G3TEXT('\n'));
      return out;
   }



 // end static functions section

   void LogMessage::overrideLogDetailsFunc(LogDetailsFunc func) const{
      _logDetailsToStringFunc = func;
   }



   // Format the log message according to it's type
   TString LogMessage::toString(LogDetailsFunc formattingFunc) const {
      overrideLogDetailsFunc(formattingFunc);

      if (false == wasFatal()) {
         return LogMessage::normalToString(*this);
      }

      const auto level_value = _level.value;
      if (internal::FATAL_SIGNAL.value == _level.value) {
         return LogMessage::fatalSignalToString(*this);
      }

      if (internal::FATAL_EXCEPTION.value == _level.value) {
         return LogMessage::fatalExceptionToString(*this);
      }

      if (FATAL.value == _level.value) {
         return LogMessage::fatalLogToString(*this);
      }

      if (internal::CONTRACT.value == level_value) {
         return LogMessage::fatalCheckToString(*this);
      }

      // What? Did we hit a custom made level?
      auto out = _logDetailsToStringFunc(*this);
      static const TString errorUnknown = {G3TEXT("UNKNOWN or Custom made Log Message Type")};
      out.append(G3TEXT("\t*******") + errorUnknown + G3TEXT("\n\t") + message() + G3TEXT('\n'));
      return out;
   }



   TString LogMessage::timestamp(const TString& time_look) const {
      return g3::localtime_formatted(to_system_time(_timestamp), time_look);
   }


// By copy, not by reference. See this explanation for details:
// http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
   LogMessage& LogMessage::operator=(LogMessage other) {
      swap(*this, other);
      return *this;
   }


   LogMessage::LogMessage(TString file, const int line,
                          TString function, const LEVELS level)
      : _logDetailsToStringFunc(LogMessage::DefaultLogDetailsToString)
      , _timestamp(std::chrono::high_resolution_clock::now())
      , _call_thread_id(std::this_thread::get_id())
#if defined(G3_LOG_FULL_FILENAME)
      , _file(file)
#else
	   , _file(LogMessage::splitFileName(file))
#endif
      , _file_path(file)
      , _line(line)
      , _function(function)
      , _level(level) {
   }


   LogMessage::LogMessage(const TString& fatalOsSignalCrashMessage)
      : LogMessage( {G3TEXT("")}, 0, {G3TEXT("")}, internal::FATAL_SIGNAL) {
      _message.append(fatalOsSignalCrashMessage);
   }

   LogMessage::LogMessage(const LogMessage& other)
      : _logDetailsToStringFunc(other._logDetailsToStringFunc)
      , _timestamp(other._timestamp)
      , _call_thread_id(other._call_thread_id)
      , _file(other._file)
      , _file_path(other._file_path)
      , _line(other._line)
      , _function(other._function)
      , _level(other._level)
      , _expression(other._expression)
      , _message(other._message) {
   }

   LogMessage::LogMessage(LogMessage&& other)
      : _logDetailsToStringFunc(other._logDetailsToStringFunc)
      , _timestamp(other._timestamp)
      , _call_thread_id(other._call_thread_id)
      , _file(std::move(other._file))
      , _file_path(std::move(other._file_path))
      , _line(other._line)
      , _function(std::move(other._function))
      , _level(other._level)
      , _expression(std::move(other._expression))
      , _message(std::move(other._message)) {
   }



   TString LogMessage::threadID() const {
      g3::OTStringStream oss;
      oss << _call_thread_id;
      return oss.str();
   }



   FatalMessage::FatalMessage(const LogMessage& details, g3::SignalType signal_id)
      : LogMessage(details), _signal_id(signal_id) { }



   FatalMessage::FatalMessage(const FatalMessage& other)
      : LogMessage(other), _signal_id(other._signal_id) {}


   LogMessage  FatalMessage::copyToLogMessage() const {
      return LogMessage(*this);
   }

   TString FatalMessage::reason() const {
      return internal::exitReasonName(_level, _signal_id);
   }


} // g3
