/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_IPTRACKINFO
#define JETTAGINFO_IPTRACKINFO

#include "AthLinks/ElementLink.h"
#include "JetTagInfo/TrackGrade.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include <iostream>

class MsgStream;

namespace Analysis {

  class IPTrackInfo {
    public:
      IPTrackInfo();
      IPTrackInfo(const Rec::TrackParticleContainer* coll, 
                  const Rec::TrackParticle* trk,
		  const TrackGrade& grade,
                  bool isFromV0,
                  double d0val, double d0sig,
                  double z0val, double z0sig);
      IPTrackInfo(const ElementLink<Rec::TrackParticleContainer>& track,
		  const TrackGrade& grade,
                  bool isFromV0,
                  double d0val, double d0sig,
                  double z0val, double z0sig);
      virtual ~IPTrackInfo();
      inline void setTrackWeight2D(double w) { m_trackWeight2D = w; }
      inline void setTrackWeight3D(double w) { m_trackWeight3D = w; }
      inline void setTrackProbJP(double p) { m_trackProbJP = p; }
      inline void setTrackProbJPneg(double p) { m_trackProbJPneg = p; }
      const Rec::TrackParticle* track() const;
      inline const ElementLink<Rec::TrackParticleContainer>& trackLink() const
      { return m_track; }
      inline const TrackGrade& trackGrade() const { return m_trackGrade; } 
      inline bool isFromV0() const { return m_isFromV0; }
      inline double d0Value() const { return m_valD0wrtPV; } 
      inline double d0Significance() const { return m_sigD0wrtPV; } 
      inline double z0Value() const { return m_valZ0wrtPV; } 
      inline double z0Significance() const { return m_sigZ0wrtPV; } 
      inline double trackWeight2D() const { return m_trackWeight2D; } 
      inline double trackWeight3D() const { return m_trackWeight3D; } 
      inline double trackProbJP() const { return m_trackProbJP; } 
      inline double trackProbJPneg() const { return m_trackProbJPneg; } 
      inline void resetW() { m_trackWeight2D = 0.; m_trackWeight3D = 0.; m_trackProbJP = 1; m_trackProbJPneg = 1;}
    private:
      ElementLink<Rec::TrackParticleContainer> m_track;
      TrackGrade m_trackGrade;
      bool m_isFromV0;
      double m_valD0wrtPV;
      double m_sigD0wrtPV;
      double m_valZ0wrtPV;
      double m_sigZ0wrtPV;
      double m_trackWeight2D;
      double m_trackWeight3D;
      double m_trackProbJP;
      double m_trackProbJPneg;
  };

  MsgStream& operator<<(MsgStream& out, const IPTrackInfo&);
  std::ostream& operator<<(std::ostream& out, const IPTrackInfo&);

}
#endif // JETTAGINFO_IPTRACKINFO
