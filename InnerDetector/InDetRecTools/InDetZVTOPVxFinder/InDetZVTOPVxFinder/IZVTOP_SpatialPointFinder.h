/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IZVTOP_SpatialPointFinder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETZVTOP_SPATIALPOINTFINDER_H
#define IINDETZVTOP_SPATIALPOINTFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

namespace Rec {
  class TrackParticle;
}
namespace Trk {
  class Track;
  class TrackParticleBase;
  class RecVertex;
  class Vertex;
  //class TrackParameters;
}
namespace InDet 
{


  static const InterfaceID IID_IZVTOP_SpatialPointFinder("InDet::IZVTOP_SpatialPointFinder", 1, 0);

  class IZVTOP_SpatialPointFinder : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // declaration of interface-defining member functions
    virtual Trk::Vertex* findSpatialPoint(const Trk::Track* trk_1, const Trk::Track* trk_2) = 0;
    virtual Trk::Vertex* findSpatialPoint(const Rec::TrackParticle* trk_1, const Rec::TrackParticle* trk_2) = 0;
    virtual Trk::Vertex* findSpatialPoint(const Trk::TrackParticleBase* trk_1, const Trk::TrackParticleBase* trk_2) = 0;
    virtual Trk::Vertex* findSpatialPoint(const Trk::RecVertex vtx, const Trk::Track* trk_1) = 0;
    virtual Trk::Vertex* findSpatialPoint(const Trk::RecVertex vtx, const Rec::TrackParticle* trk_1) = 0;
    virtual Trk::Vertex* findSpatialPoint(const Trk::RecVertex vtx, const Trk::TrackParticleBase* trk_1) = 0;
    virtual Trk::Vertex* findSpatialPoint(const Trk::TrackParameters* perigee_1, const Trk::TrackParameters* perigee_2) = 0;
    virtual Trk::Vertex* findSpatialPoint(const Trk::RecVertex vtx, const Trk::TrackParameters* perigee_1) = 0;
  };

  inline const InterfaceID& InDet::IZVTOP_SpatialPointFinder::interfaceID()
    { 
      return IID_IZVTOP_SpatialPointFinder; 
    }

} // end of namespace

#endif 
