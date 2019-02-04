/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauJet_p4.h
///
/// Declaration of persistent class
/// for Analysis::TauJet v4
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Noel Dawe
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created by Lukasz Janyst in May 2008
///
/// $Id: TauJet_p4.h,v 1.5 2009-05-08 14:32:44 tburgess Exp $

#ifndef tauEventTPCnv_TAUJET_P4_H
#define tauEventTPCnv_TAUJET_P4_H

#include <vector>
#include "DataModelAthenaPool/ElementLink_p3.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMFloat_p2.h"
#include "ParticleEventTPCnv/ParticleBase_p2.h"

class TauJetCnv_p4;
class TauJetContainer_p1_old_cnv;

class TauJet_p4
{
    friend class TauJetCnv_p4;
    friend class TauJetContainer_p1_old_cnv;
public:
    TauJet_p4():
	m_flags( 0 ),
	m_vetoFlags( 0 ),
	m_isTauFlags( 0 ),
	m_numberOfTracks(0),
	m_roiWord(0),
	m_params()
	{
	    
	}

    ~TauJet_p4() = default;

private:
    P4EEtaPhiMFloat_p2  m_momentum;
    ParticleBase_p2 m_particleBase;
    ElementLinkInt_p3 m_cluster;
    ElementLinkInt_p3 m_cellCluster;
    ElementLinkInt_p3 m_jet;
    ElementLinkIntVector_p1 m_tracks;
    ElementLinkIntVector_p1 m_tauDetails;
    unsigned char m_flags;  // 0 bit - has TauPID object
    // 1 bit - is Tau // obsolete
    // 2 bit - author TauRec
    // 3 bit - author Tau1P3P
    unsigned long m_vetoFlags;
    unsigned long m_isTauFlags;
    unsigned long m_numberOfTracks;
    unsigned int  m_roiWord;  // requested by trigger
    std :: vector<std :: pair<int, float> > m_params;
    ElementLinkIntVector_p1 m_conversionTracks;
    ElementLinkIntVector_p1 m_seedCalo_tracks;
    ElementLinkIntVector_p1 m_seedTrk_tracks;
};

#endif //> tauEventTPCnv_TAUJET_P4_H
