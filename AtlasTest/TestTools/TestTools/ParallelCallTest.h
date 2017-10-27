/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestTools_ParallelCallTest_h
#define TestTools_ParallelCallTest_h

#include <cstddef>
#include <initializer_list>
/**
 * @brief Provides interface and helper functions to perform stress testing of the thread-safe code.
 * 
 * It is applicable to services which are meant to return identical reposnse to the same request/query independently 
 * of the order of execution. Examples of such services are all sort of cache handlers.
 *
 * Testing idea is that the implementation of the ParallelCallTest interface provides two methods. 
 * The firstCall which performs request/query of the service and treats the response as a reference. 
 * The callAndCompare (called several times in parallel) repeats the query and compares the result to the reference. 
 *   When successive responses differ from the reference it means that the service is not thread-safe.
 *
 * Typically one shoudl construct several (compeeting) call scenarios and launch them all in parallel. 
 * The helper function launchTests facilitates this task.
 *
 * An example of testing is available in the tests/PrallelCalltest_example.cxx
 * Implementation uses tbb::parallel_* so in order to make the testing effective a tbb thread pool has to be > 1.
 **/


class ParallelCallTest {
public:
  /**
   * @brief runs the stress test by invoking it the firstCall and then repetitively 
   * the callAndCompare @arg nrepeats times (>=1)
   * A single failure of the callAndCompare would result in the whole execution failed
   **/
  bool run( size_t nrepeats );
  /**
   * @brief a method that will be called to obtain first results from the service
   * It should set the reference quantities
   **/
  virtual void firstCall() = 0;

  /**
   * @brief a function that performs request, and compares the results obtained
   * with the result of the first execution
   * When result differ this function is supposed to return false, otherwise true
   **/
  virtual bool callAndCompare() const = 0;

  /**
   * @brief Method to run launch number of tests in parallel (increasing the stress of the calle)
   * It has a potential that also the "first" calls can be launched in parallel and thus be the worse 
   * case compared to a situation when the first calls are invoked sequentially while further calls in parallel.
   **/
  
  static bool launchTests( size_t nrepeats, std::initializer_list<ParallelCallTest*> testList );
  
};
#endif 
