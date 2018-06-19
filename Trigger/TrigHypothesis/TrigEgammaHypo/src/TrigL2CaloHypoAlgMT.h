/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2CALOHYPOALGMT_H
#define TRIGEGAMMAHYPO_TRIGL2CALOHYPOALGMT_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"

#include "ITrigL2CaloHypoTool.h"

/**
 * @class TrigL2CaloHypoAlgMT
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigL2CaloHypoAlgMT : public ::HypoBase {
 public: 

  TrigL2CaloHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigL2CaloHypoAlgMT(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigL2CaloHypoAlgMT();
  ToolHandleArray< ITrigL2CaloHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::TrigEMClusterContainer > m_clustersKey { this, "CaloClusters", "CaloClusters", "CaloClusters in view" };  
  

}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGL2CALOHYPOALG_H
