/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
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
  auto isect = std::make_unique<TrackSurfaceIntersection> (*trackIntersection);
  ++m_countExtrapolations;

  // straight line distance along track to closest approach to line
  const Amg::Vector3D&	lineDirection = surface.transform().rotation().col(2);
  double stepLength = distanceToLine (*isect, surface.center(),lineDirection);
  step(*isect, stepLength);
    
  return isect.release();
}
	
/**IIntersector interface method for specific Surface type : StraightLineSurface */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::approachStraightLineSurface(const StraightLineSurface& surface,
						     const TrackSurfaceIntersection*	trackIntersection,
						     const double      		/*qOverP*/)
{
  auto isect = std::make_unique<TrackSurfaceIntersection> (*trackIntersection);
  ++m_countExtrapolations;

  // straight line distance along track to closest approach to line
  const Amg::Vector3D&	lineDirection = surface.transform().rotation().col(2);
  double stepLength = distanceToLine (*isect, surface.center(),lineDirection);
  step(*isect, stepLength);

  return isect.release();
}
            
/**IIntersector interface method for specific Surface type : CylinderSurface */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::intersectCylinderSurface (const CylinderSurface&	surface,
						   const TrackSurfaceIntersection*		trackIntersection,
						   const double      		/*qOverP*/)
{
  auto isect = std::make_unique<TrackSurfaceIntersection> (*trackIntersection);
  ++m_countExtrapolations;
  
  // calculate straight line distance along track to intersect with cylinder radius
  double cylinderRadius = surface.globalReferencePoint().perp();
  double stepLength = distanceToCylinder(*isect, cylinderRadius);
  step(*isect, stepLength);

  return isect.release();
}

/**IIntersector interface method for specific Surface type : DiscSurface */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::intersectDiscSurface (const DiscSurface&	surface,
					       const TrackSurfaceIntersection*	trackIntersection,
					       const double      	/*qOverP*/)
{
  auto isect = std::make_unique<TrackSurfaceIntersection> (*trackIntersection);
  ++m_countExtrapolations;

  // straight line distance along track to intersect with disc
  double stepLength = distanceToDisc(*isect, surface.center().z());
  step(*isect, stepLength);
  
  return isect.release();
}

/**IIntersector interface method for specific Surface type : PlaneSurface */
const Trk::TrackSurfaceIntersection*
StraightLineIntersector::intersectPlaneSurface(const PlaneSurface&	surface,
					       const TrackSurfaceIntersection*	trackIntersection,
					       const double      	/*qOverP*/)
{
  auto isect = std::make_unique<TrackSurfaceIntersection> (*trackIntersection);
  ++m_countExtrapolations;

  // straight line distance along track to intersect with plane
  double stepLength = distanceToPlane (*isect, surface.center(),surface.normal());
  step(*isect, stepLength);
  stepLength = distanceToPlane (*isect, surface.center(),surface.normal());

  return isect.release();
}


} // end of namespace
