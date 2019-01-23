/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEFMISSINGET_EFMISSINGETALGMT_H
#define TRIGEFMISSINGET_EFMISSINGETALGMT_H 1

#include <string>
#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigEFMissingET/EFMissingETFromHelper.h"
#include "TrigEFMissingET/IMissingETTool.h"


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
  virtual StatusCode  execute(const EventContext& context) const override;

  //!< Converts from MeV to GeV and them log10, preserving the sign, the minimum dictated by monitoring histograms
  static double toLogGeV(double x, double fallback=0, double epsilon=1e-6);
  
  //!< converts to from MeV to GeV if above threshold, else falback value
  static double toLinGeV(double x, double fallback=0, double epsilon=1e-6); 
 private: 
  
  SG::WriteHandleKey<xAOD::TrigMissingETContainer> m_metContainerKey { this, "METContainerKey", "HLT_MET", "Name of the output collection" };
  ToolHandleArray<IMissingETTool> m_metTools { this, "METTools", {}, "Tools to fill MET object" };
  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
  ToolHandle<EFMissingETFromHelper> m_helperTool{ this, "HelperTool", "EFMissingETFromHelper/HelperTool", "Helper tool" };
}; 


#endif //> !TRIGEFMISSINGET_EFMISSINGETALGMT_H
