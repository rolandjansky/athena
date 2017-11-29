/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool to obtain Perigee or Track Parameters corresponding to a simulated 
 particle, either from its barcode or from a HepMC GenParticle.
***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include "GaudiKernel/IPartPropSvc.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "GenInterfaces/ITruthSelector.h"
#include "iPatTruthTrajectory/TruthParameters.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TruthParameters::TruthParameters (const std::string&	type,
				  const std::string&	name, 
				  const IInterface*	parent)
  :	AthAlgTool		(type, name, parent),
	m_intersector		("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
	m_particleDataTable	(0),
	m_truthSelector		("TruthSelector/TruthSelector"),
	m_intersection		(nullptr),
	m_qOverP		(0.)
{
    declareInterface<ITruthParameters>(this);
    declareProperty("Intersector",	m_intersector);
    declareProperty("TruthSelector",	m_truthSelector);
}

TruthParameters::~TruthParameters (void) 
{}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS>>>>>>

const HepMC::GenParticle*
TruthParameters::findParticle (int barCode)
{
    // protect against real data, fakes ...
    const HepMC::GenParticle* particle = 0;
    if (barCode == 0) return particle;
  
    // retrieve McEventCollection (from truthSelector to give G3/4 simulation independence)
    const HepMC::GenEvent* event = *m_truthSelector->mcEventCollection()->begin();
    particle = event->barcode_to_particle(barCode);

    if (particle == 0)	ATH_MSG_DEBUG( "particle not found:  barCode " << barCode );

    return particle;
}

void
TruthParameters::trackFromParticle(const HepMC::GenParticle& particle)
{
    // protect against missing production_vertex
    if (! particle.production_vertex()) return;

    // ensure known particle
    m_intersection		=  0;
    int	pdgCode			=  particle.pdg_id();
    if (! m_particleDataTable->particle(std::abs(pdgCode))) return;

    // fill intersection, qOverP
    Amg::Vector3D position( particle.production_vertex()->point3d().x(),
			    particle.production_vertex()->point3d().y(),
			    particle.production_vertex()->point3d().z());
    m_qOverP			=  1./std::sqrt(particle.momentum().px()*particle.momentum().px() +
						particle.momentum().py()*particle.momentum().py() +
						particle.momentum().pz()*particle.momentum().pz());
    Amg::Vector3D direction( m_qOverP*particle.momentum().px(),
			     m_qOverP*particle.momentum().py(),
			     m_qOverP*particle.momentum().pz());
    m_intersection		=  new Trk::TrackSurfaceIntersection(position,direction,0.);
	
    // fill trackParameters
    double cosTheta		=  direction(2);
    double sinThetaInv		=  1./std::sqrt(1. - cosTheta*cosTheta);
    double cosPhi		=  sinThetaInv*direction(0);
    double sinPhi		=  sinThetaInv*direction(1);
    if (pdgCode > 0)
    {
	m_qOverP		*= m_particleDataTable->particle(pdgCode)->charge();
    }
    else
    {
	m_qOverP		*= -m_particleDataTable->particle(-pdgCode)->charge();
    }
    m_trackParameters.fill(position,cosPhi,sinPhi,sinThetaInv*cosTheta,sinThetaInv*m_qOverP);
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TruthParameters::initialize()
{
    // print where you are
    ATH_MSG_INFO( "TruthParameters::initialize() - package version " << PACKAGE_VERSION );

    // get the Particle Properties Service
    IPartPropSvc* partPropSvc = 0;
    if (StatusCode::SUCCESS != service("PartPropSvc", partPropSvc, true))
    {
	ATH_MSG_FATAL( " Could not initialize Particle Properties Service" );
	return StatusCode::FAILURE;
    }      
    m_particleDataTable = partPropSvc->PDT();
       
    // retrieve the necessary tools
    if (m_intersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_intersector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_intersector );
    }
    if (m_truthSelector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_truthSelector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_truthSelector );
    }

    return StatusCode::SUCCESS;
}

