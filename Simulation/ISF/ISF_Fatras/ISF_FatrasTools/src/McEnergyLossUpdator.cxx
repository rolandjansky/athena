/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// McMaterialEffectsUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/McEnergyLossUpdator.h"

// Trk
#include "TrkGeometry/MaterialProperties.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"

#include "CLHEP/Random/RandLandau.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

// static partilce masses
Trk::ParticleMasses iFatras::McEnergyLossUpdator::s_particleMasses;

iFatras::McEnergyLossUpdator::McEnergyLossUpdator( const std::string& type, const std::string& name, const IInterface* parent )
  :
  AthAlgTool( type, name, parent ),
  m_energyLossUpdator("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator" ),
  m_energyLossDistribution(3),
  m_rndGenSvc("AtDSFMTGenSvc", name),
  m_randomEngine(0),
  m_randomEngineName("FatrasRnd"),
  m_usePDGformula(false)
{
  declareInterface<IEnergyLossUpdator>(this);
  // heavy particles energy loss
  declareProperty( "EnergyLossUpdator",                 m_energyLossUpdator);
  // random number service
  declareProperty( "RandomNumberService",               m_rndGenSvc);
  declareProperty( "UsePDG_EnergyLossFormula",          m_usePDGformula);
  declareProperty( "EnergyLossDistribution",            m_energyLossDistribution);
  declareProperty( "RandomStreamName",                  m_randomEngineName,     "Name of the random number stream");
}

iFatras::McEnergyLossUpdator::~McEnergyLossUpdator()
{}

StatusCode iFatras::McEnergyLossUpdator::initialize()
{

  ATH_MSG_INFO( "initialize()" );
   
  // Retrieve the energy loss updator tool
  if ( m_energyLossUpdator.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve " << m_energyLossUpdator );
    return StatusCode::FAILURE;
  }

  // get the random generator serice
   if (m_rndGenSvc.retrieve().isFailure()){
        ATH_MSG_FATAL( "Could not retrieve " << m_rndGenSvc );
        return StatusCode::FAILURE;
   } else 
        ATH_MSG_VERBOSE( "Successfully retrieved " << m_rndGenSvc );
  
  //Get own engine with own seeds:
  m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
      ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode iFatras::McEnergyLossUpdator::finalize()
{

  ATH_MSG_INFO( "finalize() successful" );

  return StatusCode::SUCCESS;

}

double iFatras::McEnergyLossUpdator::dEdX( const Trk::MaterialProperties& materialProperties,
                                          double momentum,
                                          Trk::ParticleHypothesis particleHypothesis ) const
{
  // simply forward the dEdX from the standard energy loss updator
  return m_energyLossUpdator->dEdX( materialProperties, momentum, particleHypothesis );
}

Trk::EnergyLoss* iFatras::McEnergyLossUpdator::energyLoss(
                   const Trk::MaterialProperties& materialProperties,
                   double momentum,
                   double pathCorrection,
                   Trk::PropDirection direction,
                   Trk::ParticleHypothesis particleHypothesis,
                   bool, bool) const
{

  bool mpvSwitch = m_energyLossDistribution<2 ? false : true;
 
  // get the number of the material effects distribution
  Trk::EnergyLoss* sampledEloss = m_energyLossUpdator->energyLoss( 
	                                  materialProperties,
                                          momentum,
                                          pathCorrection,
                                          direction,
                                          particleHypothesis,
                                          mpvSwitch,
					  m_usePDGformula);     


  if (!sampledEloss) return 0; 

  // smear according to preselected distribution
  switch (m_energyLossDistribution) {
    // no straggling
  case 0 : { } break;
    // gaussian smearing
  case 1 : { 
    float deIoni = sampledEloss->sigmaIoni() * CLHEP::RandGaussZiggurat::shoot(m_randomEngine);
    float deRad  = sampledEloss->sigmaRad() * CLHEP::RandGaussZiggurat::shoot(m_randomEngine);
    sampledEloss->update(deIoni,0.,deRad,0.,false);
  } break;
  case 2 : {
    float deIoni = -sampledEloss->sigmaIoni() * CLHEP::RandLandau::shoot(m_randomEngine);  // TODO :check sign
    sampledEloss->update(deIoni,0.,0.,0.,false);   
  } break;
    // landau smearing
    default : {
      float deIoni = -sampledEloss->sigmaIoni() * CLHEP::RandLandau::shoot(m_randomEngine);  // TODO :check sign
      float deRad  = -sampledEloss->sigmaRad() * CLHEP::RandLandau::shoot(m_randomEngine);   // TODO :check sign      
      sampledEloss->update(deIoni,0.,deRad,0.,false);   
    } break;
  }

  // protection due to straggling - maximum energy loss is E-m
  double m     = s_particleMasses.mass[particleHypothesis];
  double E     = sqrt(momentum*momentum+m*m);

  if (sampledEloss->deltaE()+E<m ) {    // particle stopping - rest energy
      float dRad_rest = m-E-sampledEloss->deltaE();
      sampledEloss->update(0.,0.,dRad_rest,0.,false);   
  }

  return sampledEloss;
}
