/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACKFAKEBUILDER_H
#define TRKTRACKFAKEBUILDER_H

namespace InDetDD {class PixelDetectorManager;}
namespace Trk {class Track; }
namespace Rec {class TrackParticle;}

class FakeTrackBuilder {
public:
  static Trk::Track*         buildTrack(        const InDetDD::PixelDetectorManager* pixMgr);
  static Trk::Track*         buildBrokenTrack(  const InDetDD::PixelDetectorManager* pixMgr);
  static Rec::TrackParticle* buildTrackParticle(const InDetDD::PixelDetectorManager* pixMgr);
};

#endif
