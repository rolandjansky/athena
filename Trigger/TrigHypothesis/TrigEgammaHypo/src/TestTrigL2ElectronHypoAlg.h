/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TESTTRIGL2ELECTRONHYPOALG_H
#define TRIGEGAMMAHYPO_TESTTRIGL2ELECTRONHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthViews/View.h"

#include "TrigL2ElectronHypoTool.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/HypoBase.h"


/**
 * @class Implements Hypo selection on L2 electrons
 * @brief 
 **/

class TestTrigL2ElectronHypoAlg  :  public ::HypoBase 
{ 
 public: 

  TestTrigL2ElectronHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestTrigL2ElectronHypoAlg(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r(const EventContext& context) const override;
  virtual StatusCode  finalize() override;

  
 
 private: 
  TestTrigL2ElectronHypoAlg();
  ToolHandleArray< TrigL2ElectronHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
  Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };
  // internally used to getch from views
  SG::ReadHandleKey< xAOD::TrigElectronContainer > m_electronsKey {this, "Electrons", "L2ElectronContainer", "Input"};

  
  bool  recursivelyFindFeature( const TrigCompositeUtils::Decision* d, ElementLink<xAOD::TrigEMClusterContainer>& clusterlink) const;
  

}; 

#endif //> !TRIGEGAMMAHYPO_TRIGL2ELECTRONHYPOALG_H
