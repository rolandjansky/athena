/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGEGAMMFASTCALOHYPOTOOLMULT_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOTOOLMULT_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaFastCaloHypoTool.h"



/**
 * @class Implementation of the Egamma selection for CaloClusters with multiplicity
 * @brief 
 **/

class TrigEgammaFastCaloHypoToolMult : public extends<AthAlgTool, ITrigEgammaFastCaloHypoTool> { 
 public: 
  TrigEgammaFastCaloHypoToolMult( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigEgammaFastCaloHypoToolMult();
  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaFastCaloHypoTool::FastClusterInfo>& input )  const override;

  virtual bool decide( const ITrigEgammaFastCaloHypoTool::FastClusterInfo& ) const override { 
    REPORT_MESSAGE(MSG::ERROR) << "this method should never be called";
    return false;
  }

 private:
  HLT::Identifier m_decisionId;

  ToolHandleArray<ITrigEgammaFastCaloHypoTool> m_subTools { this, "SubTools", {}, "Sub tools performing cuts" };
  
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGEgammaFastCALOHYPOTOOLMULT_H
