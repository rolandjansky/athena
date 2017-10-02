/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2CALOHYPOALG_H
#define TRIGEGAMMAHYPO_TRIGL2CALOHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "ITrigL2CaloHypoTool.h"

/**
 * @class Implements egamma calo selection for the new HLT framework
 * @brief 
 **/

class TrigL2CaloHypoAlg : public ::AthReentrantAlgorithm { 
 public: 

  TrigL2CaloHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigL2CaloHypoAlg(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r( const EventContext& context ) const override;

 
 private: 
  TrigL2CaloHypoAlg();
  ToolHandleArray< ITrigL2CaloHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
  
  SG::ReadHandleKey< std::vector< SG::View* > > m_viewsKey { this, "Views", "Unspecified", "Input Views" };
  SG::ReadHandleKey< xAOD::TrigEMClusterContainer > m_clustersKey { this, "CaloClusters", "CaloClusters", "CaloClusters in view" };
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roisKey { this, "RoIs", "RoIs", "RoIs key in the view" };
  SG::ReadHandleKey< TrigCompositeUtils::DecisionContainer > m_previousDecisionsKey { this, "L1Decisions", "Undefined", "Key for L1 decisions per RoI" };
  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_decisionsKey { this, "Decisions", "Unspecified", "Decisions" };
 
}; 

DECLARE_ALGORITHM_FACTORY( TrigL2CaloHypoAlg )
#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOALG_H
