/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Athena
#include "xAODEgamma/EgammaxAODHelpers.h"

// Local
#include "PrimaryVertexReFitter.h"

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

//======================================================================================================
Prompt::PrimaryVertexReFitter::PrimaryVertexReFitter(const std::string& name, ISvcLocator *pSvcLocator): 
  AthAlgorithm     (name, pSvcLocator),
  m_vertexFitterSvc("Prompt::VertexFittingSvc/PromptVertexFittingSvc", name)
{
  declareProperty("PrintTime",                         m_printTime     = false);
  declareProperty("Debug",                             m_debug         = false);

  declareProperty("LeptonContainerName",               m_leptonContainerName);
  declareProperty("PriVtxName",                        m_primaryVertexName);
  declareProperty("ReFitPriVtxName",                   m_reFitPrimaryVertexName);

  declareProperty("DistToRefittedPriVtxName",          m_distToRefittedPriVtxName);
  declareProperty("NormDistToRefittedPriVtxName",      m_normDistToRefittedPriVtxName);
  declareProperty("RefittedVtxLinkName",               m_lepVtxLinkName);
  declareProperty("RefittedVtxWithoutLeptonLinkName",  m_lepRefittedVtxWithoutLeptonLinkName);

  declareProperty("VertexFittingSvc",                  m_vertexFitterSvc);
}

