/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 build a Track from the hit_list formed by TrackFollowing an existing Track
 --------------------------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include "iPatInterfaces/ITrackQuality.h"
#include "iPatInterfaces/IiPatFitter.h"
#include "iPatTrack/Track.h"
#include "iPatTrackFollower/TrackBuilder.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TrackBuilder::TrackBuilder(const std::string&	type,
			   const std::string&	name,
			   const IInterface*	parent)
    :   AthAlgTool		(type, name, parent),
	m_trackFitter		("TrackFitter/TrackFitter"),
	m_trackQuality		("TrackQuality/TrackQuality"),
	m_buildCount		(0)
{
    declareInterface<ITrackBuilder>(this);
    declareProperty("TrackFitter",	m_trackFitter);
    declareProperty("TrackQuality",	m_trackQuality);
}

TrackBuilder::~TrackBuilder()
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TrackBuilder::initialize()
{
    ATH_MSG_INFO( "TrackBuilder::initialize() - package version " << PACKAGE_VERSION );
    
    // get the Tools
    if (m_trackFitter.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackFitter );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackFitter );
    }
    if (m_trackQuality.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackQuality );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackQuality );
    }

    return StatusCode::SUCCESS;
}

StatusCode
TrackBuilder::finalize()
{
    ATH_MSG_INFO( "finalised after " << m_buildCount << " tracks built " );
  
    return StatusCode::SUCCESS;
}

Track*
TrackBuilder::trackFromHits (TrackStatus status,
			     std::list<HitOnTrack*>* hits,
			     const Track& track) const
{
    // fail if insufficient hits
    if (hits == 0) return 0;
    
    HitQuality*	hitQuality = new HitQuality;
    if (! m_trackQuality->sufficientHits(status,
					 *hitQuality,
					 hits->begin(),
					 hits->end()))
    {
	for (std::list<HitOnTrack*>::const_iterator h = hits->begin(); h != hits->end(); ++h)
	    delete *h;
	delete hits;
	delete hitQuality;
	ATH_MSG_DEBUG( "trackFromHits: FAIL with insufficient hits" );
	return 0;
    }
    FitQuality*		fitQuality		= 0;
    PerigeeParameters*	perigeeParameters	= 0;
    parameter_vector*	scattererParameters	= 0;
    Track*		builtTrack		= 0;
    
    // copy fit results when no modification to the silicon hits
    // (but only for tracks with final material allocation)
    if (hitQuality->number_planar_clusters() == track.hitQuality().number_planar_clusters()
	&& status >= secondary)
    {
	if (m_trackQuality->goodTrack(track.status(),track.fitQuality(),*hitQuality))
	{
	    // use copy constructor ?
	    fitQuality		= new FitQuality(track.fitQuality());
	    perigeeParameters	= new PerigeeParameters(track.perigee_parameters());
	    scattererParameters	= new parameter_vector;
	    for (parameter_const_iterator p = track.parameters_begin();
		 p != track.parameters_end();
		 ++p)
	    {
		scattererParameters->push_back(new ScattererParameters(**p));
	    }
	    builtTrack	= new Track(status,
				    hits,
				    scattererParameters,
				    perigeeParameters,
				    fitQuality,
				    hitQuality);
	    ++m_buildCount;
	    ATH_MSG_DEBUG( "trackFromHits: OK fit parameters copied" );
	    return builtTrack;
	}
    }
    else if (status < long_segment || status == secondary)
    {
	// fit with residuals
	perigeeParameters	= new PerigeeParameters;
	perigeeParameters->fillPerigee(track.perigee_parameters());
	m_trackFitter->fitWithResiduals(status,
					fitQuality,
					*perigeeParameters,
					scattererParameters,
					hits->begin(),
					hits->end());
	ATH_MSG_DEBUG( "trackFromHits: made fitWithResiduals " );
    }
    else
    {
	// allow high residual hit rejection when it's a truncated or primary track
	perigeeParameters	= new PerigeeParameters;
	perigeeParameters->fillPerigee(track.perigee_parameters());
	m_trackFitter->fitWithRejection(status,
					fitQuality,
					m_trackQuality->maxReject(*hitQuality),
					*perigeeParameters,
					scattererParameters,
					hits->begin(),
					hits->end(),
					track.hit_list_begin(),
					track.hit_list_end());
	ATH_MSG_DEBUG( "trackFromHits: made fitWithRejection " );
    }

    if (fitQuality != 0
	&& m_trackQuality->sufficientHits(status,*hitQuality,hits->begin(),hits->end())
	&& m_trackQuality->goodTrack(status,*fitQuality,*hitQuality))
    {
	builtTrack	= new Track(status,
				    hits,
				    scattererParameters,
				    perigeeParameters,
				    fitQuality,
				    hitQuality);
	++m_buildCount;
	ATH_MSG_DEBUG( "trackFromHits: OK track successfully built" );
	return builtTrack;
    }
    else
    {
	delete hitQuality;
	for (std::list<HitOnTrack*>::const_iterator h = hits->begin(); h != hits->end(); ++h)
	    delete *h;
	hits->clear();
	delete hits;
	delete fitQuality;
	delete perigeeParameters;
	if (scattererParameters)
	{
	    for (parameter_iterator s = scattererParameters->begin();
		 s != scattererParameters->end();
		 ++s)
		delete *s;
	    delete scattererParameters;
	}
	ATH_MSG_DEBUG( "trackFromHitsFAIL with bad fit quality" );
	return 0;
    }
}   

Track*
TrackBuilder::trackRefit (const Track& track) const
{
    HitQuality*	hitQuality = new HitQuality;
    if (! m_trackQuality->sufficientHits(track.status(),
					 *hitQuality,
					 track.hit_list_begin(),
					 track.hit_list_end()))
    {
	delete hitQuality;
	ATH_MSG_DEBUG( "trackRefit: FAIL with insufficient hits" );
	return 0;
    }
    hit_list*		hits			= new hit_list;
    for (std::list<HitOnTrack*>::const_iterator h = track.hit_list_begin();
	 h != track.hit_list_end();
	 ++h)
    {
	hits->push_back(new HitOnTrack(*(*h)));
    }
    FitQuality*		fitQuality		= 0;
    parameter_vector*	scattererParameters	= 0;
    Track*		builtTrack		= 0;
    PerigeeParameters*	perigeeParameters	= new PerigeeParameters;
    perigeeParameters->fillPerigee(track.perigee_parameters());
    m_trackFitter->fitWithResiduals(track.status(),
				    fitQuality,
				    *perigeeParameters,
				    scattererParameters,
				    hits->begin(),
				    hits->end());

    if (m_trackQuality->goodTrack(track.status(),*fitQuality,*hitQuality))
    {
	builtTrack	= new Track(track.status(),
				    hits,
				    scattererParameters,
				    perigeeParameters,
				    fitQuality,
				    hitQuality);
	++m_buildCount;
	ATH_MSG_DEBUG( "trackRefit: OK" );
	return builtTrack;
    }
    else
    {
	// TODO: the plan is to build last-layers SCT + trt secondary tracks here
	delete hitQuality;
	for (std::list<HitOnTrack*>::const_iterator h = hits->begin(); h != hits->end(); ++h)
	    delete *h;
	hits->clear();
	delete hits;
	delete fitQuality;
	delete perigeeParameters;
	if (scattererParameters)
	{
	    for (parameter_iterator s = scattererParameters->begin();
		 s != scattererParameters->end();
		 ++s)
		delete *s;
	    delete scattererParameters;
	}
	ATH_MSG_DEBUG( "trackRefit: FAIL with bad fit quality" );
	return 0;
    }
}
