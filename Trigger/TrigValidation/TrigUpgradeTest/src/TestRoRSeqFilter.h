/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTRORSEQFILTER_H
#define TRIGUPGRADETEST_TESTRORSEQFILTER_H 1


#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

namespace HLTTest {

/**
 * @class TestRoRSeqFilter for validation of the CF sequencers barrier concept 
 * @brief This alg will read in implicit data, potentially merge, but above all it will decide to accep/reject 
 * and thus to continue reco in the sequence
 **/

class TestRoRSeqFilter
  : public ::AthAlgorithm
{ 
 public: 
  TestRoRSeqFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestRoRSeqFilter(); 

  //TestRoRSeqFilter &operator=(const TestRoRSeqFilter &alg); 

  StatusCode  initialize() override;
  StatusCode  execute( ) override;
  StatusCode  finalize() override;

 private: 
  TestRoRSeqFilter();
  std::vector<std::string> m_inputs;
}; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTRORSEQFILTER_H
