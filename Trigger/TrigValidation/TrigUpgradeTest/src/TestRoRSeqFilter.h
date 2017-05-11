/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTRORSEQFILTER_H
#define TRIGUPGRADETEST_TESTRORSEQFILTER_H 1


#include <string>

#include "AthenaBaseComps/AthFilterAlgorithm.h"

namespace HLTTest {

/**
 * @class $(klass)s
 * @brief 
 **/

class TestRoRSeqFilter
  : public ::AthFilterAlgorithm
{ 
 public: 
  TestRoRSeqFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestRoRSeqFilter(); 

  //TestRoRSeqFilter &operator=(const TestRoRSeqFilter &alg); 

  StatusCode  initialize() override;
  StatusCode  execute() override;
  StatusCode  finalize() override;

 private: 
  TestRoRSeqFilter();

}; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTRORSEQFILTER_H
