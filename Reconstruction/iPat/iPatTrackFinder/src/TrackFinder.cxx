/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 the iPatTrackFinder InnerDetector pattern recognition algorithm
  - finds all tracks satisfying quality specifications above minPt

 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>
#include "GaudiKernel/SystemOfUnits.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "VxVertex/RecVertex.h"
#include "iPatInterfaces/IAssignedHits.h"
#include "iPatInterfaces/ICandidateBuilder.h"
#include "iPatInterfaces/ICombinationMaker.h"
#include "iPatInterfaces/IFinderConfiguration.h"
#include "iPatInterfaces/ITrackBuilder.h"
#include "iPatInterfaces/ITrackFollower.h"
#include "iPatInterfaces/ITrackManager.h"
#include "iPatTrack/Track.h"
#include "iPatTrackFinder/Point.h"
#include "iPatTrackFinder/PrimaryCandidate.h"
#include "iPatTrackFinder/SecondaryCandidate.h"
#include "iPatTrackFinder/TrackFinder.h"
#include "iPatUtility/VertexRegion.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TrackFinder::TrackFinder(const std::string&	type,
			 const std::string&	name,
			 const IInterface*	parent)
    :   AthAlgTool		(type, name, parent),
	m_assignedHits		("AssignedHits/AssignedHits"),
	m_beamCondSvc		("BeamCondSvc", name),
	m_candidateBuilder	("CandidateBuilder/CandidateBuilder"),
	m_combination		("CombinationMaker/CombinationMaker"),
	m_finderTolerances	("FinderTolerances/FinderTolerances"),
	m_trackBuilder		("TrackBuilder/TrackBuilder"),
	m_trackFollower		("TrackFollower/TrackFollower"),
	m_trackManager		("TrackManager/TrackManager"),
	m_minPt			(0.),
	m_perigeeVertex		(Amg::Vector3D(0.,0.,0.)),
	m_printLevel		(0),
	m_searchForPrimaries	(true),
	m_searchForSecondaries	(true),
	m_searchForVertexRegion	(true),
	m_maxPt			(0),
	m_vertex		(0)
{
    declareInterface<ITrackFinder>(this);
    declareProperty("AssignedHits",		m_assignedHits);
    declareProperty("CandidateBuilder",		m_candidateBuilder);
    declareProperty("CombinationMaker",		m_combination);
    declareProperty("FinderConfiguration",	m_finderTolerances);
    declareProperty("TrackBuilder",		m_trackBuilder);
    declareProperty("TrackFollower",		m_trackFollower);
    declareProperty("TrackManager",		m_trackManager);
    declareProperty("searchForPrimaries", 	m_searchForPrimaries);
    declareProperty("searchForSecondaries",	m_searchForSecondaries);
    declareProperty("searchForVertexRegion",	m_searchForVertexRegion);
}

TrackFinder::~TrackFinder (void)
{}
				    
//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TrackFinder::initialize()
{
    ATH_MSG_INFO( "TrackFinder::initialize()" );

    // check BeamCondSvc service available
    if (m_beamCondSvc.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve service " << m_beamCondSvc );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved service " << m_beamCondSvc );
    }

    // get the Tools
    if (m_assignedHits.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_assignedHits );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_assignedHits );
    }
    if (m_candidateBuilder.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_candidateBuilder );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_candidateBuilder );
    }
    if (m_combination.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_combination );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_combination );
    }
    if (m_finderTolerances.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_finderTolerances );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_finderTolerances );
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
    if (m_trackFollower.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackFollower );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackFollower );
    }
    if (m_trackManager.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackManager );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackManager );
    }

    return StatusCode::SUCCESS;
}

StatusCode
TrackFinder::finalize()
{
    ATH_MSG_DEBUG( "TrackFinder::finalize()" );

    delete m_vertex;
    
    return StatusCode::SUCCESS; 
}

void
TrackFinder::setProperties (double		maxSecondaryImpact,
			    double		minPt,
			    int			printLevel,
			    const VertexRegion&	vertexRegion)
{
    m_minPt			= minPt;
    m_printLevel		= printLevel;
    m_finderTolerances->setProperties(maxSecondaryImpact,
				      minPt,
				      printLevel,
				      vertexRegion);
    m_trackManager->setProperties(m_printLevel);
    m_vertexRegionCentre	= vertexRegion.centre();
    m_vertexRegionWidth		= vertexRegion.width();
}

