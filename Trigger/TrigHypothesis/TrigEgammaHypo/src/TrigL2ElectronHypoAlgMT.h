/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2ELECTRONHYPOALGMT_H
#define TRIGEGAMMAHYPO_TRIGL2ELECTRONHYPOALGMT_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/View.h"

#include "TrigL2ElectronHypoTool.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/HypoBase.h"


/**
 * @class TrigL2ElectronHypoAlgMT
 * @brief Implements Hypo selection on L2 electrons
 **/

class TrigL2ElectronHypoAlgMT  :  public ::HypoBase 
{ 
 public: 

  TrigL2ElectronHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute(const EventContext& context) const override;

 private: 
  ToolHandleArray< TrigL2ElectronHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
  Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };
  // internally used to getch from views
  SG::ReadHandleKey< xAOD::TrigElectronContainer > m_electronsKey {this, "Electrons", "L2ElectronContainer", "Input"};
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGL2ELECTRONHYPOALGMT_H
