/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "pool.h"

#include <thread>
#include <mutex>

namespace {

  using WorkList_cit = std::vector<WorkUnit>::const_iterator;
  
  const WorkList_cit safe_advance(WorkList_cit& next, const WorkList_cit end)
  {
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    return next != end ? next++ : end;
  }

  void worker(WorkList_cit& next, const WorkList_cit end)
  {
    for (;;) {
      const WorkList_cit last = safe_advance(next, end);
      if (last == end) { return; }
      (*last)();
    }
  }
  
  struct Threads 
  {
    Threads(const size_t n, std::function<void(void)> function) 
      {
	for (size_t k = 0; k != n; k++) { threads.emplace_back( function ); }
      }
    ~Threads() { for (auto& t : threads) { t.join(); } }
  private:
    std::vector<std::thread> threads;
  };

} //namespace

void process(const WorkList& workList, const size_t nThreads)
{
  WorkList_cit begin(std::begin(workList)), end(std::end(workList));
  if (nThreads == 0) {
    worker(begin, end);
  } else {
    Threads(nThreads, [&](){ worker(begin, end); });
  }
}

