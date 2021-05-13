/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigT2CaloCommon_TestCaloDataAccess_h
#define TrigT2CaloCommon_TestCaloDataAccess_h

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"
#include "LArRawConditions/LArMCSym.h"
#include "StoreGate/ReadCondHandleKey.h"


/**
 * @class Test algorithm for the data access svc
 * @brief It is supposed to try out all the methods + do that from several threads to assure 
 * service robustness
 **/

class ParallelCallTest;

class TestCaloDataAccess
  : public ::AthReentrantAlgorithm
{ 
 public: 

  TestCaloDataAccess( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TestCaloDataAccess(); 
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute(const EventContext& context) const override;
 
 private: 
  TestCaloDataAccess();
  void emulateRoIs( const EventContext& context, std::vector<ParallelCallTest*>& allRoIs ) const;
  void emulateFixedRoIs( const EventContext& context, std::vector<ParallelCallTest*>& allRoIs ) const;
  ServiceHandle<ITrigCaloDataAccessSvc> m_dataAccessSvc; 

  int m_nFixedRoIs;
  bool m_emulateRoIs;
  bool m_emulateFixedRoIs;
  SG::ReadCondHandleKey<LArMCSym> m_mcsymKey
    {this, "MCSymKey", "LArMCSym", "SG Key of LArMCSym object"};
}; 


#endif 
