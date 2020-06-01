/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMULTIVARHYPO_TRIGL2CALORINGERHYPOTOOLMULT_H
#define TRIGMULTIVARHYPO_TRIGL2CALORINGERHYPOTOOLMULT_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "ITrigL2CaloRingerHypoToolMT.h"



/**
 * @class Implementation of the Egamma selection for CaloClusters with multiplicity
 * @brief 
 **/

class TrigL2CaloRingerHypoToolMTMult : public extends<AthAlgTool, ITrigL2CaloRingerHypoToolMT> { 
 public: 
  TrigL2CaloRingerHypoToolMTMult( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigL2CaloRingerHypoToolMTMult();
  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigL2CaloRingerHypoToolMT::RingerInfo>& input )  const override;

  virtual bool decide( const ITrigL2CaloRingerHypoToolMT::RingerInfo& ) const override { 
    REPORT_MESSAGE(MSG::ERROR) << "this method should never be called";
    return false;
  }

 private:
  HLT::Identifier m_decisionId;

  ToolHandleArray<ITrigL2CaloRingerHypoToolMT> m_subTools { this, "SubTools", {}, "Sub tools performing cuts" };
  
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOLMULT_H
