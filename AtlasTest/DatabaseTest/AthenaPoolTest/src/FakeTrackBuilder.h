/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACKFAKEBUILDER_H
#define TRKTRACKFAKEBUILDER_H

namespace InDetDD {class SiDetectorElementCollection;}
namespace Trk {class Track; }
namespace Rec {class TrackParticle;}

class FakeTrackBuilder {
public:
  static Trk::Track*         buildTrack(        const InDetDD::SiDetectorElementCollection* elements);
  static Trk::Track*         buildBrokenTrack(  const InDetDD::SiDetectorElementCollection* elements);
  static Rec::TrackParticle* buildTrackParticle(const InDetDD::SiDetectorElementCollection* elements);
};

#endif
