/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEFINVMASSHYPO_TRIGMUONEFINVMASSHYPOTOOL_H 
#define TRIGMUONEFINVMASSHYPO_TRIGMUONEFINVMASSHYPOTOOL_H 1
#include <string>
#include "AthenaBaseComps/AthAlgTool.h" 
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h" 
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "xAODMuon/MuonContainer.h"

class StoreGateSvc;
class TriggerElement;
class TrigMuonEFInvMassHypoTool: public ::AthAlgTool {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuonEFInvMassHypoTool(const std::string& type, const std::string & name, const IInterface* parent);
  ~TrigMuonEFInvMassHypoTool();
 
  struct MuonEFInfo {
  MuonEFInfo( TrigCompositeUtils::Decision* d, 
              const xAOD::Muon* m1,
              const xAOD::Muon* m2,
	      const TrigCompositeUtils::Decision* previousDecision1,
	      const TrigCompositeUtils::Decision* previousDecision2 )
    : decision( d ), 
      muon1( m1 ),
      muon2( m2 ),
      previousDecisionIDs1(TrigCompositeUtils::decisionIDs( previousDecision1 ).begin(), 
			  TrigCompositeUtils::decisionIDs( previousDecision1 ).end() ),
      previousDecisionIDs2(TrigCompositeUtils::decisionIDs( previousDecision2 ).begin(), 
			  TrigCompositeUtils::decisionIDs( previousDecision2 ).end() )
    {}
      
    TrigCompositeUtils::Decision* decision;
    const xAOD::Muon* muon1;
    const xAOD::Muon* muon2;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs1;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs2;
  };
  virtual StatusCode initialize() override;    
  StatusCode decide(std::vector<TrigMuonEFInvMassHypoTool::MuonEFInfo>& toolInput) const ;
 private:
  bool decideOnMassPair(TrigMuonEFInvMassHypoTool::MuonEFInfo& input) const;
  HLT::Identifier m_decisionId;
  // Properties:
  Gaudi::Property< double> m_invMassLow {
    this, "InvMassLow", -1.0, "Low threshold for invariant mass cut" };
  Gaudi::Property< double> m_invMassHigh {
    this, "InvMassHigh", -1.0, "High threshold for invariant mass cut" };
  Gaudi::Property< bool > m_acceptAll {
    this, "AcceptAll", false, "Ignore selection" };

  // Other members:   
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
};
#endif
