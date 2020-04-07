/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONCALOHYPOTOOLMULT_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONCALOHYPOTOOLMULT_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaPrecisionCaloHypoTool.h"

//// Forward declarations
//namespace xAOD {
//        class CaloCluster;
//}
//class TrigRoiDescriptor;



/**
 * @class Implementation of the Egamma selection for CaloClusters with multiplicity
 * @brief 
 **/

class TrigEgammaPrecisionCaloHypoToolMult : public extends<AthAlgTool, ITrigEgammaPrecisionCaloHypoTool> { 
 public: 
  TrigEgammaPrecisionCaloHypoToolMult( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigEgammaPrecisionCaloHypoToolMult();
  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaPrecisionCaloHypoTool::ClusterInfo>& input )  const override;

  virtual bool decide( const ITrigEgammaPrecisionCaloHypoTool::ClusterInfo& ) const override { 
    REPORT_MESSAGE(MSG::ERROR) << "this method should never be called";
    return false;
  }

 private:
  HLT::Identifier m_decisionId;

  ToolHandleArray<ITrigEgammaPrecisionCaloHypoTool> m_subTools { this, "SubTools", {}, "Sub tools performing cuts" };
  
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGPRECISIONCALOHYPOTOOLMULT_H
