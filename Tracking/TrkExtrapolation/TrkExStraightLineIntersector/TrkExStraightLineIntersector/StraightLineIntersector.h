/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    void		distanceToCylinder (const double cylinderRadius);
    void		distanceToDisc (const double discZ);
    void		distanceToLine (const Amg::Vector3D&	linePosition,
					const Amg::Vector3D&	lineDirection);
    void		distanceToPlane (const Amg::Vector3D&	planePosition,
					 const Amg::Vector3D&	planeNormal);
    void		step (void);
    
    // current parameters:
    Amg::Vector3D		m_direction;
    unsigned			m_intersectionNumber;
    Amg::Vector3D		m_position;
    double       		m_stepLength;
    double			m_transverseLength;

    // counters
    unsigned long long		m_countExtrapolations;
    
};

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>

inline void
StraightLineIntersector::distanceToCylinder (const double cylinderRadius)
{
    double sinThsqinv	= 1./m_direction.perp2();
    m_stepLength	= (-m_position.x()*m_direction.x() - m_position.y()*m_direction.y()) *
			  sinThsqinv;
    double deltaRSq	= (cylinderRadius*cylinderRadius - m_position.perp2())*sinThsqinv +
			  m_stepLength*m_stepLength;
    if (deltaRSq > 0.) m_stepLength += sqrt(deltaRSq);
}
    
inline void
StraightLineIntersector::distanceToDisc (const double discZ)
{
    m_stepLength	= (discZ - m_position.z())/m_direction.z();
}

inline void
  StraightLineIntersector::distanceToLine (const Amg::Vector3D&	linePosition,
					   const Amg::Vector3D&	lineDirection)
{
    // offset joining track to line is given by
    //   offset = linePosition + a*lineDirection - trackPosition - b*trackDirection
    // 
    // offset is perpendicular to both line and track at solution i.e.
    //   lineDirection.dot(offset) = 0
    //   trackDirection.dot(offset) = 0
    double cosAngle	= lineDirection.dot(m_direction);
    m_stepLength	= (linePosition - m_position).dot(m_direction - lineDirection*cosAngle) /
			  (1. - cosAngle*cosAngle);
}
    
inline void
  StraightLineIntersector::distanceToPlane (const Amg::Vector3D&	planePosition,
					    const Amg::Vector3D&	planeNormal)
{
    // take the normal component of the offset from track position to plane position
    // this is equal to the normal component of the required distance along the track direction
    m_stepLength	= planeNormal.dot(planePosition - m_position) /
			  planeNormal.dot(m_direction);
}

inline void
StraightLineIntersector::step (void)
{
    m_position		+= m_stepLength*m_direction;
    m_transverseLength	+= m_stepLength;
}

    
} // end of namespace


#endif // TRKEXSTRAIGHTLINEINTERSECTOR_STRAIGHTLINEINTERSECTOR_H
