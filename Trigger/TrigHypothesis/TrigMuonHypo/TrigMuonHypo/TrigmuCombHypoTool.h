/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUCOMBHYPO_TRIGMUCOMBHYPOTOOL_H 
#define TRIGMUCOMBHYPO_TRIGMUCOMBHYPOTOOL_H 1

#include <string>
#include "AthenaBaseComps/AthAlgTool.h" 
#include "DecisionHandling/HLTIdentifier.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/versions/L2CombinedMuonContainer_v1.h"
#include "xAODTrigMuon/L2CombinedMuon.h" 

#include "DecisionHandling/TrigCompositeUtils.h" 

#include "AthenaMonitoring/GenericMonitoringTool.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigmuCombHypoToolConsts {
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigmuCombHypoTool: public ::AthAlgTool {	
  enum { MaxNumberTools = 20 }; 
  public:

    TrigmuCombHypoTool(const std::string& type, 
		       const std::string & name,
		       const IInterface* parent );

    struct CombinedMuonInfo {
      TrigCompositeUtils::Decision* decision; 
      const xAOD::L2CombinedMuon* cluster;
      const xAOD::L2StandAloneMuon* muon;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };

    virtual ~TrigmuCombHypoTool();

    virtual StatusCode initialize() override;    
    
    virtual StatusCode decide(TrigmuCombHypoTool::CombinedMuonInfo& input) const;

    virtual StatusCode decide(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& hypoToolInput) const;
    
  private:

    HLT::Identifier m_decisionId;
    
    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };   

    // Properties:
    std::vector<float> m_ptBins;
    std::vector<float> m_ptThresholds;
   
    BooleanProperty m_acceptAll;
   
    // pi/K rejections flags
    BooleanProperty m_pikCuts;
    DoubleProperty  m_maxPtToApplyPik;
    DoubleProperty  m_chi2MaxID;
   
    // Apply strategy dependent pT cuts (B/T/S = 0 fields, L1 seeded, ...)
    BooleanProperty     m_strategydependent;
    DoubleArrayProperty m_strategyDependentPtCuts;
   
    // Other members:   
    std::vector<float>::size_type m_bins;
};

DECLARE_TOOL_FACTORY( TrigmuCombHypoTool )
#endif
