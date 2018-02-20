/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_MonitorConditionsTestAlg.h
*
* @brief Header file for the SCT_MonitorConditionsTestAlg class 
*  in package SCT_ConditionsAlgorithms
*
* @author Kazu Hanagaki
**/

#ifndef SCT_MonitorConditionsTestAlg_H
#define SCT_MonitorConditionsTestAlg_H

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"

//Gaudi
#include "GaudiKernel/ServiceHandle.h"

// Read Handle Key
#include "StoreGate/ReadHandleKey.h"
// Event Info
#include "xAODEventInfo/EventInfo.h"

//Forward declarations
class ISCT_MonitorConditionsSvc;
class SCT_ID;

///Example class to show calling the SCT_MonitorConditions
class SCT_MonitorConditionsTestAlg : public AthAlgorithm {
 public:
  SCT_MonitorConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
  virtual ~SCT_MonitorConditionsTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
   
 private:
  ServiceHandle<ISCT_MonitorConditionsSvc> m_pMonitorConditionsSvc;
  const SCT_ID*                            m_sctId;

  // Parameters to control the db access 
  SG::ReadHandleKey<xAOD::EventInfo>       m_evtKey;
  UnsignedIntegerProperty                  m_select_run;
  UnsignedIntegerProperty                  m_select_event;

}; // end of class

#endif // SCT_MonitorConditionsTestAlg_H
