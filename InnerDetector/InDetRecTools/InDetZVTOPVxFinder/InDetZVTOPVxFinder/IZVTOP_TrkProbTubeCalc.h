/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


  static const InterfaceID IID_IZVTOP_TrkProbTubeCalc("InDet::IZVTOP_TrkProbTubeCalc", 1, 0);

  class IZVTOP_TrkProbTubeCalc : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // declaration of interface-defining member functions
    //trk
    virtual double calcProbTube(const Trk::Track& trk, Trk::Vertex& vec) = 0;
    virtual double calcProbTube(const Rec::TrackParticle& trk, Trk::Vertex& vec) = 0;
    virtual double calcProbTube(const Trk::TrackParticleBase& trk, Trk::Vertex& vec) = 0;
    virtual double calcProbTube(const Trk::Perigee* trk, Trk::Vertex& vec) = 0;
    //beam spot
    virtual double calcProbTube(const Trk::RecVertex& vtx, Trk::Vertex& vec) = 0;

  };

  inline const InterfaceID& InDet::IZVTOP_TrkProbTubeCalc::interfaceID()
    { 
      return IID_IZVTOP_TrkProbTubeCalc; 
    }

} // end of namespace

#endif 
