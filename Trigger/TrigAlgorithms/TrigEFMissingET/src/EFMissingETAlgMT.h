/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEFMISSINGET_EFMISSINGETALGMT_H
#define TRIGEFMISSINGET_EFMISSINGETALGMT_H 1

#include <string>
#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

#include "./IMissingETTool.h"


/**
 * @class HLT MET algorithm
 * @brief It is responsible for creation of the MET object and execution of number of MET tools filling it
 **/

class EFMissingETAlgMT : public ::AthReentrantAlgorithm
{ 
 public: 

  EFMissingETAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EFMissingETAlgMT(); 


  //EFMissingETMT &operator=(const EFMissingETMT &alg); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r(const EventContext& context) const override;
 
 private: 
  
  SG::WriteHandleKey<xAOD::TrigMissingETContainer> m_metContainerKey { this, "METContainerKey", "HLT_MET", "Name of the output collection" };
  ToolHandleArray<IMissingETTool> m_metTools { this, "METTools", {}, "Tools to fille MET object" };
  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 


#endif //> !TRIGEFMISSINGET_EFMISSINGETMT_H
