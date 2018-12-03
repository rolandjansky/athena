/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossSamplerBetheHeitler.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISF_FatrasTools/EnergyLossSamplerBetheHeitler.h"

#include "TrkGeometry/MaterialProperties.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "CLHEP/Random/RandGamma.h"
#include "CLHEP/Random/RandLandau.h"
#include "CLHEP/Random/RandFlat.h"
#include <cmath>

// static partilce masses
Trk::ParticleMasses iFatras::EnergyLossSamplerBetheHeitler::s_particleMasses;

iFatras::EnergyLossSamplerBetheHeitler::EnergyLossSamplerBetheHeitler( const std::string& type, const std::string& name, const IInterface* parent )
  :  base_class( type, name, parent ),
     m_rndmGenSvc("AtDSFMTGenSvc", name),
     m_randomEngine(0),
     m_randomEngineName("FatrasRnd"),
     m_scaleFactor(1.0)
{
  // get the property from outside
  declareProperty("ScaleFactor",      m_scaleFactor);
  declareProperty("RandomNumberService"                 , m_rndmGenSvc         , "Random number generator");
  declareProperty("RandomStreamName"                    , m_randomEngineName   , "Name of the random number stream");
	
}

iFatras::EnergyLossSamplerBetheHeitler::~EnergyLossSamplerBetheHeitler(){}

StatusCode iFatras::EnergyLossSamplerBetheHeitler::initialize()
{
  ATH_MSG_INFO( "initialize()" );
  
  if ( m_rndmGenSvc.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve " << m_rndmGenSvc );
    return StatusCode::FAILURE;
  } else ATH_MSG_VERBOSE( "Successfully retrieved " << m_rndmGenSvc );
  
  //Get own engine with own seeds:
  m_randomEngine = m_rndmGenSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE;
  } else ATH_MSG_VERBOSE ( "Successfully got random engine '" << m_randomEngineName << "'");
  
  return StatusCode::SUCCESS;  
}

StatusCode iFatras::EnergyLossSamplerBetheHeitler::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;	
}

Trk::EnergyLoss* iFatras::EnergyLossSamplerBetheHeitler::energyLoss( const Trk::MaterialProperties& materialProperties,
								     double pInitial,
								     double pathCorrection,
								     Trk::PropDirection direction,
								     Trk::ParticleHypothesis,
								     bool) const
{
  
  double pathLength = pathCorrection*materialProperties.thicknessInX0();

  if (pathLength==0.) return new Trk::EnergyLoss(0.,0.);

  double p    = pInitial;
  double me   = s_particleMasses.mass[Trk::electron];
  double E    = sqrt(p*p+me*me);
  
  // the following formulas are imported from STEP
  // preparation of kinetic constants
 
  double beta  = p/E;
  double gamma = E/me;
  double eta2 = beta*gamma; eta2 *= eta2;  
  
  //Ionization - Bethe-Bloch
  double I = 16.e-6 * std::pow(materialProperties.averageZ(),0.9); //16 eV * Z**0.9 - bring to MeV
  
  double delta = 0.;
  if (gamma > 10.) {
    double eplasma = 28.816e-6 * sqrt(1000.*materialProperties.zOverAtimesRho());
    delta = 2.*log(eplasma/I) + log(eta2) - 1.;
  }
  
  //K/A*Z = 0.5 * 30.7075MeV/(g/mm2) * Z/A * rho[g/mm3]  / scale to mm by this
  double kaz = 0.5*30.7075*materialProperties.zOverAtimesRho();
  double kazL = kaz*pathCorrection;
  
  // for electrons use slightly different BetheBloch adaption
  // see Stampfer, et al, "Track Fitting With Energy Loss", Comp. Pyhs. Comm. 79 (1994), 157-164
  //
  // the landau sigmaL is path length dependent
  //    PDG formula 32.11 for MOP value from http://http://pdg.lbl.gov/2014/reviews/rpp2014-rev-passage-particles-matter.pdf
  //
  
  double MOP =  -kazL*kaz*(log(2.*me*eta2/I) + log(kazL/I) + 0.2 - (beta*beta) - delta);

  double energyLossSigma = 0.424*4.*kazL; //0.424: scale factor from sigma to FWHM
  
  double simulatedDeltaE = fabs(MOP)+energyLossSigma*CLHEP::RandLandau::shoot(m_randomEngine);  


  //Bethe-Heitler for electron brem description as described here:
  // "A Gaussian-mixture approximation of the Bethe–Heitler model of electron energy loss by bremsstrahlung"
  // R. Frühwirth
  
  double u = CLHEP::RandGamma::shoot(m_randomEngine, pathLength / log(2.), 1.);
  double z = exp( -1. * u );
  double deltaE(0.);

  if ( direction == Trk::alongMomentum )
    deltaE = m_scaleFactor*E * ( z - 1. );
  else
    deltaE = m_scaleFactor*E * ( 1. / z - 1. );

  simulatedDeltaE+=fabs(deltaE);
  
  // transform into negative energyloss
  simulatedDeltaE *= direction;
  
  // protection due to straggling - maximum energy loss is E
  simulatedDeltaE = simulatedDeltaE*simulatedDeltaE > E*E ? E : simulatedDeltaE;
  
  ATH_MSG_VERBOSE( "[+] EnergyLossSamplerBetheHeitler created random deltaP as : " << -1.*simulatedDeltaE );
  
  return new Trk::EnergyLoss( -1.*simulatedDeltaE, energyLossSigma);
     
}

