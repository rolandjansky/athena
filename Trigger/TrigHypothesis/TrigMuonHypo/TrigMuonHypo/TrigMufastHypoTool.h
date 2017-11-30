/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUFASTHYPO_TRIGMUFASTHYPOTOOL_H 
#define TRIGMUFASTHYPO_TRIGMUFASTHYPOTOOL_H 1

#include <string>
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h" 
#include "DecisionHandling/HLTIdentifier.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "DecisionHandling/TrigCompositeUtils.h" 
#include "AthenaMonitoring/GenericMonitoringTool.h"

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
    MuonClusterInfo( TrigCompositeUtils::Decision* d, const TrigRoiDescriptor* r, const xAOD::L2StandAloneMuon* c,
  		     const TrigCompositeUtils::Decision* previousDecision )
    : decision( d ), 
      roi( r ), 
      cluster( c ),
      previousDecisionIDs(TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			  TrigCompositeUtils::decisionIDs( previousDecision ).end() )
      {}
      
      TrigCompositeUtils::Decision* decision;
      const TrigRoiDescriptor* roi;
      const xAOD::L2StandAloneMuon* cluster;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };

    virtual StatusCode initialize() override;    

    virtual StatusCode decide(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const;
    virtual StatusCode decide(TrigMufastHypoTool::MuonClusterInfo& input) const;
    
  private:

    HLT::Identifier m_decisionId;
    
    float getLocalPhi(float, float, float) const;
    TrigMufastHypoToolConsts::ECRegions whichECRegion(const float eta, const float phi) const;
    
    // Properties:
    std::vector<float> m_ptBins;
    std::vector<float> m_ptThresholds;
    BooleanProperty m_acceptAll;
    BooleanProperty m_selectPV;
    float m_Z_PV;
    float m_R_PV;
    
    float m_ptThresholdForECWeakBRegionA;
    float m_ptThresholdForECWeakBRegionB;

    // Other members:   
    std::vector<float>::size_type m_bins;

    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };   
};

DECLARE_TOOL_FACTORY( TrigMufastHypoTool )
#endif
