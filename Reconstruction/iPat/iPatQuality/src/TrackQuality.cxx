/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 apply centralised track quality cuts
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "iPatQuality/TrackQuality.h"
#include "iPatTrack/Track.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TrackQuality::TrackQuality	(const std::string&	type,
				 const std::string&	name,
				 const IInterface*	parent)
    :   AthAlgTool	     (type, name, parent),
	m_maxPlanarHoles     (0),
	m_minEffectiveLayers (0),
	m_minFitProbability  (0.),
	m_minPlanarClusters  (0),
	m_minPt	             (0.),
	m_status	     (segment)
{
    declareInterface<ITrackQuality>(this);
}

TrackQuality::~TrackQuality (void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TrackQuality::initialize()
{
    // print where you are
    ATH_MSG_DEBUG( "initialize()" << " - package version " << PACKAGE_VERSION );

    return StatusCode::SUCCESS;
}

StatusCode
TrackQuality::finalize()
{
    ATH_MSG_DEBUG( "TrackQuality::finalize()" );

    return StatusCode::SUCCESS; 
}
	
bool
TrackQuality::aboveMinPt (const PerigeeParameters& perigee) const
{
    if (perigee.abs_pt() > m_minPt) return true;
    
    // otherwise cut at 3 sigma
    double sinThetaSqInv	= 1. + perigee.cotTheta()*perigee.cotTheta();
    //// CHECK covariance indexing
    double maxPt		= 1./ (std::abs(perigee.qOverPt()) -
				       3.0*sqrt(sinThetaSqInv*perigee.covariance()(4,4)));
    if (maxPt < m_minPt) return false;
    return true;
}
	
bool
TrackQuality::goodCandidate (const FitQuality& fitQuality,
			     const PerigeeParameters& perigee) const
{
    double minFitProbability	= 0.001;
    double minPt	       	= 0.90*m_minPt;  // loose cut
    
    if (fitQuality.fit_code()		> 0
	|| fitQuality.fit_probability()	< minFitProbability
	|| perigee.abs_pt()		< minPt)
    {
	return false;
    }
    else
    {
	return true;
    }
}

bool
TrackQuality::goodTrack (TrackStatus		status,
			 const FitQuality&	fitQuality,
			 const HitQuality&	hitQuality) const
{
    setStatus(status);
    if ((hitQuality.number_drift_hits()		== 0
	 && hitQuality.number_effective_layers()      	< m_minEffectiveLayers)
	|| hitQuality.number_planar_clusters()	      	< m_minPlanarClusters
	|| hitQuality.number_planar_holes()   		> m_maxPlanarHoles
	|| fitQuality.fit_code()	       		> 0
	|| fitQuality.fit_probability()	      	< m_minFitProbability)
    {
	return false;
    }
    else
    {
	return true;
    }
}

int
TrackQuality::maxReject (const HitQuality& hitQuality) const
{ 
    int maxHits		= hitQuality.number_planar_clusters() - m_minPlanarClusters;
    int maxHoles	= m_maxPlanarHoles - hitQuality.number_planar_holes();
    if (maxHoles < maxHits) return maxHoles;
    return maxHits;
}

void
TrackQuality::minPt (double minPt)
{ m_minPt = minPt; }
    
void
TrackQuality::print (const FitQuality& fitQuality, const PerigeeParameters& perigeeParameters) const
{
    if (fitQuality.fit_code() == 0)
    {
	if (fitQuality.fit_probability() < m_minFitProbability)
	    ATH_MSG_DEBUG( " QC: fit_probability " << fitQuality.fit_probability()
			   << "   minimum " << m_minFitProbability );
	if (perigeeParameters.abs_pt() < m_minPt)
	    ATH_MSG_DEBUG( " QC: absPt" << perigeeParameters.abs_pt()
			   << "   minimum " << m_minPt );
    }
    else
    {
	ATH_MSG_DEBUG( " QC: fit_code " << fitQuality.fit_code() );
    }
}
	
void
TrackQuality::print (const HitQuality& hitQuality) const
{
    if (hitQuality.number_effective_layers() <	m_minEffectiveLayers)
	ATH_MSG_DEBUG( " QC: " 
		       << " effective layers "
		       << hitQuality.number_layers()-hitQuality.missing_layers()
		       << "   minimum " << m_minEffectiveLayers );
    if (hitQuality.number_planar_clusters() < m_minPlanarClusters)
	ATH_MSG_DEBUG( " QC: " 
		       << " numberPlanarClusters "
		       << hitQuality.number_planar_clusters()
		       << "   minimum " << m_minPlanarClusters );
    if (hitQuality.number_planar_holes()	> m_maxPlanarHoles)
	ATH_MSG_DEBUG( " QC: " 
		       << " numberPlanarHoles "
		       << hitQuality.number_planar_holes()
		       << "   maximum " << m_maxPlanarHoles );
}

bool
TrackQuality::sufficientHits (TrackStatus				status,
			      HitQuality&				hitQuality,
			      std::list<HitOnTrack*>::const_iterator	hits_begin,
			      std::list<HitOnTrack*>::const_iterator	hits_end) const 
{
    setStatus(status);
    hitQuality.fill(hits_begin,hits_end);
    // standard hit/hole requirements
    if (hitQuality.number_planar_clusters()		< m_minPlanarClusters
	|| hitQuality.number_planar_holes()		> m_maxPlanarHoles
	|| !hitQuality.satisfies_inward_continuity())
    {
	// 	if (!hitQuality.satisfies_inward_continuity()) 
	// 	    std::cout << " QC failure for inwards continuity_check " << std::endl;
	return false;
    }

    // extra requirements when not confirmed by TRT
    else if (hitQuality.number_drift_hits()		== 0
	     && hitQuality.number_effective_layers()	< m_minEffectiveLayers
	     && !hitQuality.satisfies_outward_continuity())
    {
	// 	if (!hitQuality.satisfies_outward_continuity()) 
	// 	    std::cout << " QC failure for outwards continuity_check " << std::endl;
	return false;
    }
    return true;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
TrackQuality::setStatus (TrackStatus status) const
{
    if (status == m_status) return;
    m_status	= status;
    switch (status)
    {
    case two_point:
    case segment:
	m_maxPlanarHoles	= 2;
	m_minPlanarClusters	= 4;
	m_minEffectiveLayers	= 3;
	m_minFitProbability	= 0.001;
// 	m_minPt			= m_minPt;
	break;
    case three_point:
    case long_segment:
	m_maxPlanarHoles	= 3;
	m_minPlanarClusters	= 5;
	m_minEffectiveLayers	= 4;
	m_minFitProbability	= 0.001;
// 	m_minPt			= 0.90*m_minPt;  // loose cut
	break;
    case fast_segment:
	m_maxPlanarHoles	= 1;
	m_minPlanarClusters	= 6;
	m_minEffectiveLayers	= 4;
	m_minFitProbability	= 0.01;
// 	m_minPt			= 0.90*m_minPt;  // loose cut
	break;
    case secondary:
	m_maxPlanarHoles	= 1;
	m_minPlanarClusters	= 5;
	m_minEffectiveLayers	= 3;
	m_minFitProbability	= 0.001;
// 	m_minPt			= 0.;
	break;
    case truncated:
	m_maxPlanarHoles	= 1;
	m_minPlanarClusters	= 5;
	m_minEffectiveLayers	= 4; 
	m_minFitProbability	= 0.001;
// 	m_minPt			= 0.;
	break;
    case pendingTRT:
    case primary:
	m_maxPlanarHoles	= 3;
	m_minPlanarClusters	= 5;
	m_minEffectiveLayers	= 4;
	m_minFitProbability	= 0.001;
// 	m_minPt			= 0.;
	break;
    case muon:
    case combined_muon:
	m_maxPlanarHoles	= 3;
	m_minPlanarClusters	= 2;
	m_minEffectiveLayers	= 2;
	m_minFitProbability	= 0.001;
// 	m_minPt			= 0.;
	break;    
    default:
	ATH_MSG_WARNING( " QC: unexpected TrackStatus " << status );
    };
}
