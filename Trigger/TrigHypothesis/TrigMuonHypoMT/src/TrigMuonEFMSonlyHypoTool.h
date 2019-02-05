/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "CLHEP/Units/SystemOfUnits.h"
class StoreGateSvc;
class TriggerElement;
class TrigMuonEFMSonlyHypoTool: public ::AthAlgTool {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuonEFMSonlyHypoTool(const std::string& type, const std::string & name, const IInterface* parent);
  ~TrigMuonEFMSonlyHypoTool();
 
  struct MuonEFInfo {
  MuonEFInfo( TrigCompositeUtils::Decision* d, 
              const TrigRoiDescriptor* r, 
              const xAOD::Muon* m,
	      const TrigCompositeUtils::Decision* previousDecision )
    : decision( d ), 
      roi( r ),
      muon( m ),
      previousDecisionIDs(TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			  TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
      
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const xAOD::Muon* muon;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  StatusCode initialize() override;    
  StatusCode decide(std::vector<TrigMuonEFMSonlyHypoTool::MuonEFInfo>& toolInput) const ;
  bool decideOnSingleObject(TrigMuonEFMSonlyHypoTool::MuonEFInfo& input, size_t cutIndex) const;
  StatusCode inclusiveSelection(std::vector<TrigMuonEFMSonlyHypoTool::MuonEFInfo>& toolInput) const;
  StatusCode multiplicitySelection(std::vector<TrigMuonEFMSonlyHypoTool::MuonEFInfo>& toolInput) const;
 private:
  HLT::Identifier m_decisionId;
  // Properties:
  Gaudi::Property< std::vector<std::vector<double>> > m_ptBins {
    this, "PtBins", { {0, 2.5} }, "Bins range of each pT threshold" };
  Gaudi::Property< std::vector<std::vector<double>> > m_ptThresholds {
    this, "PtThresholds", { {5.49*CLHEP::GeV} }, "Track pT requirement ( separate threshold for each muon )" };
  Gaudi::Property< bool > m_acceptAll {
    this, "AcceptAll", false, "Ignore selection" };
  Gaudi::Property<bool>  m_decisionPerRoI { 
    this, "DecisionPerRoI", true, "Is multiplicity requirement refering to muons ( false ) or RoIs with muons ( true ), relevant only in when multiplicity > 1" };
  // Other members:   
  std::vector<size_t> m_bins={0};
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
};
#endif
