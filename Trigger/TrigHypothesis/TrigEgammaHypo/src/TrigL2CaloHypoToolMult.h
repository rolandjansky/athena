/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOLMULT_H
#define TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOLMULT_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "ITrigL2CaloHypoTool.h"



/**
 * @class Implementation of the Egamma selection for CaloClusters with multiplicity
 * @brief 
 **/

class TrigL2CaloHypoToolMult : public extends<AthAlgTool, ITrigL2CaloHypoTool> { 
 public: 
  TrigL2CaloHypoToolMult( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigL2CaloHypoToolMult();
  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigL2CaloHypoTool::ClusterInfo>& input )  const override;

  virtual bool decide( const ITrigL2CaloHypoTool::ClusterInfo& ) const override { 
    REPORT_MESSAGE(MSG::ERROR) << "this method should never be called";
    return false;
  }

 private:
  HLT::Identifier m_decisionId;

  ToolHandleArray<ITrigL2CaloHypoTool> m_subTools { this, "SubTools", {}, "Sub tools performing cuts" };
  
}; 
DECLARE_TOOL_FACTORY( TrigL2CaloHypoToolMult )
#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOLMULT_H
