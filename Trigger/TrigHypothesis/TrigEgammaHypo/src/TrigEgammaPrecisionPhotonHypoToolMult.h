/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOTOOLMULT_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOTOOLMULT_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODEgamma/Photon.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaPrecisionPhotonHypoTool.h"


/**
 * @class Implementation of the Egamma selection for Photons with multiplicity
 * @brief 
 **/

class TrigEgammaPrecisionPhotonHypoToolMult : public extends<AthAlgTool, ITrigEgammaPrecisionPhotonHypoTool> { 
 public: 
  TrigEgammaPrecisionPhotonHypoToolMult( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigEgammaPrecisionPhotonHypoToolMult();
  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaPrecisionPhotonHypoTool::PhotonInfo>& input )  const override;

  virtual bool decide( const ITrigEgammaPrecisionPhotonHypoTool::PhotonInfo& ) const override { 
    REPORT_MESSAGE(MSG::ERROR) << "this method should never be called";
    return false;
  }

 private:
  HLT::Identifier m_decisionId;

  ToolHandleArray<ITrigEgammaPrecisionPhotonHypoTool> m_subTools { this, "SubTools", {}, "Sub tools performing cuts" };
  
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOTOOLMULT_H
