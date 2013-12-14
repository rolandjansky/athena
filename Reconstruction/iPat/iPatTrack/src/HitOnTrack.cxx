/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 generic hit on track with status, identifier, coordinates etc
 -------------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "iPatTrack/HitOnTrack.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

HitOnTrack::HitOnTrack (void)
    :	m_barrel         	(true),
	m_detectorElement	(0),
	m_identifier      	(Identifier()),
	m_intersection		(0),
	m_materialEffects	(0),
	m_position		(Amg::Vector3D(0.,0.,0.)),
	m_residual		(0.),
	m_rio			(0),
	m_rotBroad		(0),
	m_rotPrecise		(0),
	m_secondResidual	(0.),
	m_secondSigmaBroad     	(0.),
	m_secondSigmaPrecise   	(0.),
	m_shared	       	(false),
	m_sigmaBroad		(0.),
	m_sigmaPrecise		(0.),
	m_sinStereo		(0.),
	m_status		(pending),
	m_surface		(0),
	m_surfaceOwner		(false),
	m_trtThreshold  	(false)
{}

// copy constructor
HitOnTrack::HitOnTrack (const HitOnTrack& h)
    :	m_barrel          	(h.m_barrel),
	m_detectorElement	(h.m_detectorElement),
	m_identifier      	(h.m_identifier),
	m_intersection		(0),
	m_materialEffects	(0),
	m_position		(h.m_position),
	m_residual		(h.m_residual),
	m_rio			(h.m_rio),
	m_rotBroad		(0),
	m_rotPrecise		(0),
	m_secondResidual	(h.m_secondResidual),
	m_secondSigmaBroad	(h.m_secondSigmaBroad),
	m_secondSigmaPrecise	(h.m_secondSigmaPrecise),
	m_shared		(h.m_shared),
	m_sigmaBroad	       	(h.m_sigmaBroad),
	m_sigmaPrecise		(h.m_sigmaPrecise),
	m_sinStereo		(h.m_sinStereo),
	m_status          	(h.m_status),
	m_surface		(h.m_surface),
	m_surfaceOwner	       	(h.m_surfaceOwner),
	m_trtThreshold		(h.m_trtThreshold)
{
    if (h.m_intersection)	m_intersection	= new Trk::TrackSurfaceIntersection(*h.m_intersection);
    if (h.m_materialEffects)	m_materialEffects = h.m_materialEffects->clone();
    if (h.m_rotBroad)		m_rotBroad	= h.m_rotBroad->clone();
    if (h.m_rotPrecise)		m_rotPrecise	= h.m_rotPrecise->clone();
    if (h.m_surfaceOwner)
    {
	m_surface				= h.m_surface->clone();
    }
}

// Silicon 'hole' constructor
HitOnTrack::HitOnTrack (bool                    	barrel,
			const Trk::TrkDetElementBase*	detectorElement,
			const Amg::Vector3D&		position,
			HitStatus               	status)
    : 	m_barrel         	(barrel),
	m_detectorElement	(detectorElement),
	m_identifier      	(Identifier()),
	m_intersection		(0),
	m_materialEffects	(0),
	m_position		(position),
	m_residual		(0.),
	m_rio			(0),
	m_rotBroad		(0),
	m_rotPrecise		(0),
	m_secondResidual	(0.),
	m_secondSigmaBroad     	(0.),
	m_secondSigmaPrecise   	(0.),
	m_shared	       	(false),
	m_sigmaBroad		(0.),
	m_sigmaPrecise		(0.),
	m_sinStereo		(0.),
	m_status		(status),
	m_surface		(&detectorElement->surface()),
	m_surfaceOwner		(false),
	m_trtThreshold  	(false)
{}

