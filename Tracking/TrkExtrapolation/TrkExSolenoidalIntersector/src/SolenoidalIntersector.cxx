/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolenoidalIntersector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkExSolenoidalIntersector/SolenoidalIntersector.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"

namespace Trk
{
SolenoidalIntersector::SolenoidalIntersector (const std::string&	type,
					      const std::string&	name, 
					      const IInterface*		parent)
    :	AthAlgTool			(type, name, parent),
	m_incidentSvc			("IncidentSvc", name),
	m_magFieldSvc			("MagField::AtlasFieldSvc/AtlasFieldSvc", name),
	m_rungeKuttaIntersector		("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
	m_cotTheta			(0.),
	m_currentMax			(0.),
	m_currentMin			(0.),
	m_deltaPhiTolerance		(0.01),	// upper limit for small angle approx
	m_intersectionNumber		(0),
	m_oneOverSinTheta		(1.),
	m_pathLength			(0.),
	m_qOverP			(0.),
	m_qOverPt			(0.),
	m_radius			(0.),
	m_sinTheta			(1.),
	m_solenoidParametrization	(0),
	m_surfaceTolerance		(2.0*Gaudi::Units::micrometer),
	m_validRadius			(0.),
	m_validZ			(0.),
	m_countExtrapolations		(0),
	m_countRKSwitches		(0)
{
    declareInterface<Trk::IIntersector>(this);
    declareProperty("MagFieldSvc",		m_magFieldSvc );
    declareProperty("RungeKuttaIntersector",	m_rungeKuttaIntersector);
    declareProperty("SurfaceTolerance",		m_surfaceTolerance);
}

SolenoidalIntersector::~SolenoidalIntersector (void)
{}
 
StatusCode
SolenoidalIntersector::initialize()
{
    // print name and package version
    ATH_MSG_INFO( "SolenoidalIntersector::initialize() - package version " << PACKAGE_VERSION );

    if (m_incidentSvc.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve service " << m_incidentSvc );
	return StatusCode::FAILURE;
    }
    // register to the incident service:
    // 	BeginEvent needed to ensure the solenoid field parametrization is still valid
    m_incidentSvc->addListener( this, IncidentType::BeginEvent);

    if (! m_magFieldSvc.empty())
    {
	if (m_magFieldSvc.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve service " << m_magFieldSvc );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved service " << m_magFieldSvc );
	}
    }

    if (m_rungeKuttaIntersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_rungeKuttaIntersector );
	return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_rungeKuttaIntersector );
    }

    return StatusCode::SUCCESS;
}

StatusCode
SolenoidalIntersector::finalize()
{
    ATH_MSG_INFO( "finalized after " << m_countExtrapolations << " extrapolations with "
		  << m_countRKSwitches << " switches to RK integration");

    return StatusCode::SUCCESS;
}

/** handle for incident service */    
void
SolenoidalIntersector::handle(const Incident& inc)
{
    // (re)parametrize the solenoidal field whenever necessary at BeginEvent
    if (inc.type() == IncidentType::BeginEvent )
    {
	// intersector invalid when field off
	if (! m_magFieldSvc->solenoidOn())
	{
	    m_currentMax	= 0.;
	    m_validRadius	= 0.;
	    m_validZ		= 0.;
	    return;
	}
    
	if (m_magFieldSvc->solenoidCurrent()	> m_currentMax
	    || m_magFieldSvc->solenoidCurrent()	< m_currentMin)
	{
	    // allow 0.1% fluctuation
	    m_currentMax	 = 1.001*m_magFieldSvc->solenoidCurrent();
	    m_currentMin	 = 0.999*m_magFieldSvc->solenoidCurrent();
	
	    // instantiate the field parametrisation
	    SolenoidParametrization::clearInstance();
	    m_solenoidParametrization	= SolenoidParametrization::instance(&*m_magFieldSvc);
	
	    // set limit of validity (cylinder bounds)
	    if (! m_solenoidParametrization)
	    {
		m_validRadius	= 0.;
		m_validZ	= 0.;
		ATH_MSG_WARNING(" mag field parametrization fails" );
		return;
	    }
	    m_validRadius	= m_solenoidParametrization->maximumR();
	    m_validZ		= m_solenoidParametrization->maximumZ();
	    ATH_MSG_INFO( " SolenoidParametrization  current: " << m_magFieldSvc->solenoidCurrent()
			  << "  valid radius,Z : " << m_validRadius << ", " << m_validZ);

	    if (msgLvl(MSG::DEBUG)) validationAction();
	}
    }
}	

