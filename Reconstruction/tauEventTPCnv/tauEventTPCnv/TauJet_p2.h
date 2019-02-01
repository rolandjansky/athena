/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJet_p2.h 
// author: Ilija Vukotic
// date:   April 2007
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJET_P2_H 
#define tauEventTPCnv_TAUJET_P2_H

#include <vector>
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMFloat_p2.h"
#include "ParticleEventTPCnv/ParticleBase_p1.h"

class TauJetCnv_p2;
class TauJetContainer_p1_old_cnv;

class TauJet_p2
{
    friend class TauJetCnv_p2;
    friend class TauJetContainer_p1_old_cnv;
    public:
        TauJet_p2()
          : m_flags( 0 ),
            m_vetoFlags(0),
            m_numberOfTracks(0),
            m_roiWord(0)
         {};
        ~TauJet_p2() = default;

    private:
        P4EEtaPhiMFloat_p2      m_momentum;
        ParticleBase_p1         m_particleBase;
        ElementLinkInt_p1       m_cluster;
        ElementLinkInt_p1       m_cellCluster;
        ElementLinkInt_p1       m_jet;
        ElementLinkIntVector_p1 m_tracks;
        ElementLinkIntVector_p1 m_tauDetails;
        unsigned char           m_flags;                    // 0 bit - has TauPID object
                                                            // 1 bit - is Tau
                                                            // 2 bit - author TauRec
                                                            // 3 bit - author Tau1P3P
        unsigned long           m_vetoFlags;
        unsigned long           m_numberOfTracks;
        unsigned int                m_roiWord;  // requested by trigger
        std :: vector<std :: pair<int, double> > m_params;
}; 

#endif //> tauEventTPCnv_TAUJET_P2_H
