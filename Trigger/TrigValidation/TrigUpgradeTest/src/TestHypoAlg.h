/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTHYPOALG_H
#define TRIGUPGRADETEST_TESTHYPOALG_H 1


#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

namespace HLTTest {

/**
 * @class $(klass)s
 * @brief 
 **/

class TestHypoAlg
  : public ::AthAlgorithm
{ 
 public: 
  TestHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestHypoAlg(); 

  //TestHypoAlg &operator=(const TestHypoAlg &alg); 

  StatusCode  initialize() override;
  StatusCode  execute() override;
  StatusCode  finalize() override;

 private: 
  TestHypoAlg();

}; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTHYPOALG_H
