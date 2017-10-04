/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
  AlgTool to follow track (adding HitOnTrack's) using interpolation and/or
 extrapolation methods
 -------------------------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/Surface.h"
#include "iPatGeometry/SiliconLayer.h"
#include "iPatInterfaces/ILayerAllocator.h"
#include "iPatInterfaces/ISiliconLayerAssociator.h"
#include "iPatInterfaces/ISiClusterProperties.h"
#include "iPatInterfaces/ITRT_Recon.h"
#include "iPatInterfaces/ITrackBuilder.h"
#include "iPatTrack/Track.h"
#include "iPatTrackFollower/LayerPredictor.h"
#include "iPatTrackFollower/SiliconClusterMap.h"
#include "iPatTrackFollower/TrackFollower.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TrackFollower::TrackFollower(const std::string&	type,
			     const std::string&	name,
			     const IInterface*	parent)
    :   AthAlgTool		(type, name, parent),
	m_clusterProperties	("SiClusterProperties/SiClusterProperties"),
	m_intersector           ("Trk::SolenoidalIntersector/SolenoidalIntersector"),
	m_layerAllocator	("LayerAllocator/LayerAllocator"),
	m_layerAssociator	("SiliconLayerAssociator/SiliconLayerAssociator"),
	m_trackBuilder		("TrackBuilder/TrackBuilder"),
	m_trtRecon		("TRT_Recon/TRT_Recon"),
	m_sigmaSquared		(3.0*3.0),
	m_clusterMap		(0)
{
    declareInterface<ITrackFollower>(this);
    declareProperty("SiClusterProperties",	m_clusterProperties);
    declareProperty("Intersector",		m_intersector);
    declareProperty("LayerAllocator",		m_layerAllocator);
    declareProperty("SiliconLayerAssociator",	m_layerAssociator);
    declareProperty("TrackBuilder",		m_trackBuilder);
    declareProperty("TRT_Recon",		m_trtRecon);
    declareProperty("SigmaSquared",		m_sigmaSquared);
}

TrackFollower::~TrackFollower()
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TrackFollower::initialize()
{
    ATH_MSG_INFO( "TrackFollower::initialize() - package version " << PACKAGE_VERSION );

    // get the Tools
    if (m_clusterProperties.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_clusterProperties );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_clusterProperties );
    }
    if (m_intersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_intersector );
	return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << m_intersector );
    if (m_layerAllocator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_layerAllocator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_layerAllocator );
    }
    if (m_layerAssociator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_layerAssociator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_layerAssociator );
    }
    if (m_trackBuilder.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackBuilder );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackBuilder );
    }
    if (! m_trtRecon.empty())
    {
	if (m_trtRecon.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_trtRecon );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_trtRecon );
	}
    }

    return StatusCode::SUCCESS;
}

StatusCode
TrackFollower::finalize()
{
    ATH_MSG_INFO( "iPatTrackFollower::finalize()" );
    delete m_clusterMap;    
    return StatusCode::SUCCESS; 
}

