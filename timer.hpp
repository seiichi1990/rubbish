#ifndef TIMER_HPP_
#define TIMER_HPP_


// #include <sys/resource.h>
#include <sys/time.h>
#include <float.h>


class Timer {
 private:
  
  // rusage start_;
  // rusage stop_;
  struct timeval start_;
  struct timeval stop_;
  float elapsed_time_;
  float accum_time_;
  float max_time_;
  float min_time_;
  int num_calls_;
  std::string *name_;

 public:

  Timer() :
      accum_time_(0),
      max_time_(0),
      min_time_(FLT_MAX),
      num_calls_(0),
      name_(NULL) {}

  Timer(const char *name) :
      accum_time_(0),
      max_time_(0),
      min_time_(FLT_MAX),
      num_calls_(0),
      name_(new std::string(name)) {}
 
  void Start(void) {
    // getrusage(RUSAGE_SELF, &start_);
    gettimeofday(&start_, NULL);
  }

  inline void SetElapsedTime() {
    // float sec = stop_.ru_utime.tv_sec - start_.ru_utime.tv_sec;
    // float usec = stop_.ru_utime.tv_usec - start_.ru_utime.tv_usec;
    float sec = stop_.tv_sec - start_.tv_sec;
    float usec = stop_.tv_usec - start_.tv_usec;
    elapsed_time_ = (sec * 1000) + (usec / 1000);
    if (elapsed_time_ > max_time_) max_time_ = elapsed_time_;
    if (elapsed_time_ < min_time_) min_time_ = elapsed_time_;
  }
  
  void Stop(void) {
    // getrusage(RUSAGE_SELF, &stop_);
    gettimeofday(&stop_, NULL);
    SetElapsedTime();
    accum_time_ += elapsed_time_;
    ++num_calls_;
  }

  float GetElapsedTime(void) {
    return elapsed_time_;
  }

  void GetAccumulatedTime(float &accum_time, int &count) {
    accum_time = accum_time_;
    count = num_calls_;
  }

  float GetMeanTime() {
    return accum_time_ / num_calls_;
  }

  void PrintTime() {
    // printf("Time (ms)\tCalls\tAvg (ms)\tMin (ms) \tMax (ms)\tName\n");
    printf("%e\t", accum_time_);
    printf("%d\t", num_calls_);
    printf("%e\t", GetMeanTime());
    printf("%e\t", min_time_);
    printf("%e\t", max_time_);
    if (name_) {
      const char *name = name_->c_str();
      printf("%s", name);
    }
    printf("\n");
  }
};



#endif  // end TIMER_HPP_
