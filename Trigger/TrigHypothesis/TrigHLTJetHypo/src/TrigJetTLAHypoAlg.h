/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

*/

#ifndef TrigHLTJetHypo_TrigJetTLAHypoAlg_H
#define TrigHLTJetHypo_TrigJetTLAHypoAlg_H

#include <string>

#include "TrigJetTLAHypoAlg.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODJet/JetContainer.h"

#include "DecisionHandling/HypoBase.h"

#include "TrigJetTLAHypoTool.h"
#include "TrigBtagTLATool.h"

/**
 * @class TrigJetTLAHypoAlg
 * @brief HypoAlg for TLA algorithm
 * @details HypoAlg needed to associate trigger navigation to jets seleected for writing out by TLA algorithm 
 * @author Antonio Boveia
 * @author Caterina Doglioni
 * @author Teng Jian Khoo
 * @author Tim Martin
 * @author Rafal Bielski
 **/

class TrigJetTLAHypoAlg : public ::HypoBase {
 public: 

  TrigJetTLAHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private:

  ToolHandleArray<TrigJetTLAHypoTool> m_hypoTools {
    this, "HypoTools", {}, "Hypo tools"};
     
  SG::WriteHandleKey< xAOD::JetContainer > m_TLAjetsKey {
    this, "TLAOutputName", "TLAOutputName", "TLA jet container key"};  

  Gaudi::Property<bool> m_attach_btag{this, "AttachBtag", true, "Use BtagJetTool to attach btag information to TLA jets."};
  ToolHandle<TrigBtagTLATool> m_btag_record_tool { this, "BtagJetTool", "BtagJetTool", "Btag jet tool"};
  
}; 

#endif
