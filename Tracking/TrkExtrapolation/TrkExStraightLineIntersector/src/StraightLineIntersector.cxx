/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// provide straight line intersection to a surface
// (useful for abstract interfaces in track/segment fitters)
// (c) ATLAS Tracking software
//////////////////////////////////////////////////////////////////////

#include <cmath>
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrkExStraightLineIntersector/StraightLineIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
//#include "TrkParameters/Perigee.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"

namespace Trk
{

StraightLineIntersector::StraightLineIntersector (const std::string&	type,
						  const std::string&	name, 
						  const IInterface*	parent)
    :	AthAlgTool		(type, name, parent),
	m_intersectionNumber	(0),
	m_countExtrapolations	(0)
{
    declareInterface<Trk::IIntersector>(this);
}

StraightLineIntersector::~StraightLineIntersector (void)
{}
 
StatusCode
StraightLineIntersector::initialize()
{
    // print name and package version
    ATH_MSG_INFO( "StraightLineIntersector::initialize()"
		  << " - package version " << PACKAGE_VERSION );

    // initialize base class
    if (StatusCode::SUCCESS != AlgTool::initialize()) return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

StatusCode
StraightLineIntersector::finalize()
{
    ATH_MSG_INFO( "finalized after " << m_countExtrapolations << " extrapolations" );

    return StatusCode::SUCCESS;
}

/**IIntersector interface method for general Surface type */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::intersectSurface(const Surface&	surface,
					  const TrackSurfaceIntersection*	trackIntersection,
					  const double      	qOverP)
{
    const PlaneSurface* plane			= dynamic_cast<const PlaneSurface*>(&surface);
    if (plane)		return intersectPlaneSurface(*plane,trackIntersection,qOverP);
    
    const StraightLineSurface* straightLine	= dynamic_cast<const StraightLineSurface*>(&surface);
    if (straightLine)	return approachStraightLineSurface(*straightLine,trackIntersection,qOverP);

    const CylinderSurface* cylinder		= dynamic_cast<const CylinderSurface*>(&surface);
    if (cylinder)	return intersectCylinderSurface(*cylinder,trackIntersection,qOverP);
	
    const DiscSurface* disc			= dynamic_cast<const DiscSurface*>(&surface);
    if (disc)		return intersectDiscSurface(*disc,trackIntersection,qOverP);
    
    const PerigeeSurface* perigee		= dynamic_cast<const PerigeeSurface*>(&surface);
    if (perigee)	return approachPerigeeSurface(*perigee,trackIntersection,qOverP);
    
    ATH_MSG_WARNING( " unrecognized Surface" );
    return 0;
}
                                    
/**IIntersector interface method for specific Surface type : PerigeeSurface */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::approachPerigeeSurface(const PerigeeSurface&	surface,
						const TrackSurfaceIntersection*	trackIntersection,
						const double      	/*qOverP*/)
{
    // set member data
    if (trackIntersection->serialNumber() != m_intersectionNumber)
    {
	m_position		= trackIntersection->position();
	m_direction		= trackIntersection->direction();
	m_transverseLength	= trackIntersection->pathlength();
	++m_countExtrapolations;
    }

    // straight line distance along track to closest approach to line
    const Amg::Vector3D&	lineDirection = surface.transform().rotation().col(2);
    distanceToLine (surface.center(),lineDirection);
    step();
    
    const Trk::TrackSurfaceIntersection* intersection	= new TrackSurfaceIntersection(m_position,
								       m_direction,
								       m_transverseLength);
    m_intersectionNumber		= intersection->serialNumber();
    return intersection;
}
	
/**IIntersector interface method for specific Surface type : StraightLineSurface */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::approachStraightLineSurface(const StraightLineSurface& surface,
						     const TrackSurfaceIntersection*	trackIntersection,
						     const double      		/*qOverP*/)
{
    // set member data
    if (trackIntersection->serialNumber() != m_intersectionNumber)
    {
	m_position		= trackIntersection->position();
	m_direction		= trackIntersection->direction();
	m_transverseLength	= trackIntersection->pathlength();
	++m_countExtrapolations;
    }

    // straight line distance along track to closest approach to line
    const Amg::Vector3D&	lineDirection = surface.transform().rotation().col(2);
    distanceToLine (surface.center(),lineDirection);
    step();

    const Trk::TrackSurfaceIntersection* intersection	= new TrackSurfaceIntersection(m_position,
								       m_direction,
								       m_transverseLength);
    m_intersectionNumber		= intersection->serialNumber();
    return intersection;
}
            
/**IIntersector interface method for specific Surface type : CylinderSurface */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::intersectCylinderSurface (const CylinderSurface&	surface,
						   const TrackSurfaceIntersection*		trackIntersection,
						   const double      		/*qOverP*/)
{
    // set member data
    if (trackIntersection->serialNumber() != m_intersectionNumber)
    {
	m_position		= trackIntersection->position();
	m_direction		= trackIntersection->direction();
	m_transverseLength	= trackIntersection->pathlength();
	++m_countExtrapolations;
    }

    // calculate straight line distance along track to intersect with cylinder radius
    double cylinderRadius = surface.globalReferencePoint().perp();
    distanceToCylinder(cylinderRadius);
    step();

    const Trk::TrackSurfaceIntersection* intersection	= new TrackSurfaceIntersection(m_position,
								       m_direction,
								       m_transverseLength);
    m_intersectionNumber		= intersection->serialNumber();
    return intersection;
}

/**IIntersector interface method for specific Surface type : DiscSurface */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::intersectDiscSurface (const DiscSurface&	surface,
					       const TrackSurfaceIntersection*	trackIntersection,
					       const double      	/*qOverP*/)
{
    if (trackIntersection->serialNumber() != m_intersectionNumber)
    {
	m_position		= trackIntersection->position();
	m_direction		= trackIntersection->direction();
	m_transverseLength	= trackIntersection->pathlength();
	++m_countExtrapolations;
    }

    // straight line distance along track to intersect with disc
    distanceToDisc(surface.center().z());
    step();
  
    const Trk::TrackSurfaceIntersection* intersection	= new TrackSurfaceIntersection(m_position,
								       m_direction,
								       m_transverseLength);
    m_intersectionNumber		= intersection->serialNumber();
    return intersection;
}

/**IIntersector interface method for specific Surface type : PlaneSurface */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::intersectPlaneSurface(const PlaneSurface&	surface,
					       const TrackSurfaceIntersection*	trackIntersection,
					       const double      	/*qOverP*/)
{
    // set member data
    if (trackIntersection->serialNumber() != m_intersectionNumber)
    {
	m_position		= trackIntersection->position();
	m_direction		= trackIntersection->direction();
	m_transverseLength	= trackIntersection->pathlength();
	++m_countExtrapolations;
    }

    // straight line distance along track to intersect with plane
    distanceToPlane (surface.center(),surface.normal());
    step();
    distanceToPlane(surface.center(),surface.normal());

    const Trk::TrackSurfaceIntersection* intersection	= new TrackSurfaceIntersection(m_position,
								       m_direction,
								       m_transverseLength);
    m_intersectionNumber		= intersection->serialNumber();
    return intersection;
}


} // end of namespace
