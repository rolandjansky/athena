/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

*/

#ifndef TrigHLTJetHypo_TrigJetCRHypoAlg_H
#define TrigHLTJetHypo_TrigJetCRHypoAlg_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"

#include "DecisionHandling/HypoBase.h"

#include "TrigJetCRHypoTool.h"

/**
 * @class TrigJetCRHypoAlg
 * @brief HypoAlg for low-EMF trackless jets algorithm
 * @details HypoAlg needed to associate trigger navigation to jets selected for writing out by exotic jets algorithms
 * @author
 **/

class TrigJetCRHypoAlg : public ::HypoBase {
 public: 

  TrigJetCRHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private:
  ToolHandleArray< TrigJetCRHypoTool > m_hypoTools {this, 
      "HypoTools", 
	{}, 
      "Tools to perfrom selection"};
  
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleKey {this, 
      "Tracks","Key for precision tracks"};

  SG::ReadHandleKey<CaloConstCellContainer> m_cellKey {this,
      "Cells","Key for input CaloCellContainer"};

}; 

#endif
