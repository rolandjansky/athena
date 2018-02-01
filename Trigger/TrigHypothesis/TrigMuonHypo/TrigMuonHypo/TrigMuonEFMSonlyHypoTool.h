/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEFMSONLYHYPO_TRIGMUONEFMSONLYHYPOTOOL_H 
#define TRIGMUONEFMSONLYHYPO_TRIGMUONEFMSONLYHYPOTOOL_H 1

#include <string>
#include "AthenaBaseComps/AthAlgTool.h" 
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h" 
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "xAODMuon/MuonContainer.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonEFMSonlyHypoTool: public ::AthAlgTool {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuonEFMSonlyHypoTool(const std::string& type, const std::string & name, const IInterface* parent);
  ~TrigMuonEFMSonlyHypoTool();
 
  struct MuonEFInfo {
  MuonEFInfo( TrigCompositeUtils::Decision* d, const TrigRoiDescriptor* r, const xAOD::MuonContainer* m,
	      const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
      roi( r ),
      muons( m ),
      previousDecisionIDs(TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			  TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
      
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const xAOD::MuonContainer* muons;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };

  StatusCode initialize() override;    
  StatusCode decide(std::vector<TrigMuonEFMSonlyHypoTool::MuonEFInfo>& toolInput) const ;
  StatusCode decide(TrigMuonEFMSonlyHypoTool::MuonEFInfo& input) const;

 private:

  HLT::Identifier m_decisionId;

  // Properties:
  std::vector<float> m_ptBins;
  std::vector<float> m_ptThresholds; 
  BooleanProperty m_acceptAll;

  // Other members:   
  std::vector<float>::size_type m_bins;

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
};

DECLARE_TOOL_FACTORY(TrigMuonEFMSonlyHypoTool)
#endif
