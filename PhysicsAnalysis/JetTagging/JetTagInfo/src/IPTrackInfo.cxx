/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/IPTrackInfo.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

namespace Analysis {

IPTrackInfo::IPTrackInfo() :
  m_track(ElementLink<Rec::TrackParticleContainer>()),
  m_trackGrade(),
  m_isFromV0(false),
  m_valD0wrtPV(0),
  m_sigD0wrtPV(0),
  m_valZ0wrtPV(0),
  m_sigZ0wrtPV(0),
  m_trackWeight2D(0),
  m_trackWeight3D(0),
  m_trackProbJP(0),
  m_trackProbJPneg(0)
{
}

IPTrackInfo::IPTrackInfo(const Rec::TrackParticleContainer* coll, 
                         const Rec::TrackParticle* trk,
                         const TrackGrade& grade,
                         bool isFromV0,
                         double d0val, double d0sig,
                         double z0val, double z0sig)
  : m_trackWeight2D(0),
    m_trackWeight3D(0),
    m_trackProbJP(0),
    m_trackProbJPneg(0)
{
        ElementLink<Rec::TrackParticleContainer> link;
        if( link.toContainedElement(*coll, const_cast<Rec::TrackParticle*>(trk)) ) {
	  m_track = link;
	}
	m_trackGrade = grade;
	m_isFromV0 = isFromV0;
	m_valD0wrtPV = d0val;
	m_sigD0wrtPV = d0sig;
	m_valZ0wrtPV = z0val;
	m_sigZ0wrtPV = z0sig;
}
	
IPTrackInfo::IPTrackInfo(const ElementLink<Rec::TrackParticleContainer>& track,
                         const TrackGrade& grade,
                         bool isFromV0,
                         double d0val, double d0sig,
                         double z0val, double z0sig)
  : m_track (track),
    m_trackGrade (grade),
    m_isFromV0 (isFromV0),
    m_valD0wrtPV (d0val),
    m_sigD0wrtPV (d0sig),
    m_valZ0wrtPV (z0val),
    m_sigZ0wrtPV (z0sig),
    m_trackWeight2D(0),
    m_trackWeight3D(0),
    m_trackProbJP(0),
    m_trackProbJPneg(0)
{
}
	
const Rec::TrackParticle* IPTrackInfo::track() const {
  if( m_track.isValid() ) {
    return *(m_track);
  } else {
    return 0;
  }
}

MsgStream& operator<<( MsgStream& out, const IPTrackInfo& info) {
      const Rec::TrackParticle* tp = info.track();
      double phi = 0.;
      double d0 = 0.;
      double pt = 0.;
      double eta = 0.;
      if(tp) {
        phi = tp->measuredPerigee()->parameters()[Trk::phi];
        d0 = tp->measuredPerigee()->parameters()[Trk::d0];
        pt = tp->pt();
        eta = tp->eta();
      }
      out << " -> IPTrackInfo " 
          << " grade= " << info.trackGrade().gradeString()
	  << " fromV0= " << info.isFromV0()
          << " d0wrtPV= " << info.d0Value() << " S=" << info.d0Significance()
          << " z0wrtPV= " << info.z0Value() << " S=" << info.z0Significance()
          << " original pt,eta,phi,d0 = "<<pt<<" "<<eta<<" "<<phi<<" "<<d0
          << " weight2D= " << info.trackWeight2D() 
          << " weight3D= " << info.trackWeight3D() 
          << " probJP= " << info.trackProbJP() 
          << " probJPneg= " << info.trackProbJPneg() 
          << endmsg; 
      return out;
}

std::ostream& operator<<( std::ostream& out, const IPTrackInfo& info) {
      const Rec::TrackParticle* tp = info.track();
      double phi = 0.;
      double d0 = 0.;
      double pt = 0.;
      double eta = 0.;
      if(tp) {
        phi = tp->measuredPerigee()->parameters()[Trk::phi];
        d0 = tp->measuredPerigee()->parameters()[Trk::d0];
        pt = tp->pt();
        eta = tp->eta();
      }
      out << " -> IPTrackInfo " 
          << " grade= " << info.trackGrade().gradeString()
	  << " fromV0= " << info.isFromV0()
          << " d0wrtPV= " << info.d0Value() << " S=" << info.d0Significance()
          << " z0wrtPV= " << info.z0Value() << " S=" << info.z0Significance()
          << " original pt,eta,phi,d0 = "<<pt<<" "<<eta<<" "<<phi<<" "<<d0
          << " weight2D= " << info.trackWeight2D() 
          << " weight3D= " << info.trackWeight3D() 
          << " probJP= " << info.trackProbJP() 
          << " probJPneg= " << info.trackProbJPneg() 
          << std::endl; 
      return out;
}

} 
