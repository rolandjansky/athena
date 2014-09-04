/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ProcessSamplingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/ProcessSamplingTool.h"

// Tracking
#include "TrkParameters/TrackParameters.h"
// ISF includes
#include "ISF_Event/ISFParticle.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandFlat.h"
//
 
#include <iostream>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::ProcessSamplingTool::ProcessSamplingTool( const std::string& t,
                                      const std::string& n,
                                      const IInterface*  p )
  : AthAlgTool(t,n,p),
   m_rndGenSvc("AtDSFMTGenSvc", n),
   m_randomEngine(0),
   m_randomEngineName("FatrasRnd")
{
  declareInterface<IProcessSamplingTool>(this);
  // service handles
  declareProperty( "RandomNumberService",       m_rndGenSvc                );
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::ProcessSamplingTool::~ProcessSamplingTool()
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::ProcessSamplingTool::initialize()
{
  
  ATH_MSG_INFO( "initialize()" );

  if ( m_rndGenSvc.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve " << m_rndGenSvc );
    return StatusCode::FAILURE;
  }
  //Get own engine with own seeds:
  m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode iFatras::ProcessSamplingTool::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

Trk::PathLimit iFatras::ProcessSamplingTool::sampleProcess(double momentum,double charge,Trk::ParticleHypothesis particle) 
{
  int process=0;
  double x0Max = -1.;

  if ( particle == Trk::electron && charge<0. ) return Trk::PathLimit(x0Max,process);

  if ( particle == Trk::muon || particle == Trk::nonInteractingMuon ) return Trk::PathLimit(x0Max,process);

  double rndx = CLHEP::RandFlat::shoot(m_randomEngine);

  if ( particle == Trk::photon ) {

    // conversion (14), compton effect (13), photo-effect (12)

    double  p0  =       -7.01612e-03;
    double  p1  =        7.69040e-02;
    double  p2  =       -6.07682e-01;
    // calculate xi
    double xi = p0 + p1*pow(momentum/1000.,p2);
    double attenuation = -7.777e-01*(1.-xi);

    x0Max = log(rndx)/attenuation ;

    // comment additional interactions till properly implemented
    /*
    // first estimate : compton ~ 4% , use attenuation factor as for conversion (till parameterized )
    double rndp = CLHEP::RandFlat::shoot(m_randomEngine);

    if ( rndp > 0.96 ) { // do Compton scattering
      process = 13;
    } else if (rndp < 5.53/pow(momentum,1.184) ) {
      process = 12;
      attenuation = -0.378;
      x0Max = log(rndx)/attenuation *momentum ;
    } else {
      process = 14; 
      //x0Max *= 0.5;
    }
    */
   
    process = 14;

    return Trk::PathLimit(x0Max,process);
  }

  if ( particle == Trk::electron && charge>0. ) {   // positron

    double mass = m_particleMasses.mass[Trk::electron];
    double gamma = momentum/mass; 

    // annihilation
    // energy dependent factor 
    double fpa = log(2*gamma)/gamma; 
    // material dependent factor - use Al(Z=13) for the moment
    double fza = 13.;
    double attenuation = -fpa*fza; // adjust
    
    x0Max = log(rndx)/attenuation ;
    process = 5;

    //std::cout <<"annihilation path limit:"<< x0Max << std::endl;

    return Trk::PathLimit(x0Max,process);

  }

  // presumably here we are left with hadrons only

  // sample free path in terms of nuclear interaction length
  double al = 1.;          // scaling here 

  /*

  if ( particle == Trk::pion || particle == Trk::kaon || particle == Trk::pi0 || particle == Trk::k0) {
    al *= 1./(1.+ exp(-0.5*(momentum-270.)*(momentum-270.)/60./60.));
  }
  if ( particle == Trk::proton || particle == Trk::neutron ) al *=0.7;
  if ( particle == Trk::pion || particle == Trk::pi0) al *=0.9;
  */
  
  x0Max = -log(rndx)*al ;

  process = 121;

  //std::cout <<"hadronic path limit:"<<momentum<<","<<al<<","<< x0Max << std::endl;
  
  return Trk::PathLimit(x0Max,process);
    
  // return Trk::PathLimit(x0Max,process);
}


