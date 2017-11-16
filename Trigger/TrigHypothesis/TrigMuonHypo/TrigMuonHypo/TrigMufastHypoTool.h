/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUFASTHYPO_TRIGMUFASTHYPOTOOL_H 
#define TRIGMUFASTHYPO_TRIGMUFASTHYPOTOOL_H 1

#include <string>
#include "AthenaBaseComps/AthAlgTool.h" 

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

class TrigMufastHypoTool: virtual public ::AthAlgTool {	
  enum { MaxNumberTools = 20 };  
  public:

    TrigMufastHypoTool(const std::string& type, 
		       const std::string & name,
		       ISvcLocator* pSvcLocator);
    ~TrigMufastHypoTool();
    
    StatusCode initialize();    
    StatusCode decide(const xAOD::L2StandAloneMuonContainer* vectorOfMuons,
		      bool& pass);
    
  private:

    //HLT::Identifier m_decisionId;
    
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

//DECLARE_ALGORITHM_FACTORY( TrigMufastHypoTool )
#endif
