/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONHYPOMT_TRIGMUCOMBHYPOTOOL_H 
#define TRIGMUONHYPOMT_TRIGMUCOMBHYPOTOOL_H 1

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

class StoreGateSvc;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigmuCombHypoToolConsts {
const int  errorCode_cannot_get_EL         = 1;
const int  errorCode_EL_not_valid          = 2;
const int  errorCode_inconsistent_overlap1 = 3;
const int  errorCode_inconsistent_overlap2 = 4;
const int  errorCode_cannot_get_RoI        = 5;
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
                           TrigCompositeUtils::decisionIDs( previousDecision ).end() ),
      passOR( true )
      {}

      TrigCompositeUtils::Decision* decision;
      const xAOD::L2CombinedMuon* muComb;
      const xAOD::L2StandAloneMuon* muFast;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
      bool passOR;
    };

    virtual ~TrigmuCombHypoTool();

    virtual StatusCode initialize() override;    

    virtual StatusCode decide(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& toolInput) const;
    
  private:

    bool decideOnSingleObject(TrigmuCombHypoTool::CombinedMuonInfo& input, size_t cutIndex) const;

    StatusCode inclusiveSelection(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& input) const;

    StatusCode multiplicitySelection(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& input) const;
   
    StatusCode applyOverlapRemoval(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& toolInput) const;
    StatusCode checkOverlap(std::vector<TrigmuCombHypoTool::CombinedMuonInfo*>& input) const;
    bool isOverlap(const xAOD::L2CombinedMuon *mf1, const xAOD::L2CombinedMuon *mf2) const;
    double dR(double eta1, double phi1, double eta2, double phi2) const;
    double invMass(double m1, double pt1, double eta1, double phi1,
                   double m2, double pt2, double eta2, double phi2) const;
    StatusCode chooseBestMuon(std::vector<TrigmuCombHypoTool::CombinedMuonInfo*>& input, std::vector<unsigned int> mucombResult) const;

    HLT::Identifier m_decisionId;
    
    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };   

    Gaudi::Property< std::vector<std::vector<double>> > m_ptBins {
	this, "PtBins", { {0, 2.5} }, "Bins range of each pT threshold" };

    Gaudi::Property< std::vector<std::vector<double>> > m_ptThresholds {
    this, "PtThresholds", { {5.83 * Gaudi::Units::GeV} }, "Track pT requirement ( separate threshold for each muon )" };

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


    // Members for overlap removal
    Gaudi::Property< bool > m_applyOR {
      this, "ApplyOR", false, "apply overlap removal for mucomb" };

    Gaudi::Property<bool> m_requireDR{
        this, "RequireDR", true, "require or not DR cut for overlap removal"};

    Gaudi::Property<bool> m_requireMufastDR{
        this, "RequireMufastDR", true, "require or not mufast DR cut for overlap removal"};

    Gaudi::Property<bool> m_requireMass{
        this, "RequireMass", true, "require or not mass cut for overlap removal"};

    Gaudi::Property<bool> m_requireSameSign{
        this, "RequireSameSign", true, "require or not charge cut for overlap removal"};

    Gaudi::Property< std::vector<float> > m_etaBins {
        this, "EtaBins", {0, 1.9, 2.1, 9.9}, "eta bins of DR and mass thresholds"};

    Gaudi::Property< std::vector<float> > m_dRThres {
        this, "DRThres", {0.06, 0.05, 0.05}, "DR threshold"};

    Gaudi::Property< std::vector<float> > m_mufastDRThres {
        this, "MufastDRThres", {0.06, 0.05, 0.05}, "mufast DR threshold"};

    Gaudi::Property< std::vector<float> > m_massThres {
        this, "MassThres", {0.20, 0.15, 0.10}, "mass threshold"};


    // Other members:   
    std::vector<size_t> m_bins = {0};
};

#endif
