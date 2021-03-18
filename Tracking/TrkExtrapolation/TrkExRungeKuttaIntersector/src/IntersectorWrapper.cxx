/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

//////////////////////////////////////////////////////////////////////
// wrapper to IIntersector tool to provide IPropagator functionality
//   default configuration wraps the RungeKutta intersector
// (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////////

#include "TrkExRungeKuttaIntersector/IntersectorWrapper.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/IntersectionSolution.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include <iomanip>
#include <memory>
#include <vector>

namespace Trk
{

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

IntersectorWrapper::IntersectorWrapper	(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
  :AthAlgTool		(type, name, parent),
  m_intersector		("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
  m_linePropagator	("")
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

  if (!m_linePropagator.empty()) {
    ATH_CHECK( m_linePropagator.retrieve());
  }
  return StatusCode::SUCCESS;
}


StatusCode IntersectorWrapper::finalize()
{
  return StatusCode::SUCCESS;
}

std::unique_ptr<NeutralParameters>
IntersectorWrapper::propagate (const NeutralParameters&		parameters,
                               const Surface&			surface,
                               PropDirection			dir,
                               const BoundaryCheck& 			boundsCheck,
                               bool				curvilinear) const
{
  return m_linePropagator->propagate(parameters,surface,dir,boundsCheck,curvilinear);
}

std::unique_ptr<TrackParameters>
IntersectorWrapper::propagate (const EventContext&              /*ctx*/,
                               const TrackParameters&		parameters,
                               const Surface&			surface,
                               PropDirection			dir,
                               const BoundaryCheck& 			boundsCheck,
                               const MagneticFieldProperties&	/*magProperties*/,
                               ParticleHypothesis		/*particle*/,
                               bool				curvilinear,
                               const TrackingVolume*) const
{
  Cache cache{};
  findIntersection(cache,parameters,surface, dir);
  createParameters(cache,surface,boundsCheck,curvilinear);
  return std::move(cache.m_parameters);
}

std::unique_ptr<TrackParameters>
IntersectorWrapper::propagate (const EventContext&              /*ctx*/,
                               const TrackParameters&		parameters,
                               const Surface&			surface,
                               PropDirection			dir,
                               const BoundaryCheck& 			boundsCheck,
                               const MagneticFieldProperties&	/*magProperties*/,
                               TransportJacobian*&		/*transportJac*/,
                               double&,
                               ParticleHypothesis		/*particle*/,
                               bool				curvilinear,
                               const TrackingVolume*) const
{
  Cache cache{};
  findIntersection(cache,parameters,surface,dir);
  createParameters(cache,surface,boundsCheck,curvilinear);
  return std::move(cache.m_parameters);
}

std::unique_ptr<TrackParameters>
IntersectorWrapper::propagateParameters (const EventContext&              /*ctx*/,
                                         const TrackParameters&		parameters,
                                         const Surface&			surface,
                                         PropDirection			dir,
                                         const BoundaryCheck& 			boundsCheck,
                                         const MagneticFieldProperties&	/*magProperties*/,
                                         ParticleHypothesis		/*particle*/,
                                         bool				curvilinear,
                                         const TrackingVolume*) const
{

  Cache cache{};
  findIntersection(cache,parameters,surface,dir);
  createParameters(cache,surface,boundsCheck,curvilinear);
  return std::move(cache.m_parameters);
}

std::unique_ptr<TrackParameters>
IntersectorWrapper::propagateParameters (const EventContext&              /*ctx*/,
                                         const TrackParameters&		parameters,
                                         const Surface&			surface,
                                         PropDirection			dir,
                                         const BoundaryCheck& 			boundsCheck,
                                         const MagneticFieldProperties& /*magProperties*/,
                                         TransportJacobian*&		/*transportJac*/,
                                         ParticleHypothesis		/*particle*/,
                                         bool				curvilinear,
                                         const TrackingVolume*) const
{
  Cache cache{};
  findIntersection(cache,parameters,surface,dir);
  createParameters(cache,surface,boundsCheck,curvilinear);
  return std::move(cache.m_parameters);
}

const IntersectionSolution*
IntersectorWrapper::intersect (const EventContext&              /*ctx*/,
                               const TrackParameters&		parameters,
                               const Surface&			surface,
                               const MagneticFieldProperties&	/*magProperties*/,
                               ParticleHypothesis		/*particle*/,
                               const TrackingVolume*) const
{
  Cache cache{};
  findIntersection(cache,parameters,surface);
  IntersectionSolution* solution = new IntersectionSolution;
  if (cache.m_intersection) {
    solution->push_back(cache.m_intersection.release());
  }
  return solution;
}

void
IntersectorWrapper::globalPositions (const EventContext&              /*ctx*/,
                                     std::list<Amg::Vector3D>&,
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
IntersectorWrapper::createParameters (Cache& cache,
                                      const Surface&	surface,
                                      const BoundaryCheck& 	/*boundsCheck*/,
                                      bool		curvilinear) const
{
  cache.m_parameters	= nullptr;
  if (! cache.m_intersection) return;

  // curvilinear special (simple) case
  if (curvilinear)
  {
    cache.m_parameters=std::make_unique<CurvilinearParameters>(cache.m_intersection->position(),
                                                 cache.m_intersection->direction().phi(),
                                                 cache.m_intersection->direction().theta(),
                                                 cache.m_qOverP);
    return;
  }

  cache.m_parameters=surface.createUniqueTrackParameters(cache.m_intersection->position(),
                                                   cache.m_intersection->direction(),
                                                   cache.m_charge,nullptr);
  // unrecognized Surface
  if( !cache.m_parameters ) ATH_MSG_WARNING( " Failed to create parameters " );
}

void
IntersectorWrapper::findIntersection (Cache& cache,
                                      const TrackParameters&	parameters,
                                      const Surface&		surface,
                                      PropDirection	       	dir) const
{
  cache.m_charge	=  parameters.charge();
  cache.m_momentum	=  parameters.momentum();
  cache.m_parameters= nullptr;
  cache.m_position	=  Amg::Vector3D(parameters.position());
  cache.m_qOverP	=  1./cache.m_momentum.mag();
  cache.m_intersection = std::make_unique<TrackSurfaceIntersection>(cache.m_position,cache.m_momentum*cache.m_qOverP,0.);
  cache.m_qOverP	*= cache.m_charge;

  const TrackSurfaceIntersection* oldIntersection = cache.m_intersection.release();
  cache.m_intersection.reset( m_intersector->intersectSurface(surface,
                                                              oldIntersection,
                                                              cache.m_qOverP));
  delete oldIntersection;
  if (! cache.m_intersection)
  {
    ATH_MSG_DEBUG( " no intersection found " );
    return;
  }

  // check for correct propagation direction
  if ((dir == Trk::alongMomentum && cache.m_intersection->pathlength() < 0.)
      || (dir == Trk::oppositeMomentum && cache.m_intersection->pathlength() > 0.))
  {
    if (msgLvl(MSG::DEBUG))
    {
      if (dir == Trk::alongMomentum)
      {
        ATH_MSG_DEBUG( " requested alongMomentum, but pathlength "
                       << cache.m_intersection->pathlength() );
      }
      else if (dir == Trk::oppositeMomentum)
      {
        ATH_MSG_DEBUG( " requested oppositeMomentum, but pathlength "
                       << cache.m_intersection->pathlength() );
      }
    }
    cache.m_intersection=nullptr;
    return;
  }

  ATH_MSG_DEBUG( std::setiosflags(std::ios::fixed)
                 << " intersection at r,phi,z "
                 << std::setw(10) << std::setprecision(1) << cache.m_intersection->position().perp()
                 << std::setw(9)  << std::setprecision(4) << cache.m_intersection->position().phi()
                 << std::setw(10) << std::setprecision(1) << cache.m_intersection->position().z()
                 << " momentum "
                 << std::setw(9) << std::setprecision(3) << 1./fabs(cache.m_qOverP*Gaudi::Units::GeV) );
}

} // end of namespace

