/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetails_p1.h
/// 
/// Declaration of persistent class
/// for Analysis::TauCommonDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess
/// @author Ilija Vukotic

#ifndef TAUEVENTTPCNV_TAUCOMMONDETAILS_P2_H
#define TAUEVENTTPCNV_TAUCOMMONDETAILS_P2_H

#include <vector>
#include "EventCommonTPCnv/HepLorentzVector_p1.h"
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

//Forward declarations
class TauCommonDetailsCnv_p2;

///Persistent class dor Analysis::TauCommonDetails
class TauCommonDetails_p2
{
    friend class TauCommonDetailsCnv_p2;
public:

    /// Make sure that members added to this class since the first version
    /// get initialized.  (Otherwise, when reading an old version, they'll
    /// be left uninitialized.)
    TauCommonDetails_p2();

    /// Destructor
   ~TauCommonDetails_p2() = default;

private:

    /// Vector containing all the data
    std::vector<float> m_allTheData;

    ///Loose track links
    ElementLinkIntVector_p1 m_looseTrk;

    ///Loose conversion track links
    ElementLinkIntVector_p1 m_looseConvTrk;

    ///SeedCalo wide tracks 
    ElementLinkIntVector_p1 m_seedCalo_wideTrk;

    ///SeedTrk wide tracks 
    ElementLinkIntVector_p1 m_seedTrk_wideTrk;

    ///Other tracks 
    ElementLinkIntVector_p1 m_otherTrk; 

    ///Pi0 cluster links
    ElementLinkIntVector_p1 m_pi0;

    ///EM calo clusters
    ElementLinkInt_p1 m_cellEM012Cluster;

    ///Pi0 sum vector
    HepLorentzVector_p1	m_sumPi0Vec;

    ///Secondary vertex
    TPObjRef m_secVtx;
};

#endif // TAUEVENTTPCNV_TAUCOMMONDETAILS_P2_H
