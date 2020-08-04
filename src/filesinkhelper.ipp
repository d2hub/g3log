/** ==========================================================================
 * 2013 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/

#pragma once


#include <memory>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace g3 {
   namespace internal {
      static const TString file_name_time_formatted = G3TEXT("%Y%m%d-%H%M%S");

      // check for filename validity -  filename should not be part of PATH
      bool isValidFilename(const TString &prefix_filename) {
         TString illegal_characters(G3TEXT("/,|<>:#$%{}[]\'\"^!?+* "));
         size_t pos = prefix_filename.find_first_of(illegal_characters, 0);
         if (pos != TString::npos) {
            tcerr << G3TEXT("Illegal character [") << prefix_filename.at(pos) << G3TEXT("] in logname prefix: ") << G3TEXT("[") << prefix_filename << G3TEXT("]") << std::endl;
            return false;
         } else if (prefix_filename.empty()) {
            tcerr << G3TEXT("Empty filename prefix is not allowed") << std::endl;
            return false;
         }

         return true;
      }

      TString prefixSanityFix(TString prefix) {
         prefix.erase(std::remove_if(prefix.begin(), prefix.end(), ::isspace), prefix.end());
         prefix.erase(std::remove(prefix.begin(), prefix.end(), '/'), prefix.end());
         prefix.erase(std::remove(prefix.begin(), prefix.end(), '\\'), prefix.end());
         prefix.erase(std::remove(prefix.begin(), prefix.end(), '.'), prefix.end());
         prefix.erase(std::remove(prefix.begin(), prefix.end(), ':'), prefix.end());
         if (!isValidFilename(prefix)) {
            return
            {
            };
         }
         return prefix;
      }

      TString pathSanityFix(TString path, TString file_name) {
         // Unify the delimeters,. maybe sketchy solution but it seems to work
         // on at least win7 + ubuntu. All bets are off for older windows
         std::replace(path.begin(), path.end(), G3TEXT('\\'), G3TEXT('/'));

         // clean up in case of multiples
         auto contains_end = [&](TString & in) -> bool {
            size_t size = in.size();
            if (!size) return false;
            g3::TCHAR end = in[size - 1];
            return (end == G3TEXT('/') || end == G3TEXT(' '));
         };

         while (contains_end(path)) {
            path.erase(path.size() - 1);
         }

         if (!path.empty()) {
            path.insert(path.end(), G3TEXT('/'));
         }

         path.insert(path.size(), file_name);
         return path;
      }

      TString header(const TString& headerFormat) {
         OTStringStream ss_entry;
         //  Day Month Date Time Year: is written as "%a %b %d %H:%M:%S %Y" and formatted output as : Wed Sep 19 08:28:16 2012
         auto now = std::chrono::system_clock::now();
         ss_entry << G3TEXT("\t\tg3log created log at: ") << g3::localtime_formatted(now, G3TEXT("%a %b %d %H:%M:%S %Y")) << G3TEXT("\n");
         ss_entry << headerFormat;         
         return ss_entry.str();
      }

      TString createLogFileName(const TString &verified_prefix, const TString &logger_id) {
         OTStringStream oss_name;
         oss_name << verified_prefix << G3TEXT(".");
         if( logger_id != G3TEXT("") ) {
            oss_name << logger_id << G3TEXT(".");
         }
         auto now = std::chrono::system_clock::now();
         oss_name << g3::localtime_formatted(now, file_name_time_formatted);
         oss_name << G3TEXT(".log");
         return oss_name.str();
      }

      bool openLogFile(const TString &complete_file_with_path, OTFStream &outstream) {
         std::ios_base::openmode mode = std::ios_base::out; // for clarity: it's really overkill since it's an ofstream
         mode |= std::ios_base::trunc;
         outstream.open(complete_file_with_path, mode);
         if (!outstream.is_open()) {
            OTStringStream ss_error;
            ss_error << G3TEXT("FILE ERROR:  could not open log file:[") << complete_file_with_path << G3TEXT("]");
            ss_error << G3TEXT("\n\t\t std::ios_base state = ") << outstream.rdstate();
            tcerr << ss_error.str().c_str() << std::endl;
            outstream.close();
            return false;
         }
         return true;
      }

      std::unique_ptr<OTFStream> createLogFile(const TString &file_with_full_path) {
         std::unique_ptr<OTFStream> out(new OTFStream);
         OTFStream &stream(*(out.get()));
         bool success_with_open_file = openLogFile(file_with_full_path, stream);
         if (false == success_with_open_file) {
            out.release();
         }
         return out;
      }


   }
}