// scatterer constructor
HitOnTrack::HitOnTrack (bool			barrel,
			double                  energyDeposit,
			const Amg::Vector3D&	position,
			double                  radiationThickness,
			HitStatus               status)
    :	m_barrel		(barrel),
	m_detectorElement	(0),
	m_identifier      	(Identifier()),
	m_intersection		(0),
	m_materialEffects	(0),
	m_position		(position),
	m_residual		(0.),
	m_rio			(0),
	m_rotBroad		(0),
	m_rotPrecise		(0),
	m_secondResidual	(0.),
	m_secondSigmaBroad     	(0.),
	m_secondSigmaPrecise   	(0.),
	m_shared		(false),
	m_sigmaBroad		(0.),
	m_sigmaPrecise		(0.),
	m_sinStereo		(0.),
	m_status		(status),
	m_trtThreshold		(false)
{
    setSurface(barrel,position);
    std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> typePattern;
    typePattern.set(Trk::MaterialEffectsBase::ScatteringEffects);
    Trk::EnergyLoss* energyLoss	= new Trk::EnergyLoss(-energyDeposit,0.);
    m_materialEffects		= new Trk::MaterialEffectsOnTrack(radiationThickness,
								  energyLoss,
								  *m_surface,
								  typePattern);
}

// SpacePoint pattern recognition constructor 
HitOnTrack::HitOnTrack (const Amg::Vector3D&		position,
                        double                  	sigmaBroad,
                        double                  	sigmaPrecise,
                        double                  	secondSigmaBroad,
                        double                  	secondSigmaPrecise,
                        double                  	sinStereo,
                        double,                  	// unused
			bool                    	barrel,
			const Trk::TrkDetElementBase*	detectorElement,
                        const Identifier&       	identifier,
			const Trk::PrepRawData*		rio,
			const Trk::MeasurementBase*,	// unused
                        HitStatus               	status)
    :	m_barrel		(barrel),
	m_detectorElement	(detectorElement),					
	m_identifier		(identifier),
	m_intersection		(0),
	m_materialEffects	(0),
	m_position		(position),
	m_residual		(0.),
	m_rio			(rio),
	m_rotBroad		(0),
	m_rotPrecise		(0),
	m_secondResidual	(0.),
	m_secondSigmaBroad	(secondSigmaBroad),
	m_secondSigmaPrecise	(secondSigmaPrecise),
	m_shared		(false),
	m_sigmaBroad		(sigmaBroad),
	m_sigmaPrecise		(sigmaPrecise),
	m_sinStereo		(sinStereo),
	m_status		(status),
	m_surface		(&detectorElement->surface(identifier)),
	m_surfaceOwner		(false),
	m_trtThreshold		(false)
{}

// constructor from ROTs
HitOnTrack::HitOnTrack (bool            		barrel,
			const Identifier&		identifier,
			const Trk::PrepRawData*		rio,
			const Trk::MeasurementBase*	rotBroad,
			const Trk::MeasurementBase*	rotPrecise,
			HitStatus			status,
			int				trtThreshold)
    :	m_barrel          	(barrel),
	m_identifier      	(identifier),
	m_intersection		(0),
	m_materialEffects	(0),
	m_residual		(0.),
	m_rio			(rio),
	m_rotBroad		(rotBroad),
	m_rotPrecise		(rotPrecise),
	m_secondResidual	(0.),
	m_secondSigmaBroad     	(0.),
	m_secondSigmaPrecise   	(0.),
	m_shared		(false),
	m_sinStereo		(0.),
	m_status          	(status),
	m_surfaceOwner		(false),
	m_trtThreshold		(false)
{
    const Trk::MeasurementBase* rot	= m_rotBroad;
    m_sigmaBroad			= Amg::error(rot->localCovariance(),Trk::locX);
    if (m_status == pixel_cluster || m_status == broad_pixel)
    {
	m_secondSigmaBroad		= Amg::error(rot->localCovariance(),Trk::locY);
	if (m_rotPrecise && m_status == pixel_cluster)
	{
	    rot				= m_rotPrecise;
	    m_sigmaPrecise		= Amg::error(rot->localCovariance(),Trk::locX);
	    m_secondSigmaPrecise	= Amg::error(rot->localCovariance(),Trk::locY);
	}
	else
	{
	    m_sigmaPrecise		= m_sigmaBroad;
	    m_secondSigmaPrecise	= m_secondSigmaBroad;
	}
    }
    else if (m_rotPrecise && (m_status == strip_cluster || m_status == drift_time))
    {
	rot				= m_rotPrecise;
	m_sigmaPrecise			= Amg::error(rot->localCovariance(),Trk::locX);
    }
    else
    {
	m_sigmaPrecise			= m_sigmaBroad;
    }
    m_position		= rot->globalPosition();
    m_surface		= &rot->associatedSurface();
    m_detectorElement	= m_surface->associatedDetectorElement();
    if (trtThreshold > 0) m_trtThreshold = true;
}

