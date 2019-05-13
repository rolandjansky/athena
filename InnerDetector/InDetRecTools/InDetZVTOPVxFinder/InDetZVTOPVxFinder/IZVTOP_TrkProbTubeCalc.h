/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IZVTOP_TrkProbTubeCalc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETZVTOP_TRKPROBTUBECALC_H
#define IINDETZVTOP_TRKPROBTUBECALC_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk
{
  class Track;
  class RecVertex;
  class Vertex;
  class TrackParticleBase;
}

namespace Rec {
  class TrackParticle;
}
namespace InDet 
{


  class IZVTOP_TrkProbTubeCalc : virtual public IAlgTool {
  public:
    DeclareInterfaceID (IZVTOP_TrkProbTubeCalc, 1, 0);

    // declaration of interface-defining member functions
    //trk
    virtual double calcProbTube(const Trk::Track& trk, Trk::Vertex& vec) const = 0;
    virtual double calcProbTube(const Rec::TrackParticle& trk, Trk::Vertex& vec) const = 0;
    virtual double calcProbTube(const Trk::TrackParticleBase& trk, Trk::Vertex& vec) const = 0;
    virtual double calcProbTube(const Trk::Perigee* trk, Trk::Vertex& vec) const = 0;
    //beam spot
    virtual double calcProbTube(const Trk::RecVertex& vtx, Trk::Vertex& vec) const = 0;

  };

} // end of namespace

#endif 
