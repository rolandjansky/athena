/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTAUHYPO_TRIGL2TAUCALOHYPOALGMT_H
#define TRIGTAUHYPO_TRIGL2TAUCALOHYPOALGMT_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
//#include "xAODTrigCalo/TrigEMClusterContainer.h"
//#include "xAODTrigCalo/TrigCaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"

#include "ITrigL2TauCaloHypoTool.h"

/**
 * @class TrigL2CaloHypoAlgMT
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigL2TauCaloHypoAlgMT : public ::HypoBase {
 public: 

  TrigL2TauCaloHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigL2TauCaloHypoAlgMT(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigL2TauCaloHypoAlgMT();
  ToolHandleArray< ITrigL2TauCaloHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::CaloClusterContainer > m_clustersKey { this, "caloclusters", "caloclusters", "caloclusters in view" };
  

}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGL2TAUCALOHYPOALG_H
