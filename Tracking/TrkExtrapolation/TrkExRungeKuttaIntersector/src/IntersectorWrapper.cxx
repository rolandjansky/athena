/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// wrapper to IIntersector tool to provide IPropagator functionality
//   default configuration wraps the RungeKutta intersector
// (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////////

#include <iomanip>
#include <vector>
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExRungeKuttaIntersector/IntersectorWrapper.h"
#include "TrkExUtils/IntersectionSolution.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk
{

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
    
IntersectorWrapper::IntersectorWrapper	(const std::string& type, 
					 const std::string& name,
					 const IInterface* parent)
    :	AthAlgTool		(type, name, parent),
	m_intersector		("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
	m_linePropagator	(""),
	m_charge{},
	m_intersection		(nullptr),
	m_momentum{},
	m_parameters		(nullptr),
	m_position{},
	m_qOverP{}
{
    declareInterface<Trk::IPropagator>(this);
    declareProperty("Intersector",	m_intersector);
    declareProperty("LinePropagator",	m_linePropagator);
}
    
IntersectorWrapper::~IntersectorWrapper	(void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
IntersectorWrapper::initialize(){
    // get the Tools
    ATH_CHECK(m_intersector.retrieve());
    ATH_MSG_VERBOSE( "Retrieved tool " << m_intersector );
    if (!m_linePropagator.empty()) ATH_CHECK( m_linePropagator.retrieve()); 
    return StatusCode::SUCCESS;
}
 
StatusCode
IntersectorWrapper::finalize()
{
    delete m_intersection;
    
    return StatusCode::SUCCESS;
}

const NeutralParameters*
IntersectorWrapper::propagate (const NeutralParameters&		parameters,
			       const Surface&			surface,
			       PropDirection			dir,
			       BoundaryCheck			boundsCheck,
			       bool				curvilinear) const
{
    return m_linePropagator->propagate(parameters,surface,dir,boundsCheck,curvilinear);
}

const TrackParameters*
IntersectorWrapper::propagate (const TrackParameters&		parameters,
			       const Surface&			surface,
			       PropDirection			dir,
			       BoundaryCheck			boundsCheck,
			       const MagneticFieldProperties&	/*magProperties*/,
			       ParticleHypothesis		/*particle*/,
			       bool				curvilinear,
			       const TrackingVolume*) const
{
    findIntersection(parameters,surface, dir);
    createParameters(surface,boundsCheck,curvilinear);
    return m_parameters;
}
       
const TrackParameters*
IntersectorWrapper::propagate (const TrackParameters&		parameters,
			       const Surface&			surface,
			       PropDirection			dir,
			       BoundaryCheck			boundsCheck,
			       const MagneticFieldProperties&	/*magProperties*/,
			       TransportJacobian*&		/*transportJac*/,
                   double&,
			       ParticleHypothesis		/*particle*/,
			       bool				curvilinear,
			       const TrackingVolume*) const
{
    findIntersection(parameters,surface,dir);
    createParameters(surface,boundsCheck,curvilinear);
    return m_parameters;
}

const TrackParameters*
IntersectorWrapper::propagateParameters (const TrackParameters&		parameters,
					 const Surface&			surface,
					 PropDirection			dir,
					 BoundaryCheck			boundsCheck,
					 const MagneticFieldProperties&	/*magProperties*/,
					 ParticleHypothesis		/*particle*/,
					 bool				curvilinear,
					 const TrackingVolume*) const
{
    
    findIntersection(parameters,surface,dir);
    createParameters(surface,boundsCheck,curvilinear);
    return m_parameters;
}

const TrackParameters*
IntersectorWrapper::propagateParameters (const TrackParameters&		parameters,
					 const Surface&			surface,
					 PropDirection			dir,
					 BoundaryCheck			boundsCheck,
					 const MagneticFieldProperties& /*magProperties*/,
					 TransportJacobian*&		/*transportJac*/,
					 ParticleHypothesis		/*particle*/,
					 bool				curvilinear,
					 const TrackingVolume*) const
{
    findIntersection(parameters,surface,dir);
    createParameters(surface,boundsCheck,curvilinear);
    return m_parameters;
}

const IntersectionSolution*
IntersectorWrapper::intersect (const TrackParameters&		parameters,
			       const Surface&			surface,
			       const MagneticFieldProperties&	/*magProperties*/,
			       ParticleHypothesis		/*particle*/,
			       const TrackingVolume*) const
{
    findIntersection(parameters,surface);
    IntersectionSolution* solution = new IntersectionSolution;
    if (m_intersection) solution->push_back(m_intersection);
    return solution;
}

void
IntersectorWrapper::globalPositions (std::list<Amg::Vector3D>&,
				     const TrackParameters&,
				     const MagneticFieldProperties&,
				     const CylinderBounds&,
				     double,
				     ParticleHypothesis,
				     const TrackingVolume* ) const
{
    ATH_MSG_WARNING( "globalPositions method from the IPropagator interface"
		     << " has NOT been implememnted" );
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
IntersectorWrapper::createParameters (const Surface&	surface,
				      BoundaryCheck	/*boundsCheck*/,
				      bool		curvilinear) const
{
    m_parameters	= 0;
    if (! m_intersection) return;

    // curvilinear special (simple) case
    if (curvilinear)
    {
	m_parameters	=  new CurvilinearParameters(m_intersection->position(),
						     m_intersection->direction().phi(),
						     m_intersection->direction().theta(),
						     m_qOverP);
	return;
    }
	
    m_parameters =  surface.createTrackParameters(m_intersection->position(),m_intersection->direction(),m_charge,nullptr);
    // unrecognized Surface
    if( !m_parameters ) ATH_MSG_WARNING( " Failed to create parameters " );
}

void
IntersectorWrapper::findIntersection (const TrackParameters&	parameters,
				      const Surface&		surface,
				      PropDirection	       	dir) const
{
    //if (! m_intersection || &parameters != m_parameters)
    {
	delete m_intersection;
	m_charge	=  parameters.charge();
	m_momentum	=  parameters.momentum();
	m_parameters	=  &parameters;
	m_position	=  parameters.position();
	m_position	=  Amg::Vector3D(parameters.position());
	m_qOverP	=  1./m_momentum.mag();
	m_intersection	=  new TrackSurfaceIntersection(m_position,m_momentum*m_qOverP,0.);
	m_qOverP	*= m_charge;
    }

    const TrackSurfaceIntersection* oldIntersection = m_intersection;
    m_intersection = m_intersector->intersectSurface(surface,
						     oldIntersection,
						     m_qOverP);
    delete oldIntersection;
    if (! m_intersection)
    {
	ATH_MSG_DEBUG( " no intersection found " );
	return;
    }

    // check for correct propagation direction
    if ((dir == Trk::alongMomentum && m_intersection->pathlength() < 0.)
	|| (dir == Trk::oppositeMomentum && m_intersection->pathlength() > 0.))
    {
	if (msgLvl(MSG::DEBUG))
	{
	    if (dir == Trk::alongMomentum)
	    {
		ATH_MSG_DEBUG( " requested alongMomentum, but pathlength "
			       << m_intersection->pathlength() );
	    }
	    else if (dir == Trk::oppositeMomentum)
	    {
		ATH_MSG_DEBUG( " requested oppositeMomentum, but pathlength "
			       << m_intersection->pathlength() );
	    }
	}
	delete m_intersection;
	m_intersection = 0;
	return;
    }
    
    ATH_MSG_DEBUG( std::setiosflags(std::ios::fixed)
		   << " intersection at r,phi,z "
		   << std::setw(10) << std::setprecision(1) << m_intersection->position().perp()
		   << std::setw(9)  << std::setprecision(4) << m_intersection->position().phi()
		   << std::setw(10) << std::setprecision(1) << m_intersection->position().z()
		   << " momentum "
		   << std::setw(9) << std::setprecision(3) << 1./fabs(m_qOverP*Gaudi::Units::GeV) );
}

} // end of namespace

