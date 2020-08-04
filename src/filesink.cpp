/** ==========================================================================
 * 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#include "g3log/filesink.hpp"
#include "filesinkhelper.ipp"
#include <cassert>
#include <chrono>

namespace g3 {
   using namespace internal;

   FileSink::FileSink(const TString &log_prefix, const TString &log_directory, const TString& logger_id)
      : _log_details_func(&LogMessage::DefaultLogDetailsToString)
      ,_log_file_with_path(log_directory)
      , _log_prefix_backup(log_prefix)
      , _outptr(new OTFStream)
      , _header(G3TEXT("\t\tLOG format: [YYYY/MM/DD hh:mm:ss uuu* LEVEL FILE->FUNCTION:LINE] messagen\n\t\t(uuu*: microseconds fractions of the seconds value)\n\n"))
      , _firstEntry(true)
   {
      _log_prefix_backup = prefixSanityFix(log_prefix);
      if (!isValidFilename(_log_prefix_backup)) {
         tcerr << G3TEXT("g3log: forced abort due to illegal log prefix [") << log_prefix << G3TEXT("]") << std::endl;
         abort();
      }

      TString file_name = createLogFileName(_log_prefix_backup, logger_id);
      _log_file_with_path = pathSanityFix(_log_file_with_path, file_name);
      _outptr = createLogFile(_log_file_with_path);

      if (!_outptr) {
         tcerr << G3TEXT("Cannot write log file to location, attempting current directory") << std::endl;
         _log_file_with_path = G3TEXT("./") + file_name;
         _outptr = createLogFile(_log_file_with_path);
      }
      assert(_outptr && "cannot open log file at startup");
	  _outptr->imbue(std::locale(""));
   }


   FileSink::~FileSink() {
      TString exit_msg {G3TEXT("g3log g3FileSink shutdown at: ")};
      auto now = std::chrono::system_clock::now();
      exit_msg.append(localtime_formatted(now, internal::time_formatted)).append(G3TEXT("\n"));
      filestream() << exit_msg << std::flush;

      exit_msg.append(G3TEXT("Log file at: [")).append(_log_file_with_path).append(G3TEXT("]\n"));
      tcerr << exit_msg << std::flush;
   }

   // The actual log receiving function
   void FileSink::fileWrite(LogMessageMover message) {
      if (_firstEntry ) {
          addLogFileHeader();
         _firstEntry = false;
      }

      OTFStream &out(filestream());
      out << message.get().toString(_log_details_func) << std::flush;
   }

   TString FileSink::changeLogFile(const TString &directory, const TString &logger_id) {

      auto now = std::chrono::system_clock::now();
      auto now_formatted = g3::localtime_formatted(now, {internal::date_formatted + G3TEXT(" ") + internal::time_formatted});

      TString file_name = createLogFileName(_log_prefix_backup, logger_id);
      TString prospect_log = directory + file_name;
      std::unique_ptr<OTFStream> log_stream = createLogFile(prospect_log);
      if (nullptr == log_stream) {
         filestream() << G3TEXT("\n") << now_formatted << G3TEXT(" Unable to change log file. Illegal filename or busy? Unsuccessful log name was: ") << prospect_log;
         return {}; // no success
      }

      addLogFileHeader();
      OTStringStream ss_change;
      ss_change << G3TEXT("\n\tChanging log file from : ") << _log_file_with_path;
      ss_change << G3TEXT("\n\tto new location: ") << prospect_log << G3TEXT("\n");
      filestream() << now_formatted << ss_change.str();
      ss_change.str(G3TEXT(""));

      TString old_log = _log_file_with_path;
      _log_file_with_path = prospect_log;
      _outptr = std::move(log_stream);
      ss_change << G3TEXT("\n\tNew log file. The previous log file was at: ");
      ss_change << old_log << G3TEXT("\n");
      filestream() << now_formatted << ss_change.str();
      return _log_file_with_path;
   }
   TString FileSink::fileName() {
      return _log_file_with_path;
   }

   void FileSink::overrideLogDetails(LogMessage::LogDetailsFunc func) {
      _log_details_func = func;
   }

   void FileSink::overrideLogHeader(const TString& change) {
      _header = change;
   }


   void FileSink::addLogFileHeader() {
      filestream() << header(_header);
   }
} // g3