//=============================================================================
StatusCode Prompt::PrimaryVertexReFitter::initialize()
{   
  if(m_printTime) {
    //
    // Reset timers
    //
    m_timerAll .Reset();
    m_timerExec.Reset();

    //
    // Start full timer
    //
    m_timerAll.Start();
  }

  m_distToRefittedPriVtx      = std::make_unique<decoratorFloat_t>  (m_distToRefittedPriVtxName);
  m_normdistToRefittedPriVtx  = std::make_unique<decoratorFloat_t>  (m_normDistToRefittedPriVtxName);
  m_lepRefittedRMVtxLinkDec   = std::make_unique<decoratorElemVtx_t>(m_lepRefittedVtxWithoutLeptonLinkName);

  ATH_CHECK(m_vertexFitterSvc.retrieve());

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode Prompt::PrimaryVertexReFitter::finalize()
{
  if(m_printTime) {
    //
    // Print full time stopwatch
    //
    m_timerAll.Stop();

    ATH_MSG_INFO("Real time: " << m_timerAll.RealTime() << "\t CPU time: " << m_timerAll.CpuTime());
    ATH_MSG_INFO("Execute time: " << PrintResetStopWatch(m_timerExec));
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode Prompt::PrimaryVertexReFitter::execute()
{  

  //
  // Start execute timer
  //
  TimerScopeHelper timer(m_timerExec);

  //  
  // Find Inner Detector tracks
  //  
  const xAOD::TrackParticleContainer *inDetTracks = 0;

  if(evtStore()->retrieve(inDetTracks, "InDetTrackParticles").isFailure() || !inDetTracks) {
    ATH_MSG_FATAL("execute - failed to find the InDetTrackParticles");
    return StatusCode::FAILURE;
  }

  //
  // Retrieve containers from evtStore
  //
  const xAOD::IParticleContainer *leptonContainer = 0;
  const xAOD::VertexContainer    *vertices        = 0;
	
  ATH_CHECK(evtStore()->retrieve(leptonContainer, m_leptonContainerName)); 
  ATH_CHECK(evtStore()->retrieve(vertices,        m_primaryVertexName));

  Prompt::FittingInput fittingInput(inDetTracks, 0, 0);

  for(const xAOD::Vertex *vertex: *vertices) {
    if(vertex->vertexType() == 1) {
      fittingInput.priVtx = dynamic_cast<const xAOD::Vertex*>(vertex);
      break;
    }
  }

  if(!fittingInput.priVtx) {
    ATH_MSG_WARNING("Failed to find primary vertices");
    return StatusCode::SUCCESS;    
  }

  //
  // Collect tracks used for primary vertex fit
  //
  std::vector<const xAOD::TrackParticle *> priVtx_tracks;

  for(unsigned k = 0; k < fittingInput.priVtx->nTrackParticles(); ++k) {
    const xAOD::TrackParticle *track  = fittingInput.priVtx->trackParticle(k);

    if(track) {
      priVtx_tracks.push_back(track);
    }   
  }

  //
  // Create/get container for refitted primary vertex with/without lepton
  //
  if(m_reFitPrimaryVertexName.empty()) {
    ATH_MSG_FATAL("execute - SecVtx container invalid name: \"" << m_reFitPrimaryVertexName << "\"");    
    return StatusCode::FAILURE;
  }

  std::unique_ptr<xAOD::VertexContainer>    refitVtxContainer    = std::make_unique<xAOD::VertexContainer>();
  std::unique_ptr<xAOD::VertexAuxContainer> refitVtxContainerAux = std::make_unique<xAOD::VertexAuxContainer>();

  refitVtxContainer->setStore(refitVtxContainerAux.get());

  //
  // Refit primary vertex
  //
  xAOD::Vertex *refittedPriVtx = m_vertexFitterSvc->fitVertexWithSeed(fittingInput, priVtx_tracks, fittingInput.priVtx->position(), Prompt::kRefittedPriVtx).first;

  if(!refittedPriVtx) {
    ATH_MSG_WARNING("Failed to refit primary vertex");
    return StatusCode::SUCCESS;    
  }

  if(!m_vertexFitterSvc->moveVertexPointerToContainer(refittedPriVtx, *refitVtxContainer)) {
    ATH_MSG_WARNING("execute - failed to release xAOD::Vertex from fitting service - expect troubles");
    return StatusCode::SUCCESS;
  }

  //
  // Save refitted primary vertex for fitting service
  //
  fittingInput.refittedPriVtx = refittedPriVtx;

  ATH_MSG_DEBUG("execute --          primary vertex NTrack = " << fittingInput.priVtx         ->nTrackParticles());
  ATH_MSG_DEBUG("execute -- refitted primary vertex NTrack = " << fittingInput.refittedPriVtx->nTrackParticles());

  //
  // Dynamic cast IParticle container to electron or muon container
  //
  ATH_MSG_DEBUG("=======================================" 
		<< "\n\t\t\t  Size of lepton container:    " << leptonContainer  ->size() 
		<< "\n-----------------------------------------------------------------");

  for(const xAOD::IParticle *lepton: *leptonContainer) {
    const xAOD::TrackParticle *tracklep = 0;
    const xAOD::Electron      *elec     = dynamic_cast<const xAOD::Electron*>(lepton);
    const xAOD::Muon          *muon     = dynamic_cast<const xAOD::Muon    *>(lepton);
    
    if(elec) { 
      //
      // get GSF track
      //
      const xAOD::TrackParticle *bestmatchedGSFElTrack=elec->trackParticle(0);

      //
      // get origin ID track for later study
      //
      tracklep = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(bestmatchedGSFElTrack);
    }
    else if(muon) { 
      if(muon->inDetTrackParticleLink().isValid()) {
	tracklep = *(muon->inDetTrackParticleLink());
      }
      else {
	ATH_MSG_DEBUG("PrimaryVertexReFitter::execute - skip muon without valid inDetTrackParticleLink()");
	continue;
      }
    }

    if(!tracklep) {
      ATH_MSG_WARNING("PrimaryVertexReFitter::execute - cannot find muon->inDetTrackParticleLink() nor electron->trackParticle()");
      continue;
    }

    decorateLepWithReFitPrimaryVertex(fittingInput, tracklep, lepton, priVtx_tracks, *refitVtxContainer);
  }


  CHECK(evtStore()->record(std::move(refitVtxContainerAux), m_reFitPrimaryVertexName+"Aux."));
  CHECK(evtStore()->record(std::move(refitVtxContainer),    m_reFitPrimaryVertexName));
  
  ATH_MSG_DEBUG("SV Vertex container " << m_reFitPrimaryVertexName << " recorded in store");

  //
  // Reset fitting service - this deletes all vertexes that were not saved in SG
  //
  m_vertexFitterSvc->resetEvent();

  ATH_MSG_DEBUG("execute - all done");
  ATH_MSG_DEBUG("=======================================");

  return StatusCode::SUCCESS;
}

//=============================================================================
bool Prompt::PrimaryVertexReFitter::decorateLepWithReFitPrimaryVertex(const FittingInput &input,
								      const xAOD::TrackParticle* tracklep,
                                                                      const xAOD::IParticle *lep,
                                                                      const std::vector<const xAOD::TrackParticle*> &tracks,
                                                                      xAOD::VertexContainer &refitVtxContainer)
{
  //
  // Check if the lepton track has been used for primary vertex reconstruction.
  // if true, then remove the lepton track from the input track list, re-fit primary vertex again.
  // Save the ElementLink of the re-fit primary vertex to the lepton
  //  
  if(!input.priVtx) {
    ATH_MSG_WARNING("decorateLepWithReFitPrimaryVertex - invalid input primary vertex pointer");
    return false;
  }

  //--------------------------------------------------------
  // Remove the lepton track from the track list
  // get re-fitted non-prompt primary vertex
  //
  std::vector<const xAOD::TrackParticle*> priVtx_tracks_pass;
  bool isRefit = false;

  for(const xAOD::TrackParticle *track: tracks) {
    if(track == tracklep) {
      isRefit = true;
      ATH_MSG_DEBUG("decorateLepWithReFitPrimaryVertex -- lepton has been used, lepton pT =" << tracklep->pt() << ", track pT =" << track->pt());
      continue;
    }

    priVtx_tracks_pass.push_back(track);
  }

  ElementLink<xAOD::VertexContainer> refittedRM_pv_link;
  
  if(!isRefit) {
    ATH_MSG_DEBUG("decorateLepWithReFitPrimaryVertex -- Skip the primary vertex without lepton track");

    (*m_lepRefittedRMVtxLinkDec)(*lep) = refittedRM_pv_link;
    return false;
  }

  if(priVtx_tracks_pass.size() < 2) {
    ATH_MSG_DEBUG("decorateLepWithReFitPrimaryVertex -- Skip the primary vertex refitting: N tracks =" << priVtx_tracks_pass.size());

    (*m_lepRefittedRMVtxLinkDec)(*lep) = refittedRM_pv_link;
    return false;
  }

  std::pair<xAOD::Vertex *, int> refittedVtxRMLep = m_vertexFitterSvc->fitVertexWithSeed(input, priVtx_tracks_pass, input.priVtx->position(), Prompt::kRefittedPriVtxWithoutLep);

  if(refittedVtxRMLep.first) {
    //
    // Record vertex with output container
    //
    if(!m_vertexFitterSvc->moveVertexPointerToContainer(refittedVtxRMLep.first, refitVtxContainer)) {
      ATH_MSG_WARNING("decorateLepWithReFitPrimaryVertex - failed to release xAOD::Vertex from fitting service - expect troubles");
    }

    //
    // Add refitted non-prompt vertex ElementLink to the lepton
    //
    refittedRM_pv_link.toContainedElement(refitVtxContainer, refittedVtxRMLep.first);

    ATH_MSG_DEBUG("decorateLepWithReFitPrimaryVertex -- save refitted non-prompt primary vertex with NTrack = " << refittedVtxRMLep.first->nTrackParticles());
    
    if(input.refittedPriVtx) {
      (*m_distToRefittedPriVtx)    (*refittedVtxRMLep.first) = Prompt::GetDistance(input.refittedPriVtx->position(), refittedVtxRMLep.first->position()); 
      (*m_normdistToRefittedPriVtx)(*refittedVtxRMLep.first) = Prompt::GetNormDist(input.refittedPriVtx->position(), refittedVtxRMLep.first->position(), 
										   refittedVtxRMLep.first->covariance(), msg(MSG::WARNING)); 
    }
  }

  (*m_lepRefittedRMVtxLinkDec)(*lep) = refittedRM_pv_link;

  return true;
}
