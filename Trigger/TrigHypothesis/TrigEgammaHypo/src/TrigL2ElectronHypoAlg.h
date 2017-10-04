/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2ELECTRONHYPOALG_H
#define TRIGEGAMMAHYPO_TRIGL2ELECTRONHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthViews/View.h"

#include "TrigL2ElectronHypoTool.h"

/**
 * @class Implements Hypo selection on L2 electrons
 * @brief 
 **/

class TrigL2ElectronHypoAlg
  : public ::AthReentrantAlgorithm
{ 
 public: 

  TrigL2ElectronHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigL2ElectronHypoAlg(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r(const EventContext& context) const override;

 
 private: 
  TrigL2ElectronHypoAlg();
  ToolHandleArray< TrigL2ElectronHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};


  SG::ReadHandleKey< std::vector<SG::View*> > m_views {this, "Views", "Unspecified", "Views to read electrons from" };
  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_decisionsKey {this, "ElectronDecisions", "ElectronDecisions", "Output decisions"};
  SG::ReadHandleKey< TrigCompositeUtils::DecisionContainer > m_clusterDecisionsKey {this, "ClusterDecisions", "L2ClusterContainer", "Decisions for clusters"};

  // internally used to getch from views
  SG::ReadHandleKey< xAOD::TrigElectronContainer > m_electronsKey {this, "Electrons", "L2ElectronContainer", "Input"};

}; 
DECLARE_ALGORITHM_FACTORY( TrigL2ElectronHypoAlg )

#endif //> !TRIGEGAMMAHYPO_TRIGL2ELECTRONHYPOALG_H
