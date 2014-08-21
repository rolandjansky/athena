/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUONCANDIDATE_H
#define MUONCOMBINEDEVENT_MUONCANDIDATE_H

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthLinks/ElementLink.h"
#include "MuonCombinedEvent/TagBase.h"
#include <vector>


namespace Trk {
  class Track;
}

namespace MuonCombined {
  
  class TagBase;

  class MuonCandidate {
    
  public:
    /** constructor taking an Trk::Track reference to the spectromter track
	Users should ensure that the element link is valid and the lifetime of the Track object is longer that the MuonCandidate
	as it internally caches a pointer to it. 
    */
    MuonCandidate( const Trk::Track& msTrack );

    /** constructor taking an Trk::Track reference to the spectromter track
	Users should ensure that the element link is valid and the lifetime of the Track object is longer that the MuonCandidate
	as it internally caches a pointer to it. Class takes ownership of the extrapolatedTrack.
    */
    MuonCandidate( const Trk::Track& msTrack, const Trk::Track* extrapolatedTrack );

    /** constructor taking an ElementLink to a xAOD::TrackParicle&
	Users should ensure that the element link is valid and the lifetime of the Track object is longer that the MuonCandidate
	as it internally caches a pointer to it. 
    */
    MuonCandidate( const ElementLink<xAOD::TrackParticleContainer>& msTrackLink );

    /** constructor taking an ElementLink to a xAOD::TrackParicle&
	Users should ensure that the element link is valid and the lifetime of the Track object is longer that the MuonCandidate
	as it internally caches a pointer to it. Class takes ownership of the extrapolatedTrack.
    */
    MuonCandidate( const ElementLink<xAOD::TrackParticleContainer>& msTrackLink, const Trk::Track* extrapolatedTrack );

    /** constructor taking an const reference for the MS track and an
	ElementLink to an xAOD::TrackParticle for the extrapolated track.
	Use this function when you don't want MuonCandidate to take ownership of the extrapolated track
    */
    MuonCandidate( const Trk::Track& msTrack, const ElementLink<xAOD::TrackParticleContainer>& saTrackLink );

    /** destructor */
    ~MuonCandidate();
    
    /** access spectrometer track, always there */
    const Trk::Track& muonSpectrometerTrack() const;

    /** access spectrometer track, always there */
    const ElementLink<xAOD::TrackParticleContainer>& muonSpectrometerTrackLink() const;

    /** add extrapolated track to the MuonCandidate, takes ownership of the track and delete the current one if present */
    void updateExtrapolatedTrack( const Trk::Track* extrapolatedTrack );

    /** access extrapolated track, can be zero if back extrapolation failed */
    const Trk::Track* extrapolatedTrack() const;
    
    /** access extrapolated track, can be zero if back extrapolation failed */
    const Trk::Track* releaseExtrapolatedTrack();

    /** access extrapolated track element link, will not point to anything if we own the Track */
    const ElementLink<xAOD::TrackParticleContainer>& extrapolatedTrackLink() const;

    /** ask if the MuonCandidate owns the extrapolated track */
    bool ownsExtrapolatedTrack() const;
    
    /** print candidate to string */
    std::string toString() const;

  private:
    /** element link to spectrometer track */
    const ElementLink<xAOD::TrackParticleContainer>   m_muonSpectrometerTrackLink;

    /** element link to extrapolated track */
    const ElementLink<xAOD::TrackParticleContainer>   m_extrapolatedTrackLink;

    /** pointer to extrapolated track, may be owned */
    const Trk::Track*   m_extrapolatedTrack;

    /** pointer to spectrometer track, not owned */
    const Trk::Track*   m_muonSpectrometerTrack;

    /** flag to see if we own the extrapolated track */
    bool m_ownsExtrapolatedTrack;
  };

  inline const Trk::Track& MuonCandidate::muonSpectrometerTrack() const { return *m_muonSpectrometerTrack; }

  inline const ElementLink<xAOD::TrackParticleContainer>& MuonCandidate::muonSpectrometerTrackLink() const { return m_muonSpectrometerTrackLink; }

  inline void MuonCandidate::updateExtrapolatedTrack(const Trk::Track* extrapolatedTrack ) { 
    if( m_extrapolatedTrack && m_ownsExtrapolatedTrack) delete m_extrapolatedTrack;
    m_extrapolatedTrack = extrapolatedTrack; 
    m_ownsExtrapolatedTrack=true;
  }
  
  inline const Trk::Track* MuonCandidate::extrapolatedTrack() const { return m_extrapolatedTrack; }
  
  inline const Trk::Track* MuonCandidate::releaseExtrapolatedTrack() { 
    const Trk::Track* tmp = m_extrapolatedTrack; 
    m_extrapolatedTrack = 0; 
    return tmp; 
  }

  inline bool MuonCandidate::ownsExtrapolatedTrack() const { return m_ownsExtrapolatedTrack; }

  inline const ElementLink<xAOD::TrackParticleContainer>& MuonCandidate::extrapolatedTrackLink() const { return m_extrapolatedTrackLink; }
}


#endif
