/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFQUALITYHYPO_H
#define TRIG_TRIGMUONEFQUALITYHYPO_H

// std lib includes
#include <string>

// include base class
#include "TrigInterfaces/HypoAlgo.h"

// include Muon SelectionTool for quality criteria
#include "MuonSelectorTools/IMuonSelectionTool.h"


class StoreGateSvc;
class TriggerElement;

/**
 * \brief Hypothesis class for EF muon quality criteria.
 * \author 
 * \details This class applies cuts on quality criteria
 *          to the EF muons based on the offline muon WP algo.
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

    BooleanProperty m_looseCut;

};//class TrigMuonEFQuality

#endif //TRIG_TRIGMUONEFQUALITY_H
