/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 owns a Track to which public (write) access is provided for the track's hits
 and parameters - to fulfill the needs of track fitting and following while
 respecting encapsulation in the Track class itself
 ***************************************************************************/

#ifndef IPATTRACK_TRACKENVELOPE_H
# define IPATTRACK_TRACKENVELOPE_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "EventPrimitives/EventPrimitives.h"
#include "iPatTrack/Track.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>


//  class TrackEnvelope
//
//  This class grants write access to the member data of an encapsulated Track object.
//  A track consists of a collection of hits together with derived (fitted) parameters
//  plus fit and hit quality information. This data becomes invalid as soon as a small
//  change is made to one of the hits.
//  So the integrity of a track is fragile and the envelope pattern is used to emphasise
//  this. Thus Track should have const access methods, with editing performed via a
//  track envelope.
//
//  TrackEnvelope provides get (a reference to member data objects) and set (a pointer
//  passing ownership of the data object) methods. Constructor and fill methods can be 
//  used to copy a track into an envelope. The empty method releases the contained
//  track and clears the envelope. The destructor also destroys the contained track.
//

class TrackEnvelope
{

public:
    TrackEnvelope (TrackStatus		status,
		   hit_list* 		hits);  	// TrackBuilder constructor
    TrackEnvelope (const Track&);			// copy from Track
    ~TrackEnvelope (void);				// destructor

    Track*			empty (void);
    FitQuality*&		fitQuality (void);
    HitQuality&			hitQuality (void);
    hit_list&			hits (void);
    AmgSymMatrix(5)&		perigeeCovariance (void);
    PerigeeParameters&		perigeeParameters (void);
    void			perigeeParameters (PerigeeParameters*);
    TrackStatus			status (void) const;
    void			status (TrackStatus);
    const Track&		track (void) const;
    parameter_vector&		trackParameters (void);
    void			verifyTrack (void) const;

private:
    // enumerated track status
    TrackStatus			m_status;
    
    // list of associated hits 
    hit_list*			m_hits;

    // track parameters 
    parameter_vector*		m_parameters;
    AmgSymMatrix(5)*		m_perigeeCovariance;
    PerigeeParameters*		m_perigee;
     
    // track quality 
    FitQuality*			m_fitQuality;
    HitQuality*			m_hitQuality;

    // the corresponding track in the envelope
    Track*			m_track;

    // void and copy constructors, assignment: no semantics, no implementation
    TrackEnvelope (void);
    TrackEnvelope (const TrackEnvelope&);
    TrackEnvelope &operator= (const TrackEnvelope&);
};


//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline Track*
TrackEnvelope::empty (void)
{
    // check envelope status hasn't been invalidated
    assert(m_track->status() == m_status);
    
    // clear member data
    Track*	track 		= m_track;
    m_hits	 		= 0;
    m_perigeeCovariance  	= 0; 
    m_perigee 			= 0;
    m_fitQuality 		= 0;	
    m_hitQuality		= 0;
    m_track	 		= 0;
    return track;
}

inline FitQuality*&
TrackEnvelope::fitQuality (void)
{ return m_fitQuality; }

inline HitQuality&
TrackEnvelope::hitQuality (void)
{ return *m_hitQuality; }

inline hit_list&
TrackEnvelope::hits (void)
{ return *m_hits; }

inline AmgSymMatrix(5)&
TrackEnvelope::perigeeCovariance (void)
{ return *(m_perigeeCovariance); }

inline PerigeeParameters&
TrackEnvelope::perigeeParameters (void)
{ return *m_perigee; }

inline void
TrackEnvelope::perigeeParameters (PerigeeParameters* perigee)
{
    m_track->perigee_parameters(0);
    m_perigee		= perigee;
    m_perigeeCovariance	= new AmgSymMatrix(5);
    m_perigee->covariance(m_perigeeCovariance);
    m_track->perigee_parameters(m_perigee);
}

inline TrackStatus
TrackEnvelope::status (void) const
{ return m_track->status(); }

inline void
TrackEnvelope::status (TrackStatus status)
{ m_track->status(status); }

inline const Track&
TrackEnvelope::track (void) const
{ return *m_track; }

inline parameter_vector&
TrackEnvelope::trackParameters (void)
{ return *m_parameters; }

#endif // IPATTRACK_TRACKENVELOPE_H
