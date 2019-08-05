/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFQUALITYHYPO_H
#define TRIG_TRIGMUONEFQUALITYHYPO_H

// std lib includes
#include <string>

// include base class
#include "TrigInterfaces/HypoAlgo.h"

//#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/ToolHandle.h"

#include "MuonSelectorTools/IMuonSelectionTool.h"


class StoreGateSvc;
class TriggerElement;

/**
 * \brief Hypothesis class for EF muon calo isolation.
 * \author 
 * \details This class applies cuts on the variables calculated
 *          by the EF muon calorimeter isolation algorithm.
 *          
 *          
 */
class TrigMuonEFQualityHypo: public HLT::HypoAlgo {
    enum { MaxNumberTools = 20 };
public:
    /// Standard constructor
    TrigMuonEFQualityHypo(const std::string &name, ISvcLocator *pSvcLocator);
    /// Destructor
    ~TrigMuonEFQualityHypo();

    /// Initialize the algorithm
    HLT::ErrorCode hltInitialize();
    /// Execute the algorithm
    HLT::ErrorCode hltExecute(const HLT::TriggerElement *outputTE, bool &pass);
    /// Finalize the algorithm
    HLT::ErrorCode hltFinalize();

protected:
   /// Muon selection too
   ToolHandle<CP::IMuonSelectionTool> m_muonSelTool; 

private:

    // Properties:

    BooleanProperty m_acceptAll; // Accept all events if true

    BooleanProperty m_requireCombined;

    /** Cut counter. */
    int   m_cutCounter;
    /** Quality criteria */
    float m_reducedChi2;
    
    float m_isBadMuon;

};//class TrigMuonEFQuality

#endif //TRIG_TRIGMUONEFQUALITY_H
