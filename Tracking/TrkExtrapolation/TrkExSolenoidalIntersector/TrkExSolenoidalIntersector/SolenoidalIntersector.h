/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolenoidalIntersector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXSOLENOIDALINTERSECTOR_SOLENOIDALINTERSECTOR_H
#define TRKEXSOLENOIDALINTERSECTOR_SOLENOIDALINTERSECTOR_H

#include <cmath>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkExSolenoidalIntersector/SolenoidParametrization.h"

class IIncidentSvc;
namespace Trk
{

class SolenoidalIntersector: public AthAlgTool,
			     virtual public IIntersector, virtual public IIncidentListener
{
    
public:
    SolenoidalIntersector	(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~SolenoidalIntersector	(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    /** handle for incident service */
    void 			handle(const Incident& inc) ;

    /**IIntersector interface method for general Surface type */
    const TrackSurfaceIntersection*		intersectSurface(const Surface&		surface,
						 const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						 const double      	qOverP);
	                                     
    /**IIntersector interface method for specific Surface type : PerigeeSurface */
    const TrackSurfaceIntersection*		approachPerigeeSurface(const PerigeeSurface&	surface,
						       const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						       const double      	qOverP);
	
    /**IIntersector interface method for specific Surface type : StraightLineSurface */
    const TrackSurfaceIntersection*		approachStraightLineSurface(const StraightLineSurface& surface,
							    const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
							    const double      	qOverP);
              
    /**IIntersector interface method for specific Surface type : CylinderSurface */
    const TrackSurfaceIntersection*		intersectCylinderSurface (const CylinderSurface& surface,
							  const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
							  const double      	qOverP);

    /**IIntersector interface method for specific Surface type : DiscSurface */
    const TrackSurfaceIntersection*		intersectDiscSurface (const DiscSurface&	surface,
						      const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						      const double      	qOverP);

    /**IIntersector interface method for specific Surface type : PlaneSurface */
    const TrackSurfaceIntersection*		intersectPlaneSurface(const PlaneSurface&	surface,
						      const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						      const double      	qOverP);

    /**IIntersector interface method to check validity of parametrization within extrapolation range */
    bool			isValid (Amg::Vector3D startPosition,
					 Amg::Vector3D endPosition) const;

    /** tabulate parametrization details */
    void			validationAction() const;
    
private:
    double					circularArcLength(double, double, double, double, double,
								    double, double&, double&);
    double					linearArcLength(double);
    bool					extrapolateToR(double endRadius);
    bool					extrapolateToZ(double endZ);
    const TrackSurfaceIntersection*		intersection(const Surface&	surface);
    void					setParameters(const TrackSurfaceIntersection*	intersection,
							      double			qOverP);
    
    // services and tools:
    ServiceHandle<IIncidentSvc>			m_incidentSvc;   //!< IncidentSvc to catch begin of event
    ServiceHandle<MagField::IMagFieldSvc>	m_magFieldSvc;
    ToolHandle<IIntersector>			m_rungeKuttaIntersector;

    double					m_cotTheta;
    float					m_currentMax;
    float					m_currentMin;
    double					m_deltaPhiTolerance;
    Amg::Vector3D				m_direction;
    unsigned					m_intersectionNumber;
    double					m_oneOverSinTheta;
    double					m_pathLength;
    Amg::Vector3D				m_position;
    double					m_qOverP;
    double					m_qOverPt;
    double					m_radius;
    double					m_sinTheta;
    SolenoidParametrization*			m_solenoidParametrization;
    double					m_surfaceTolerance;
    double					m_validRadius;
    double					m_validZ;

    // counters
    unsigned long long				m_countExtrapolations;
    unsigned long long				m_countRKSwitches;
    
};


// arc length to intersect of 2 circles: circular track and circle at (0,0) with radius endRadius
inline double
SolenoidalIntersector::circularArcLength(double 	endRadius,
					 double		radiusOfCurvature,
					 double		xCentre,
					 double		yCentre,
					 double		cosPhi,
					 double		sinPhi,
					 double&	cosPhiIntersect,
					 double&	sinPhiIntersect)
{
    int		trapped	= 0;
    double	radiusSquared 	= xCentre*xCentre + yCentre*yCentre;
    double	term 		= 0.5*(radiusSquared +
				       radiusOfCurvature*radiusOfCurvature -
				       endRadius*endRadius)/(radiusSquared*radiusOfCurvature);
    if (std::abs(xCentre) < std::abs(yCentre))
    {
	double	dx2 = yCentre*yCentre * (1./(radiusSquared*term*term) - 1.);
	if (dx2 < 0.)
	{
	    trapped = 1;
	}
	else
	{
	    if (yCentre*term > 0.)
	    {
		sinPhiIntersect = term*(-xCentre+std::sqrt(dx2));
	    }
	    else
	    {
		sinPhiIntersect = term*(-xCentre-std::sqrt(dx2));
	    }
	    cosPhiIntersect	= (sinPhiIntersect*xCentre + radiusSquared*term)/yCentre;
	}
    }
    else
    {
	double	dy2 = xCentre*xCentre * (1./(radiusSquared*term*term) - 1.);
	if (dy2 < 0.)
	{
	    trapped = 1;
	}
	else
	{
	    if (xCentre*term > 0.)
	    {
		cosPhiIntersect = term*(yCentre+std::sqrt(dy2));
	    }
	    else
	    {
		cosPhiIntersect = term*(yCentre-std::sqrt(dy2));
	    }
	    sinPhiIntersect	= (cosPhiIntersect*yCentre - radiusSquared*term)/xCentre;
	}
    }
    if (trapped == 0)
    {
	double	deltaPhi;
	double	sinDeltaPhi	= sinPhiIntersect*cosPhi - cosPhiIntersect*sinPhi;
	if (std::abs(sinDeltaPhi) > 0.1)
	{
	    deltaPhi = asin(sinDeltaPhi);
	}
	else
	{
	    deltaPhi = sinDeltaPhi*(1. + 0.166667*sinDeltaPhi*sinDeltaPhi);
	}
	return (radiusOfCurvature*deltaPhi);
    }
    else
    {
	cosPhiIntersect	= cosPhi;
	sinPhiIntersect = sinPhi;
	return 0.;
    }
}    
	    
// arc length to intersect of a line to a circle of radius endRadius centred at (0,0)
// +ve (-ve) endRadius selects the solution on the same (opposite) side of (0,0)
inline double
SolenoidalIntersector::linearArcLength(double	endRadius)
{
    double 	arcLength     	= (-m_direction.x()*m_position.x() - m_direction.y()*m_position.y()) *
				  m_oneOverSinTheta;
    double 	radiusSquared	= endRadius*endRadius - m_radius*m_radius + arcLength*arcLength;
    if (radiusSquared > 0.)
    {
	if (endRadius > 0.)
	{
	    arcLength += std::sqrt(radiusSquared);
	}
	else
	{
	    arcLength -= std::sqrt(radiusSquared);
	}
    }
    return arcLength;
}
    
inline const TrackSurfaceIntersection*
SolenoidalIntersector::intersection(const Surface&	surface)
{
    Intersection SLIntersect	= surface.straightLineIntersection(m_position, m_direction, false, false);
    if (! SLIntersect.valid)		return 0;
    
    const TrackSurfaceIntersection* intersection	= new TrackSurfaceIntersection(SLIntersect.position,
							   m_direction,
							   m_pathLength);
    // // validate
    // if (! intersection)
    // {
    // 	ATH_MSG_WARNING(" this should never fail");
    // 	return 0;
    // }
    
    // const Amg::Vector2D* localPos = surface.positionOnSurface(intersection->position(), false);
    // if (! localPos)
    // {
    // 	ATH_MSG_INFO("   localPos fails  surface type " << surface.type()
    // 		     << "   at R,Z " << m_position.perp() << ", " << m_position.z()
    // 		     << "  surface R " << surface.globalReferencePoint().perp());
    // 	return 0;
    // }
    

    // ATH_MSG_INFO(" serial diff " << intersection->serialNumber() - m_intersectionNumber
    // 	<< "  at R,Z: " << m_radius << ", " << m_position.z());
    
    m_intersectionNumber = intersection->serialNumber();
    return intersection;
}

inline void
SolenoidalIntersector::setParameters(const TrackSurfaceIntersection* trackTrackSurfaceIntersection, double qOverP)
{
    if (trackTrackSurfaceIntersection->serialNumber() != m_intersectionNumber || qOverP != m_qOverP)
    {
	// ATH_MSG_INFO(" initialize parameters.  Diff: " << trackTrackSurfaceIntersection->serialNumber()-m_intersectionNumber
	// 	     << "  at R,Z: " << trackTrackSurfaceIntersection->position().perp() << ", " << trackTrackSurfaceIntersection->position().z());
	++m_countExtrapolations;
	m_position.x()		= trackTrackSurfaceIntersection->position().x();
	m_position.y()		= trackTrackSurfaceIntersection->position().y();
	m_position.z()		= trackTrackSurfaceIntersection->position().z();
	m_radius		= m_position.perp();
	m_direction.x()		= trackTrackSurfaceIntersection->direction().x();
	m_direction.y()		= trackTrackSurfaceIntersection->direction().y();
	m_direction.z()		= trackTrackSurfaceIntersection->direction().z();
	m_sinTheta		= m_direction.perp();
	m_oneOverSinTheta	= 1./m_sinTheta;
	m_cotTheta		= m_direction.z() * m_oneOverSinTheta;
	m_pathLength		= trackTrackSurfaceIntersection->pathlength();
	m_qOverP		= qOverP;
	m_qOverPt		= qOverP * m_oneOverSinTheta;
	m_solenoidParametrization->setParameters(m_radius,m_position.z(),m_cotTheta);
    }
}

} // end of namespace


#endif // TRKEXSOLENOIDALINTERSECTOR_SOLENOIDALINTERSECTOR_H


