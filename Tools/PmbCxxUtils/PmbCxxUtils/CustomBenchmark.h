/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class CustomBenchmark                     //
//                                                            //
//  Description: Helper class for performing custom           //
//               bench-marking of CPU. This is a simple       //
//               implementation without support for nested    //
//               bench-markings (i.e. a call to begin(..)     //
//               must always be followed by a call to end(),  //
//               never another call to begin(..)).            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2012                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef CUSTOMBENCHMARK_H
#define CUSTOMBENCHMARK_H

#include <ctime>
#include "stdint.h"//<cstdint>
#include <cassert>
#include <limits>

namespace PMonUtils {
  class CustomBenchmark {
  public:

    CustomBenchmark(unsigned nmax);
    ~CustomBenchmark();

    //For benchmarking
    void begin(unsigned id);
    void end(int count);

    //For accessing the results:
    void getData(unsigned id, uint64_t& count, double& time_ms) const;
    unsigned nMax() const { return m_nmax; }
    unsigned size() const { return m_nmax; }

  private:
    struct Data {
      Data() { init(); }
      void init() { time_spent = 0; time_at_begin = 0; count = 0; }
      int64_t time_spent;
      int64_t time_at_begin;
      unsigned count;
    };

    const unsigned m_nmax;
    Data * m_data;
    Data * m_data_current;

    // It is illegal to copy/assign a CustomBenchmark:
    CustomBenchmark( const CustomBenchmark & );
    CustomBenchmark & operator= ( const CustomBenchmark & );

    //A non-overflowing "clock()":
    static int64_t clock_nooverflow();

  };

  inline void CustomBenchmark::begin(unsigned id)
  {
    assert(id<m_nmax);
    m_data_current = &(m_data[id]);
    m_data_current->time_at_begin = clock_nooverflow();
  }
  inline void CustomBenchmark::end(int count=1)
  {
    assert(m_data_current);
    m_data_current->count += count;
    m_data_current->time_spent += clock_nooverflow() - m_data_current->time_at_begin;
    m_data_current=0;
  }

  inline void CustomBenchmark::getData(unsigned id, uint64_t& count, double& time_ms) const
  {
    assert(id<m_nmax);
    Data * data = &(m_data[id]);
    count = data->count;
    time_ms = data->time_spent * (1000.0/CLOCKS_PER_SEC);
  }

  inline int64_t CustomBenchmark::clock_nooverflow() {
    //Copied from PerfMonComps/trunk/src/SemiDetMisc.h
    //
    //In gnu, clock_t is a long and CLOCKS_PER_SECOND 1000000 so clock()
    //will overflow in 32bit builds after a few thousands of seconds. To
    //avoid this, we have the following method instead which notices when
    //overflow occurs and corrects for it (it won't notice if it doesn't
    //get called for >4000s, but this should be ok for almost all of our
    //use cases):
    assert(std::numeric_limits<clock_t>::is_integer);
    if (sizeof(clock_t)>=sizeof(int64_t))
      return clock();//64bit builds shouldn't have overflow issues.

    //not so clean with statics i guess:
    static clock_t last=clock();
    static int64_t offset=0;
    clock_t c=clock();
    if (c<last)
      offset+=int64_t(std::numeric_limits<unsigned>::max())-int64_t(std::numeric_limits<unsigned>::min());
    last=c;
    return offset+c;
  }

  //A class which makes for instance makes it easier to guarantee that
  //an exception in the benchmarked code will still result in
  //CustomBenchmark::end() will be called properly. Will do nothing if
  //called with a null CustomBenchmark*:

  class CustomBenchmarkGuard
  {
  public:
    CustomBenchmarkGuard(CustomBenchmark* cb, unsigned id, int count=1) : m_cb(cb), m_count(count) { if (m_cb) m_cb->begin(id); }
    ~CustomBenchmarkGuard() { if (m_cb) m_cb->end(m_count); }
  private:
    CustomBenchmark * m_cb;
    int               m_count;
  };

}


#endif