StatusCode
TruthParameters::finalize()
{
    return StatusCode::SUCCESS;
}

const PerigeeParameters*
TruthParameters::perigeeParameters (int barCode, const Amg::Vector3D& vertex)
{
    const HepMC::GenParticle* particle = findParticle(barCode);
    if (! particle)		return 0;
    trackFromParticle(*particle);
    if (! m_intersection)	return 0;

    Trk::PerigeeSurface surface(vertex);
    const Trk::TrackSurfaceIntersection* intersection =
	m_intersector->intersectSurface(surface,m_intersection,m_qOverP);
    if (! intersection)
    {
	ATH_MSG_DEBUG(" failure to intersect vertex at " << vertex.x() << ", "
		      << vertex.y() << ", " << vertex.z() );
	delete m_intersection;
	return 0;
    }

    delete m_intersection;
    m_intersection		= intersection;
    double cosTheta		= m_intersection->direction()(2);
    double sinThetaInv		= 1./std::sqrt(1. - cosTheta*cosTheta);
    double cosPhi		= sinThetaInv*m_intersection->direction()(0);
    double sinPhi		= sinThetaInv*m_intersection->direction()(1);
    double transverseImpact	= (m_intersection->position()(1) - vertex(1))*cosPhi -
				  (m_intersection->position()(0) - vertex(0))*sinPhi;
    
    m_perigeeParameters.vertex(vertex);
    m_perigeeParameters.fillPerigee(cosPhi,
				    sinThetaInv*cosTheta,
				    sinThetaInv*m_qOverP,
				    sinPhi,
				    transverseImpact,
				    m_intersection->position()(2));
    // m_perigeeParameters.print();
    delete m_intersection;
    return &m_perigeeParameters;
}

const PerigeeParameters*
TruthParameters::perigeeParameters (const HepMC::GenParticle& particle, const Amg::Vector3D& vertex)
{
    trackFromParticle(particle);
    if (! m_intersection)	return 0;
    
    Trk::PerigeeSurface surface(vertex);
    const Trk::TrackSurfaceIntersection* intersection =
	m_intersector->intersectSurface(surface,m_intersection,m_qOverP);
    if (! intersection)
    {
	ATH_MSG_DEBUG(" failure to intersect vertex at " << vertex.x() << ", "
		      << vertex.y() << ", " << vertex.z() );
	delete m_intersection;
	return 0;
    }

    delete m_intersection;
    m_intersection		= intersection;
    double cosTheta		= m_intersection->direction()(2);
    double sinThetaInv		= 1./std::sqrt(1. - cosTheta*cosTheta);
    double cosPhi		= sinThetaInv*m_intersection->direction()(0);
    double sinPhi		= sinThetaInv*m_intersection->direction()(1);
    double transverseImpact	= (m_intersection->position()(1) - vertex(1))*cosPhi -
				  (m_intersection->position()(0) - vertex(0))*sinPhi;
    
    m_perigeeParameters.vertex(vertex);
    m_perigeeParameters.fillPerigee(cosPhi,
				    sinThetaInv*cosTheta,
				    sinThetaInv*m_qOverP,
				    sinPhi,
				    transverseImpact,
				    m_intersection->position()(2));
    delete m_intersection;
    return &m_perigeeParameters;
}

const TrackParameters*
TruthParameters::trackParameters (int barCode)
{
    const HepMC::GenParticle* particle = findParticle(barCode);
    if (! particle)		return 0;
    trackFromParticle(*particle);
    if (! m_intersection)	return 0;
    delete m_intersection;
    return &m_trackParameters;
}

const TrackParameters*
TruthParameters::trackParameters (const HepMC::GenParticle& particle)
{
    trackFromParticle(particle);
    if (! m_intersection)	return 0;
    delete m_intersection;
    return &m_trackParameters;
}

 
