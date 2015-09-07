/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFCALOISOLATIONHYPO_H
#define TRIG_TRIGMUONEFCALOISOLATIONHYPO_H

// std lib includes
#include <string>

// include base class
#include "TrigInterfaces/HypoAlgo.h"

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
class TrigMuonEFCaloIsolationHypo: public HLT::HypoAlgo {
    enum { MaxNumberTools = 20 };
public:
    /// Standard constructor
    TrigMuonEFCaloIsolationHypo(const std::string &name, ISvcLocator *pSvcLocator);
    /// Destructor
    ~TrigMuonEFCaloIsolationHypo();

    /// Initialize the algorithm
    HLT::ErrorCode hltInitialize();
    /// Execute the algorithm
    HLT::ErrorCode hltExecute(const HLT::TriggerElement *outputTE, bool &pass);
    /// Finalize the algorithm
    HLT::ErrorCode hltFinalize();

private:

    // Properties:

    BooleanProperty m_acceptAll; // Accept all events if true

    FloatProperty m_CaloConeSize; // Calorimetric cone size (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)

    // Max Et within calo Eta
    FloatProperty m_MaxCaloIso_1;
    FloatProperty m_MaxCaloIso_2;
    FloatProperty m_MaxCaloIso_3;

    // Eta regions
    FloatProperty m_EtaRegion_1;
    FloatProperty m_EtaRegion_2;
    FloatProperty m_EtaRegion_3;

    BooleanProperty m_abscut; // Cut on absolute (if true) or sumet/pt (if false)

    /** Cut counter. */
    int   m_cutCounter;
    /** Transverse Energy sum calo */
    float m_SumEtCone;

};//class TrigMuonEFTrackIsolation

#endif //TRIG_TRIGMUONEFTRACKISOLATIONHYPO_H
