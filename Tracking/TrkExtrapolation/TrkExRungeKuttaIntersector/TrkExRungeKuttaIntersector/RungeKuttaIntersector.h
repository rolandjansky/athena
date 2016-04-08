/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// Runge-Kutta method for tracking a particle through a magnetic field
// (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////////

#ifndef TRKEXRUNGEKUTTAINTERSECTOR_RUNGEKUTTAINTERSECTOR_H
#define TRKEXRUNGEKUTTAINTERSECTOR_RUNGEKUTTAINTERSECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"

namespace Trk
{
    
class RungeKuttaIntersector: public AthAlgTool,
			     virtual public IIntersector
{
    
public:
    RungeKuttaIntersector	(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~RungeKuttaIntersector	(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    /**IIntersector interface method for general Surface type */
    const TrackSurfaceIntersection*		intersectSurface (const Surface&			surface,
								  const TrackSurfaceIntersection*	trackIntersection,
								  const double				qOverP);
	                                     
    /**IIntersector interface method for specific Surface type : PerigeeSurface */
    const TrackSurfaceIntersection*		approachPerigeeSurface (const PerigeeSurface&		surface,
									const TrackSurfaceIntersection*	trackIntersection,
									const double			qOverP);
	
    /**IIntersector interface method for specific Surface type : StraightLineSurface */
    const TrackSurfaceIntersection*		approachStraightLineSurface (const StraightLineSurface&		surface,
									     const TrackSurfaceIntersection*	trackIntersection,
									     const double			qOverP);
              
    /**IIntersector interface method for specific Surface type : CylinderSurface */
    const TrackSurfaceIntersection*		intersectCylinderSurface (const CylinderSurface&		surface,
									  const TrackSurfaceIntersection*	trackIntersection,
									  const double				qOverP);

    /**IIntersector interface method for specific Surface type : DiscSurface */
    const TrackSurfaceIntersection*		intersectDiscSurface (const DiscSurface&		surface,
								      const TrackSurfaceIntersection*	trackIntersection,
								      const double			qOverP);

    /**IIntersector interface method for specific Surface type : PlaneSurface */
    const TrackSurfaceIntersection*		intersectPlaneSurface (const PlaneSurface&		surface,
								       const TrackSurfaceIntersection*	trackIntersection,
								       const double			qOverP);
 
    /**IIntersector interface method for validity check over a particular extrapolation range */
    bool					isValid (Amg::Vector3D	/*startPosition*/,
							 Amg::Vector3D	/*endPosition*/) const
   	{ return true; }
	
private:

    // private methods
    void				assignStepLength (void);
    void				debugFailure (const Surface&	surface);
    void				distanceToCylinder (const double		cylinderRadius,
							    const double		offsetRadius, 
							    const Amg::Vector3D&	offset);
    void				distanceToDisc (const double discZ);
    void				distanceToLine (const Amg::Vector3D&	linePosition,
							const Amg::Vector3D&	lineDirection);
    void				distanceToPlane (const Amg::Vector3D&	planePosition,
							 const Amg::Vector3D&	planeNormal);
    Amg::Vector3D			field (const Amg::Vector3D&	point) const;
    bool				notTrapped (void);
    const TrackSurfaceIntersection*	newIntersection (const Surface&	surface);
    void				setCache (const TrackSurfaceIntersection*	trackIntersection,
						  const double				qOverP);
    void				shortStep (void);
    void				step (void);

    // field access:
    ServiceHandle<MagField::IMagFieldSvc>	m_magFieldSvc;

    // additional configuration
    bool					m_productionMode;
    
    // some precalculated constants:
    const double		m_caloR0;
    const double		m_caloR1;
    const double		m_caloR2;
    const double		m_caloR3;
    const double		m_caloR4;
    const double		m_caloZ0;
    const double		m_caloZ1;
    const double		m_caloZ2;
    const double		m_caloZ3;
    const double		m_inDetR0;
    const double		m_inDetR1;
    const double		m_inDetR2;
    const double		m_inDetZ0;
    const double		m_inDetZ1;
    const double		m_inDetZ2;
    const double		m_momentumThreshold;
    const double		m_momentumWarnThreshold;
    const double		m_muonR0;
    const double       		m_muonZ0;
    double			m_shortStepMax;
    const double		m_shortStepMin;
    const double		m_solenoidR ;
    const double		m_solenoidZ;
    double			m_stepMax0;
    double			m_stepMax1;
    double			m_stepMax2;
    double			m_stepMax3;
    double			m_stepMax4;
    int				m_stepsUntilTrapped;
    const double		m_third;
    const double		m_toroidR0;
    const double		m_toroidR1;
    const double		m_toroidR2;
    const double		m_toroidR3;
    const double		m_toroidZ0;
    const double		m_toroidZ1;
    const double		m_toroidZ2;
    const double		m_toroidZ3;
    const double		m_toroidZ4;
    const double		m_toroidZ5;
    const double		m_toroidZ6;
    const double		m_toroidZ7;
    const double		m_toroidZ8;
    
    // current parameters:
    double			m_cOverP;
    Amg::Vector3D		m_direction;
    double			m_distance;
    Amg::Vector3D		m_fieldValue;
    unsigned			m_intersectionNumber;
    unsigned long long		m_maxSteps;
    double			m_pathLength;
    Amg::Vector3D		m_position;
    double			m_previousDistance;
    double			m_qOverP;
    double			m_rCurrent;
    double			m_rStart;
    double			m_sinTheta;
    double			m_stepLength;
    bool			m_trapped;
    double			m_zStart;

    // counters
    unsigned long long		m_countExtrapolations;
    unsigned long long		m_countShortStep;
    unsigned long long		m_countStep;
    unsigned long long		m_countStepReduction;    
};

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>

inline void
RungeKuttaIntersector::distanceToCylinder (const double		cylinderRadius,
					   const double		offsetRadius, 
					   const Amg::Vector3D&	offset)
{
    double sinThsqinv	= 1./m_direction.perp2();
    m_stepLength	= (offset.x()*m_direction.x() + offset.y()*m_direction.y())*sinThsqinv;
    double deltaRSq	= (cylinderRadius - offsetRadius) * (cylinderRadius + offsetRadius) *
			  sinThsqinv + m_stepLength*m_stepLength;
    if (deltaRSq > 0.) m_stepLength += sqrt(deltaRSq);
    m_distance		= std::abs(m_stepLength);
}
    
inline void
RungeKuttaIntersector::distanceToDisc (const double discZ)
{
    m_distance		= discZ - m_position.z();
    m_stepLength	= m_distance/m_direction.z();
    m_distance		= std::abs(m_distance);
}

inline void
RungeKuttaIntersector::distanceToLine (const Amg::Vector3D&	linePosition,
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
    m_distance		= std::abs(m_stepLength);
}
    
inline void
RungeKuttaIntersector::distanceToPlane (const Amg::Vector3D&	planePosition,
					const Amg::Vector3D&	planeNormal)
{
    // take the normal component of the offset from track position to plane position
    // this is equal to the normal component of the required distance along the track direction
    m_distance		= planeNormal.dot(planePosition - m_position);
    m_stepLength	= m_distance / planeNormal.dot(m_direction);
    m_distance		= std::abs(m_distance);
}

inline Amg::Vector3D
RungeKuttaIntersector::field (const Amg::Vector3D& position) const
{
    Amg::Vector3D field;
    m_magFieldSvc->getField(&position,&field);
    return field;
}
    
inline const TrackSurfaceIntersection*
RungeKuttaIntersector::newIntersection (const Surface& surface)
{
    // ensure intersection is valid (ie. on surface)
    Intersection SLIntersect				= surface.straightLineIntersection(m_position, m_direction, false, false);
    if (SLIntersect.valid)
    {
	const TrackSurfaceIntersection* intersection	= new TrackSurfaceIntersection(SLIntersect.position,
										       m_direction,
										       m_pathLength);
	m_intersectionNumber				= intersection->serialNumber();
	return intersection;
    }

    // invalid: take care to invalidate cache!
    m_intersectionNumber = 0;
    if (msgLvl(MSG::DEBUG))	debugFailure (surface);

    return 0;
}

inline void
RungeKuttaIntersector::setCache (const TrackSurfaceIntersection*	trackIntersection,
				 const double				qOverP)
{
    if (trackIntersection->serialNumber() == m_intersectionNumber
	&& qOverP == m_qOverP)		return;

    m_direction		= trackIntersection->direction();
    m_position		= trackIntersection->position();
    m_fieldValue	= field(m_position);
    m_pathLength	= trackIntersection->pathlength();
    m_qOverP		= qOverP;
    m_cOverP		= Gaudi::Units::c_light*qOverP;
    m_rCurrent		= m_position.perp();
    m_rStart		= m_rCurrent;
    m_sinTheta		= m_direction.perp();
    m_trapped		= false;
    m_zStart		= m_position.z();
    ++m_countExtrapolations;
}
   
} // end of namespace


#endif // TRKEXRUNGEKUTTAINTERSECTOR_RUNGEKUTTAINTERSECTOR_H
