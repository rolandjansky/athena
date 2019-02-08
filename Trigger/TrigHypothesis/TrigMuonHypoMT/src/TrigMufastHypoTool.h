/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUFASTHYPO_TRIGMUFASTHYPOTOOL_H 
#define TRIGMUFASTHYPO_TRIGMUFASTHYPOTOOL_H 1

#include <string>
#include "AthenaBaseComps/AthAlgTool.h" 
#include "DecisionHandling/HLTIdentifier.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "DecisionHandling/TrigCompositeUtils.h" 
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "DecisionHandling/Combinators.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigMufastHypoToolConsts {
enum ECRegions{ Bulk, WeakBFieldA, WeakBFieldB };
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
			   TrigCompositeUtils::decisionIDs( previousDecision ).end() )
      {}
      
      TrigCompositeUtils::Decision* decision;
      const TrigRoiDescriptor* roi;
      const xAOD::L2StandAloneMuon* muFast;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };

    virtual StatusCode initialize() override;    

    virtual StatusCode decide(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const;

    bool decideOnSingleObject(TrigMufastHypoTool::MuonClusterInfo& input, size_t cutIndex) const;

    // for single muon event
    StatusCode inclusiveSelection(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const;

    // for multipul muon event    
    StatusCode multiplicitySelection(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const;
    
  private:

    HLT::Identifier m_decisionId;
    
    float getLocalPhi(float, float, float) const;
    //TrigMufastHypoToolConsts::ECRegions whichECRegion(const float eta, const float phi) const;
    
    // Properties:
    Gaudi::Property< std::vector<std::vector<double>> > m_ptBins {
        this, "PtBins", { {0, 2.5} }, "Bins range of each pT threshold" };

    Gaudi::Property< std::vector<std::vector<double>> > m_ptThresholds {
        this, "PtThresholds", { {5.49*CLHEP::GeV} }, "Track pT requirement ( separate threshold for each muon )" };

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

    // Other members:   
    std::vector<size_t> m_bins = {0};

    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };   


};

#endif
