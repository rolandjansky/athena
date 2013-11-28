/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackFindingEvent/MuPatCandidateBase.h"


namespace Muon {

  MuPatCandidateBase::MuPatCandidateBase() : 
    m_hasMomentum(false),nmdtHitsMl1(0),nmdtHitsMl2(0),ncscHitsEta(0),ncscHitsPhi(0),nrpcHitsEta(0),
    nrpcHitsPhi(0),ntgcHitsEta(0),ntgcHitsPhi(0),
    m_hasEndcap(false), m_hasSmallChamber(false), m_hasLargeChamber(false), m_hasSLOverlap(false)
  {
    
  }

  MuPatCandidateBase::MuPatCandidateBase( const MeasVec& etaHits, const MeasVec& phiHits, const MeasVec& fakePhiHits, const MeasVec& allHits ) :
    m_etaHits(etaHits),m_phiHits(phiHits),m_fakePhiHits(fakePhiHits),m_allHits(allHits),m_hasMomentum(false),
    nmdtHitsMl1(0),nmdtHitsMl2(0),ncscHitsEta(0),ncscHitsPhi(0),nrpcHitsEta(0),
    nrpcHitsPhi(0),ntgcHitsEta(0),ntgcHitsPhi(0),
    m_hasEndcap(false), m_hasSmallChamber(false), m_hasLargeChamber(false), m_hasSLOverlap(false)
  {    
  }

  MuPatCandidateBase::~MuPatCandidateBase() {
  }

  void MuPatCandidateBase::setChambers( const std::set<MuonStationIndex::ChIndex>& chambers ) {
    m_chambers = chambers;
    m_stations.clear();
    chamber_iterator it = chambers.begin(), it_end = chambers.end();
    for ( ; it != it_end; ++it ) m_stations.insert( MuonStationIndex::toStationIndex(*it) );
  }

  bool MuPatCandidateBase::shareChambers( const MuPatCandidateBase& entry ) const {
    std::vector<Identifier> intersection;
    std::set_intersection(entry.chamberIds().begin(),entry.chamberIds().end(),
			  chamberIds().begin(),chamberIds().end(),
			  std::back_inserter(intersection));

    unsigned int intersectionSize = intersection.size();
    if( intersectionSize == 0 ) return false;

    return true;
  }


}
