/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/TransportEngine.h"

// Tracking
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkExInterfaces/IExtrapolationEngine.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"

#include "TrkGeometry/Layer.h"
#include "TrkGeometry/TrackingVolume.h"

// iFatras includes
#include "ISF_FatrasInterfaces/ISimHitCreator.h"
#include "ISF_FatrasInterfaces/IParticleDecayHelper.h"
#include "ISF_FatrasInterfaces/IProcessSamplingTool.h"
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ParticleClipboard.h"
#include "ISF_Interfaces/IParticleFilter.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandFlat.h"

// AtlasDetDescr
#include "AtlasDetDescr/AtlasRegion.h"

#include <iostream>

//#include "AtlasDetDescr/AtlasDetectorID.h"
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::TransportEngine::TransportEngine( const std::string& t,
				       const std::string& n,
				       const IInterface*  p )
  : base_class(t,n,p),
    m_rndGenSvc("AtDSFMTGenSvc", n), 
    m_randomEngine(0),
    m_randomEngineName("FatrasRnd"),
    m_particleDecayHelper(""),
    m_simHitCreatorID(""),
    m_extrapolationEngine(""),
    m_trackFilter(""),
    m_neutralHadronFilter(""),
    m_photonFilter(""),
    m_samplingTool(""),
    m_validationMode(false),
    m_validationTool("")
{
  // validation output section
  declareProperty( "ValidationMode",  m_validationMode );
  declareProperty( "PhysicsValidationTool",     m_validationTool  );
  // tool handle for the particle decayer
  declareProperty( "ParticleDecayHelper",       m_particleDecayHelper      );  
  // tool handle for the track creator                                      
  declareProperty( "SimHitCreatorID",           m_simHitCreatorID          );            
  // tool handles for extrapolator(s)                                       
  declareProperty( "Extrapolator",              m_extrapolationEngine      );
  // tool handles for the filtering setup                                   
  declareProperty( "TrackFilter",               m_trackFilter              );
  declareProperty( "NeutralFilter",             m_neutralHadronFilter      );
  declareProperty( "PhotonFilter",              m_photonFilter             );  
  // tools handles ISF Framework
  declareProperty( "ProcessSamplingTool",       m_samplingTool             );
  // service handles
  declareProperty( "RandomNumberService",       m_rndGenSvc                );
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::TransportEngine::~TransportEngine()
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::TransportEngine::initialize()
{
  
  ATH_MSG_INFO( "initialize()" );

  // Retrieve the tools one by one
  if (retrieveTool<iFatras::IParticleDecayHelper>(m_particleDecayHelper).isFailure())
    return StatusCode::FAILURE;      
  if (retrieveTool<iFatras::ISimHitCreator>(m_simHitCreatorID).isFailure())
    return StatusCode::FAILURE;
  if (retrieveTool<Trk::IExtrapolationEngine>(m_extrapolationEngine).isFailure()) 
    return StatusCode::FAILURE;  
  if (retrieveTool<ISF::IParticleFilter>(m_trackFilter).isFailure())
    return StatusCode::FAILURE;
  if (retrieveTool<ISF::IParticleFilter>(m_photonFilter).isFailure())
    return StatusCode::FAILURE;
  if (retrieveTool<ISF::IParticleFilter>(m_neutralHadronFilter).isFailure())
    return StatusCode::FAILURE;
  if (retrieveTool<iFatras::IProcessSamplingTool>(m_samplingTool).isFailure())
    return StatusCode::FAILURE;
  if (m_validationMode && retrieveTool<iFatras::IPhysicsValidationTool>(m_validationTool).isFailure())
      return StatusCode::FAILURE;

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
StatusCode iFatras::TransportEngine::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
ISF::ISFParticle* iFatras::TransportEngine::process( const ISF::ISFParticle& isp)
{
  // copy the current particle onto the particle clipboard
  ISF::ParticleClipboard::getInstance().setParticle(isp);
  
  ATH_MSG_DEBUG ("[ fatras transport ] processing particle " << isp.pdgCode() );

  // [0] ============================================================================================
  //  - INPUT PARTICLE PREPARATION
  //
  // particle Hypothesis for the extrapolation
  Trk::ParticleHypothesis pHypothesis = m_pdgToParticleHypothesis.convert(isp.pdgCode(),isp.charge());
  int absPdg   = abs(isp.pdgCode());
  bool charged = false;

  // geantinos not handled by PdgToParticleHypothesis - fix there
  if ( absPdg == 999 ) pHypothesis = Trk::geantino;

  // chose the filter & extrapolator
  const ISF::IParticleFilter* filter  = m_trackFilter.empty() ? 0 :  &(*m_trackFilter); 
  
  // switch the cases based on the particle PDG value
  if ( absPdg == 22 ) {
    ATH_MSG_VERBOSE( "[ fatras transport ] Particle is photon   -> neutral transport and eventual conversion");
    filter        =  m_photonFilter.empty() ? 0 :  &(*m_photonFilter);
  } else if  ( (absPdg == 12) || (absPdg == 14) || (absPdg == 16) ) {
    ATH_MSG_VERBOSE( "[ fatras transport ] Particle is neutrino -> particle going to be ignored.");
    return 0;
  } else if ( !((isp.charge()*isp.charge()) > 0.) ){
    ATH_MSG_VERBOSE( "[ fatras transport ] Particle not charged -> neutral transport and eventual (hadronic) interaction");
    filter        =  m_neutralHadronFilter.empty() ? 0 :  &(*m_neutralHadronFilter);
  } else {
    ATH_MSG_VERBOSE( "[ fatras transport ] Particle is a lepton or charged hadron -> charged transport and according material interactions.");
    // filter is already track filter at default
    charged   = true;
  }

  if (filter)
    ATH_MSG_VERBOSE( "[ fatras transport ] Determined processor and filter.");
  else 
    ATH_MSG_VERBOSE( "[ fatras transport ] Determined processor w/o filter.");
  
  // now transport the particle - if it passes the filter
  //if ( filter && !filter->passFilter(isp) ) {
  //  ATH_MSG_DEBUG( "[ fatras transport ] Filter not passed, ignore particle.");
  //  ATH_MSG_DEBUG( "   -> for more information, turn on debug/verbose output of  " << filter->name() );
  //  return 0;
  //}
  
  //ATH_MSG_DEBUG( "[ fatras transport ] The StackParticle passed filter - starting transport.");
  ATH_MSG_DEBUG( "[ fatras transport ] starting transport.");
  
  // [a] DECAY section ------------------------------------------------------------------------------
  //
  // now check stable versus non-stable particle 
  double pathLimit = ( !m_particleDecayHelper.empty()) ? m_particleDecayHelper->freePath(isp) : -1.; 
  ATH_MSG_VERBOSE( "[ fatras transport ] Particle free path : " << pathLimit);
  
  // prompt decay to be handled immediatedly
  if ( pathLimit > 0. && pathLimit < 0.01 ) {
    if (!m_particleDecayHelper.empty()) {
      ATH_MSG_VERBOSE( "[ fatras transport ] Decay is triggered for input particle.");
      m_particleDecayHelper->decay(isp,isp.position(),isp.momentum(),isp.timeStamp());
    }

    // validation mode - for all particle registered into stack
    if ( m_validationMode && m_validationTool ) {
      Trk::CurvilinearParameters inputPar(isp.position(),isp.momentum(),isp.charge());
      m_validationTool->saveISFParticleInfo(isp,201,&inputPar,isp.timeStamp(),0.); 
    }
    return 0;
  }
  
  // [b] MATERIAL section ------------------------------------------------------------------------------
  //
  //
  double materialLimitX0 = -1.;
  double materialLimitL0 = -1.;
  int materialProcess = -1;
  double materialX0=0.;
  double materialL0=0.;
  // now sample the path limit if not done already
  ISF::MaterialPathInfo* matLimit = isp.getUserInformation() ? isp.getUserInformation()->materialLimit() : 0;
  if (matLimit) {
    materialProcess = matLimit->process;
    if (materialProcess==121) materialLimitL0 = matLimit->dMax;
    else  materialLimitX0 = matLimit->dMax;
    if (materialProcess==121) materialL0=matLimit->dCollected;
    else materialX0 = matLimit->dCollected;
  } else if (absPdg!=999 && pHypothesis<99) { // need to resample   
    Trk::PathLimit matLim = m_samplingTool->sampleProcess(isp.momentum().mag(),isp.charge(),pHypothesis);  
    materialProcess = matLim.process;
    if (materialProcess==121) materialLimitL0 = matLim.x0Max;
    else  materialLimitX0 = matLim.x0Max;        
  }
  
  // [1] ============================================================================================  
  //  - TRANSPORT THROUGH THE DETECTOR
  //
  // the return particle (if any)
  ISF::ISFParticle* rParticle = 0;
  // 
  // [A] Neutral transport, includes
  //  - no hit creation 
  //  - propagation to decay point (if configured to do so by pathLimit > 0)
  //  - propagation to material limit (if configured to so by materialLimitX0 > 0 || materialLimitL0 > 0)
  //  - eventually stop at detector boundary
  if (!charged) {
    // netural parameter transport  
    Trk::NeutralCurvilinearParameters neutralInput(isp.position(),isp.momentum(),isp.charge());
    // neutral extrapolation cell & configure it
    Trk::ExtrapolationCell <Trk::NeutralParameters> ecc(neutralInput);
    configureExtrapolationCell< Trk::ExtrapolationCell <Trk::NeutralParameters> >(ecc,
                                                                                  pHypothesis,
                                                                                  pathLimit,
                                                                                  materialLimitX0,
                                                                                  materialLimitL0);
    ecc.materialProcess = materialProcess;
    if (ecc.materialLimitL0 != -1) {
      ecc.materialL0 = materialL0;
      ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitL0);   
    }
    if (ecc.materialLimitX0 != -1) {
      ecc.materialX0 = materialX0;
      ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitX0);
    }
      
    ecc.addConfigurationMode(Trk::ExtrapolationMode::FATRAS);   

    // do the transport using the new engine 
    Trk::ExtrapolationCode eCode =  m_extrapolationEngine->extrapolate(ecc);
 
    // validation info
    if (m_validationMode ) m_validationTool->saveISFParticleInfo(isp,ecc,eCode); 
	
    // resolve the different success states
    if (eCode.isSuccess()) {
      // this is a success - let's find out which one
      ATH_MSG_VERBOSE( "[ fatras transport ] Successfully run extrapolation for neutral particle.");
      // either give a particle back (at boundary) or the sub tools took care of it
      rParticle = handleExtrapolationResult(isp,
                                            eCode,
                                            ecc.endParameters->position(),
                                            ecc.endParameters->momentum(),
					    ecc.pathLength/CLHEP::c_light,
					    ecc.nextGeometrySignature);
      // memory cleanup  
      delete ecc.endParameters;
      // no hit creation -> return can be done now
      if (rParticle && m_validationMode) {
	// save validation info
	ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
	validInfo->setProcess(0);
	if (isp.getUserInformation()) validInfo->setGeneration(isp.getUserInformation()->generation());
	else validInfo->setGeneration(0);     // assume primary parent
	rParticle->setUserInformation(validInfo);
      }

      return rParticle;
    } else {
      ATH_MSG_WARNING( "Error code " << eCode.toString() << " running the Extrapolator for neutral particle!" );
      return 0;
    }
  } else {
    // [B] Charged transport 
    // 
    //  - hit creation 
    //  - propagation to decay point (if configured to do so by pathLimit > 0)
    //  - propagation to material limit (if configured to so by materialLimitX0 > 0 || materialLimitL0 > 0)
    //  - eventually stop at detector boudnary 
    std::vector<Trk::HitInfo>  hitVector;
    // parameter transport  
    Trk::CurvilinearParameters chargedInput(isp.position(),isp.momentum(),isp.charge());
    // charged extrapolation cell & configure it  
    Trk::ExtrapolationCell< Trk::TrackParameters > ecc(chargedInput);
    configureExtrapolationCell< Trk::ExtrapolationCell <Trk::TrackParameters> >(ecc,
                                                                                pHypothesis,
                                                                                pathLimit,
                                                                                materialLimitX0,
                                                                                materialLimitL0);
    ecc.addConfigurationMode(Trk::ExtrapolationMode::CollectSensitive);
    ecc.materialProcess = materialProcess;
    if (ecc.materialLimitL0 != -1) {
      ecc.materialL0 = materialL0;
      ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitL0);
    }
    if (ecc.materialLimitX0 != -1) {
      ecc.materialX0 = materialX0;
      ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitX0);
    }

    ecc.addConfigurationMode(Trk::ExtrapolationMode::FATRAS);   

    // do the transport using the new engine 
    // [B - 1] extrapoalte
    Trk::ExtrapolationCode eCode =  m_extrapolationEngine->extrapolate(ecc);	

    // validation info
    if (m_validationMode ) m_validationTool->saveISFParticleInfo(isp,ecc,eCode); 

    // resolve the different success states
    if (eCode.isSuccess()) {
        // this is a success - let's find out which one
        ATH_MSG_VERBOSE( "[ fatras transport ] Successfully run extrapolation for charged particle.");

	// either give a particle back (at boundary) or the sub tools took care of it
        rParticle = handleExtrapolationResult(isp,
                                              eCode,
                                              ecc.endParameters->position(),
                                              ecc.endParameters->momentum(),
					      ecc.pathLength/CLHEP::c_light,
					      ecc.nextGeometrySignature);
        // memory cleanup       
        delete ecc.endParameters;
    } else {
      // Extrapolation did NOT succeed
      ATH_MSG_WARNING( "Error code " << eCode.toString() << " running the Extrapolator for charged particle - may be incomplete track." );
    }
    // [B - 2] collect hit parameters -----------------------------------------------------------------------------
    // 
    // now loop over the collected information to create hits
    for (auto& es : ecc.extrapolationSteps) {
        // get the parameters
         const Trk::TrackParameters* parameters = es.parameters;
         // transfer into a detector ID (by layer type)
	 hitVector.push_back(Trk::HitInfo(parameters, (parameters->position()).mag()/CLHEP::c_light, 1, 0.));
    }
    // [B - 3] create hits ----------------------------------------------------------------------------------------
    //   
    // create hits 
    if (hitVector.size()>0){
       ATH_MSG_INFO( "[ fatras transport ] processing " << hitVector.size() << " hits from charged extrapolation.");
       if (!m_simHitCreatorID.empty()) m_simHitCreatorID->createHits(isp, hitVector);
       ATH_MSG_INFO( "[ fatras transport ] ID hits processed.");
       // memory cleanup
       for (auto& es : ecc.extrapolationSteps) {
           // get the parameters
            const Trk::TrackParameters* parameters = es.parameters;
            delete parameters;
        }
     }
  }
   
  if (rParticle && m_validationMode) {
    // save validation info
    ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
    validInfo->setProcess(0);
    if (isp.getUserInformation()) validInfo->setGeneration(isp.getUserInformation()->generation());
    else validInfo->setGeneration(0);     // assume primary parent
    rParticle->setUserInformation(validInfo);
  }

  // return what you have to the ISF 
  return rParticle;
}


