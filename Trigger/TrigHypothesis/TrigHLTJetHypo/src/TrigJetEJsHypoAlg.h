/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

*/

#ifndef TrigHLTJetHypo_TrigJetEJsHypoAlg_H
#define TrigHLTJetHypo_TrigJetEJsHypoAlg_H

#include <string>

#include "TrigJetEJsHypoAlg.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"

#include "DecisionHandling/HypoBase.h"

#include "TrigJetEJsHypoTool.h"

/**
 * @class TrigJetEJsHypoAlg
 * @brief HypoAlg for exotic jets / emerging jets algorithm
 * @details HypoAlg needed to associate trigger navigation to jets selected for writing out by exotic jets algorithms
 * @author Aaron Kilgallon
 * @author Antonio Boveia
 * @author Caterina Doglioni
 * @author Teng Jian Khoo
 * @author Tim Martin
 * @author Rafal Bielski
 **/

class TrigJetEJsHypoAlg : public ::HypoBase {
 public: 

  TrigJetEJsHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private:

  ToolHandleArray<TrigJetEJsHypoTool> m_hypoTools {
    this, "HypoTools", {}, "Hypo tools"};

  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_tracksKey 
    {this,"Tracks","Key for precision tracks"};
  SG::ReadHandleKey< xAOD::VertexContainer > m_PV
    {this,"PV","Key for Primary Vertex"};
  
}; 

#endif