TrackList::track_list*
TrackFinder::trackSearch (const std::vector<Trk::SpacePoint*>& spacePoints)
{
    m_trackManager->newTrackSearch();

    // book-keeping of used hits
    m_combination->setProperties(spacePoints,m_vertexRegionCentre);

    // define vertex with beam position from conditions db
    vertexRegionDefinition();

    // set vertex region
    m_combination->vertex_region(m_vertexRegionCentre.z(),m_vertexRegionWidth.z());
    //m_fitter->transverse_vertex_width(m_vertexRegionWidth.perp());

    // define the vertex region if a stiff track can be found
    if (m_searchForVertexRegion) searchForVertexRegion();

    // set minPt
    m_combination->set_tolerances(m_minPt);

    // search for small impact parameter tracks with hits in the innermost layers
    if (m_searchForPrimaries) searchForPrimaries();

    // now get secondaries (missing inner layers, large impact parameters)
    if (m_searchForSecondaries) searchForSecondaries();

    // trackList cleanup and final track fitting
    m_trackManager->finalizeTrackFitting();

    // be nice to the other reco Algs - release some memory
    m_assignedHits->clear();
    m_combination->clear();

    return m_trackManager->trackList();
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
TrackFinder::doublePixelSearch (void)
{
    PrimaryCandidate candidate(m_finderTolerances, m_perigeeVertex);
    while (m_combination->next_outer_point())
    {
	m_trackManager->resetCurrentTracks();
	candidate.setOuterPoint (*m_combination->outer_point());
	while (m_combination->next_inner_point())
	{ 
	    if (candidate.addInnerPixel(*m_combination->inner_point()))
	    {
		m_combination->intermediatePhiRange();
		bool compatible = false;
		while (!compatible && m_combination->next_intermediate_point())
		{
		    if (candidate.isPixelCompatible(*m_combination->intermediate_point()))
		    {
			compatible = true;
			const Track& twoPoint =
			    m_candidateBuilder->twoPointTrack(candidate.perigee(),
							      *m_combination->inner_point(),
							      *m_combination->outer_point(),
							      m_combination->pixelInterchange(),
							      *m_vertex);
			if (twoPoint.status() == two_point)
			    threePointSearch(twoPoint);
		    }
		}
	    }	
	}   // end loop on inner points

	m_trackManager->saveCurrentTracks();
    }  // end loop on outer points

    // clean out duplicates (with too many shared hits)
    m_trackManager->uniquenessCheck();
    if (m_printLevel > 2) std::cout << "   after doublePixel search: "
				    << m_trackManager->trackListSize() << " tracks,  "
				    << m_assignedHits->numberOfClusters() << " clusters "  << std::endl;
    
}

void
TrackFinder::fastSegmentSearch (void)
{
    if (! m_combination->vertexSearch(m_minPt)) return;
    PrimaryCandidate candidate(m_finderTolerances,
			       m_perigeeVertex);
    
    while (m_combination->next_outer_point())
    {
	m_trackManager->resetCurrentTracks();
	candidate.setOuterPoint (*m_combination->outer_point());
	while (m_combination->next_inner_point())
	{
	    if (candidate.addInnerPixel(*m_combination->inner_point()))
	    {
		m_combination->intermediatePhiRange();
		bool compatible = false;
		while (!compatible && m_combination->next_intermediate_point())
		{
		    if (candidate.isPointCompatible(*m_combination->intermediate_point()))
		    {
			compatible = true;
			const Track& twoPoint =
			    m_candidateBuilder->twoPointTrack(candidate.perigee(),
							      *m_combination->inner_point(),
							      *m_combination->outer_point(),
							      m_combination->pixelInterchange(),
							      *m_vertex);
			Track*	track	= 0;
			if (twoPoint.status() == two_point)
			{
			    track = m_trackFollower->fast_interpolate_outwards(twoPoint);
			    if (track != 0 && track->status() == fast_segment)
			    {
				m_trackManager->addCurrentSegment(track);
			    }
			    else
			    {
				delete track;
			    }
			}
		    }
		}
	    }	
	}   // end loop on inner points

	m_trackManager->saveCurrentTracks();
    }  	// end loop on outer points

    if (m_printLevel > 2) std::cout << "   after fastSegment search: "
				    << m_trackManager->trackListSize() << " tracks,  "
				    << m_assignedHits->numberOfClusters() << " clusters "  << std::endl;
    
}

void
TrackFinder::searchForPrimaries (void)
{
    // set 1mm vertex width for primary search strategies
    //m_fitter->transverse_vertex_width(1.0*Gaudi::Units::mm);

    // loop over vertices
    if (m_zVertices.size() == 0)
    {
	if (m_printLevel > 2)
	{
	    std::cout << "   set default vertex region " << std::endl;
	}
	m_finderTolerances->vertexZLimits();
	if (m_combination->doublePixelPrimarySearch())	doublePixelSearch();
	if (m_combination->singlePixelSearch())		singlePixelSearch();
    }
    else
    {
	for (std::vector<double>::iterator z = m_zVertices.begin();
	     z != m_zVertices.end();
	     ++z)
	{
	    if (m_printLevel > 2)
	    {
		std::cout << "   set vertex region around z = " << *z << std::endl;
	    }
	    m_combination->vertex_region(*z, 5.0*Gaudi::Units::mm);
	    m_finderTolerances->vertexZLimits(*z);
	    if (m_combination->doublePixelPrimarySearch())	doublePixelSearch();
	    if (m_combination->singlePixelSearch())		singlePixelSearch();
	}
    }

    // set narrow width for truncated search
    //m_fitter->transverse_vertex_width(0.1*Gaudi::Units::mm);

    if (m_zVertices.size() == 0)
    {
	if (m_combination->doublePixelTruncatedSearch()) doublePixelSearch();
    }
    else
    {
	for (std::vector<double>::iterator z = m_zVertices.begin();
	     z != m_zVertices.end();
	     ++z)
	{
	    if (m_zVertices.size() > 1)
	    {
		if (m_printLevel > 2)
		{
		    std::cout << "   set vertex region around z = " << *z << std::endl;
		}
		m_combination->vertex_region(*z, 5.0*Gaudi::Units::mm);
		m_finderTolerances->vertexZLimits(*z);
	    }
	    if (m_combination->doublePixelTruncatedSearch()) doublePixelSearch();
	}
    }
}

void
TrackFinder::searchForSecondaries (void)
{
    // search for secondary tracks (mainly using SP's from SCT strips)
    if (!m_combination->secondarySearch()) return;
    m_finderTolerances->vertexZLimits();
    double centre = 0.5*(m_finderTolerances->maxVertexZ() + m_finderTolerances->minVertexZ());
    double width  = 0.5*(m_finderTolerances->maxVertexZ() - m_finderTolerances->minVertexZ());
    m_combination->vertex_region(centre,width);
    SecondaryCandidate candidate(m_perigeeVertex,
				 m_finderTolerances,
				 m_zVertices.begin(),
				 m_zVertices.end());
    
    while (m_combination->next_outer_point())
    {
	m_trackManager->resetCurrentTracks();
	candidate.setOuterPoint (*m_combination->outer_point());
	while (m_combination->next_inner_point())
	{
	    if (candidate.addInnerPoint(*m_combination->inner_point()))
	    {
		m_combination->intermediatePhiRange();
		while (m_combination->next_intermediate_point())
		{
		    if (candidate.addIntermediatePoint(*m_combination->intermediate_point()))
		    {
			const Track& threePoint =
			    m_candidateBuilder->threePointTrack(candidate.perigee(),
								*m_combination->inner_point(),
								*m_combination->intermediate_point(),
								*m_combination->outer_point());
			if (threePoint.status() == three_point)
			    m_trackManager->addCurrentSecondary(m_trackFollower->interpolate(threePoint));
		    }
		}
	    }
	}	// end loop on inner points
	
	m_trackManager->saveCurrentTracks();	
    }

    // clean out duplicates (with too many shared hits)
    m_trackManager->uniquenessCheck();
    if (m_printLevel > 2) std::cout << "   after secondaries search: "
                                    << m_trackManager->trackListSize() << " tracks,  "
                                    << m_assignedHits->numberOfClusters() << " clusters "  << std::endl;
}
	    
void
TrackFinder::searchForVertexRegion (void)
{
    // simple algorithm:
    //   take z-vertex = any stiff track z-vertex with pT above sliding threshold;
    //   otherwise vertex region left unset.
    for (std::vector<double>::reverse_iterator z = m_zVertices.rbegin();
	 z != m_zVertices.rend();
	 ++z)
	m_zVertices.pop_back();

    // start with full vertex region for z-range with narrow (1mm) transverse vertex width
    m_finderTolerances->vertexZLimits();
    //m_fitter->transverse_vertex_width(1.0*Gaudi::Units::mm);

    // set pt thresholds for z-vertex search
    double      ptThresh[2]     = {4.*Gaudi::Units::GeV, 3.*Gaudi::Units::GeV};
    std::vector<double> thresholds(&ptThresh[0], &ptThresh[2]);
    std::vector<double>::iterator threshold_iterator	= thresholds.begin();
    m_maxPt			= 1000.*Gaudi::Units::GeV;
    
    // set min_pt at threshold and try to pick up a stiff track
    while (threshold_iterator != thresholds.end())
    {
	double minPt	= *threshold_iterator;
	// skip last (low) threshold unless nothing found yet
	if (++threshold_iterator == thresholds.end()
	    && m_trackManager->trackListSize() > 2) break;
	m_finderTolerances->minPt(minPt);
	if (m_combination->vertexSearch(minPt)) vertexDefinitionSearch();
	m_maxPt	= minPt;
    }

    // clean out the fast_segments before full track finding starts
    m_trackManager->clearTrackList();

    if (m_printLevel > 2)
    {
	int prec	= std::cout.precision();
	std::cout << "   found " << m_zVertices.size() << " vertices";
	for (std::vector<double>::const_iterator z = m_zVertices.begin();
	     z != m_zVertices.end();
	     ++z)
	{
	    std::cout << std::setiosflags(std::ios::fixed)
		      << std::setw(8) << std::setprecision(1) << *z;
	}
	std::cout << std::resetiosflags(std::ios::fixed) << std::endl;
	std::cout.precision(prec);
    }

    // restore tolerances for minPt
    m_finderTolerances->minPt(m_minPt);
}

void
TrackFinder::singlePixelSearch (void)
{
    // search for primary and secondary tracks in outer pixel layers + SCT strips.
    // first set origin to be 1 cm cone about stiffest track already found
    if (m_searchForVertexRegion
	&& m_zVertices.size() == 0
	&& m_trackManager->trackListSize())
    {
	double z = m_trackManager->zFirstTrack();
	m_zVertices.push_back(z);
	m_finderTolerances->vertexZLimits(z);
    }
    
    PrimaryCandidate candidate(m_finderTolerances, m_perigeeVertex);
    while (m_combination->next_outer_point())
    {
	m_trackManager->resetCurrentTracks();
	candidate.setOuterPoint (*m_combination->outer_point());
	while (m_combination->next_inner_point())
	{
	    if (candidate.addInnerPixel(*m_combination->inner_point()))
	    {
		m_combination->intermediatePhiRange();
		bool compatible = false;
		while (!compatible && m_combination->next_intermediate_point())
		{
		    if (candidate.isPointCompatible(*m_combination->intermediate_point()))
		    {
			compatible = true;
			const Track& twoPoint = m_candidateBuilder->twoPointTrack
						(candidate.perigee(),
						 *m_combination->inner_point(),
						 *m_combination->outer_point(),
						 m_combination->pixelInterchange(),
						 *m_vertex);
			if (twoPoint.status() == two_point)
			{
			    if (m_combination->intermediate_point()->isPixel())
			    {
				assert(m_combination->has_vertex_points());
				threePointSearch(twoPoint);
			    }
			    else
			    {
				Track* track_segment = m_trackFollower->interpolate(twoPoint);
				if (track_segment != 0)
				{
				    if (m_combination->has_vertex_points())
				    {
					threePointSearch(*track_segment);
					delete track_segment;
				    }
				    else
				    {
				        Track* track =
					    m_trackFollower->interpolate_from_beam_spot(*track_segment);
					if (track == 0)
					{
					    m_trackManager->addCurrentTrack(track_segment);
					}
					else
					{
					    delete track_segment;
					    m_trackManager->addCurrentTrack(track);
					}
				    }
				}
			    }
			}
		    }
		}
	    }	
	}   // end loop on inner points

	m_trackManager->saveCurrentTracks();
    }  // end loop on outer points

    // clean out duplicates (with too many shared hits)
    m_trackManager->uniquenessCheck();
    if (m_printLevel > 2) std::cout << "   after singlePixel search: "
				    << m_trackManager->trackListSize() << " tracks,  "
				    << m_assignedHits->numberOfClusters() << " clusters "  << std::endl;
    
}

void
TrackFinder::threePointSearch (const Track& track)
{
    m_combination->set_vertex_points(track.perigee_parameters());
    while (m_combination->next_vertex_point()) 
    {
	const Track& threePoint = 
	    m_candidateBuilder->threePointTrack(track.perigee_parameters(),
						*m_combination->vertex_point(),
						*m_combination->inner_point(),
						*m_combination->outer_point(),
						m_combination->pixelInterchange());
	if (threePoint.status() == three_point)
	    m_trackManager->addCurrentTrack(m_trackFollower->interpolate_from_beam_spot(threePoint));
    }
}

void
TrackFinder::vertexDefinitionSearch (void)
{
    unsigned size = m_trackManager->trackListSize();
    PrimaryCandidate candidate(m_finderTolerances, m_perigeeVertex);
    
    while (m_combination->next_outer_point())
    {
	m_trackManager->resetCurrentTracks();
	candidate.setOuterPoint (*m_combination->outer_point());
	while (m_combination->next_inner_point())
	{
	    if (candidate.addInnerPixelAtNewVtx(*m_combination->inner_point(),
						m_zVertices.begin(),
						m_zVertices.end()))
	    {
		m_combination->intermediatePhiRange();
		bool compatible = false;
		while (!compatible && m_combination->next_intermediate_point())
		{
		    if (candidate.isPointCompatible(*m_combination->intermediate_point()))
		    {
			compatible = true;
			const Track& twoPoint =
			    m_candidateBuilder->twoPointTrack(candidate.perigee(),
							      *m_combination->inner_point(),
							      *m_combination->outer_point(),
							      m_combination->pixelInterchange(),
							      *m_vertex);
			if (twoPoint.status() == two_point
			    && twoPoint.perigee_parameters().abs_pt() < m_maxPt)
			{
			    Track* track = m_trackFollower->fast_interpolate_outwards(twoPoint);
			    if (track != 0 && track->status() == fast_segment)
			    {
				m_trackManager->addCurrentSegment(track);
			    }
			    else
			    {
				delete track;
			    }
			}
		    }
		}
	    }	
	}   // end loop on inner points

	m_trackManager->saveCurrentTracks();
	if (m_trackManager->trackListSize() > size)
	{
	    bool newVertex	= true;
	    size		= m_trackManager->trackListSize();
	    double z		= m_trackManager->zLastTrack();
	    for (std::vector<double>::const_iterator v = m_zVertices.begin();
		 v != m_zVertices.end();
		 ++v)
		if (std::abs(*v - z) < 5.0*Gaudi::Units::mm) newVertex = false;	// TODO!! rm hardwired number

	    if (newVertex) m_zVertices.push_back(z);
	    //	(*m_trackManager->trackList()->rbegin())->print_parameters();
	}
    }  	// end loop on outer points

    if (m_printLevel > 2) std::cout << "   after vertexDefinition search: "
				    << m_trackManager->trackListSize() << " tracks,  "
				    << m_assignedHits->numberOfClusters() << " clusters "  << std::endl;
    
}

void
TrackFinder::vertexRegionDefinition (void)
{
    // get beam position from conditions db
    delete m_vertex;
    Amg::Vector3D beamPosition	= m_beamCondSvc->beamPos();

    // vertex for transverse vertex constraint
    // assume flat distribution for error (only x and y components will be used)
    Amg::MatrixX covariance(3,3);
    covariance.setZero();
    double sigmaSquared = (m_finderTolerances->maxPrimaryImpact() *
			   m_finderTolerances->maxPrimaryImpact())/3.;
    covariance(0,0)	= sigmaSquared;
    covariance(1,1)	= sigmaSquared;
    covariance(0,2)	= sigmaSquared;
    m_vertex		= new Trk::RecVertex(beamPosition,covariance);
    
//     std::cout << " beam vertex " << beamPosition << std::endl;
//     	      << "  beamSigma " << m_beamCondSvc->beamSigma(2) << std::endl;

}
