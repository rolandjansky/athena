///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Muon_p6.cxx 
// Implementation file for class Muon_p6
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 


// RecTPCnv includes
#include "RecTPCnv/Muon_p6.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

Muon_p6::Muon_p6()
  : m_author(0),
    m_hasCombinedMuon(0),
    m_hasInDetTrackParticle(0),
    m_hasMuonExtrapolatedTrackParticle(0),
    m_hasInnerExtrapolatedTrackParticle(0),
    m_hasCombinedMuonTrackParticle(0),
    m_hasCluster(0),
    m_matchChi2(0),
    m_bestMatch(0),
    m_matchNumberDoF(0),
    m_isAlsoFoundByLowPt(0),
    m_isAlsoFoundByCaloMuonId(0),
    m_caloMuonAlsoFoundByMuonReco(0),
    m_isCorrected(0),
    m_allAuthors(0),
    m_isMuonBits(0),
    m_isMuonLikelihood(0),
    m_outerMatchNumberDoF(0)
{}

