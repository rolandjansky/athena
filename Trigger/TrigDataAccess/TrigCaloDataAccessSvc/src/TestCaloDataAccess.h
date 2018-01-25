/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigCaloDataAccessSvc_TestCaloDataAccess_h
#define TrigCaloDataAccessSvc_TestCaloDataAccess_h

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigCaloDataAccessSvc/ITrigCaloDataAccessSvc.h"


/**
 * @class Test algorithm for the data access svc
 * @brief It is supposed to try out all the methods + do that from several threads to assure 
 * service robustness
 **/

class TestCaloDataAccess
  : public ::AthReentrantAlgorithm
{ 
 public: 

  TestCaloDataAccess( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TestCaloDataAccess(); 
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r(const EventContext& context) const override;
 
 private: 
  TestCaloDataAccess();
  ServiceHandle<ITrigCaloDataAccessSvc> m_dataAccessSvc; 
}; 


#endif 
