/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "DecisionHandling/Combinators.h"

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
    CombinedMuonInfo( TrigCompositeUtils::Decision* d, 
                      const xAOD::L2CombinedMuon* c,
                      const xAOD::L2StandAloneMuon* f,
                      const TrigCompositeUtils::Decision* previousDecision )
    : decision( d ),
      muComb( c ),
      muFast( f ),
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(),
                           TrigCompositeUtils::decisionIDs( previousDecision ).end() )
      {}

      TrigCompositeUtils::Decision* decision;
      const xAOD::L2CombinedMuon* muComb;
      const xAOD::L2StandAloneMuon* muFast;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };

    virtual ~TrigmuCombHypoTool();

    virtual StatusCode initialize() override;    

    virtual StatusCode decide(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& toolInput) const;
    
    bool decideOnSingleObject(TrigmuCombHypoTool::CombinedMuonInfo& input, size_t cutIndex) const;

    StatusCode inclusiveSelection(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& input) const;

    StatusCode multiplicitySelection(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& input) const;
   
  private:

    HLT::Identifier m_decisionId;
    
    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };   

    Gaudi::Property< std::vector<std::vector<double>> > m_ptBins {
	this, "PtBins", { {0, 2.5} }, "Bins range of each pT threshold" };

    Gaudi::Property< std::vector<std::vector<double>> > m_ptThresholds {
	this, "PtThresholds", { {5.83 * CLHEP::GeV} }, "Track pT requirement ( separate threshold for each muon )" };

    Gaudi::Property< bool > m_acceptAll {
	this, "AcceptAll", false, "Ignore selection" };     

    // Enable pi/k decay rejection
    Gaudi::Property< bool > m_pikCuts {
	this, "ApplyPikCuts", false, "Enable pi/k decay rejection" };

    Gaudi::Property< double > m_maxPtToApplyPik {
	this, "MaxPtToApplyPik", 25., "" }; 

    Gaudi::Property< double > m_chi2MaxID {
	this, "MaxChi2IDPik", 3.5, "" };

    // Apply strategy dependent pT cuts (B/T/S = 0 fields, L1 seeded, ...)
    // strategy dependent pT cuts
    //0: Std
    //1: simplified match Pt combined
    //2: simplified match Pt ID
    //3: simplified match Pt MU
    //4: simplified match Pt infinite 
    Gaudi::Property< bool > m_strategydependent {
	this, "ApplyStrategyDependentCuts", false, "" };

    Gaudi::Property< std::vector<double> > m_strategyDependentPtCuts {
	this, "StrategyDependentPtCuts",{ 0.0, 0.0, 0.0, -3.0 }, "" };
    // { strategy 1 (in GeV) (if<0 do not apply the standard signature cut), 
    //   strategy 2 (in GeV),
    //   strategy 3 (in GeV),
    //   strategy 4 (in GeV) }

    Gaudi::Property<bool>  m_decisionPerCluster { 
         this, "DecisionPerCluster", true, "Is multiplicity requirement refering to muons ( false ) or RoIs/clusters with muons ( false ), relevant only in when multiplicity > 1" };


    // Other members:   
    std::vector<size_t> m_bins = {0};
};

#endif
