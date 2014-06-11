/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SORTMUPATHITS_H
#define SORTMUPATHITS_H

#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "MuonTrackFindingEvent/MuPatHit.h"

namespace Muon {
  
  class DistanceAlongParameters {
  public:
    double operator()( const Trk::TrackParameters& pars, const Amg::Vector3D& pos ) const {
      Amg::Vector3D difPos = pos - pars.position();
      return difPos.dot(pars.momentum().unit());
    }

    double operator()( const Trk::TrackParameters& pars1, const Trk::TrackParameters& pars2 ) const {
      return this->operator()(pars1,pars2.position());
    }

    double operator()( const Trk::TrackParameters& pars, const Trk::MeasurementBase& meas ) const {
      return this->operator()(pars,meas.globalPosition());
    }

  };


  class MuPatHitDistanceAlongParameters {
  public:
    double operator()( const MuPatHit* hit1, const MuPatHit* hit2 ) const {
      return distanceCalculator(hit1->parameters(),hit2->parameters());
    }
    
    DistanceAlongParameters distanceCalculator;
  };

  class SortMuPatHits {
  public:

    bool operator()( const MuPatHit* hit1, const MuPatHit* hit2 ) const {
      double dist = distanceCalculator(hit1->parameters(),hit2->parameters());
      if( dist < -0.01 ) return true;
      else if( dist > 0.01 ) return false;

      // most likely on same surface
      const MuPatHit::Info& info1 = hit1->info();
      const MuPatHit::Info& info2 = hit2->info();

      // for MDT's compare IDs
      if( info1.type == MuPatHit::MDT ) return info1.id < info2.id;
      // for cluster type measurements compare types and eta/phi
      if( info1.type != info2.type || info1.measuresPhi == info2.measuresPhi ) return info1.id < info2.id;
      // phi measurements larger than eta measurements 
      if( info1.measuresPhi ) return true;
      return false;      
    }
    
    DistanceAlongParameters distanceCalculator;
  };

}


#endif
