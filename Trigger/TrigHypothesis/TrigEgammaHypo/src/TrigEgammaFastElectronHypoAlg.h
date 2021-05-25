/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTELECTRONHYPOALG_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTELECTRONHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/View.h"
#include "TrigEgammaFastElectronHypoTool.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/HypoBase.h"


/**
 * @class TrigEgammaFastElectronHypoAlg
 * @brief Implements Hypo selection on L2 electrons
 **/

class TrigEgammaFastElectronHypoAlg  :  public ::HypoBase 
{ 
 public: 

  TrigEgammaFastElectronHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute(const EventContext& context) const override;

 private: 
  ToolHandleArray< TrigEgammaFastElectronHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
  Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };
  // internally used to getch from views
  SG::ReadHandleKey< xAOD::TrigElectronContainer > m_electronsKey {this, "Electrons", "L2ElectronContainer", "Input"};
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGEGAMMAFASTELECTRONHYPOALG_H
