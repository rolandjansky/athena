/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef BYTESTREAMCNVSVCBASE_ROBDATAPROVIDERMTTEST_H
#define BYTESTREAMCNVSVCBASE_ROBDATAPROVIDERMTTEST_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"


/**
 * @class $(klass)s
 * @brief 
 **/

class ROBDataProviderMTTest
  : public ::AthReentrantAlgorithm
{ 
 public: 

  ROBDataProviderMTTest( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ROBDataProviderMTTest(); 


  //ROBDataProviderMTTest &operator=(const ROBDataProviderMTTest &alg); 

  StatusCode  initialize() override;
  StatusCode  execute_r(const EventContext& context) const override;
  StatusCode  finalize() override;
 
 private: 
  ROBDataProviderMTTest();
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider{ this, "ROBDataProvider", "ROBDataProviderSvc/ROBDataProviderSvc", ""};
  
}; 


#endif //> !BYTESTREAMCNVSVCBASE_ROBDATAPROVIDERMTTEST_H
