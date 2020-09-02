/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONHYPOMT_TRIGMUONEFTRACKISOLATIONHYPOTOOL_H 
#define TRIGMUONHYPOMT_TRIGMUONEFTRACKISOLATIONHYPOTOOL_H 1

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "TrigCompositeUtils/TrigCompositeUtils.h" 
#include "xAODMuon/MuonContainer.h"

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


class StoreGateSvc;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuonEFTrackIsolationHypoTool: public ::AthAlgTool {	
  enum { MaxNumberTools = 20 }; 
  public:

    TrigMuonEFTrackIsolationHypoTool(const std::string& type, 
		                     const std::string & name,
		                     const IInterface* parent );

    virtual ~TrigMuonEFTrackIsolationHypoTool();

    struct EFIsolationMuonInfo {
    EFIsolationMuonInfo( TrigCompositeUtils::Decision* d, 
                         const TrigRoiDescriptor* r, 
                         const xAOD::Muon* f,
                         const TrigCompositeUtils::Decision* previousDecision,
			 const double cone20,
			 const double cone30 )
    : decision( d ), 
      roi( r ),
      muEFIso( f ),
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() ),
      ptcone20(cone20),
      ptcone30(cone30)
      {}
      
      TrigCompositeUtils::Decision* decision;
      const TrigRoiDescriptor* roi;
      const xAOD::Muon* muEFIso;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
      const double ptcone20;
      const double ptcone30;
    };

    virtual StatusCode initialize() override;    

    virtual StatusCode decide(std::vector<TrigMuonEFTrackIsolationHypoTool::EFIsolationMuonInfo>& toolInput) const;


  private:

    bool decideOnSingleObject(TrigMuonEFTrackIsolationHypoTool::EFIsolationMuonInfo& input, size_t cutIndex) const;

    // for single muon event
    StatusCode inclusiveSelection(std::vector<TrigMuonEFTrackIsolationHypoTool::EFIsolationMuonInfo>& toolInput) const;

    HLT::Identifier m_decisionId;

    Gaudi::Property<bool> m_acceptAll { this, "AcceptAll", true, "Ignore selection"};

    Gaudi::Property<float> m_ptcone02_cut { this, "PtCone02Cut", -1.0, "cut on the pt cone 0.2 variable"};

    Gaudi::Property<float> m_ptcone03_cut { this, "PtCone03Cut", -1.0, "cut on the pt cone 0.3 variable"};

    Gaudi::Property<bool> m_abscut { this, "DoAbsCut", true, "cut on absolute or sumpt/pt?"};

    Gaudi::Property<bool> m_useVarIso { this, "useVarIso", false, "use offline or online isolation variables?"};

    Gaudi::Property<bool> m_requireCombined { this, "RequireCombinedMuon", true, "only consider combined muons for isolation?"};
    
    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };   

};

#endif
