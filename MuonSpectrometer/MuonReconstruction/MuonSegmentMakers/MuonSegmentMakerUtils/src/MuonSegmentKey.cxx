/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentMakerUtils/MuonSegmentKey.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"



Muon::MuonSegmentKey::MuonSegmentKey(){

}

Muon::MuonSegmentKey::MuonSegmentKey( const Muon::MuonSegment& segment)
{

    // to ensure consistent usage of MDT signs flip 
    int signCor = segment.localDirection().angleYZ() < 0 ? -1 : 1;
    calculateKeys(segment.containedMeasurements(),signCor);
    
}

Muon::MuonSegmentKey::MuonSegmentKey( const std::vector< const Trk::MeasurementBase* >& measurements ) {
  calculateKeys(measurements,1);
}


void Muon::MuonSegmentKey::calculateKeys( const std::vector< const Trk::MeasurementBase* >& measurements, int signCor ) {
  
  // clear internal lists
  m_measKeys.clear();
  m_measKeysTrigger.clear();

  // loop over measurements
  std::vector< const Trk::MeasurementBase* >::const_iterator it = measurements.begin(), itEnd = measurements.end();
  for(;it!=itEnd;++it){
     
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*it); 
    const Muon::CompetingMuonClustersOnTrack* crot = 0;
    if( !rot ){
      crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(*it);
      
      if( crot && !crot->containedROTs().empty() ) rot = crot->containedROTs().front();
    }
    if( !rot ) continue;

    // cast Identifier to an int
    unsigned int id = rot->identify().get_identifier32().get_compact();
    int sign = 1;
    // only consider MDT's and CSCs for now
    const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(*it);
    if( mdt ){
      // get drift radius for MDTs
      const Trk::LocalParameters & locParams = (*it)->localParameters();
      
      // only take into account sign if more than 1 mm from wire
      if( signCor*locParams.get(Trk::loc1) < -1. ) sign = -1;

      // if signCor == 2 ignore sign
      if( signCor == 2 ) sign = 1;

      m_measKeys.insert(std::make_pair(id,sign));
    }else if( dynamic_cast<const Muon::CscClusterOnTrack*>(*it) ){
      m_measKeys.insert(std::make_pair(id,sign));      
    }else if( dynamic_cast<const Muon::MMClusterOnTrack*>(*it) ){
      m_measKeys.insert(std::make_pair(id,sign));
    }else if( dynamic_cast<const Muon::sTgcClusterOnTrack*>(*it) ){
      m_measKeys.insert(std::make_pair(id,sign));
    }else{
      m_measKeysTrigger.insert(std::make_pair(id,sign));
    }

  }
}


Muon::MuonSegmentKey::~MuonSegmentKey()
{}

