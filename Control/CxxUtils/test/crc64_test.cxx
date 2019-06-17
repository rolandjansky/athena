/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CxxUtils/test/crc64_test.cxx
 * @author scott snyder
 * @date May, 2018
 * @brief Regression tests for CRC-64 calculation.
 */

#undef NDEBUG

#include "CxxUtils/crc64.h"
#include "boost/timer/timer.hpp"
#include <vector>
#include <iostream>
#include <cassert>
#include <string.h>


void testextend (const std::string& str, unsigned int x)
{
  uint64_t crc1 = CxxUtils::crc64 (str);
  crc1 = CxxUtils::crc64addint (crc1, x);

  std::string str2 = str;
  while (x > 0) {
    str2.push_back ((char)(x&0xff));
    x >>= 8;
  }
  assert (crc1 == CxxUtils::crc64 (str2));
}


void test1()
{
  std::cout << "test1\n";

  const uint64_t p = 0xad93d23594c935a9;
  std::unique_ptr<CxxUtils::CRCTable> table = CxxUtils::makeCRCTable (p);

  const char* testin1 = "alkdkjasldjaldjalkdjaldjoqiwj;lknnfaoiuuhewfuasuhfaiuuhffiuuhaffoiuahfefiuauheofiuhapkjjjhdhfpiauw3hpkjajshhdfpkkuawheoiuahwefpiuawhfeiauehfpiauwehfpawieufhpaw";
  const char* testout1 = "047BDD061CE68225";
  const char* testin2 = "alkdkjasldjaldjalkdjalejoqiwj;lknnfaoiuuhewfuasuhfaiuuhffiuuhaffoiuahfefiuauheofiuhapkjjjhdhfpiauw2hpkjajshhdfpkkuawheoiuahwefpiuawhfeiauehfpiauwehfpawieufhpaw"; // Differs from 1st in two bits.
  const char* testout2 = "6051B184A58A80C6";

  uint64_t crc1 = CxxUtils::crc64_bytewise (*table, testin1, strlen (testin1));
  uint64_t crc2 = CxxUtils::crc64_bytewise (*table, testin2, strlen (testin2));
  assert (CxxUtils::crc64format (crc1) == testout1);
  assert (CxxUtils::crc64format (crc2) == testout2);

  testextend (testin1, 54356);
  testextend (testin2, 9812674);
}


// Test crc64 against crc64_bytewise.
void validate()
{
  std::cout << "validate\n";

  for (int len=0; len<1000; len++) {
    std::string s;
    s.reserve (len);
    for (int i=0; i < len; i++) {
      s += (char)((len+i)%26 + 'a');
    }

    assert (CxxUtils::crc64_bytewise(s) == CxxUtils::crc64(s));

    const char* data = s.data();
    for (int i=0; i < std::min(len,64); i++) {
      assert (CxxUtils::crc64_bytewise(data+i, len-i) == CxxUtils::crc64(data+i, len-i));
    }
  }
}


class Timer
{
public:
  Timer();

  class RunTimer
  {
  public:
    RunTimer (boost::timer::cpu_timer& timer) : m_timer (&timer)
    { timer.resume(); }
    RunTimer (RunTimer&& other) : m_timer (other.m_timer) { other.m_timer = nullptr; }
    ~RunTimer() { if (m_timer) m_timer->stop(); }
  private:
    boost::timer::cpu_timer* m_timer;
  };
  RunTimer run() { return RunTimer (m_timer); }

  std::string format() const { return m_timer.format(3); }

private:
  boost::timer::cpu_timer m_timer;
};


Timer::Timer()
{
  m_timer.stop();
}


class Corpus
{
public:
  typedef uint64_t fn_t (const std::string& s);
  
  Corpus (int nstring, int minlen, int maxlen);
  void check (fn_t* f1, fn_t* f2);
  uint64_t time (Timer& t, fn_t* f);

  
private:
  std::vector<std::string> m_strings;
};


Corpus::Corpus (int nstring, int minlen, int maxlen)
{
  m_strings.reserve (nstring);
  int len = minlen;
  for (int i=0; i < nstring; i++) {
    std::string s;
    s.reserve (len);
    for (int i=0; i < len; i++) {
      s += (char)((len+i)%26 + 'a');
    }

    m_strings.emplace_back (std::move (s));
    
    ++len;
    if (len > maxlen) len = minlen;
  }
}


void Corpus::check (fn_t* f1, fn_t* f2)
{
  for (const std::string& s : m_strings) {
    assert (f1(s) == f2(s));
  }
}


uint64_t Corpus::time (Timer& t, fn_t* f)
{
  Timer::RunTimer rt (t.run());
  uint64_t sum = 0;
  for (const std::string& s : m_strings) {
    sum ^= f(s);
  }
  return sum;
}



uint64_t perftest()
{
  Corpus corpus_short (1000000, 20, 40);
  corpus_short.check (CxxUtils::crc64_bytewise, CxxUtils::crc64);
  Timer t_short_crc64;
  Timer t_short_crc64_vec;
  Timer t_long_crc64;
  Timer t_long_crc64_vec;

  Corpus corpus_long (1000, 50000, 60000);
  corpus_long.check (CxxUtils::crc64_bytewise, CxxUtils::crc64);

  uint64_t sum = 0;
  for (int i=0; i < 10; i++) {
    sum ^= corpus_short.time (t_short_crc64, CxxUtils::crc64_bytewise);
    sum ^= corpus_long.time (t_long_crc64, CxxUtils::crc64_bytewise);
    sum ^= corpus_short.time (t_short_crc64_vec, CxxUtils::crc64);
    sum ^= corpus_long.time (t_long_crc64_vec, CxxUtils::crc64);
  }

  std::cout << "short, old: " << t_short_crc64.format();
  std::cout << "short, vec: " << t_short_crc64_vec.format();
  std::cout << "long,  old: " << t_long_crc64.format();
  std::cout << "long,  vec: " << t_long_crc64_vec.format();

  return sum;
}


int main (int argc, const char** argv)
{
  if (argc > 1 && strcmp (argv[1], "--perf") == 0) {
    perftest();
    return 0;
  }

  std::cout << "crc64_test\n";
  test1();
  validate();
  return 0;
}