Track*
TrackFollower::associate_trt (const Track& track) 
{
    // note return possibilities:
    //   null pointer   - outside TRT acceptance or association not required
    //   void Track     - TRT veto (i.e. no TRT association)
    //   Track with associated TRT hits
    //
    // By definition, only primary and secondary tracks can have trt association.
    // If association successful, pendingTRT will become primary 
    // while existing primary are skipped as they already have association.
    if (m_trtRecon.empty()
	|| (track.status() != pendingTRT
	    && track.status() != secondary))	return 0;
    
    // find the last cluster plus its associated (following) scatterer
    bool haveCluster		= false;
    HitOnTrack*	lastCluster	= 0;
    HitOnTrack*	lastScatterer	= 0;
    for (hit_citerator h = track.hit_list_begin();
	 h != track.hit_list_end();
	 ++h)
    {
	if ((**h).isCluster())
	{
	    haveCluster		= true;
	    lastCluster		= *h;
	}
	else if (haveCluster && (**h).status() == scatterer)
	{
	    haveCluster		= false;
	    lastScatterer	= *h;
	}
    }

    // should never happen !
    if (!lastCluster)				return (new Track);
    
    // create Trk::TrackParameters at the cluster surface
    const Trk::TrackSurfaceIntersection*	intersection	= lastCluster->intersection();
    Amg::Vector3D				position	= intersection->position();
    const Trk::Surface*				surface		= lastCluster->surface();
    TrackParameters& 				params		= **track.parameters_rbegin();

    // TODO:: investigate why one in a million tracks need correcting to surface !!!
    position += surface->normal()*(surface->center() - position).dot(surface->normal());
    const Trk::AtaPlane trkParameters(position,
				      intersection->direction()/std::abs(params.qOverP()),
				      params.charge(),
				      dynamic_cast<const Trk::PlaneSurface&>(*surface));

    // use TRT_Recon to return the associated TRT segment HitList
	const std::vector<HitOnTrack*>* trtHits =
	m_trtRecon->reconstructSegment(track.perigee_parameters(),
				       **track.parameters_rbegin(),
				       trkParameters);

    // outside TRT acceptance (no association)
    if (!trtHits)				return 0;

    // fails TRT association (i.e. veto)
    if (trtHits->begin() == trtHits->end())	return (new Track);

    // have got TRT confirmation:
    //  going to make a copy of the existing track hits
    hit_list* hits = new hit_list;
    
    //  if the TRT hits includes material then copy up to lastScatterer
    if ((**trtHits->begin()).isMaterial())
    {
	//std::cout << " got TRT material " << std::endl;
	for (hit_citerator h = track.hit_list_begin();
	     h != track.hit_list_end();
	     ++h)
	{
	    hits->push_back(new HitOnTrack(**h));
	    if (*h != lastScatterer) continue;
	    lastScatterer = hits->back();
	    // change last scatterer to brem hit when flagged by TRT_Recon
	    if (m_trtRecon->bremLike()) lastScatterer->status(brem);
	    break;
	}

	// append the TRT hits
	for (std::vector<HitOnTrack*>::const_iterator h = trtHits->begin();
	     h != trtHits->end();
	     ++h)
	{
	    hits->push_back(*h);
	}
    }
    else
    {
	//  make a copy of all the existing track hits
	for (hit_citerator h = track.hit_list_begin();
	     h != track.hit_list_end();
	     ++h)
	{
	    hits->push_back(new HitOnTrack(**h));
	    if (*h == lastScatterer) lastScatterer = hits->back();
	}

	//  and change any trailing scatterers to inactive
	hit_reverse_iterator r = hits->rbegin();
	while (*r != lastScatterer)
	{
	    if ((**r).status() == scatterer) (**r).status(inactive);
	    ++r;
	}

	// change last scatterer (i.e. at last cluster measurement) to brem hit when flagged by TRT_Recon
	if (m_trtRecon->bremLike())	lastScatterer->status(brem);

	// append TRT hits with provisional (crude) material treatment
	// start with first scatterer at first hit:
	int		num_straw		= 0;
	int		new_straws		= 0;
	int		scatterers		= 0;
	int		scatterers_in_endcap	= 0;
	double	z0			= track.perigee_parameters().z();
	double	cotTheta		= track.perigee_parameters().cotTheta();
    
	for (std::vector<HitOnTrack*>::const_iterator hit = trtHits->begin();
	     hit != trtHits->end();
	     ++hit, ++num_straw)
	{
	    if ((**hit).isBarrel())
	    {
		// add up to 3 barrel scatterers
		if (scatterers == 0
		    || (scatterers == 1 
			&& ((**hit).position().perp() > 730.0 || new_straws > 12))
		    || (scatterers == 2 
			&& ((**hit).position().perp() > 900.0 || new_straws > 12)))
		{
		    hits->push_back(new HitOnTrack((**hit).isBarrel(),
						   0.00,
						   (**hit).position(),
						   0.05,
						   scatterer));
		    new_straws	= 0;
		    ++scatterers;
		}
	    }
	    else
	    {
		if (scatterers == 0 )
		{
		    hit_citerator h = track.hit_list_end();
		    --h;
		    if (!(**h).isBarrel())		
			hits->push_back(new HitOnTrack((**hit).isBarrel(),
						       0.00,
						       0.5*((**hit).position() + (**h).position()),
						       0.10,
						       inactive));
		}

		double rpred = ((**hit).position().z() - z0)/cotTheta;
		// add up to 3 endcap scatterers
		if (scatterers_in_endcap == 0
		    || (new_straws > 12 && scatterers_in_endcap < 3)
		    || (scatterers == 1 && rpred > 750.0)
		    || (scatterers == 2 && rpred > 870.0))
		{
		    hits->push_back(new HitOnTrack((**hit).isBarrel(),
						   0.00,
						   (**hit).position(),
						   0.05,
						   scatterer));
		    new_straws	= 0;
		    ++scatterers_in_endcap;
		    ++scatterers;
		}
	    }
	    ++new_straws;
	    hits->push_back(*hit);
	}
    }
    
    TrackStatus	status	= track.status();
    if (status == pendingTRT) status = primary;
    Track* trt_track = m_trackBuilder->trackFromHits(status,hits,track);
    return trt_track;
}

