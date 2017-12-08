/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/TransportTool.h"

// Tracking
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/ITimedExtrapolator.h" 
// iFatras includes
#include "ISF_FatrasInterfaces/ISimHitCreator.h"
#include "ISF_FatrasInterfaces/IParticleDecayHelper.h"
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"
#include "ISF_FatrasInterfaces/IProcessSamplingTool.h"
// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ParticleClipboard.h"
#include "ISF_Event/ParticleUserInformation.h"
#include "ISF_Interfaces/IParticleFilter.h"
#include "ISF_Interfaces/IParticleHelper.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandFlat.h"
//
// ROOT includes
#include "TTree.h"
 
// AtlasDetDescr
#include "AtlasDetDescr/AtlasRegion.h"

#include <iostream>

//#include "AtlasDetDescr/AtlasDetectorID.h"
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::TransportTool::TransportTool( const std::string& t,
                                      const std::string& n,
                                      const IInterface*  p )
 : AthAlgTool(t,n,p),
   m_rndGenSvc("AtDSFMTGenSvc", n), 
   m_randomEngine(0),
   m_randomEngineName("FatrasRnd"),
   m_validationOutput(false),
   m_validationTool(""),
   m_particleDecayHelper(""),
   m_simHitCreatorID(""),
   m_simHitCreatorMS(""),
   m_extrapolator(""),
   m_trackFilter(""),
   m_neutralHadronFilter(""),
   m_photonFilter(""),
   m_iparticleHelper(""),
   m_samplingTool(""),
   m_errorPropagation(false),
   m_hitsOff(false)
{
  declareInterface<ISF::IParticleProcessor>(this);
  
  // validation output section
  declareProperty( "ValidationOutput",  m_validationOutput );
  declareProperty( "PhysicsValidationTool",     m_validationTool  );
  // tool handle for the particle decayer
  declareProperty( "ParticleDecayHelper",       m_particleDecayHelper      );
  // tool handle for the track creator                                      
  declareProperty( "SimHitCreatorID",           m_simHitCreatorID          );            
  declareProperty( "SimHitCreatorMS",           m_simHitCreatorMS          );            
  // tool handles for extrapolator(s)                                       
  declareProperty( "Extrapolator",              m_extrapolator        );
  // tool handles for the filtering setup                                   
  declareProperty( "TrackFilter",               m_trackFilter              );
  declareProperty( "NeutralFilter",             m_neutralHadronFilter      );
  declareProperty( "PhotonFilter",              m_photonFilter             );  
  // tool handles ISF Framework                              
  declareProperty( "ParticleHelper",            m_iparticleHelper          );
  declareProperty( "TurnOffHitCreation",        m_hitsOff                  );        
  declareProperty( "ProcessSamplingTool",       m_samplingTool             );
  declareProperty("DoErrorPropagation",         m_errorPropagation         );
  // service handles
  declareProperty( "RandomNumberService",       m_rndGenSvc                );
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::TransportTool::~TransportTool()
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::TransportTool::initialize()
{
  
  ATH_MSG_INFO( "initialize()" );

  // Retrieve the tools one by one
  if (retrieveTool<iFatras::ISimHitCreator>(m_simHitCreatorID).isFailure())
      return StatusCode::FAILURE;
  if (retrieveTool<iFatras::ISimHitCreator>(m_simHitCreatorMS).isFailure())
      return StatusCode::FAILURE;
  if (retrieveTool<Trk::ITimedExtrapolator>(m_extrapolator).isFailure()) 
      return StatusCode::FAILURE;
  if (retrieveTool<iFatras::IParticleDecayHelper>(m_particleDecayHelper).isFailure())
      return StatusCode::FAILURE;      
  if (retrieveTool<ISF::IParticleFilter>(m_trackFilter).isFailure())
      return StatusCode::FAILURE;
  if (retrieveTool<ISF::IParticleFilter>(m_photonFilter).isFailure())
      return StatusCode::FAILURE;
  if (retrieveTool<ISF::IParticleFilter>(m_neutralHadronFilter).isFailure())
      return StatusCode::FAILURE;
  if (retrieveTool<ISF::IParticleHelper>(m_iparticleHelper).isFailure())
      return StatusCode::FAILURE;
  if (retrieveTool<iFatras::IProcessSamplingTool>(m_samplingTool).isFailure())
      return StatusCode::FAILURE;
  if (m_validationOutput && retrieveTool<iFatras::IPhysicsValidationTool>(m_validationTool).isFailure())
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
StatusCode iFatras::TransportTool::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
ISF::ISFParticle* iFatras::TransportTool::process( const ISF::ISFParticle& isp)
{
  // copy the current particle onto the particle clipboard
  ISF::ParticleClipboard::getInstance().setParticle( isp);

  // retrieve decay helper is not done already
  if (m_particleDecayHelper.empty()) {
    if (retrieveTool<iFatras::IParticleDecayHelper>(m_particleDecayHelper).isFailure()) ATH_MSG_WARNING( "decay helper not retrieved" );
  }          

  // if process/generation not set, assume primary
  //if (m_validationOutput && !isp.getUserInformation()) {
  //  ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
  //  validInfo->setProcess(0);
  //  validInfo->setGeneration(0);
  //  isp.setUserInformation(validInfo);
  //}

  ATH_MSG_VERBOSE ("[ fatras transport ] processing particle "<<isp.pdgCode() );

  std::vector<Trk::HitInfo>*     hitVector = 0;    // hit or no hit collection

  int  absPdg = abs(isp.pdgCode());
  bool photon         = (absPdg == 22);
  bool geantino       = (absPdg == 999);
  bool neutrino       = (absPdg == 12) || (absPdg == 14) || (absPdg == 16);
  bool charged        = photon || geantino ? false : (isp.charge()*isp.charge() > 0) ;

  // particle Hypothesis for the extrapolation

  Trk::ParticleHypothesis pHypothesis = m_pdgToParticleHypothesis.convert(isp.pdgCode(),isp.charge());

  // geantinos not handled by PdgToParticleHypothesis - fix there
  if ( absPdg == 999 ) pHypothesis = Trk::geantino;

  // chose the filter & extrapolator
  const Trk::ITimedExtrapolator* processor = &(*m_extrapolator); 
  const ISF::IParticleFilter* filter  = 0;

  if ( photon ) {
    ATH_MSG_VERBOSE( "[ fatras transport ] Photon - use transport tool for photons");
    filter        =  m_photonFilter.empty() ? 0 :  &(*m_photonFilter);
  } else if  ( neutrino ) {
      ATH_MSG_VERBOSE( "[ fatras transport ] Particle is neutrino -> particle ignored.");
      return 0;
  } else if ( !charged ){
    ATH_MSG_VERBOSE( "[ fatras transport ] Particle not charged - use transport tool for neutrals");
    filter        =  m_neutralHadronFilter.empty() ? 0 :  &(*m_neutralHadronFilter);
  } else {
    ATH_MSG_VERBOSE( "[ fatras transport ] Particle is a lepton or charged hadron");
    filter    =  m_trackFilter.empty() ? 0 :  &(*m_trackFilter); 
  }

  if (filter)
      ATH_MSG_DEBUG( "[ fatras transport ] Determined processor and filter.");
  else 
      ATH_MSG_DEBUG( "[ fatras transport ] Determined processor w/o filter.");
  
  // now transport the particle - if it passes the filter
  if ( !processor || ( filter && !filter->passFilter(isp)) ) {
    ATH_MSG_VERBOSE( "[ fatras transport ] Filter not passed, ignore particle.");
    return 0;
  }

  ATH_MSG_VERBOSE( "[ fatras transport ] The StackParticle passed filter - starting transport.");
  // input parameters : curvilinear parameters
  Trk::CurvilinearParameters inputPar(isp.position(),isp.momentum(),isp.charge());

  // stable vs. unstable check
  double freepath = ( !m_particleDecayHelper.empty()) ? m_particleDecayHelper->freePath(isp) : - 1.; 
  ATH_MSG_VERBOSE( "[ fatras transport ] Particle free path : " << freepath);
  // path limit -> time limit  ( TODO : extract life-time directly from decay helper )
  double tDec = freepath > 0. ? freepath : -1.;
  int decayProc = 0;

  // beta calculated here for further use in validation
  double mass = m_particleMasses.mass[pHypothesis];
  double mom = isp.momentum().mag();
  double beta = mom/sqrt(mom*mom+mass*mass); 

  if ( tDec>0.) {
    tDec = tDec/beta/CLHEP::c_light + isp.timeStamp();
    decayProc = 201;                
  }

  Trk::TimeLimit timeLim(tDec,isp.timeStamp(),decayProc);
  
  // prompt decay
  if ( freepath>0. && freepath<0.01 ) {
    if (!m_particleDecayHelper.empty()) {
      ATH_MSG_VERBOSE( "[ fatras transport ] Decay is triggered for input particle.");
      m_particleDecayHelper->decay(isp,isp.position(),isp.momentum(),isp.timeStamp());
    }

    // validation mode - for all particle registered into stack
    if ( m_validationOutput ) {
      int endProcess = decayProc;
      m_validationTool->saveISFParticleInfo(isp,endProcess,&inputPar,timeLim.time,0.); 
    }

    return 0;
  }

  // presample interactions if not done already
  Trk::PathLimit pathLim(-1.,0);
  ISF::MaterialPathInfo* matLimit = isp.getUserInformation() ? isp.getUserInformation()->materialLimit() : 0;
  if (matLimit) {
    pathLim=Trk::PathLimit( matLimit->dMax,matLimit->process);
    pathLim.updateMat(matLimit->dCollected,13.,0.);          // arbitrary Z choice : update MaterialPathInfo  
  } else if (absPdg!=999 && pHypothesis<99) { // need to resample   
    pathLim = m_samplingTool->sampleProcess(isp.momentum().mag(),isp.charge(),pHypothesis);  
  }
     
  // use extrapolation with path limit - automatic exit at subdetector boundary
  // NB: don't delete eParameters, it's memory is managed inside the extrapolator

  // additional exercise due to the current mismatch in geoID
  Trk::GeometrySignature nextGeoID=Trk::GeometrySignature(isp.nextGeoID()); 
    
  const Trk::TrackParameters* eParameters = 0;

  // hit creation/energy deposit
  hitVector = (!m_hitsOff) ? new std::vector<Trk::HitInfo> : 0;
  if (hitVector) ATH_MSG_VERBOSE( "[ fatras transport ] hit creation activated");

  if ( !charged ) {

    eParameters = processor->transportNeutralsWithPathLimit(inputPar,pathLim,timeLim,Trk::alongMomentum,pHypothesis,hitVector,nextGeoID);  
 
  } else {
    
    if (m_validationOutput && m_errorPropagation ) {
      // input covariance matrix
      AmgSymMatrix(5)* inputCov = new AmgSymMatrix(5);
      inputCov->setZero();
      const Trk::TrackParameters* measuredInputPar = inputPar.associatedSurface().createTrackParameters(inputPar.parameters()[0],
                                                                                                        inputPar.parameters()[1],
                                                                                                        inputPar.parameters()[2],
                                                                                                        inputPar.parameters()[3],
                                                                                                        inputPar.parameters()[4],
                                                                                                        inputCov);
      eParameters = processor->extrapolateWithPathLimit(*measuredInputPar,
							pathLim,
							timeLim,
							Trk::alongMomentum,
							pHypothesis,
							hitVector,
							nextGeoID);
      delete measuredInputPar;  

    } else {
      
      eParameters = processor->extrapolateWithPathLimit(inputPar,
							pathLim,
							timeLim,
							Trk::alongMomentum,
							pHypothesis,
							hitVector,
							nextGeoID);
    }
  }

  // create hits 
  if (hitVector && hitVector->size()){
    ATH_MSG_VERBOSE( "[ fatras transport ] processing .");
    // create the hits from the parameters vector
    if ( isp.nextGeoID()==AtlasDetDescr::fAtlasID ) {
      if (!m_simHitCreatorID.empty()) m_simHitCreatorID->createHits(isp, *hitVector);
      ATH_MSG_VERBOSE( "[ fatras transport ] ID hits processed.");
    } else if ( isp.nextGeoID()==AtlasDetDescr::fUndefinedAtlasRegion ) {
      ATH_MSG_WARNING( "[ fatras transport ] hit vector without Geo info");
    } else if ( isp.nextGeoID()==AtlasDetDescr::fAtlasMS ) {
      if (!m_simHitCreatorMS.empty()) m_simHitCreatorMS->createHits(isp, *hitVector);
      ATH_MSG_VERBOSE( "[ fatras transport ] MS hits processed.");
    }
    // memory cleanup
    std::vector<Trk::HitInfo>::iterator tParIter    = hitVector->begin();
    std::vector<Trk::HitInfo>::iterator tParIterEnd = hitVector->end();
    for ( ; tParIter != tParIterEnd; delete ((*tParIter).trackParms), ++tParIter);
    delete hitVector; hitVector = 0;
  }      

  // additional exercise due to the current mismatch in geoID
  AtlasDetDescr::AtlasRegion geoID=AtlasDetDescr::AtlasRegion(5);
  if (nextGeoID<99) {
    geoID = AtlasDetDescr::AtlasRegion(nextGeoID);
  }

  // validation mode - for all particle registered into stack
  if ( m_validationOutput ) {

    int dProc = ( timeLim.tMax>0. && timeLim.tMax<=timeLim.time ) ? timeLim.process : 0;
    int mProc = ( pathLim.x0Max>0. && ( pathLim.x0Max <= pathLim.x0Collected ||
                 (pathLim.process>100 && pathLim.x0Max <= pathLim.l0Collected))) ? pathLim.process : 0;   
    int endProcess = eParameters ? 0 : ( dProc > mProc ? dProc : mProc );
 
    m_validationTool->saveISFParticleInfo(isp,endProcess,eParameters,timeLim.time,pathLim.x0Collected); 

  }
    
  // create the updated particle at the end of processing step
  ISF::ISFParticle* uisp = eParameters ? m_iparticleHelper->updatedParticle(isp,
									    eParameters->position(),
									    eParameters->momentum(),
									    timeLim.time-isp.timeStamp()) : 0;     // update expects time difference
  if (uisp && m_validationOutput) {
    // save validation info
    ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
    validInfo->setProcess(0);
    if (isp.getUserInformation()) validInfo->setGeneration(isp.getUserInformation()->generation());
    else validInfo->setGeneration(0);     // assume primary parent
    uisp->setUserInformation(validInfo);
  }

  // TODO save material ? if (uisp && m_validationOutput) uisp->setX0(pathLim.x0Collected,m_eloss);
      
  // trigger decay or return to stack
  
  if (uisp && timeLim.tMax>0.  && timeLim.time >=timeLim.tMax ) {
    if (!m_particleDecayHelper.empty()) {
      ATH_MSG_VERBOSE( "[ fatras transport ] Decay is triggered for input particle.");
      m_particleDecayHelper->decay(*uisp,uisp->position(),uisp->momentum(),uisp->timeStamp());
    }
    delete uisp; 
    return 0;
  } else if (uisp) {
    ATH_MSG_VERBOSE( "[ fatras transport ] Particle transported to detector boundary, return to stack, timing:" << uisp->timeStamp());
    uisp->setNextGeoID(geoID);
    return uisp;
  }
        
  // nothing to be returned to the stack (and we should not get here at all)
  return 0;
}

