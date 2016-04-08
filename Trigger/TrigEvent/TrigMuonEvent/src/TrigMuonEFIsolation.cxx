/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include <cmath>

// "Distance" used by the comparison operator(s):
static const double DELTA = 0.001;

/**
 * Default constructor - set all isolation values to -1.
 */
TrigMuonEFIsolation::TrigMuonEFIsolation() :
  m_sumTrkPtCone02(-1.0),
  m_sumTrkPtCone03(-1.0),
  m_sumEtCone01(-1.0),
  m_sumEtCone02(-1.0),
  m_sumEtCone03(-1.0),
  m_sumEtCone04(-1.0),
  m_trkPos(-1),
  m_muonEFLink()
{

}

/**
 * Constructor to fill the track isolation in dR cone of 0.2 & 0.3
 */
TrigMuonEFIsolation::TrigMuonEFIsolation(const float trkptcone02, const float trkptcone03) :
  m_sumTrkPtCone02(trkptcone02),
  m_sumTrkPtCone03(trkptcone03),
  m_sumEtCone01(-1.0),
  m_sumEtCone02(-1.0),
  m_sumEtCone03(-1.0),
  m_sumEtCone04(-1.0),
  m_trkPos(-1),
  m_muonEFLink() {

}

TrigMuonEFIsolation::TrigMuonEFIsolation(const TrigMuonEFIsolation& iso) :
  m_sumTrkPtCone02( iso.m_sumTrkPtCone02 ),
  m_sumTrkPtCone03( iso.m_sumTrkPtCone03 ),
  m_sumEtCone01(iso.m_sumEtCone01 ),
  m_sumEtCone02(iso.m_sumEtCone02 ),
  m_sumEtCone03(iso.m_sumEtCone03 ),
  m_sumEtCone04(iso.m_sumEtCone04 ),
  m_trkPos( iso.m_trkPos),
  m_muonEFLink( iso.m_muonEFLink ) {

}

TrigMuonEFIsolation::TrigMuonEFIsolation(const TrigMuonEFIsolation* iso) :
  m_sumTrkPtCone02( -1.0 ),
  m_sumTrkPtCone03( -1.0 ),
  m_sumEtCone01(-1.0),
  m_sumEtCone02(-1.0),
  m_sumEtCone03(-1.0),
  m_sumEtCone04(-1.0),
  m_trkPos( -1 ),
  m_muonEFLink() {

  if(iso) {
    m_sumTrkPtCone02 = iso->m_sumTrkPtCone02;
    m_sumTrkPtCone03 = iso->m_sumTrkPtCone03;
    m_sumEtCone01 = iso->m_sumEtCone01;
    m_sumEtCone02 = iso->m_sumEtCone02;
    m_sumEtCone03 = iso->m_sumEtCone03;
    m_sumEtCone04 = iso->m_sumEtCone04;
    m_trkPos = iso->m_trkPos;
    m_muonEFLink = iso->m_muonEFLink;
  }
}

TrigMuonEFIsolation& TrigMuonEFIsolation::operator=(const TrigMuonEFIsolation& rhs) {
  if( this != &rhs ) {
    m_sumTrkPtCone02 = rhs.m_sumTrkPtCone02;
    m_sumTrkPtCone03 = rhs.m_sumTrkPtCone03;
    m_sumEtCone01 = rhs.m_sumEtCone01;
    m_sumEtCone02 = rhs.m_sumEtCone02;
    m_sumEtCone03 = rhs.m_sumEtCone03;
    m_sumEtCone04 = rhs.m_sumEtCone04;
    m_trkPos = rhs.m_trkPos;
    m_muonEFLink = rhs.m_muonEFLink;
  }
  return *this;
}

/**
 * Set the link to the muon we calculated the isolation for.
 * @parameter infocont - container holding the EF muon Info object
 * @parameter itrk  - position of the EF info object within the container
 */
void TrigMuonEFIsolation::setEFMuonInfoLink(const TrigMuonEFInfoContainer* infocont, const int pos) {
  m_muonEFLink.reset();
  m_muonEFLink.toIndexedElement(*infocont, pos);
}

/**
 * Set the link to the muon we calculated the isolation for.
 * @parameter itrk  - position of the EF muon info track object within the EF muon info object
 */
void TrigMuonEFIsolation::setEFMuonInfoTrackLink(const int itrk) {
  m_trkPos = itrk;
}

/**
 * Set the element link to the TrigMuonEFInfo object we are using.
 * @parameter link - the ElementLink object to the TrigMuonEFInfo object used.
 */
void TrigMuonEFIsolation::setEFMuonInfoElementLink(const ElementLink<TrigMuonEFInfoContainer> link) {
  
  m_muonEFLink = link;

}



const TrigMuonEFInfo* TrigMuonEFIsolation::getMuonInfo() const {
  if(!m_muonEFLink.isValid()) return 0;
  return *m_muonEFLink;
}

const ElementLink<TrigMuonEFInfoContainer>& TrigMuonEFIsolation::getMuonInfoLink() const {
  return m_muonEFLink;
}

const TrigMuonEFInfoTrack* TrigMuonEFIsolation::getEFMuonInfoTrack() const {

  const TrigMuonEFInfo* info = getMuonInfo();
  if(!info) return 0;
  if(!info->TrackContainer()) {
    std::cerr << "WARNING: TrigMuonEFInfo object has no TrackContainer" << std::endl;
    return 0;
  }
  if(m_trkPos >= ((int)info->TrackContainer()->size()) || m_trkPos < 0) {
    std::cerr << "WARNING TrigMuonEFInfo object has " << info->TrackContainer()->size() << " but tried to get InfoTrack " << m_trkPos << std::endl;
    return 0;
  }

  return (*info->TrackContainer())[m_trkPos];
}

/**
 * This operator checks whether two TrigMuonEFIsolation objects are the same based
 * on the private members of the class.
 */
bool operator== ( const TrigMuonEFIsolation& a, const TrigMuonEFIsolation& b) {
  if( std::abs( a.sumTrkPtCone02() - b.sumTrkPtCone02() ) > DELTA )
    return false;
  if( std::abs( a.sumTrkPtCone03() - b.sumTrkPtCone03() ) > DELTA )
    return false;
  if( std::abs( a.sumEtCone01() - b.sumEtCone01() ) > DELTA )
    return false;
  if( std::abs( a.sumEtCone02() - b.sumEtCone02() ) > DELTA )
    return false;
  if( std::abs( a.sumEtCone03() - b.sumEtCone03() ) > DELTA )
    return false;
  if( std::abs( a.sumEtCone04() - b.sumEtCone04() ) > DELTA )
    return false;
  if( a.trackPosition() != b.trackPosition() )
    return false;
  if( a.getEFMuonInfoTrack() != b.getEFMuonInfoTrack() )
    return false;

  return true;
}