Track*
TrackFollower::extrapolate_inwards (const Track& track) const
{
    // extrapolate towards the vertex region
    ATH_MSG_DEBUG( "extrapolate_inwards"
		   << std::setiosflags(std::ios::fixed)
		   << " from layer r,z " << std::setw(7) << std::setprecision(2)
		   << (**track.hit_list_begin()).position().perp() << "  "
		   << std::setw(7) << std::setprecision(2)
		   << (**track.hit_list_begin()).position().z() );

    // make a vector of all layers intersected by the track
    const PerigeeParameters&	perigee	= track.perigee_parameters();
    layer_vector*		layers	= m_layerAllocator->siliconLayers(perigee.position().perp(),
									  perigee.z(),
									  perigee.cotTheta());

    // set an iterator to the layer containing the first HitOnTrack
    hit_citerator h = track.hit_list_begin();
    while (!(**h).isPositionMeasurement()) ++h;
    layer_iterator begin = begin_layer(layers,**h);

    // associate and build track for hits in one extrapolated layer at a time (expensive!)
    std::vector<SiliconLayer*>::iterator end = begin;
    Track* extrapolatedTrack	= 0;
    Track* newTrack		= 0;
    TrackStatus status		= track.status();
    hit_citerator hitBegin	= track.hit_list_begin();
    hit_citerator hitEnd	= track.hit_list_end();
    do
    {
	// move begin backwards to the preceding active layer.
	// Include any leading material by going back again until
	// an active layer is found and then go forwards one
	while (begin != layers->begin() && (**(--begin)).isInactive()) ;
	while (begin != layers->begin() && (**(--begin)).isInactive()) ;
	if (begin != layers->begin()) ++begin;

	if (begin == layers->begin())
	{
	    switch (status)
	    {
	    case truncated:    
		status = truncated;
		break;
	    case segment:
	    case long_segment:
	    case pendingTRT:
		status = pendingTRT;
		break;
	    case secondary:
	    case primary:    
		status = primary;
		break;	
	    default:
		ATH_MSG_WARNING( " unexpected Track::extrapolate_inwards " );
		delete layers;
		return 0;
	    };
	}
	
	hit_list* hits	= associateSilicon(begin,end,false,track);
	end = begin;
	for (hit_citerator h = hitBegin; h != hitEnd; ++h)
	    hits->push_back(new HitOnTrack(**h));

	newTrack = m_trackBuilder->trackFromHits(status,hits,track);
	if (newTrack)
	{
	    // keep newTrack provided it adds cluster(s)
	    bool haveCluster = false;
	    for (hit_citerator h = newTrack->hit_list_begin();
		 h != newTrack->hit_list_end();
		 ++h)
	    {
		if ((**h).isCluster()) haveCluster = true;
		if ((**h).status() != scatterer) continue;
		if (haveCluster)
		{
		    delete extrapolatedTrack;
		    extrapolatedTrack = newTrack;
		    hitBegin	= newTrack->hit_list_begin();
		    hitEnd	= newTrack->hit_list_end();
		}
		else
		{
		    delete newTrack;
		    newTrack = 0;
		}
		break;
	    }
	}
    } while (newTrack && begin != layers->begin());

    delete layers;
    return extrapolatedTrack;
}

