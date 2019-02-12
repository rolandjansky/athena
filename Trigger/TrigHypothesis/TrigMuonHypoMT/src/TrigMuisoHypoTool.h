/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUISOHYPO_TRIGMUISOHYPOTOOL_H 
#define TRIGMUISOHYPO_TRIGMUISOHYPOTOOL_H 1

#include <string>
#include "AthenaBaseComps/AthAlgTool.h" 
#include "DecisionHandling/HLTIdentifier.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h" 
#include "DecisionHandling/TrigCompositeUtils.h" 
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "xAODTrigMuon/L2IsoMuonContainer.h"

#include "DecisionHandling/Combinators.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuisoHypoTool: public ::AthAlgTool {	
  enum { MaxNumberTools = 20 }; 
  public:

    TrigMuisoHypoTool(const std::string& type, 
		      const std::string & name,
		      const IInterface* parent );

    virtual ~TrigMuisoHypoTool();

    struct MuisoInfo {
    MuisoInfo( TrigCompositeUtils::Decision* d,
               const xAOD::L2IsoMuon* f,
               const TrigCompositeUtils::Decision* previousDecision )
    : decision( d ),
      muIso( f ),
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(),
                           TrigCompositeUtils::decisionIDs( previousDecision ).end() )
      {}
      TrigCompositeUtils::Decision* decision;
      const xAOD::L2IsoMuon* muIso;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };


    virtual StatusCode initialize() override;    

    virtual StatusCode decide( std::vector<MuisoInfo>& toolInput ) const;

    bool decideOnSingleObject( TrigMuisoHypoTool::MuisoInfo& input,
                               size_t cutIndex ) const;

  private:

    HLT::Identifier m_decisionId;
    
    /** Accept all events flag. */
    Gaudi::Property< bool > m_acceptAll {
	this, "AcceptAll", false, "Ignore selection" };

    /** Max ID Isolation regions 1/2/3 */
    Gaudi::Property< float > m_MaxIDIso_1 { this, "MaxIDIso_1", 0.03, "" };
    Gaudi::Property< float > m_MaxIDIso_2 { this, "MaxIDIso_2", 0.03, "" };
    Gaudi::Property< float > m_MaxIDIso_3 { this, "MaxIDIso_3", 0.04, "" };

    /** Max Eta regions 1/2/3 */
    Gaudi::Property< float > m_EtaRegion_1 { this, "EtaRegion_1", 1.0, "" };
    Gaudi::Property< float > m_EtaRegion_2 { this, "EtaRegion_2", 1.4, "" };
    Gaudi::Property< float > m_EtaRegion_3 { this, "EtaRegion_3", 9.9, "" };

    /** Isolation cone sizes (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)  */
    Gaudi::Property< int > m_IDConeSize { this, "IDConeSize", 2, "" };

    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
};

#endif