/**IIntersector interface method for general Surface type */
const Trk::TrackSurfaceIntersection*
SolenoidalIntersector::intersectSurface(const Surface&		surface,
					const TrackSurfaceIntersection*	trackIntersection,
					const double      	qOverP)
{
    const PlaneSurface* plane			= dynamic_cast<const PlaneSurface*>(&surface);
    if (plane)		return intersectPlaneSurface(*plane,trackIntersection,qOverP);
    
    const StraightLineSurface* straightLine	= dynamic_cast<const StraightLineSurface*>(&surface);
    if (straightLine)	return m_rungeKuttaIntersector->approachStraightLineSurface
			    (*straightLine, trackIntersection, qOverP);
    
    const CylinderSurface* cylinder		= dynamic_cast<const CylinderSurface*>(&surface);
    if (cylinder)	return intersectCylinderSurface(*cylinder,trackIntersection,qOverP);

    const DiscSurface* disc			= dynamic_cast<const DiscSurface*>(&surface);
    if (disc)		return intersectDiscSurface(*disc,trackIntersection,qOverP);

    const PerigeeSurface* perigee		= dynamic_cast<const PerigeeSurface*>(&surface);
    if (perigee)	return m_rungeKuttaIntersector->approachPerigeeSurface
			    (*perigee, trackIntersection, qOverP);
    
    ATH_MSG_WARNING( " unrecognized Surface" );
    return 0;
}
	           
/**IIntersector interface method for specific Surface type : PerigeeSurface */
const Trk::TrackSurfaceIntersection*
SolenoidalIntersector::approachPerigeeSurface(const PerigeeSurface&	surface,
					      const TrackSurfaceIntersection*	trackIntersection,
					      const double      	qOverP)
{ return m_rungeKuttaIntersector->approachPerigeeSurface(surface, trackIntersection, qOverP); }
	
/**IIntersector interface method for specific Surface type : StraightLineSurface */
const Trk::TrackSurfaceIntersection*
SolenoidalIntersector::approachStraightLineSurface(const StraightLineSurface& surface,
						   const TrackSurfaceIntersection*	trackIntersection,
						   const double      	qOverP)
{ return m_rungeKuttaIntersector->approachStraightLineSurface(surface, trackIntersection, qOverP); }
              
/**IIntersector interface method for specific Surface type : CylinderSurface */
const Trk::TrackSurfaceIntersection*
SolenoidalIntersector::intersectCylinderSurface(const CylinderSurface&	surface,
						const TrackSurfaceIntersection*	trackIntersection,
						const double      	qOverP)
{
    double endRadius		= surface.globalReferencePoint().perp();
    if (endRadius > m_validRadius)
	return m_rungeKuttaIntersector->intersectCylinderSurface(surface, trackIntersection, qOverP);
    
    // set member data unless extrapolation from existing parameters
    setParameters(trackIntersection, qOverP);

    if (std::abs(endRadius -trackIntersection->position().perp()) > m_surfaceTolerance
	&& ! extrapolateToR(endRadius))			return 0;
    return intersection(surface);
}

/**IIntersector interface method for specific Surface type : DiscSurface */
const Trk::TrackSurfaceIntersection*
SolenoidalIntersector::intersectDiscSurface (const DiscSurface&		surface,
					     const TrackSurfaceIntersection*	trackIntersection,
					     const double      		qOverP)
{
    double endZ	= surface.center().z();
    if (std::abs(endZ) > m_validZ)
	return m_rungeKuttaIntersector->intersectDiscSurface(surface, trackIntersection, qOverP);

    // set member data unless extrapolation from existing parameters
    setParameters(trackIntersection, qOverP);
    if (std::abs(endZ -trackIntersection->position().z()) > m_surfaceTolerance
	&& ! extrapolateToZ(endZ))
    {
	return 0;
    }
    
    return intersection(surface);
}

