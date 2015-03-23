#ifndef RECORDER_HPP_
#define RECORDER_HPP_


// #ifdef __cplusplus
// extern "C" {
// #endif
#include <stdio.h>
#include <time.h>
#include <locale.h>
// #ifdef __cplusplus
// }
// #endif
#include <iostream>
#include <fstream>
#include <string>


/**
 * Record each player move
 */
class Recorder {
 protected:

  std::ofstream *ofs_;

  std::string GetNowTimeString() {
    time_t t = time(NULL);
    struct tm *local_time;
    char buf[BUFSIZ];
    local_time = localtime(&t);
    strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", local_time);
    std::string ret_str = std::string(buf);
    return ret_str;
  }
  
 public:
  
  explicit Recorder() :
      ofs_(new std::ofstream("record.trx")) {}

  explicit Recorder(int dummy) {
    const std::string fname = "./record/record_" + GetNowTimeString() + ".trx";
    ofs_ = new std::ofstream(fname);
  }

  explicit Recorder(const std::string file_name) :
      ofs_(new std::ofstream(file_name)) {}

  ~Recorder() {
    delete ofs_;
  }

  void Record(const std::string move) {
    *ofs_ << move << std::endl;
  }
};


#endif  // end RECORDER_HPP_
