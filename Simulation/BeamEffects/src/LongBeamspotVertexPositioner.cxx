/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LongBeamspotVertexPositioner.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "LongBeamspotVertexPositioner.h"

// For the speed of light
#include "GaudiKernel/PhysicalConstants.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include <math.h>       /* erf */
// RandomNumber generator
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

/** Constructor */
Simulation::LongBeamspotVertexPositioner::LongBeamspotVertexPositioner( const std::string& t,
                                                                        const std::string& n,
                                                                        const IInterface* p )
  : base_class(t,n,p),
    m_L(150.0),// 150.0 mm
    m_beamCondSvc("BeamCondSvc", n),
    m_rndGenSvc("AtRndmGenSvc", n),
    m_randomEngine(0),
    m_randomEngineName("VERTEX"),
    m_timeSmearing(false)
{
  // declare properties for the configuration
  declareProperty( "LParameter"              , m_L              );
  declareProperty( "BeamCondSvc"             , m_beamCondSvc              );
  declareProperty( "RandomSvc"               , m_rndGenSvc                );
  declareProperty( "RandomStream"            , m_randomEngineName         );
  declareProperty( "SimpleTimeSmearing"      , m_timeSmearing     );
}


/** Destructor */
Simulation::LongBeamspotVertexPositioner::~LongBeamspotVertexPositioner()
{
}


/** Athena algtool's Hooks */
StatusCode  Simulation::LongBeamspotVertexPositioner::initialize()
{

  ATH_MSG_VERBOSE("Initializing ...");

  // retrieve the BeamCondService
  ATH_CHECK(m_beamCondSvc.retrieve());
  // prepare the RandonNumber generation
  ATH_CHECK(m_rndGenSvc.retrieve());
  m_randomEngine = m_rndGenSvc->GetEngine( m_randomEngineName);
  if (!m_randomEngine)
    {
      ATH_MSG_ERROR("Could not get random number engine from RandomNumberService. Abort.");
      return StatusCode::FAILURE;
    }

  // everything set up properly
  return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  Simulation::LongBeamspotVertexPositioner::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}

double Simulation::LongBeamspotVertexPositioner::beamspotFunction(double z) const
{
  //double norm(1.0/232.06);
  double temp(1.0-std::abs(z)/m_L);
  return erf(2.5*temp)*heaviside(temp); // zero for |z|>150.0
}

double Simulation::LongBeamspotVertexPositioner::getZpos() const
{
  size_t ntries(0);
  double yval(CLHEP::RandFlat::shoot(m_randomEngine, 0.0, 1.0));
  double zval(CLHEP::RandFlat::shoot(m_randomEngine, -150.0, 150.0));
  while (this->beamspotFunction(zval)<yval) {
    if(ntries>1000000) return 0.0; //just so we don't sit in this loop forever
    yval = CLHEP::RandFlat::shoot(m_randomEngine, 0.0, 1.0);
    zval = CLHEP::RandFlat::shoot(m_randomEngine, -150.0, 150.0);
    ++ntries;
  }
  return zval;
}

/** computes the vertex displacement */
CLHEP::HepLorentzVector *Simulation::LongBeamspotVertexPositioner::generate() const
{
  if (!m_randomEngine)
    {
      ATH_MSG_ERROR("No random number engine! Abort.");
      return nullptr;
    }

  // See jira issue ATLASSIM-497 for an explanation of why calling
  // shoot outside the CLHEP::HepLorentzVector constructor is
  // necessary/preferable.
  float vertexX = CLHEP::RandGaussZiggurat::shoot(m_randomEngine)*m_beamCondSvc->beamSigma(0);
  float vertexY = CLHEP::RandGaussZiggurat::shoot(m_randomEngine)*m_beamCondSvc->beamSigma(1);
  float vertexZ = this->getZpos();
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

  ATH_MSG_VERBOSE("BeamSpotSvc reported beam position as " << m_beamCondSvc->beamPos() << std::endl
                  << "\tWidth is (" << m_beamCondSvc->beamSigma(0)
                  << ", " << m_beamCondSvc->beamSigma(1) << ", "
                  << m_L << ")" << std::endl
                  << "\tTilts are " << m_beamCondSvc->beamTilt(0) << " and " << m_beamCondSvc->beamTilt(1) << std::endl
                  << "\tVertex Position before transform: " << *vertexSmearing);

  // update with the tilt
  *vertexSmearing = transform * HepGeom::Point3D<double>(*vertexSmearing);

  // See if we were asked to do time smearing as well
  if (m_timeSmearing)
    {
      /* This is ballpark code courtesy of Brian Amadio.  He provided some functions based on beam parameters.
         He provided a little trick for pulling out the beam bunch width as well.  Hard coding the crossing angle
         parameter for the time being, as the beam spot service doesn't really provide that yet.  */
      // Assumes that to make these funny beam spots we have a normal-looking bunch
      double bunch_length_z = (std::sqrt(2)*vertexZ)/0.9; // 0.9 is the crossing angle reduction factor
      //    double tLimit = 2.*(bunch_length_z+bunch_length_z)/Gaudi::Units::c_light;
      //    TF1 func = TF1("func","[0]*exp((-([3]-299792458*x)^2*[2]^2-([3]+299792458*x)^2*[1]^2)/(2*[1]^2*[2]^2))",-1*tLimit,tLimit);
      //    func.SetParameter(0,Gaudi::Units::c_light/(M_PI*bunch_length_z*bunch_length_z));
      //    func.SetParameter(1,bunch_length_z);
      //    func.SetParameter(2,bunch_length_z);
      //    func.SetParameter(3,vertexSmearing->z());
      //    double time_offset = func.GetRandom();

      // Time should be set in units of distance, which is a little funny
      double time_offset = CLHEP::RandGaussZiggurat::shoot( m_randomEngine , vertexSmearing->z()/Gaudi::Units::c_light , bunch_length_z/Gaudi::Units::c_light );

      vertexSmearing->setT( vertexSmearing->t() + time_offset*Gaudi::Units::c_light );
    }

  // and return it
  return vertexSmearing;
}