// destructor
HitOnTrack::~HitOnTrack (void)
{
    delete m_intersection;
    delete m_materialEffects;
    delete m_rotBroad;
    delete m_rotPrecise;
    if (m_surfaceOwner) delete m_surface;
    //std::cout << " HitOnTrack   surfaceOwner" << m_surfaceOwner << std::endl;
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

double
HitOnTrack::broadSigma (void) const
{
    switch (m_status)
    {
    case pixel_cluster:
    case broad_pixel:	
    case strip_cluster:
    case broad_strip:	
    case drift_time:
    case drift_wire:
	return m_sigmaBroad;
	break;
    default:
	return m_sigmaPrecise;
    };
}

void
HitOnTrack::fill (const Amg::Vector3D&	position,
		  double	       		sigmaBroad,
		  double	       		sigmaPrecise,
		  double	       		secondSigmaBroad,
		  double	       		secondSigmaPrecise,
		  double	       		sinStereo,
		  double,
		  bool             		barrel,
		  const Trk::TrkDetElementBase*	detectorElement,
		  const Identifier&		identifier,
		  HitStatus	       		status)
{
    m_position			= position;   
    m_residual			= 0.;
    m_sigmaBroad		= sigmaBroad;
    m_sigmaPrecise		= sigmaPrecise;
    m_secondResidual		= 0.;
    m_secondSigmaBroad     	= secondSigmaBroad;
    m_secondSigmaPrecise   	= secondSigmaPrecise;
    m_sinStereo			= sinStereo;
    m_barrel          		= barrel;
    m_detectorElement		= detectorElement;
    m_identifier      		= identifier;
    m_shared			= false;
    m_status          		= status;
    m_surface			= &detectorElement->surface(identifier);
    m_surfaceOwner		= false;
}

void
HitOnTrack::intersection (const Trk::TrackSurfaceIntersection* intersection)
{
    delete m_intersection;
    m_intersection = intersection;
}

bool
HitOnTrack::isCluster (void) const
{
    switch (m_status)
    {
    case pixel_cluster:
    case broad_pixel:
    case strip_cluster:
    case broad_strip:
	return true;
    default:
	return false;
    };
}

bool
HitOnTrack::isDrift (void) const
{
    switch (m_status)
    {
    case drift_time:
    case drift_wire:
	return true;
    default:
	return false;
    };
}

bool
HitOnTrack::isMaterial (void) const
{
    switch (m_status)
    {
    case scatterer:
    case inactive:
    case inert:
	return true;
    default:
	return false;
    };
}

bool
HitOnTrack::isPositionMeasurement (void) const
{
    switch (m_status)
    {
    case pixel_cluster:
    case broad_pixel:
    case strip_cluster:
    case broad_strip:
    case drift_time:
    case drift_wire:
	return true;
    default:
	return false;
    };
}

bool
HitOnTrack::isOutlier (void) const
{
    switch (m_status)
    {
    case pixel_reject:
    case strip_reject:
    case drift_reject:
	return true;
    default:
	return false;
    };
}

void
HitOnTrack::print (void) const
{
    std::cout << std::setiosflags(std::ios::fixed)
	      << "   r,phi,z " << std::setw(7) << std::setprecision(3) << m_position.perp()
	      << std::setw(7) << std::setprecision(3) << m_position.phi()
	      << std::setw(9) << std::setprecision(3) << m_position.z() 
	      << "  " << m_status;
    
    if (m_status == inactive || m_status == inert || m_status == scatterer)
    {
	double energyDeposit = 0;
	const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>
						  (m_materialEffects);
	if (meot) energyDeposit = meot->energyLoss()->deltaE();
	std::cout << " radiationThickness, energyLoss " 
		  << std::setw(7) << std::setprecision(3) << m_materialEffects->thicknessInX0()
		  << std::setw(7) << std::setprecision(4) << energyDeposit
		  << std::endl;
    }
    else
    {
	std::cout << " id " << m_identifier;
	if (m_trtThreshold) std::cout << "   above tr-threshold ";
	std::cout << std::endl;
    }
}
 
double
HitOnTrack::radiationThickness (void) const
{ return m_materialEffects->thicknessInX0(); }

const Trk::MeasurementBase*
HitOnTrack::rot (void) const
{
    if (m_rotPrecise
	&& (m_status == pixel_cluster || m_status == strip_cluster || m_status == drift_time)
	&& ! m_shared)
    {
	return m_rotPrecise;
    }
    else if (m_rotBroad)
    {
	return m_rotBroad;
    }

    return 0;
}

void
HitOnTrack::rot (const Trk::MeasurementBase* /*rot*/)
{
    if (m_rotBroad) return;

    std::cout << " rot set method deprecated - no action taken " << std::endl;

}

double
HitOnTrack::secondSigma (void) const
{
    switch (m_status)
    {
    case pixel_cluster:
	if (m_shared)
	{
	    return m_secondSigmaBroad;
	}
	else
	{
	    return m_secondSigmaPrecise;
	}
	break;
    case broad_pixel:
	return m_secondSigmaBroad;
	break;
    default:
	return m_secondSigmaPrecise;
    };
}

void
HitOnTrack::secondSigma (double value)
{
    switch (m_status)
    {
    case pixel_cluster:
	if (m_shared)
	{
	    m_secondSigmaBroad = value;
	}
	else
	{
	    m_secondSigmaPrecise = value;
	}
	break;
    case broad_pixel:
	m_secondSigmaBroad = value;
	break;
    default:
	break;
    };
}

void
HitOnTrack::setShared (void) 
{
    m_shared = true;
}

void
HitOnTrack::setSurface (bool barrel,const Amg::Vector3D& position)
{
    // note:  approx 5% iPatRec exec time in creating surfaces !
    // TODO    use curvilin ? e.g.:
    // CurvilinearUVT uvt(intersection.direction());
    // m_surface = new PlaneSurface(m_position,uvt);
    m_surfaceOwner	= true;
    if (barrel)
    {
	Amg::Transform3D* transform	= new Amg::Transform3D;
	(*transform)			= Amg::Translation3D(0.,0.,0.);
	double radius			= position.perp();
	double halfLength		= 2.*std::abs(position.z()); // arbitrary value
	m_surface = new Trk::CylinderSurface(transform,radius, halfLength);  
    }
    else
    {
	Amg::Transform3D* transform	= new Amg::Transform3D;
	(*transform)			= Amg::Translation3D(0.,0.,position.z());
	double innerRadius		= 0.;
	double outerRadius		= 2.*position.perp(); // arbitrary value
	m_surface = new Trk::DiscSurface(transform, innerRadius, outerRadius);
    }
}

double
HitOnTrack::sigma (void) const
{
    switch (m_status)
    {
    case pixel_cluster:
    case strip_cluster:
    case drift_time:
	if (m_shared)
	{
	    return m_sigmaBroad;
	}
	else
	{
	    return m_sigmaPrecise;
	}
	break;
    case broad_pixel:
    case broad_strip:
    case drift_wire:
	return m_sigmaBroad;
	break;
    default:
	return m_sigmaPrecise;
    };
}

void
HitOnTrack::status (HitStatus value)
{
    m_status = value;
}

const Trk::TrackParameters*
HitOnTrack::trkParameters (void) const
{
    if (! m_intersection)	return 0;

    // DOESNT WORK!!!
    // const Trk::TrackParameters* tp	=
    // 	m_surface->createParameters<5,Trk::Charged>(m_intersection->position(),
    // 						    m_intersection->direction(),	
    // 						    1.,
    // 						    0);

    const Trk::PlaneSurface* plane = dynamic_cast<const Trk::PlaneSurface*>(m_surface);
    if (plane)
    {
    	return new Trk::AtaPlane(m_intersection->position(),
    				 m_intersection->direction(),
    				 1.,
    				 *plane,
				 0);
    }

    const Trk::StraightLineSurface* line = dynamic_cast<const Trk::StraightLineSurface*>(m_surface);
    if (line)
    {
    	return new Trk::AtaStraightLine(m_intersection->position(),
    					m_intersection->direction(),
    					1.,
    					*line,
					0);
    }
    
    return 0;
}

