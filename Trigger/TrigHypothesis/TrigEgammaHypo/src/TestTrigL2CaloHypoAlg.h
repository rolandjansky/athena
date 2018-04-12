/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TESTTRIGL2CALOHYPOALG_H
#define TRIGEGAMMAHYPO_TESTTRIGL2CALOHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"

#include "ITrigL2CaloHypoTool.h"

/**
 * @class Implements egamma calo selection for the new HLT framework
 * @brief 
 **/
class TestTrigL2CaloHypoAlg : public ::HypoBase {
  //class TestTrigL2CaloHypoAlg : public ::AthReentrantAlgorithm { 
 public: 

  TestTrigL2CaloHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestTrigL2CaloHypoAlg(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TestTrigL2CaloHypoAlg();
  ToolHandleArray< ITrigL2CaloHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::TrigEMClusterContainer > m_clustersKey { this, "CaloClusters", "CaloClusters", "CaloClusters in view" };  
  Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };

}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGL2CALOHYPOALG_H