/** handle the return of the extrapolation */
ISF::ISFParticle* iFatras::TransportEngine::handleExtrapolationResult(const ISF::ISFParticle& isp,
                                                                      Trk::ExtrapolationCode eCode,
                                                                      const Amg::Vector3D& position,
                                                                      const Amg::Vector3D& momentum,
								      double stime,
								      Trk::GeometrySignature nextGeoID)
{

    // the return particle
    ISF::ISFParticle* rParticle = 0;
    // switch the extrapolation code returns
    if (eCode == Trk::ExtrapolationCode::SuccessBoundaryReached){
        ATH_MSG_VERBOSE( "[ fatras transport ] Successfully reached detector boundary with the particle -> return particle at boundary.");
        // create the updated particle at the end of processing step
	AtlasDetDescr::AtlasRegion geoID=AtlasDetDescr::AtlasRegion(5);
	if (nextGeoID<99) geoID = AtlasDetDescr::AtlasRegion(nextGeoID);	  
        rParticle = new ISF::ISFParticle(position,
                                         momentum,
                                         isp.mass(),
                                         isp.charge(),
                                         isp.pdgCode(),
                                         stime+isp.timeStamp(),
                                         isp,
                                         isp.barcode());
	rParticle->setNextGeoID(geoID);
    } else if (eCode == Trk::ExtrapolationCode::SuccessPathLimit){
        ATH_MSG_VERBOSE( "[ fatras transport ] Successfully reached path limit for the particle -> decay & return 0.");
        // call the decayer tool
        m_particleDecayHelper->decay(isp,position,momentum,stime+isp.timeStamp());
    } else if (eCode == Trk::ExtrapolationCode::SuccessMaterialLimit )
        ATH_MSG_VERBOSE( "[ fatras transport ] Successfully reached material limit for the particle -> return 0.");

    return rParticle;
}
      
