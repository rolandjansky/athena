/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_SVTRACKINFO
#define JETTAGINFO_SVTRACKINFO

#include "AthLinks/ElementLink.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include <iostream>

class MsgStream;

namespace Analysis {

  class SVTrackInfo {
    public:
      inline SVTrackInfo();
      inline SVTrackInfo(const Rec::TrackParticleContainer* coll, 
			 const Rec::TrackParticle* trk);
      inline SVTrackInfo(const ElementLink<Rec::TrackParticleContainer>& track);
      inline virtual ~SVTrackInfo() {}
      inline const Rec::TrackParticle* track() const;
      inline const ElementLink<Rec::TrackParticleContainer>& trackLink() const;
    private:
      ElementLink<Rec::TrackParticleContainer> m_track;
  };

  SVTrackInfo::SVTrackInfo() : m_track(ElementLink<Rec::TrackParticleContainer>()) {}

  SVTrackInfo::SVTrackInfo(const Rec::TrackParticleContainer* coll, 
			   const Rec::TrackParticle* trk) {
    ElementLink<Rec::TrackParticleContainer> link;
    if( link.toContainedElement(*coll, const_cast<Rec::TrackParticle*>(trk)) ) {
      m_track = link;
    }
  }

  SVTrackInfo::SVTrackInfo(const ElementLink<Rec::TrackParticleContainer>& track)
    : m_track (track)
  {
  }

  const Rec::TrackParticle* SVTrackInfo::track() const {
    if( m_track.isValid() ) {
      return *(m_track);
    } else {
      return 0;
    }
  }

  const ElementLink<Rec::TrackParticleContainer>& SVTrackInfo::trackLink() const
  {
    return m_track;
  }

}
#endif // JETTAGINFO_SVTRACKINFO
