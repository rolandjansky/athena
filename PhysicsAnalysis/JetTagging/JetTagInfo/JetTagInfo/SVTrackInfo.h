/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_SVTRACKINFO
#define JETTAGINFO_SVTRACKINFO

#include "DataModel/ElementLink.h"
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
      inline virtual ~SVTrackInfo() {}
      inline const Rec::TrackParticle* track() const;
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

  const Rec::TrackParticle* SVTrackInfo::track() const {
    if( m_track.isValid() ) {
      return *(m_track);
    } else {
      return 0;
    }
  }

}
#endif // JETTAGINFO_SVTRACKINFO
