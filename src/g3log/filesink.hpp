/** ==========================================================================
 * 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/
#pragma once

#include <string>
#include <memory>

#include "g3log/logmessage.hpp"
namespace g3 {

   class FileSink {
   public:
      FileSink(const TString &log_prefix, const TString &log_directory, const TString &logger_id=G3TEXT("g3log"));
      virtual ~FileSink();

      void fileWrite(LogMessageMover message);
      TString changeLogFile(const TString &directory, const TString &logger_id);
      TString fileName();
      void overrideLogDetails(LogMessage::LogDetailsFunc func);
      void overrideLogHeader(const TString& change);


   private:
      LogMessage::LogDetailsFunc _log_details_func;

      TString _log_file_with_path;
      TString _log_prefix_backup; // needed in case of future log file changes of directory
      std::unique_ptr<OTFStream> _outptr;
      TString _header;
      bool _firstEntry;

      void addLogFileHeader();
      OTFStream &filestream() {
         return *(_outptr.get());
      }


      FileSink &operator=(const FileSink &) = delete;
      FileSink(const FileSink &other) = delete;

   };
} // g3

