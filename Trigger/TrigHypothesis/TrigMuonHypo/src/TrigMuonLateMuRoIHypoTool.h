/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONLATEMUROIHYPO_TRIGMUONLATEMUROIHYPOTOOL_H 
#define TRIGMUONLATEMUROIHYPO_TRIGMUONLATEMUROIHYPOTOOL_H 1
#include <string>
#include "AthenaBaseComps/AthAlgTool.h" 
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h" 

class StoreGateSvc;
class TriggerElement;
class TrigMuonLateMuRoIHypoTool: public ::AthAlgTool {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuonLateMuRoIHypoTool(const std::string& type, const std::string & name, const IInterface* parent);
  ~TrigMuonLateMuRoIHypoTool();
 
  struct MuonEFInfo {
  MuonEFInfo( TrigCompositeUtils::Decision* d,
	      const TrigRoiDescriptor* r,
              const TrigRoiDescriptor* rl,
	      const TrigCompositeUtils::Decision* previousDecision)
  : decision( d ), 
      roi( r ),
      roiLate( rl ),
      previousDecisionIDs(TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			  TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
      
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const TrigRoiDescriptor* roiLate;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  virtual StatusCode initialize() override;    
  StatusCode decide(std::vector<TrigMuonLateMuRoIHypoTool::MuonEFInfo>& toolInput) const ;
 private:
  bool decideOnRoIs(TrigMuonLateMuRoIHypoTool::MuonEFInfo& input) const;
  HLT::Identifier m_decisionId;
  // Properties:
  Gaudi::Property< bool > m_acceptAll {
    this, "AcceptAll", false, "Ignore selection" };

};
#endif
