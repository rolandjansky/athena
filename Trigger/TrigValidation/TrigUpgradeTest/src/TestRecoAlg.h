/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTRECOALG_H
#define TRIGUPGRADETEST_TESTRECOALG_H 1


#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

namespace HLTTest {

/**
 * @class $(klass)s
 * @brief 
 **/

class TestRecoAlg
  : public ::AthAlgorithm
{ 
 public: 
  TestRecoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestRecoAlg(); 

  //TestRecoAlg &operator=(const TestRecoAlg &alg); 

  StatusCode  initialize() override;
  StatusCode  execute() override;
  StatusCode  finalize() override;

 private: 
  TestRecoAlg();

}; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTRECOALG_H