Track*
TrackFollower::extrapolate_outwards (const Track& track) const
{
    // extrapolate outwards from last assigned hit (to last SCT layer)
    ATH_MSG_DEBUG( "extrapolate_outwards"
		   << std::setiosflags(std::ios::fixed)
		   << " from layer r,z " << std::setw(7) << std::setprecision(2)
		   << (**track.hit_list_rbegin()).position().perp() << "  "
		   << std::setw(7) << std::setprecision(2)
		   << (**track.hit_list_rbegin()).position().z() );

    // TODO   fix status from LayerAllocator  laynum = 1/2 pendingTRT else secondary
    TrackStatus status = track.status();
    switch (status)
    {
    case segment:
	status = secondary;
	break;
    case long_segment:
    case truncated:
    case pendingTRT:
	status = pendingTRT;
	break;	
    default:
	ATH_MSG_WARNING( " unexpected Track::extrapolate_outwards " );
	return 0;
    };
    // make a vector of all layers intersected by the track
    const PerigeeParameters&	perigee	= track.perigee_parameters();
    layer_vector*		layers	= m_layerAllocator->siliconLayers(perigee.position().perp(),
									  perigee.z(),
									  perigee.cotTheta());

    // find iterator to layer containing the last HitOnTrack measurement
    hit_citerator h = track.hit_list_end();
    while (! (**(--h)).isPositionMeasurement()) ;
    layer_iterator	begin	= end_layer(layers->begin(),layers->end(),**h);

    // extrapolate from next layer to end of SCT
    hit_list*		hits	= associateSilicon(begin,layers->end(),true,track);

    // push existing hits at front
    for (hit_riterator h = track.hit_list_rbegin();
	 h != track.hit_list_rend();
	 ++h)
	hits->push_front(new HitOnTrack(**h));

    delete layers;
    return m_trackBuilder->trackFromHits(status,hits,track);
}

Track*
TrackFollower::fast_interpolate_outwards (const Track& track) const
{
    // interpolate to give a track segment outwards from the first hit
    ATH_MSG_DEBUG( "fast_interpolate_outwards"
		   << std::setiosflags(std::ios::fixed)
		   << " from layer r,z " << std::setw(7) << std::setprecision(2)
		   << (**track.hit_list_begin()).position().perp() << "  "
		   << std::setw(7) << std::setprecision(2)
		   << (**track.hit_list_begin()).position().z() );

    // make a vector of all layers intersected by the track
    const PerigeeParameters&	perigee	= track.perigee_parameters();
    layer_vector*		layers	= m_layerAllocator->siliconLayers(perigee.position().perp(),
									  perigee.z(),
									  perigee.cotTheta());

    // find iterators to layers containing the first/last HitOnTrack's
    layer_iterator	begin	= begin_layer(layers,**track.hit_list_begin());
    layer_iterator	end	= end_layer(begin,layers->end(),**track.hit_list_rbegin());

    // interpolate to give a segment
    hit_list*		hits	= associateSilicon(begin,end,false,track);
    delete layers;
    return m_trackBuilder->trackFromHits(fast_segment,hits,track);
}

Track*
TrackFollower::interpolate (const Track& track) const
{
    // interpolate to give a track segment joining the first and last hits
    ATH_MSG_DEBUG( "interpolate"
		   << std::setiosflags(std::ios::fixed)
		   << " from layer r,z " << std::setw(7) << std::setprecision(2)
		   << (**track.hit_list_begin()).position().perp() << "  "
		   << std::setw(7) << std::setprecision(2)
		   << (**track.hit_list_begin()).position().z()
		   << " to layer r,z " << std::setw(7) << std::setprecision(3)
		   << (**track.hit_list_rbegin()).position().perp() << "  "
		   << (**track.hit_list_rbegin()).position().z() );

    // make a vector of all layers intersected by the track
    const PerigeeParameters&	perigee	= track.perigee_parameters();
    layer_vector*		layers	= m_layerAllocator->siliconLayers(perigee.position().perp(),
									  perigee.z(),
									  perigee.cotTheta());

    // find iterators to layers containing the first/last HitOnTrack's
    layer_iterator	begin	= begin_layer(layers,**track.hit_list_begin());
    layer_iterator	end	= end_layer(begin,layers->end(),**track.hit_list_rbegin());

    // interpolate to give a segment joining first to last hit
    hit_list*		hits	= associateSilicon(begin,end,false,track);
    delete layers;
    return m_trackBuilder->trackFromHits(segment,hits,track);
}

