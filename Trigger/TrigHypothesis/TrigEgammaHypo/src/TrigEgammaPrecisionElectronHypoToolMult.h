/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONHYPOTOOLMULT_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONHYPOTOOLMULT_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODEgamma/Electron.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "ITrigEgammaPrecisionElectronHypoTool.h"


/**
 * @class Implementation of the Egamma selection for Electrons with multiplicity
 * @brief 
 **/

class TrigEgammaPrecisionElectronHypoToolMult : public extends<AthAlgTool, ITrigEgammaPrecisionElectronHypoTool> { 
 public: 
  TrigEgammaPrecisionElectronHypoToolMult( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigEgammaPrecisionElectronHypoToolMult();
  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaPrecisionElectronHypoTool::ElectronInfo>& input )  const override;

  virtual bool decide( const ITrigEgammaPrecisionElectronHypoTool::ElectronInfo& ) const override { 
    REPORT_MESSAGE(MSG::ERROR) << "this method should never be called";
    return false;
  }

 private:
  HLT::Identifier m_decisionId;

  ToolHandleArray<ITrigEgammaPrecisionElectronHypoTool> m_subTools { this, "SubTools", {}, "Sub tools performing cuts" };
  
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONHYPOTOOLMULT_H
