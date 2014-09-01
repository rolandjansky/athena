/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexBeamCondPositioner.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISF_HepMC_Tools/VertexBeamCondPositioner.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
// RandomNumber generator
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

/** Constructor */
ISF::VertexBeamCondPositioner::VertexBeamCondPositioner( const std::string& t,
                                                     const std::string& n,
                                                     const IInterface* p )
  : AthAlgTool(t,n,p)  ,
    m_beamCondSvc("BeamCondSvc", n),
    m_rndGenSvc("AtRndmGenSvc", n),
    m_randomEngine(0),
    m_randomEngineName("VERTEX")
{
    declareInterface<ISF::ILorentzVectorGenerator>(this);

    // declare properties for the configuration
    declareProperty( "BeamCondSvc"             , m_beamCondSvc              );
    declareProperty( "RandomSvc"               , m_rndGenSvc                );
    declareProperty( "RandomStream"            , m_randomEngineName         );
}


/** Destructor */
ISF::VertexBeamCondPositioner::~VertexBeamCondPositioner()
{
}


/** Athena algtool's Hooks */
StatusCode  ISF::VertexBeamCondPositioner::initialize()
{

    ATH_MSG_VERBOSE("Initializing ...");

    // retrieve the BeamCondService
    if ( m_beamCondSvc.retrieve().isFailure() ){
        ATH_MSG_ERROR("Could not retrieve BeamConditionsSvc '" << m_beamCondSvc << "'. Abort.");
        return StatusCode::FAILURE;
    }
    // prepare the RandonNumber generation
    if ( m_rndGenSvc.retrieve().isFailure() ){
        ATH_MSG_ERROR("Could not retrieve RandomNumberSvc '" << m_rndGenSvc << "'. Abort.");
        return StatusCode::FAILURE;
    }

    // everything set up properly
    return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  ISF::VertexBeamCondPositioner::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}


/** computes the vertex displacement */
CLHEP::HepLorentzVector *ISF::VertexBeamCondPositioner::generate()
{
  m_randomEngine = m_rndGenSvc->GetEngine( m_randomEngineName);
  if (!m_randomEngine){
      ATH_MSG_ERROR("Could not get random number engine from RandomNumberService. Abort.");
      return 0;
  }

  // See jira issue ATLASSIM-497 for an explanation of why calling
  // shoot outside the CLHEP::HepLorentzVector constructor is
  // necessary/preferable.
  float vertexX = CLHEP::RandGaussZiggurat::shoot(m_randomEngine)*m_beamCondSvc->beamSigma(0);
  float vertexY = CLHEP::RandGaussZiggurat::shoot(m_randomEngine)*m_beamCondSvc->beamSigma(1);
  float vertexZ = CLHEP::RandGaussZiggurat::shoot(m_randomEngine)*m_beamCondSvc->beamSigma(2);
  // calculate the vertexSmearing
  CLHEP::HepLorentzVector *vertexSmearing = new CLHEP::HepLorentzVector(
                                      vertexX,
                                      vertexY,
                                      vertexZ,
                                      0. );

  // (1) code from: Simulation/G4Atlas/G4AtlasUtilities/VertexPositioner.cxx
  const double tx = tan( m_beamCondSvc->beamTilt(1) );
  const double ty = tan( m_beamCondSvc->beamTilt(0) );

  const double sqrt_abc = sqrt(1. + tx*tx + ty*ty);
  const double sqrt_fgh = sqrt(1. + ty*ty);

  const double a = ty/sqrt_abc;
  const double b = tx/sqrt_abc;
  const double c = 1./sqrt_abc;

  HepGeom::Point3D<double> from1(0,0,1);
  HepGeom::Point3D<double> to1(a,b,c);

  const double f = 1./sqrt_fgh;
  const double g = 0.;
  const double h = -(ty)/sqrt_fgh;

  HepGeom::Point3D<double> from2(1,0,0);
  HepGeom::Point3D<double> to2(f,g,h);

  // first rotation, then translation
  HepGeom::Transform3D transform( HepGeom::Rotate3D(from1, from2, to1, to2).getRotation(),
                                  CLHEP::Hep3Vector(m_beamCondSvc->beamPos().x(), m_beamCondSvc->beamPos().y(), m_beamCondSvc->beamPos().z())
                            );

  if (msgLvl(MSG::VERBOSE)){
     msg(MSG::VERBOSE) << "BeamSpotSvc reported beam position as " << m_beamCondSvc->beamPos() << std::endl
                       << "\tWidth is (" << m_beamCondSvc->beamSigma(0)
                       << ", " << m_beamCondSvc->beamSigma(1) << ", "
                       << m_beamCondSvc->beamSigma(2) << ")" << std::endl
                       << "\tTilts are " << m_beamCondSvc->beamTilt(0) << " and " << m_beamCondSvc->beamTilt(1) << std::endl
                       << "\tVertex Position before transform: " << *vertexSmearing << endreq;
  }

  // update with the tilt
  *vertexSmearing = transform * HepGeom::Point3D<double>(*vertexSmearing);
  // and return it
  return vertexSmearing;
}