Track*
TrackFollower::interpolate_from_beam_spot (const Track& track) const
{
    // interpolate to give a long_segment joining beam-line to last hit
    ATH_MSG_DEBUG( "interpolate_from_beam_spot"
		   << std::setiosflags(std::ios::fixed)
		   << " to layer r,z " << std::setw(7) << std::setprecision(3)
		   << (**track.hit_list_rbegin()).position().perp() << "  "
		   << (**track.hit_list_rbegin()).position().z()	);

    // make a vector of all layers intersected by the track
    const PerigeeParameters&	perigee	= track.perigee_parameters();
    layer_vector*		layers	= m_layerAllocator->siliconLayers(perigee.position().perp(),
									  perigee.z(),
									  perigee.cotTheta());

    // find iterator to layer containing the last HitOnTrack
    layer_iterator	end	= end_layer(layers->begin(),layers->end(),**track.hit_list_rbegin());

    // interpolate to give a long_segment joining beam-line to last hit
    hit_list*		hits	= associateSilicon(layers->begin(),end,false,track);
    delete layers;
    return m_trackBuilder->trackFromHits(long_segment,hits,track);
}

void
TrackFollower::setDataContainers (const InDet::SiClusterContainer* pixelContainer,
				  const InDet::SiClusterContainer* sctContainer)
{
    delete m_clusterMap;
    m_clusterMap =  new SiliconClusterMap(pixelContainer,sctContainer);
    if (msgLvl(MSG::VERBOSE))
    {
	msg(MSG::VERBOSE) << "setDataContainer: setting SiClusterContainer for ";
	if (pixelContainer)	msg() << "pixel ";
	if (sctContainer)	msg() << "SCT ";
	msg() << endmsg;
    }
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

hit_list*
TrackFollower::associateSilicon (std::vector<SiliconLayer*>::iterator	begin,
				 std::vector<SiliconLayer*>::iterator	end,
				 bool					extrapolate,
				 const Track&				track) const
{
    // create an empty hit_list
    hit_list* hits							= new hit_list;

    // current, extrapolated (i.e. following) and nextFitted (at track hit) intersections
    const Trk::TrackSurfaceIntersection* intersection			= 0;
    const Trk::TrackSurfaceIntersection* extrapolatedIntersection	= 0;
    const Trk::TrackSurfaceIntersection* nextFittedIntersection		= 0;
    double nextFittedRadius						= 0.;
    parameter_const_iterator paramIter					= track.parameters_end();

    if (extrapolate)
    {
	intersection	= new Trk::TrackSurfaceIntersection((**track.parameters_rbegin()).position(),
							    (**track.parameters_rbegin()).direction(),
							    0.);
	// ATH_MSG_DEBUG(" extrapolation from radius " << intersection->position().perp() );
	
    }
    else
    {
	intersection	= new Trk::TrackSurfaceIntersection(track.perigee_parameters().position(),
							    track.perigee_parameters().direction(),
							    0.);
	if (track.parameters_begin() != track.parameters_end())
	{
	    paramIter			= track.parameters_begin();
	    nextFittedIntersection	= new Trk::TrackSurfaceIntersection((**paramIter).position(),
									    (**paramIter).direction(),
									    0.);
	    nextFittedRadius		= (**paramIter).position().perp();
	    // ATH_MSG_DEBUG(" extrapolation from radius " << intersection->position().perp()
	    // 		  ",  up to nextFitted radius " << nextFittedIntersection->position().perp() );
	}
    }

    const AmgSymMatrix(5)& covariance	=  track.perigee_parameters().covariance();
    bool positiveHemisphere		= (track.perigee_parameters().direction().z() > 0.);
    double qOverP			= track.perigee_parameters().qOverP();
    Amg::Vector3D startPosition	= track.perigee_parameters().position();
    Amg::Vector3D startDirection	= track.perigee_parameters().direction();

    // associate SiliconTracker one layer at a time
    for (std::vector<SiliconLayer*>::const_iterator l = begin; l != end; ++l)
    {
	if (! (**l).surface(positiveHemisphere))	continue;
	extrapolatedIntersection	= m_intersector->intersectSurface(*(**l).surface(positiveHemisphere),
									  intersection,
									  qOverP);
	if (! extrapolatedIntersection)			continue;
	delete intersection;
	intersection			= extrapolatedIntersection;

	// when appropriate, update to outgoing parameters at fitted measurement
	if (nextFittedIntersection && intersection->position().perp() > nextFittedRadius)
	{
	    extrapolatedIntersection	= m_intersector->intersectSurface(*(**l).surface(positiveHemisphere),
									  nextFittedIntersection,
									  qOverP);
	    if (extrapolatedIntersection)
	    {
		delete intersection;
		intersection		= extrapolatedIntersection;
		// ATH_MSG_DEBUG (" at layer with radius " << intersection->position().perp()
		// 		   << " after extrapolation from nextFittedIntersection " );
	    }
	    if (++paramIter != track.parameters_end())
	    {
		delete nextFittedIntersection;
		nextFittedIntersection	= new Trk::TrackSurfaceIntersection((**paramIter).position(),
									    (**paramIter).direction(),
									    0);
		nextFittedRadius	= (**paramIter).position().perp();
	    }
	    else
	    {
		delete nextFittedIntersection;
		nextFittedIntersection	= 0;
	    }
	}

	// use (m_sigmaSquared * covariance) for width of predicted window about intersect
	Amg::Vector3D offset	= intersection->position() - startPosition;
	double sinTheta		= intersection->direction().perp();
	double derivAngle	= offset.perp();
	double derivPt		= (offset.y()*startDirection.x() - offset.x()*startDirection.y()) *
				  sinTheta / qOverP;
	double transverseWidth	= (covariance(0,0) +
				   covariance(2,0) * 2. * derivAngle +
				   covariance(2,2) * derivAngle * derivAngle +
				   covariance(4,0) * 2. * derivPt +
				   covariance(4,2) * 2. * derivAngle * derivPt +
				   covariance(4,4) * derivPt * derivPt) * m_sigmaSquared;
	if (transverseWidth > 0.)
	{
	    transverseWidth = std::sqrt(transverseWidth);
	}
	else
	{
	    transverseWidth = 0.;
	}

	derivAngle		*= -1./(sinTheta*sinTheta);
	double longitudinalWidth=  (covariance(1,1) +
				    covariance(3,1) * 2. * derivAngle +
				    covariance(3,3) * derivAngle * derivAngle) * m_sigmaSquared;
	if (longitudinalWidth > 0.)
	{
	    longitudinalWidth	= std::sqrt(longitudinalWidth);
	    if (! (**l).isBarrel()) longitudinalWidth =
					std::abs(longitudinalWidth * sinTheta/intersection->direction().z());
	}
	else
	{
	    longitudinalWidth	= 0.;  
	}

	Amg::Vector3D transverseDirection	= intersection->direction() / sinTheta;
	LayerPredictor predictor(hits,
				 **l,
				 intersection->position(),
				 transverseDirection,
				 transverseWidth,
				 longitudinalWidth);
	m_layerAssociator->associateClusters(m_clusterMap,
					     intersection->position(),
					     transverseDirection,
					     transverseWidth,
					     longitudinalWidth,
					     predictor.begin(),
					     predictor.end());
	    
	if (predictor.insertHits(m_clusterProperties) > 0)
	{
	    predictor.insertScatterer();
	    //  		if (predictor.number_of_holes() > 0) predictor.print();
	}
    }

    // clear algtool state before return
    // m_layerAssociator->clear();
    delete intersection;
    delete nextFittedIntersection;
    return hits;
}

TrackFollower::layer_iterator
TrackFollower::begin_layer (layer_vector* layers, const HitOnTrack& hit) const
{
    double		r	= hit.position().perp();
    double		z	= std::abs(hit.position().z());
    layer_iterator	begin	= layers->begin();
    layer_iterator	end	= layers->end();
    if (hit.isBarrel())
    {
	while (begin != end
	       && ((**begin).rMax() < r
		   || ((**begin).isEndcap() && (**begin).zMax() < z))) ++begin;
    }
    else
    {
	while (begin != end
	       && ((**begin).zMax() < z
		   || ((**begin).isBarrel() && (**begin).rMax() < r))) ++begin;
    }
    return begin;
}

TrackFollower::layer_iterator
TrackFollower::end_layer (layer_iterator begin,
			  layer_iterator end,
			  const HitOnTrack& hit) const
{
    if (begin != end) ++begin;
    if (begin == end) return end;
    
    if (hit.isBarrel())
    {
	double r = hit.position().perp();
	while (--end != begin
	       && ((**end).rMin() > r
		   || ((**end).isEndcap() && (**end).rMax() > r))) ;
    }
    else
    {
	double z = std::abs(hit.position().z());
	while (--end != begin
	       && ((**end).zMin() > z
		   || ((**end).isBarrel() && (**end).zMax() > z))) ;
    }
    return ++end;
}


