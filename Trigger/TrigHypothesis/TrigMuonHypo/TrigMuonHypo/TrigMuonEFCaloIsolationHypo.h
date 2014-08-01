/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFCALOISOLATIONHYPO_H
#define TRIG_TRIGMUONEFCALOISOLATIONHYPO_H

// std lib includes
#include <string>
#include <vector>
// include base class
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

/**
 * \brief Hypothesis class for EF muon calo isolation.
 * \author Mark Owen (markowen@cern.ch)
 * \details This class applies cuts on the variables calculated
 *          by the EF muon track isolation algorithm.\n
 *          The cuts can be applied to the sum pT in 0.2 cone, 0.3 cone
 *          or both.
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

    BooleanProperty m_UseCalo; // Use iso based on Calo

    BooleanProperty m_UseAbsCalo; // Use absolute Calo iso

    BooleanProperty m_UseIDptForMu; // Use IDmeasured muon pt

    // Max Et within calo Eta
    FloatProperty m_MaxCaloIso_1;
    FloatProperty m_MaxCaloIso_2;
    FloatProperty m_MaxCaloIso_3;

    // Max Pt withon ID Eta
    FloatProperty m_MaxIDIso_1;
    FloatProperty m_MaxIDIso_2;
    FloatProperty m_MaxIDIso_3;

    // Eta regions
    FloatProperty m_EtaRegion_1;
    FloatProperty m_EtaRegion_2;
    FloatProperty m_EtaRegion_3;

    /// cut on the pt cone 0.2 variable
    // FloatProperty m_ptcone02_cut;

    /// cut on the pt cone 0.3 variable
    // FloatProperty m_ptcone03_cut;

    BooleanProperty m_abscut; // Cut on absolute (if true) or sumpt/pt (if false)

    /// monitor the variables we cut on
    std::vector<float> m_fex_ptcone02;
    std::vector<float> m_fex_ptcone03;

    /** Cut counter. */
    int   m_cutCounter;
    /** Transverse Energy sum calo */
    float m_SumEtCone;
    /** Transverse Momentum sum ID */
    float m_SumPtCone;

};//class TrigMuonEFTrackIsolation

#endif //TRIG_TRIGMUONEFTRACKISOLATIONHYPO_H
