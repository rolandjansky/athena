/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAHYPO_TRIGL2PHOTONHYPOALGMT_H
#define TRIGEGAMMAHYPO_TRIGL2PHOTONHYPOALGMT_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigL2PhotonHypoTool.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/HypoBase.h"
#include "ITrigL2CaloHypoTool.h"


/**
 * @class TrigL2PhotonHypoAlgoMT
 * @brief Implements Hypo selection on L2 photons
 **/

class TrigL2PhotonHypoAlgMT  :  public ::HypoBase 
{ 
 public: 

  TrigL2PhotonHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute(const EventContext& context) const override;

 private: 
  ToolHandleArray< TrigL2PhotonHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
  Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };
  // internally used to getch from views
  SG::ReadHandleKey< xAOD::TrigPhotonContainer > m_photonsKey {this, "Photons", "L2PhotonContainer", "Input"};
  
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGL2PHOTONHYPOALGMT_H                                                                                                                                                          
