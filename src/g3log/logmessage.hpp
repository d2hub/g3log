/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#pragma once



#include "g3log/loglevels.hpp"
#include "g3log/time.hpp"
#include "g3log/moveoncopy.hpp"
#include "g3log/crashhandler.hpp"

#include <string>
#include <sstream>
#include <thread>
#include <memory>

namespace g3 {

   /** LogMessage contains all the data collected from the LOG(...) call.
   * If the sink receives a TString it will be the TString toString()... function
   * that will format the data into a string
   *
   * For sinks that receive a LogMessage they can either use the toString() function, or use
   * the helper functions or even the public raw data to format the saved log message any
   * desired way.
   */
   struct LogMessage {
      TString file_path() const {
         return _file_path;
      }
      TString file() const {
         return _file;
      }
      TString line() const {
         return TO_STRING(_line);
      }
      TString function() const {
         return _function;
      }
      TString level() const {
         return _level.text;
      }

      /// use a different format string to get a different look on the time.
      //  default look is Y/M/D H:M:S
      TString timestamp(const TString& time_format = {internal::date_formatted + G3TEXT(" ") + internal::time_formatted}) const;

      TString message() const  {
         return _message;
      }
      TString& write() const {
         return _message;
      }

      TString expression() const  {
         return _expression;
      }
      bool wasFatal() const {
         return internal::wasFatal(_level);
      }

      TString threadID() const;

      void setExpression(const TString expression) {
         _expression = expression;
      }


      LogMessage& operator=(LogMessage other);


      LogMessage(TString file, const int line, TString function, const LEVELS level);

      explicit LogMessage(const TString& fatalOsSignalCrashMessage);
      LogMessage(const LogMessage& other);
      LogMessage(LogMessage&& other);
      virtual ~LogMessage() {}


      // helper log printing functions used by "toString()"
      static TString splitFileName(const TString& str);
      static TString fatalSignalToString(const LogMessage& msg);
      // windows only: fatalExceptionToString
      static  TString fatalExceptionToString(const LogMessage& msg);
      static TString fatalLogToString(const LogMessage& msg);
      static TString fatalCheckToString(const LogMessage& msg);
      static TString normalToString(const LogMessage& msg);     



      // the default formatting option
      static TString DefaultLogDetailsToString(const LogMessage& msg);

      // this function can be used by the logging sink to add thread ID 
      // see this concept and it is easy to make your own custom formatting 
      static TString FullLogDetailsToString(const LogMessage& msg);

      using LogDetailsFunc = TString (*) (const LogMessage&);
      TString toString(LogDetailsFunc formattingFunc = DefaultLogDetailsToString) const;


     void overrideLogDetailsFunc(LogDetailsFunc func) const;



      //
      // Complete access to the raw data in case the helper functions above
      // are not enough.
      //
      mutable LogDetailsFunc _logDetailsToStringFunc;
      g3::high_resolution_time_point _timestamp;
      std::thread::id _call_thread_id;
      TString _file;
      TString _file_path;
      int _line;
      TString _function;
      LEVELS _level;
      TString _expression; // only with content for CHECK(...) calls
      mutable TString _message;



      friend void swap(LogMessage& first, LogMessage& second) {
         using std::swap;
         swap(first._timestamp, second._timestamp);
         swap(first._call_thread_id, second._call_thread_id);
         swap(first._file, second._file);
         swap(first._line, second._line);
         swap(first._function, second._function);
         swap(first._level, second._level);
         swap(first._expression, second._expression);
         swap(first._message, second._message);
      }

   };

 


   /** Trigger for flushing the message queue and exiting the application
    * A thread that causes a FatalMessage will sleep forever until the
    * application has exited (after message flush) */
   struct FatalMessage : public LogMessage {
      FatalMessage(const LogMessage& details, g3::SignalType signal_id);
      FatalMessage(const FatalMessage&);
      virtual ~FatalMessage() {}

      LogMessage copyToLogMessage() const;
      TString reason() const;

      const SignalType _signal_id;
   };


   typedef MoveOnCopy<std::unique_ptr<FatalMessage>> FatalMessagePtr;
   typedef MoveOnCopy<std::unique_ptr<LogMessage>> LogMessagePtr;
   typedef MoveOnCopy<LogMessage> LogMessageMover;
} // g3
