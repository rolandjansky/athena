/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigMuonTLAHypoAlg_H
#define TrigMuonTLAHypoAlg_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "DecisionHandling/HypoBase.h"


#include "TrigMuonEFHypoTool.h"
#include "xAODMuon/MuonContainer.h"


class TrigMuonTLAHypoAlg : public HypoBase {

  public:
    TrigMuonTLAHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual StatusCode initialize() override;
    virtual StatusCode execute( const EventContext& context ) const override;


  private:
    // tmp HypoTools
    ToolHandleArray<TrigMuonEFHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    
    SG::WriteHandleKey< xAOD::MuonContainer > m_TLAMuonsKey {
      this, "TLAOutputName", "Muons", "TLA Muon container key"
    };

};

#endif