/**IIntersector interface method for specific Surface type : PlaneSurface */
const Trk::TrackSurfaceIntersection*
SolenoidalIntersector::intersectPlaneSurface(const PlaneSurface&	surface,
					     const TrackSurfaceIntersection*	trackIntersection,
					     const double      		qOverP)
{
    if (std::abs(surface.center().z())	> m_validZ
    	|| surface.center().perp()	> m_validRadius)
	return m_rungeKuttaIntersector->intersectPlaneSurface(surface, trackIntersection, qOverP);

    // set member data unless extrapolation from existing parameters
    setParameters(trackIntersection, qOverP);

    // step until sufficiently near to plane surface
    // this gives wrong answer!
    // DistanceSolution solution	= surface.straightLineDistanceEstimate(m_position,m_direction);
    // if (! solution.signedDistance())						return 0;
    // double distance		= solution.currentDistance(true);

    int numberSteps	= 0;
    double dot		= surface.normal().dot(m_direction);
    double offset	= surface.normal().dot(surface.center() - m_position);

    while (std::abs(offset) > m_surfaceTolerance*std::abs(dot))
    {
	// take care if grazing incidence - quit if looper
	if (std::abs(dot) < 0.0001) 						return 0;
	double distance	= offset/dot;

	// extrapolate
	if (m_sinTheta < 0.9)
	{
	    if (! extrapolateToZ(m_position.z()+distance*m_direction.z()))	return 0;
	}
	else
	{
	    if (! extrapolateToR((m_position+distance*m_direction).perp()))	return 0;
	}

	// check we are getting closer to the plane, switch to RK in case of difficulty
	dot		= surface.normal().dot(m_direction);
	offset		= surface.normal().dot(surface.center() - m_position);
	if (std::abs(offset) > m_surfaceTolerance*std::abs(dot)
	    && (++numberSteps > 5 || std::abs(offset) > 0.5*std::abs(distance*dot)))
	{
	    ++m_countRKSwitches;
	    ATH_MSG_DEBUG(" switch to RK after " << numberSteps << " steps at offset "
			 << offset << "   dot " << surface.normal().dot(m_direction));
	    
	    return m_rungeKuttaIntersector->intersectPlaneSurface(surface, trackIntersection, qOverP);
	}
    };

     return intersection(surface);
}

/**IIntersector interface method to check validity of parametrization within extrapolation range */
bool
SolenoidalIntersector::isValid (Amg::Vector3D startPosition, Amg::Vector3D endPosition) const
{
    // check cylinder bounds for valid parametrization
    if (std::abs(endPosition.z())	< m_validZ
	&& endPosition.perp()		< m_validRadius
	&& m_solenoidParametrization->validOrigin(startPosition))
    {
	// ATH_MSG_INFO("  choose solenoidal");
	
	return true;
    }
    
    // ATH_MSG_INFO("  choose rungeKutta");
	
    return false;
}

/** tabulate parametrization details */ 
void
SolenoidalIntersector::validationAction() const
{
    // validate parametrization
    if (m_solenoidParametrization)
    {
	for (int ieta = 0; ieta != 27; ++ieta)
	{
	    double eta	= 0.05 + 0.1*static_cast<double>(ieta);
	    m_solenoidParametrization->printParametersForEtaLine(+eta,0.);
	    m_solenoidParametrization->printParametersForEtaLine(-eta,0.);
	}
	for (int ieta = 0; ieta != 27; ++ieta)
	{
	    double eta	= 0.05 + 0.1*static_cast<double>(ieta);
	    m_solenoidParametrization->printResidualForEtaLine(+eta,0.);
	    m_solenoidParametrization->printResidualForEtaLine(-eta,0.);
	}
	m_solenoidParametrization->printFieldIntegrals();
    }
}

// private methods


bool
SolenoidalIntersector::extrapolateToR(double 	endR)
{
    // ATH_MSG_INFO(" extrapolateToR  endR " << endR << " from " << m_position.z()
    // 		 << "   r " << m_radius << "  cotTheta " << m_cotTheta);
    
    double	fieldComponent	= m_solenoidParametrization->fieldComponent(m_position.z());
    double	curvature	= fieldComponent*m_qOverPt;
    double	arcLength     	= linearArcLength(endR);
    if (std::abs(arcLength*curvature) > m_deltaPhiTolerance)
    {
	double radiusOfCurvature	= 1./curvature;
	double xCentre			= m_position.x() -
					  radiusOfCurvature*m_direction.y()*m_oneOverSinTheta;
	double yCentre   		= m_position.y() +
					  radiusOfCurvature*m_direction.x()*m_oneOverSinTheta;
	double cosPhi;
	double sinPhi;
	arcLength = circularArcLength(endR,
				      radiusOfCurvature,
				      xCentre,
				      yCentre,
				      m_direction.x() * m_oneOverSinTheta,
				      m_direction.y() * m_oneOverSinTheta,
				      cosPhi,
				      sinPhi);
	if (std::abs(arcLength*m_cotTheta) < m_surfaceTolerance)
	{
	    m_position.x()	=  xCentre + radiusOfCurvature*sinPhi;
	    m_position.y()	=  yCentre - radiusOfCurvature*cosPhi;
	    m_position.z()	+= arcLength*m_cotTheta;
	    m_radius		=  endR;
	    m_direction.x()	=  cosPhi*m_sinTheta;
	    m_direction.y()	=  sinPhi*m_sinTheta;
	    m_pathLength	+= arcLength*m_oneOverSinTheta;
	    arcLength		=  0.;
	}
    }
		
    double	deltaZ	= arcLength*m_cotTheta;
    if (std::abs(deltaZ) < m_surfaceTolerance)	// avoid FPE with constant curvature parabolic approx
    {
	double cosPhi	= m_direction.x()*m_oneOverSinTheta;
	double sinPhi	= m_direction.y()*m_oneOverSinTheta;
	if (std::abs(arcLength) > m_surfaceTolerance)
	{
	    double sinDPhi	= 0.5*arcLength*curvature;
	    double cosDPhi	= 1. - 0.5*sinDPhi*sinDPhi *
				  (1.0+0.25*sinDPhi*sinDPhi);
	    double temp		= cosPhi;
	    cosPhi 		= temp*cosDPhi - sinPhi*sinDPhi;
	    sinPhi 		= temp*sinDPhi + sinPhi*cosDPhi;
	    m_direction.x()	= (cosPhi*cosDPhi - sinPhi*sinDPhi)*m_sinTheta;
	    m_direction.y()	= (sinPhi*cosDPhi + cosPhi*sinDPhi)*m_sinTheta;
	}
		    
	m_position.x()	+= arcLength*cosPhi;
	m_position.y()	+= arcLength*sinPhi;
	m_position.z()	+= arcLength*m_cotTheta;
	m_radius	=  endR;
	m_pathLength	+= arcLength*m_oneOverSinTheta;
    }
    else
    {
	extrapolateToZ(m_position.z() + deltaZ);
	if (std::abs(endR - m_radius) > m_surfaceTolerance)
	{
	    deltaZ	= linearArcLength(endR) * m_cotTheta;
	    extrapolateToZ(m_position.z() + deltaZ);
	}
    }

    return true;
}

