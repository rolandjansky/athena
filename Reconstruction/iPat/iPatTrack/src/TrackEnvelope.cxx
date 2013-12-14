/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 owns a Track to which public (write) access is provided for the track's hits
 and parameters - to fulfill the needs of track fitting and following while
 respecting encapsulation in the Track class itself
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "iPatTrack/TrackEnvelope.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TrackEnvelope::TrackEnvelope (TrackStatus	status,
			      hit_list* 	hits)
    :	m_status		(status),
	m_hits			(hits),
	m_parameters		(new parameter_vector),
	m_perigeeCovariance   	(new AmgSymMatrix(5)),
	m_perigee   		(new PerigeeParameters),
	m_fitQuality	       	(new FitQuality),
	m_hitQuality	       	(new HitQuality),
	m_track			(0)
{
    // std::cout << " TrackEnvelope constructor " << status << std::endl;
    m_perigee->covariance(m_perigeeCovariance);
    m_track	= new Track (m_hits,
			     m_parameters,
			     m_perigee,
			     m_fitQuality,
			     m_hitQuality);
    m_track->status(status);
    // m_hits	= &m_track->hits_for_edit();
    // verify_track();
}	

TrackEnvelope::TrackEnvelope (const Track& track)
    :	m_status		(track.status()),
	m_hits			(new hit_list),
	m_parameters		(new parameter_vector),
	m_perigeeCovariance   	(new AmgSymMatrix(5)(track.perigee_parameters().covariance())),
	m_perigee		(new PerigeeParameters),
	m_fitQuality		(new FitQuality(track.fitQuality())),
	m_hitQuality		(new HitQuality(track.hitQuality())),
	m_track			(0)
{
    // needs protection via Track has methods
    ///////////////////////////////////////// FIX_ME
    // std::cout << " Track -> TrackEnvelope constructor " << std::endl;
     
    for (hit_const_iterator h = track.hit_list_begin();
	 h != track.hit_list_end();
	 ++h)
    {
	m_hits->push_back(new HitOnTrack(*(*h)));
    }

    m_perigee->fillPerigee(track.perigee_parameters());
    m_perigee->covariance(m_perigeeCovariance);
    m_track	= new Track (m_status,
			     m_hits,
			     m_parameters,
			     m_perigee,
			     m_fitQuality,
			     m_hitQuality);
    // m_hits	= &m_track->hits_for_edit();
    // verifyTrack();
    // std::cout << " end copy constructor " << std::endl;
}

TrackEnvelope::~TrackEnvelope (void)
{ 
    m_track->hits_for_edit().clear();
    delete m_track;
    delete m_parameters;
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

void
TrackEnvelope::verifyTrack (void) const
{
    // std::cout << " TrackEnvelope::verify_track " << m_status << std::endl;
    hit_const_iterator h = m_hits->begin();
    if (h != m_track->hit_list_begin()) assert(h == m_track->hit_list_begin());
    
    if (m_fitQuality != 0)
	assert (m_fitQuality == &m_track->fitQuality());
    
    if (m_hitQuality != 0)
	assert (m_hitQuality == &m_track->hitQuality());

    if (m_perigee != 0)
    {
	assert (m_perigee == &m_track->perigee_parameters());
//  	if (m_perigee->has_covariance())
//  	    assert (m_perigee->covariance() == m_track->perigee_parameters().covariance());
    }
    
}


