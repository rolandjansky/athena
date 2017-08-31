/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2CALOHYPOALG_H
#define TRIGEGAMMAHYPO_TRIGL2CALOHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "TrigL2CaloHypoTool.h"

/**
 * @class Implements egamma calo selection for the new HLT framework
 * @brief 
 **/

class TrigL2CaloHypoAlg
  : public ::AthReentrantAlgorithm
{ 
 public: 

  TrigL2CaloHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigL2CaloHypoAlg(); 

  StatusCode  initialize() override;
  StatusCode  execute_r( const EventContext& context ) const override;
  StatusCode  finalize() override;
 
 private: 
  TrigL2CaloHypoAlg();
  ToolHandleArray< TrigL2CaloHypoTool > m_hypoTools;
  SG::ReadHandleKey< xAOD::TrigEMClusterContainer > m_clustersKey;
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roisKey;
  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_decisionsKey;
 
}; 


#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOALG_H
