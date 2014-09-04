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
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

// static partilce masses
Trk::ParticleMasses iFatras::McEnergyLossUpdator::s_particleMasses;

iFatras::McEnergyLossUpdator::McEnergyLossUpdator( const std::string& type, const std::string& name, const IInterface* parent )
  :
  AthAlgTool( type, name, parent ),
  m_energyLossUpdator("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator" ),
  m_energyLossDistribution(2),
  m_dedicatedElectronUpdator(true),
  m_elEnergyLossUpdator("iFatras::McBetheHeitlerEnergyLossUpdator/FatrasBetheHeitlerEnergyLossUpdator" ),
  m_rndGenSvc("AtDSFMTGenSvc", name),
  m_randomEngine(0),
  m_randomEngineName("FatrasRnd")
{
  declareInterface<IEnergyLossUpdator>(this);
  // heavy particles energy loss
  declareProperty( "EnergyLossUpdator",                 m_energyLossUpdator);
  // dedicated electron updates
  declareProperty( "UseElectronUpdator",                m_dedicatedElectronUpdator );
  declareProperty( "ElectronEnergyLossUpdator",         m_elEnergyLossUpdator);
  // random number service
  declareProperty( "RandomNumberService",               m_rndGenSvc);
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


  // Retrieve the dedicated electron energz loss updator
  if (m_dedicatedElectronUpdator){
    // Retrieve the energy loss updator tool
    if ( m_elEnergyLossUpdator.retrieve().isFailure() ){
      ATH_MSG_FATAL( "Could not retrieve " << m_elEnergyLossUpdator );
      return StatusCode::FAILURE;
    }  
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
                   bool) const
{

  double simulatedDeltaE = 0.;
 
  // get the number of the material effects distribution
  Trk::EnergyLoss* sampledEloss = m_energyLossUpdator->energyLoss( 
	                                  materialProperties,
                                          momentum,
                                          pathCorrection,
                                          direction,
                                          particleHypothesis,
                                          true);

  
  double energyLoss      = 0.;
  double energyLossSigma = 0.;

  double m     = s_particleMasses.mass[particleHypothesis];
  double E     = sqrt(momentum*momentum+m*m);
  
  // get the sampled energy loss
  if(sampledEloss){

    // get the energy loss and its sigma 
    // for gauss: mean, sigma
    // for landau: mpv, sigma
    energyLoss      = fabs(sampledEloss->deltaE());
    energyLossSigma = sampledEloss->sigmaDeltaE();
    // memory cleanup
    delete sampledEloss;
  }

  switch (m_energyLossDistribution) {
    // no straggling
    case 0 : { simulatedDeltaE += energyLoss; } break;
    // gaussian smearing
    case 1 : { simulatedDeltaE += energyLoss + energyLossSigma * CLHEP::RandGaussZiggurat::shoot(m_randomEngine); } break;
    // landau smearing
    default : {
       simulatedDeltaE += (energyLoss+energyLossSigma*CLHEP::RandLandau::shoot(m_randomEngine));  
   
    } break;
  }

  // transform into negative nergyloss
  simulatedDeltaE *= direction;

  /* ST radiative corrections treated separately           
  // it is an electron and has radiative corrections
  if (m_dedicatedElectronUpdator && particleHypothesis==Trk::electron) {
       // !< @ todo introduce cut for the calculation of bremsstrahlung  
       Trk::EnergyLoss* radiationLoss = m_elEnergyLossUpdator->energyLoss(
                materialProperties,
                momentum,
                pathCorrection,
                direction,
                particleHypothesis);
       // add the simulated bethe heitler energy loss
       double radiationDeltaE = radiationLoss ? radiationLoss->deltaE() : 0.;
       
       simulatedDeltaE += (-1*radiationDeltaE);
       delete radiationLoss;
  }
  */

  // protection due to straggling - maximum energy loss is E
  simulatedDeltaE = simulatedDeltaE*simulatedDeltaE > E*E ? E : simulatedDeltaE;
  
  ATH_MSG_VERBOSE( "[+] FatrasEnergyUpdator created random deltaP as : " << -1.*simulatedDeltaE );

  return new Trk::EnergyLoss( -1.*simulatedDeltaE, 0. );
}