bool
SolenoidalIntersector::extrapolateToZ(double endZ)
{
    double 	firstIntegral	= 0.;
    double 	secondIntegral	= 0.;
    m_solenoidParametrization->fieldIntegrals(firstIntegral,
					      secondIntegral,
					      m_position.z(),
					      endZ);
    // ATH_MSG_INFO(" extrapolateToZ  firstIntegral, secondIntegral " << 1.E6*firstIntegral
    // 		 << ", " << 1.E6*secondIntegral);
    double 	DFiMax 		= 0.1;
    double	cosPhi		= m_direction.x()*m_oneOverSinTheta;
    double	sinPhi		= m_direction.y()*m_oneOverSinTheta;
    double	cosBeta;
    double	sinBeta;
    double	deltaPhi2	= secondIntegral*m_qOverPt/std::abs(m_cotTheta);
    if (std::abs(deltaPhi2) < DFiMax)
    {
	double	deltaPhi2Squared= deltaPhi2*deltaPhi2;
	sinBeta      		= 1. - 0.166667*deltaPhi2Squared;
	cosBeta      		= -0.5*deltaPhi2*(1.-0.083333*deltaPhi2Squared);
    }
    else if (2.*std::abs(deltaPhi2) < M_PI)
    {
	sinBeta	= std::sin(deltaPhi2) / deltaPhi2;
	cosBeta	= (std::cos(deltaPhi2) - 1.) / deltaPhi2;
    }
    else
    {
	return false;
    }
    
    double radialDistance	=  (endZ - m_position.z()) / m_cotTheta;
    m_position.x()		+= radialDistance*(sinPhi*cosBeta + cosPhi*sinBeta);
    m_position.y()		+= radialDistance*(sinPhi*sinBeta - cosPhi*cosBeta);
    m_position.z()		=  endZ;
    m_radius			=  m_position.perp();
    m_pathLength		+= radialDistance*m_oneOverSinTheta;
    
    double	cosAlpha;
    double	sinAlpha;
    double    	deltaPhi1     	=  firstIntegral*m_qOverPt / std::abs(m_cotTheta);
    if (std::abs(deltaPhi1) < DFiMax)
    {
	double	deltaPhi1Squared= deltaPhi1*deltaPhi1;
	sinAlpha      		= deltaPhi1*(1. - 0.166667*deltaPhi1Squared);
	cosAlpha      		= 1. - 0.5*deltaPhi1Squared*(1.-0.083333*deltaPhi1Squared);
    }
    else
    {
	sinAlpha		= std::sin(deltaPhi1);
	cosAlpha		= std::cos(deltaPhi1);
    }
    m_direction.x()	= (cosPhi*cosAlpha - sinPhi*sinAlpha)*m_sinTheta;
    m_direction.y()	= (sinPhi*cosAlpha + cosPhi*sinAlpha)*m_sinTheta;;
    return true;
}

} // end of namespace
