/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// provide straight line intersection to a surface
// (useful for abstract interfaces in track/segment fitters)
// (c) ATLAS Tracking software
//////////////////////////////////////////////////////////////////////

#ifndef TRKEXSTRAIGHTLINEINTERSECTOR_STRAIGHTLINEINTERSECTOR_H
#define TRKEXSTRAIGHTLINEINTERSECTOR_STRAIGHTLINEINTERSECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include <atomic>

namespace Trk
{
    
class StraightLineIntersector: public AthAlgTool,
			       virtual public IIntersector
{
    
public:
    StraightLineIntersector	(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~StraightLineIntersector	(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    /**IIntersector interface method for general Surface type */
    const TrackSurfaceIntersection*		intersectSurface(const Surface&		surface,
						 const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						 const double      	qOverP);
	                                     
    /**IIntersector interface method for specific Surface type : PerigeeSurface */
    const TrackSurfaceIntersection*		approachPerigeeSurface(const PerigeeSurface&	surface,
						       const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						       const double      	/*qOverP*/);
	
    /**IIntersector interface method for specific Surface type : StraightLineSurface */
    const TrackSurfaceIntersection*		approachStraightLineSurface(const StraightLineSurface& surface,
							    const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
							    const double      	/*qOverP*/);
              
    /**IIntersector interface method for specific Surface type : CylinderSurface */
    const TrackSurfaceIntersection*		intersectCylinderSurface (const CylinderSurface& surface,
							  const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
							  const double      	/*qOverP*/);

    /**IIntersector interface method for specific Surface type : DiscSurface */
    const TrackSurfaceIntersection*		intersectDiscSurface (const DiscSurface&	surface,
						      const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						      const double      	/*qOverP*/);

    /**IIntersector interface method for specific Surface type : PlaneSurface */
    const TrackSurfaceIntersection*		intersectPlaneSurface(const PlaneSurface&	surface,
						      const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						      const double      	/*qOverP*/);
 
    /**IIntersector interface method for validity check over a particular extrapolation range */
    bool			isValid(Amg::Vector3D /*startPosition*/,
					Amg::Vector3D /*endPosition*/) const
	{ return true; }
	
private:
    // private methods
    double		distanceToCylinder (const TrackSurfaceIntersection& isect,
                                            const double cylinderRadius) const;
    double		distanceToDisc (const TrackSurfaceIntersection& isect,
                                        const double discZ) const;
    double		distanceToLine (const TrackSurfaceIntersection& isect,
                                        const Amg::Vector3D&	linePosition,
					const Amg::Vector3D&	lineDirection) const;
    double		distanceToPlane (const TrackSurfaceIntersection& isect,
                                         const Amg::Vector3D&	planePosition,
					 const Amg::Vector3D&	planeNormal) const;
    void		step (TrackSurfaceIntersection& isect, double stepLength) const;
    
    // counters
    mutable std::atomic<unsigned long long>		m_countExtrapolations;
    
};

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>

inline double
StraightLineIntersector::distanceToCylinder (const TrackSurfaceIntersection& isect,
                                             const double cylinderRadius) const
{
  const Amg::Vector3D& pos = isect.position();
  const Amg::Vector3D& dir = isect.direction();
  double sinThsqinv	= 1./dir.perp2();
  double stepLength	= (-pos.x()*dir.x() - pos.y()*dir.y()) * sinThsqinv;
  double deltaRSq	= (cylinderRadius*cylinderRadius - pos.perp2())*sinThsqinv +
    stepLength*stepLength;
  if (deltaRSq > 0.) stepLength += sqrt(deltaRSq);
  return stepLength;
}
    
inline double
StraightLineIntersector::distanceToDisc (const TrackSurfaceIntersection& isect,
                                         const double discZ) const
{
  const Amg::Vector3D& pos = isect.position();
  const Amg::Vector3D& dir = isect.direction();
  return (discZ - pos.z())/dir.z();
}

inline double
StraightLineIntersector::distanceToLine (const TrackSurfaceIntersection& isect,
                                         const Amg::Vector3D&	linePosition,
                                         const Amg::Vector3D&	lineDirection) const
{
  // offset joining track to line is given by
  //   offset = linePosition + a*lineDirection - trackPosition - b*trackDirection
  // 
  // offset is perpendicular to both line and track at solution i.e.
  //   lineDirection.dot(offset) = 0
  //   trackDirection.dot(offset) = 0
  const Amg::Vector3D& pos = isect.position();
  const Amg::Vector3D& dir = isect.direction();
  double cosAngle	= lineDirection.dot(dir);
  return (linePosition - pos).dot(dir - lineDirection*cosAngle) /
      (1. - cosAngle*cosAngle);
}
    
inline double
  StraightLineIntersector::distanceToPlane (const TrackSurfaceIntersection& isect,
                                            const Amg::Vector3D&	planePosition,
					    const Amg::Vector3D&	planeNormal) const
{
  // take the normal component of the offset from track position to plane position
  // this is equal to the normal component of the required distance along the track direction
  const Amg::Vector3D& pos = isect.position();
  const Amg::Vector3D& dir = isect.direction();
  return planeNormal.dot(planePosition - pos) / planeNormal.dot(dir);
}

    
inline void
StraightLineIntersector::step (TrackSurfaceIntersection& isect, double stepLength) const
{
    isect.position()	+= stepLength*isect.direction();
    isect.pathlength()	+= stepLength;
}

    
} // end of namespace


#endif // TRKEXSTRAIGHTLINEINTERSECTOR_STRAIGHTLINEINTERSECTOR_H
