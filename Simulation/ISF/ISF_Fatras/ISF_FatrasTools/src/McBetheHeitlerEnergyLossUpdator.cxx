/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************
			McBetheHeitlerEnergyLossUpdator.cxx -  description
			---------------------------------------------------
begin                : Wednesday 28th June 2006
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for McBetheHeitlerEnergyLossUpdator class
***************************************************************************************/

// class header include
#include "ISF_FatrasTools/McBetheHeitlerEnergyLossUpdator.h"

#include "TrkGeometry/MaterialProperties.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "CLHEP/Random/RandGamma.h"
#include <cmath>


// static partilce masses
Trk::ParticleMasses iFatras::McBetheHeitlerEnergyLossUpdator::s_particleMasses;

iFatras::McBetheHeitlerEnergyLossUpdator::McBetheHeitlerEnergyLossUpdator( const std::string& type, const std::string& name, const IInterface* parent )
  :
  AthAlgTool( type, name, parent ),
  m_rndmGenSvc("AtDSFMTGenSvc", name),
  m_randomEngine(0),
  m_randomEngineName("FatrasRnd"),
  m_cashedPathLength(0.),
  m_numberPointsInPDF(100000),
  m_scaleFactor(1.0)
{
  
  declareInterface<IEnergyLossUpdator>(this);
  // get the property from outside
  declareProperty("ScaleFactor",      m_scaleFactor);
  declareProperty("RandomNumberService"                 , m_rndmGenSvc         , "Random number generator");
  declareProperty("RandomStreamName"                    , m_randomEngineName   , "Name of the random number stream");
	
}

iFatras::McBetheHeitlerEnergyLossUpdator::~McBetheHeitlerEnergyLossUpdator(){}

StatusCode iFatras::McBetheHeitlerEnergyLossUpdator::initialize()
{

  ATH_MSG_INFO( "initialize()" );
    
  if ( m_rndmGenSvc.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve " << m_rndmGenSvc );
    return StatusCode::FAILURE;
  }
  //Get own engine with own seeds:
  m_randomEngine = m_rndmGenSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
      ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
      return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;  
}

StatusCode iFatras::McBetheHeitlerEnergyLossUpdator::finalize()
{
  
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;	
}

Trk::EnergyLoss* iFatras::McBetheHeitlerEnergyLossUpdator::energyLoss(
                    const Trk::MaterialProperties& materialProperties,
                    double pInitial,
                    double pathCorrection,
                    Trk::PropDirection direction,
                    Trk::ParticleHypothesis,
                    bool) const
{
  
  double pathLength = pathCorrection * materialProperties.thickness() / materialProperties.x0();
  
  double u = CLHEP::RandGamma::shoot(m_randomEngine, pathLength / log(2.), 1.);
  
  double z = exp( -1. * u );
  
  // initial Energy
  double E = sqrt(pInitial*pInitial+s_particleMasses.mass[Trk::electron]*s_particleMasses.mass[Trk::electron]);
  
  
  /*
  if ( fabs( pathLength - m_cashedPathLength ) > 1.e-6 || !m_pdf.size() ){

    m_pdf = betheHeitlerPDF( pathLength );

    m_cashedPathLength = pathLength;
    
  }

  Rndm::Numbers betheHeitler( m_rndmGenSvc, Rndm::DefinedPdf( m_pdf, 0 ) );

  double z = betheHeitler.shoot();

  */

  double deltaE(0.);

  if ( direction == Trk::alongMomentum )
    deltaE = m_scaleFactor*E * ( z - 1. );

  else
    deltaE = m_scaleFactor*E * ( 1. / z - 1. );
    
  return new Trk::EnergyLoss(deltaE, z);
}

std::vector<double> iFatras::McBetheHeitlerEnergyLossUpdator::betheHeitlerPDF( double pathLength ) const
{

  std::vector<double> pdf;

  double c = pathLength / log(2);

  double sizeOfIncrement = 1. / (double) m_numberPointsInPDF;

  double currentZ(0.);

  int index(0);

  for( ; index < m_numberPointsInPDF ; ++index ){
    double pdfEntry = pow( -1. * log( currentZ ), c - 1. ) / CLHEP::RandGamma::shoot(m_randomEngine, c, 1.);
    pdf.push_back( pdfEntry );
    currentZ += sizeOfIncrement;
  }

  return pdf;

}
