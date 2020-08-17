/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONHYPOMT_TRIGMUFASTHYPOTOOL_H 
#define TRIGMUONHYPOMT_TRIGMUFASTHYPOTOOL_H 1

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "TrigCompositeUtils/TrigCompositeUtils.h" 
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

class StoreGateSvc;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigMufastHypoToolConsts {
enum ECRegions{ Bulk, WeakBFieldA, WeakBFieldB };
const int  errorCode_cannot_get_EL         = 1;
const int  errorCode_EL_not_valid          = 2;
const int  errorCode_inconsistent_overlap1 = 3;
const int  errorCode_inconsistent_overlap2 = 4;
const int  errorCode_cannot_get_RoI        = 5;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMufastHypoTool: public ::AthAlgTool {	
  enum { MaxNumberTools = 20 }; 
  public:

    TrigMufastHypoTool(const std::string& type, 
		       const std::string & name,
		       const IInterface* parent );

    virtual ~TrigMufastHypoTool();

    struct MuonClusterInfo {
    MuonClusterInfo( TrigCompositeUtils::Decision* d, 
                     const TrigRoiDescriptor* r, 
                     const xAOD::L2StandAloneMuon* f,
  		     const TrigCompositeUtils::Decision* previousDecision )
    : decision( d ), 
      roi( r ),
      muFast( f ),
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() ),
      passOR( true )
      {}
      
      TrigCompositeUtils::Decision* decision;
      const TrigRoiDescriptor* roi;
      const xAOD::L2StandAloneMuon* muFast;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
      bool passOR;
    };

    virtual StatusCode initialize() override;    

    virtual StatusCode decide(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const;
    
  private:

    HLT::Identifier m_decisionId;
    
    bool decideOnSingleObject(TrigMufastHypoTool::MuonClusterInfo& input, size_t cutIndex) const;

    // for single muon event
    StatusCode inclusiveSelection(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const;

    // for multipul muon event    
    StatusCode multiplicitySelection(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const;

    StatusCode applyOverlapRemoval(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const;
    StatusCode checkOverlap(std::vector<TrigMufastHypoTool::MuonClusterInfo*>& input) const;
    bool isOverlap(const xAOD::L2StandAloneMuon *mf1, const xAOD::L2StandAloneMuon *mf2) const;
    double dR(double eta1, double phi1, double eta2, double phi2) const;
    double invMass(double m1, double pt1, double eta1, double phi1,
                   double m2, double pt2, double eta2, double phi2) const;
    StatusCode chooseBestMuon(std::vector<TrigMufastHypoTool::MuonClusterInfo*>& input, std::vector<unsigned int> mufastResult) const;

    float getLocalPhi(float, float, float) const;

    // Properties:
    Gaudi::Property< std::vector<std::vector<double>> > m_ptBins {
        this, "PtBins", { {0, 2.5} }, "Bins range of each pT threshold" };

    Gaudi::Property< std::vector<std::vector<double>> > m_ptThresholds {
        this, "PtThresholds", { {5.49*Gaudi::Units::GeV} }, "Track pT requirement ( separate threshold for each muon )" };

    Gaudi::Property< bool > m_acceptAll {
        this, "AcceptAll", false, "Ignore selection" };

    Gaudi::Property< bool > m_selectPV {
        this, "SelectPV", false, "Select PV" };

    Gaudi::Property< float > m_ZPV {
        this, "ZPVBins", 400. , "Define ZPV" };

    Gaudi::Property< float > m_RPV {
        this, "RPVBins", 200., "Define RPV" };
    
    Gaudi::Property< std::vector<double> > m_ptThresholdForECWeakBRegionA {
        this, "PtThresholdForECWeakBRegionA", {3.}, "Track pT requirement on regionA of weak magnetic field ( separate threshold for each muon )" };

    Gaudi::Property< std::vector<double> > m_ptThresholdForECWeakBRegionB {
        this, "PtThresholdForECWeakBRegionB", {3.}, "Track pT requirement on regionB of weak magnetic field ( separate threshold for each muon )" };

    Gaudi::Property<bool>  m_decisionPerCluster { 
	this, "DecisionPerCluster", true, "Is multiplicity requirement refering to muons ( false ) or RoIs/clusters with muons ( false ), relevant only in when multiplicity > 1" };

    Gaudi::Property< bool > m_doCalib {
        this, "DoCalib", false, "muoncalib chain" };

    // Members for overlap removal
    Gaudi::Property< bool > m_applyOR {
        this, "ApplyOR", false, "apply overlap removal for mufast" };

    Gaudi::Property<bool> m_requireDR{
        this, "RequireDR", true, "require or not DR cut for overlap removal"};

    Gaudi::Property<bool> m_requireMass{
        this, "RequireMass", true, "require or not mass cut for overlap removal"};

    Gaudi::Property<bool> m_requireSameSign{
        this, "RequireSameSign", true, "require or not charge cut for overlap removal"};

    Gaudi::Property< float > m_dRThresBB {
        this, "DRThresBB", 0.05, "DR threshold in barel and barel region"};

    Gaudi::Property< float > m_massThresBB {
        this, "MassThresBB", 0.20, "mass threshold in barel and barel region"};

    Gaudi::Property< float > m_dRThresBE {
        this, "DRThresBE", 0.05, "DR threshold in barel and barel region"};

    Gaudi::Property< float > m_massThresBE {
        this, "MassThresBE", 0.20, "mass threshold in barel and endcap region"};

    Gaudi::Property< std::vector<float> > m_etaBinsEC {
        this, "EtaBinsEC", {0, 1.9, 2.1, 9.9}, "eta bins of DR and mass thresholds in endcap and barel region"};

    Gaudi::Property< std::vector<float> > m_dRThresEC {
        this, "DRThresEC", {0.06, 0.05, 0.05}, "DR threshold in endcap and barel region"};

    Gaudi::Property< std::vector<float> > m_massThresEC {
        this, "MassThresEC", {0.20, 0.15, 0.10}, "mass threshold in endcap and endcap region"};


    // Other members:   
    std::vector<size_t> m_bins = {0};

    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };   


};

#endif
