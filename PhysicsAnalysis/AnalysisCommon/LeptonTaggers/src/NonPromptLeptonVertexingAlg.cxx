/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// ROOT
#include "TMath.h"

// Athena
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

// Local
#include "NonPromptLeptonVertexingAlg.h"
#include "PromptUtils.h"

using namespace std;

//======================================================================================================
Prompt::NonPromptLeptonVertexingAlg::NonPromptLeptonVertexingAlg(const std::string& name, ISvcLocator *pSvcLocator): 
  AthAlgorithm     (name, pSvcLocator),
  m_vertexMerger   ("Prompt::VertexMergingTool/PromptVertexMergingTool", this),
  m_vertexFitterSvc("Prompt::VertexFittingSvc/PromptVertexFittingSvc", name),
  m_countEvents    (0)
{
  declareProperty("Debug",                             m_debug         = false);
  declareProperty("DebugMerge",                        m_debug_merge   = false);
  declareProperty("PrintTime",                         m_printTime     = false);
  declareProperty("SelectTracks",                      m_selectTracks  = false);

  declareProperty("MergeMinVtxDist",                   m_mergeMinVtxDist  = 1.0);
  declareProperty("MergeChi2OverDoF",                  m_mergeChi2OverDoF = 5.0);

  declareProperty("minTrackpT",                        m_minTrackpT            = 500.0);
  declareProperty("maxTrackEta",                       m_maxTrackEta           =   2.5);
  declareProperty("maxTrackZ0Sin",                     m_maxTrackZ0Sin         =   1.0);

  declareProperty("minTrackLeptonDR",                  m_minTrackLeptonDR      =   1.0e-6);
  declareProperty("maxTrackLeptonDR",                  m_maxTrackLeptonDR      =   0.4);

  declareProperty("minTrackSilHits",                   m_minTrackSilHits       = 7);
  declareProperty("maxTrackSharedSilHits",             m_maxTrackSharedSilHits = 1.0);
  declareProperty("maxTrackSilHoles",                  m_maxTrackSilHoles      = 2);
  declareProperty("maxTrackPixHoles",                  m_maxTrackPixHoles      = 1);

  declareProperty("LeptonContainerName",               m_leptonContainerName);
  declareProperty("ReFitPriVtxContainerName",          m_refittedPriVtxContainerName);
  declareProperty("SVContainerName",                   m_svContainerName);

  declareProperty("SecVtxLinksName",                   m_decoratorNameSecVtxLinks);
  declareProperty("NoLeptonPriVtxLinkName",            m_linkNameRefittedPriVtxWithoutLepton);
  
  declareProperty("ReFitPriVtxTypeName",               m_refittedVertexTypeName = "refittedVertexType");
  declareProperty("IndexVectorName",                   m_decoratorNameIndexVector);

  declareProperty("VertexMergingTool",                 m_vertexMerger);
  declareProperty("VertexFittingSvc",                  m_vertexFitterSvc);
}

//=============================================================================
StatusCode Prompt::NonPromptLeptonVertexingAlg::initialize()
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

  ATH_CHECK(m_vertexMerger   .retrieve());
  ATH_CHECK(m_vertexFitterSvc.retrieve());

  m_indexVectorDec           = std::make_unique<decoratorVecInt_t>    (m_decoratorNameIndexVector);
  m_indexVectorDecMerge      = std::make_unique<decoratorVecInt_t>    (m_decoratorNameIndexVector+"Merge");
  m_indexVectorDecDeepMerge  = std::make_unique<decoratorVecInt_t>    (m_decoratorNameIndexVector+"DeepMerge");
  m_lepSVElementLinksDec     = std::make_unique<decoratorVecElemVtx_t>(m_decoratorNameSecVtxLinks);
   
  ATH_MSG_INFO("LeptonContainerName      = " << m_leptonContainerName);
  ATH_MSG_INFO("ReFitPriVtxContainerName = " << m_refittedPriVtxContainerName);
  ATH_MSG_INFO("SVContainerName          = " << m_svContainerName);
  ATH_MSG_INFO("IndexVectorName          = " << m_decoratorNameIndexVector);

  ATH_MSG_INFO("mergeMinVtxDist       = " << m_mergeMinVtxDist);
  ATH_MSG_INFO("mergeChi2OverDoF      = " << m_mergeChi2OverDoF);

  ATH_MSG_INFO("minTrackLeptonDR      = " << m_minTrackLeptonDR);
  ATH_MSG_INFO("maxTrackLeptonDR      = " << m_maxTrackLeptonDR);

  ATH_MSG_INFO("selectTracks          = " << m_selectTracks);
  ATH_MSG_INFO("minTrackpT            = " << m_minTrackpT);
  ATH_MSG_INFO("maxTrackEta           = " << m_maxTrackEta);
  ATH_MSG_INFO("maxTrackZ0Sin         = " << m_maxTrackZ0Sin);

  ATH_MSG_INFO("minTrackSilHits       = " << m_minTrackSilHits);
  ATH_MSG_INFO("maxTrackSharedSilHits = " << m_maxTrackSharedSilHits);
  ATH_MSG_INFO("maxTrackSilHoles      = " << m_maxTrackSilHoles);
  ATH_MSG_INFO("maxTrackPixHoles      = " << m_maxTrackPixHoles);

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode Prompt::NonPromptLeptonVertexingAlg::finalize()
{
  if(m_printTime) {
    //
    // Print full time stopwatch
    //
    m_timerAll.Stop();

    ATH_MSG_INFO("NonPromptLeptonVertexingAlg - total time:   " << PrintResetStopWatch(m_timerAll));
    ATH_MSG_INFO("NonPromptLeptonVertexingAlg - execute time: " << PrintResetStopWatch(m_timerExec));
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode Prompt::NonPromptLeptonVertexingAlg::execute()
{  
  //
  // Start execute timer for new event
  //
  TimerScopeHelper timer(m_timerExec);

  m_countEvents++;

  //
  // Find Inner Detector tracks save them class member variable for convenience.
  //
  const xAOD::TrackParticleContainer *inDetTracks = 0;

  if(evtStore()->retrieve(inDetTracks, "InDetTrackParticles").isFailure() || !inDetTracks) {
    ATH_MSG_ERROR("NonPromptLeptonVertexingAlg::execute() -- failed to get ID track particles");
    return StatusCode::FAILURE;
  }

  //
  // Create secondary vertex container
  //
  if(m_svContainerName.empty() || evtStore()->contains<xAOD::VertexContainer>(m_svContainerName)) {
    ATH_MSG_ERROR("NonPromptLeptonVertexingAlg::execute - SV container already exists or invalid name: \"" << m_svContainerName << "\"");
    return StatusCode::FAILURE;    
  }

  //
  // Helper object for organising vertex objects
  //
  std::set< xAOD::Vertex* > svSet;

  unique_ptr<xAOD::VertexContainer>    SVContainer    = std::make_unique<xAOD::VertexContainer>();
  unique_ptr<xAOD::VertexAuxContainer> SVAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
   
  SVContainer->setStore(SVAuxContainer.get());
  
  //
  // Retrieve containers from evtStore
  //
  const xAOD::IParticleContainer *leptonContainer   = 0;
  const xAOD::VertexContainer    *vertices          = 0;
  const xAOD::VertexContainer    *refittedVertices  = 0;
	
  ATH_CHECK(evtStore()->retrieve(leptonContainer,   m_leptonContainerName)); 
  ATH_CHECK(evtStore()->retrieve(vertices,          "PrimaryVertices"));
  ATH_CHECK(evtStore()->retrieve(refittedVertices,  m_refittedPriVtxContainerName));

  ATH_MSG_DEBUG ("NonPromptLeptonVertexingAlg::execute - Read " << vertices->size()          << " primary vertices");
  ATH_MSG_DEBUG ("NonPromptLeptonVertexingAlg::execute - Read " << refittedVertices->size()  << " refitted primary vertices");

  //
  // Find default Primary Vertex
  //
  Prompt::FittingInput fittingInput(inDetTracks, 0, 0);

  for(const xAOD::Vertex *vertex: *vertices) {
    if(vertex->vertexType() == 1) {
      fittingInput.priVtx = vertex;
      break;
    }
  }

  if(!fittingInput.priVtx) {
    ATH_MSG_INFO("Failed to find primary vertex - skip this event");
    return StatusCode::SUCCESS;    
  }

  //
  // Find the refitted Primary Vertex
  //
  for(const xAOD::Vertex *vertex: *refittedVertices) {
    short refittedVertexType = 0; 

    if(GetVar(vertex, refittedVertexType, m_refittedVertexTypeName) && refittedVertexType == 1) {
      fittingInput.refittedPriVtx = vertex;      
    }

    if(fittingInput.refittedPriVtx) {
      break;
    }
  }

  //
  // Dynamic cast IParticle container to electron or muon container
  //
  ATH_MSG_DEBUG("\n\t\t\t  Size of lepton container:  " << leptonContainer ->size());

  SG::AuxElement::ConstAccessor<ElementLink<xAOD::VertexContainer> > priVtxWithoutLepAcc(m_linkNameRefittedPriVtxWithoutLepton);

  for(const xAOD::IParticle *lepton: *leptonContainer) {
    const xAOD::TrackParticle *tracklep = 0;
    const xAOD::Electron      *elec     = dynamic_cast<const xAOD::Electron*>(lepton);
    const xAOD::Muon          *muon     = dynamic_cast<const xAOD::Muon    *>(lepton);

    if(elec) { 
      //
      // Get GSF track
      //
      const xAOD::TrackParticle *bestmatchedGSFElTrack = elec->trackParticle(0);

      //
      // Get original ID track for vertex fitting
      //
      if(passElecCand(*elec) && bestmatchedGSFElTrack) {
	tracklep = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(bestmatchedGSFElTrack);
      }
    }
    else if(muon) { 
      if(passMuonCand(*muon) && muon->inDetTrackParticleLink().isValid()) {
	tracklep = *(muon->inDetTrackParticleLink());
      }
    }
    else {
      ATH_MSG_WARNING("NonPromptLeptonVertexingAlg::execute - failed to find electron or muon: should never happen!");      
    }

    if(!tracklep) {
      (*m_lepSVElementLinksDec)    (*lepton) = std::vector<ElementLink<xAOD::VertexContainer> >();
      (*m_indexVectorDec)          (*lepton) = std::vector<int>();
      (*m_indexVectorDecMerge)     (*lepton) = std::vector<int>();
      (*m_indexVectorDecDeepMerge) (*lepton) = std::vector<int>();

      ATH_MSG_DEBUG("NonPromptLeptonVertexingAlg::execute - cannot find muon->inDetTrackParticleLink() nor electron->trackParticle()");
      continue;
    }

    ATH_MSG_DEBUG("NonPromptLeptonVertexingAlg::execute - process new lepton track " << tracklep);

    //
    // Find refitted primary vertex with lepton track excluded
    //
    fittingInput.refittedPriVtxWithoutLep = 0;
  
    if(priVtxWithoutLepAcc.isAvailable(*lepton)) {
      ElementLink<xAOD::VertexContainer> vtxLink = priVtxWithoutLepAcc(*lepton);

      if(vtxLink.isValid()) {
	fittingInput.refittedPriVtxWithoutLep = *vtxLink;
	
	ATH_MSG_DEBUG("DecorateSecondaryVertex - found refitted primary vertex without lepton: " 
		      << m_linkNameRefittedPriVtxWithoutLepton << " with Ntrack =" << fittingInput.refittedPriVtxWithoutLep->nTrackParticles());
      }
    }

    //
    // Collect tracks around the lepton track
    //
    std::vector<const xAOD::TrackParticle* > ifit_tracks = findNearbyTracks(*tracklep, *inDetTracks, *fittingInput.priVtx);

    //
    // Fit 2-track vertices
    //
    std::vector<xAOD::Vertex *> twoTrk_vertices = prepLepWithTwoTrkSVVec(fittingInput, tracklep, ifit_tracks);

    //
    // Merge 2-track vertices that are close to each other
    //
    std::vector<xAOD::Vertex *> merged_vertices = prepLepWithMergedSVVec(fittingInput, tracklep, twoTrk_vertices);
    
    //
    // Deep merge 2-track vertices.
    //
    Prompt::MergeResult deep_merged_result = m_vertexMerger->mergeInitVertices(fittingInput, tracklep, twoTrk_vertices, ifit_tracks);
    
    ATH_MSG_DEBUG ("NonPromptLeptonVertexingAlg::execute -- process line: " << __LINE__);

    //
    // Save secondary vertices
    //
    std::vector<ElementLink<xAOD::VertexContainer> > sv_links;

    std::vector<int> index_vector_twoTrk;  
    std::vector<int> index_vector_merged;
    std::vector<int> index_vector_deep_merged;

    ATH_MSG_DEBUG ("NonPromptLeptonVertexingAlg::execute -- process line: " << __LINE__);
    
    //
    // Record 2-track vertexes and simple merged vertexes
    //
    saveSecondaryVertices(twoTrk_vertices, index_vector_twoTrk, sv_links, *SVContainer, svSet);
    saveSecondaryVertices(merged_vertices, index_vector_merged, sv_links, *SVContainer, svSet);

    //
    // Record both merged multi-track vertices and also unmerged 2-track vertices
    //
    saveSecondaryVertices(deep_merged_result.vtxs_new_merged,             index_vector_deep_merged, sv_links, *SVContainer, svSet);
    saveSecondaryVertices(deep_merged_result.vtxs_init_passed_not_merged, index_vector_deep_merged, sv_links, *SVContainer, svSet);

    ATH_MSG_DEBUG ("NonPromptLeptonVertexingAlg::execute -- number of two-track   SV = " << twoTrk_vertices.size());
    ATH_MSG_DEBUG ("NonPromptLeptonVertexingAlg::execute -- number of merged      SV = " << merged_vertices.size());
    ATH_MSG_DEBUG ("NonPromptLeptonVertexingAlg::execute -- number of deep merged SV = " << deep_merged_result.vtxs_new_merged.size());

    (*m_lepSVElementLinksDec)    (*lepton) = sv_links;
    (*m_indexVectorDec)          (*lepton) = index_vector_twoTrk;
    (*m_indexVectorDecMerge)     (*lepton) = index_vector_merged;
    (*m_indexVectorDecDeepMerge) (*lepton) = index_vector_deep_merged;

    ATH_MSG_DEBUG("NonPromptLeptonVertexingAlg - done with lepton pT=" << tracklep->pt() << ", " << TruthAsStr(*lepton) << endl
		  << "___________________________________________________________________________");
  }

  CHECK(evtStore()->record(std::move(SVAuxContainer), m_svContainerName+"Aux."));
  CHECK(evtStore()->record(std::move(SVContainer),    m_svContainerName));
  
  ATH_MSG_DEBUG("SV Vertex container " << m_svContainerName << " recorded in store");

  //
  // Reset fitting service - this deletes vertexes that were not saved in SG
  //
  m_vertexFitterSvc->resetEvent();

  ATH_MSG_DEBUG(" NonPromptLeptonVertexingAlg::execute - done with this event" << endl
		<< "___________________________________________________________________________");

  return StatusCode::SUCCESS;
}

//=============================================================================
bool Prompt::NonPromptLeptonVertexingAlg::passElecCand(const xAOD::Electron &elec)
{
  //
  // Check whether electron candidate pass selections
  //
  char lh_loose  = -1;
  char lh_medium = -1;
  char lh_tight  = -1;
 
  Prompt::GetAuxVar(elec, lh_loose,  "DFCommonElectronsLHLoose");
  Prompt::GetAuxVar(elec, lh_medium, "DFCommonElectronsLHMedium");
  Prompt::GetAuxVar(elec, lh_tight,  "DFCommonElectronsLHTight");

  ATH_MSG_DEBUG("NonPromptLeptonVertexingAlg::passElecCand - " 
		<< "pT=" << elec.pt() << ", eta=" << elec.eta() << ", phi=" << elec.phi() << std::endl
		<< "   DFCommonElectronsLHLoose  = " << int(lh_loose)  << std::endl
		<< "   DFCommonElectronsLHMedium = " << int(lh_medium) << std::endl
		<< "   DFCommonElectronsLHTight  = " << int(lh_tight)  << std::endl
		<< "   " << TruthAsStr(elec));

  if(!lh_loose) {
    return false;
  }

  return true;
}

//=============================================================================
bool Prompt::NonPromptLeptonVertexingAlg::passMuonCand(const xAOD::Muon &muon)
{
  //
  // Check whether electron candidate pass selections
  //
  const bool loose    = muon.quality()  <= xAOD::Muon::Loose;
  const bool medium   = muon.quality()  <= xAOD::Muon::Medium;
  const bool tight    = muon.quality()  <= xAOD::Muon::Tight;
  const bool combined = muon.muonType() == xAOD::Muon::Combined;
  const bool fwdsi    = muon.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon;

  ATH_MSG_DEBUG("NonPromptLeptonVertexingAlg::passMuonCand - " 
		<< "pT=" << muon.pt() << ", eta=" << muon.eta() << ", phi=" << muon.phi() << std::endl
		<< "   Type     = " << muon.muonType() << std::endl
		<< "   Combined = " << combined        << std::endl
		<< "   FwdSi    = " << fwdsi           << std::endl
		<< "   Loose    = " << loose           << std::endl
		<< "   Medium   = " << medium          << std::endl
		<< "   Tight    = " << tight           << std::endl
		<< "   " << TruthAsStr(muon));

  if(muon.muonType() != xAOD::Muon::Combined) {
    return false;
  }

  if(muon.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) {
    return false;
  }

  return true;
}

//=============================================================================
std::vector<const xAOD::TrackParticle*> Prompt::NonPromptLeptonVertexingAlg::findNearbyTracks(const xAOD::TrackParticle &tracklep,
                                                                                              const xAOD::TrackParticleContainer &inDetTracks, 
											      const xAOD::Vertex &priVtx)
{  
  // 
  // Select tracks -- avoid using track selection tool since z0 definition is different
  //
  std::vector<const xAOD::TrackParticle *> mytracks;

  for(const xAOD::TrackParticle *track: inDetTracks) {
    if(!track) {
      ATH_MSG_WARNING("skip null track pointer - should never happen");
      continue;
    }

    //
    // Check minimum track and lepton DR: skip the track that is probably the lepton track
    //
    if(tracklep.p4().DeltaR(track->p4()) < m_minTrackLeptonDR) {
      ATH_MSG_DEBUG("skip the track very close to the lepton ");
      continue;
    }

    //
    // Check track and lepton maximum DR
    //
    if(tracklep.p4().DeltaR(track->p4()) > m_maxTrackLeptonDR) {
      continue;
    }

    const double delta_z0 = track->z0() + track->vz() - priVtx.z();
    const double Z0Sin    = std::fabs(delta_z0*std::sin(track->theta()));
    const double abs_eta  = std::fabs(track->eta());

    uint8_t numberOfPixelHits       = 0;
    uint8_t numberOfSCTHits         = 0;
    uint8_t numberOfPixelHoles      = 0;
    uint8_t numberOfSCTHoles        = 0; 
    uint8_t numberOfPixelSharedHits = 0;
    uint8_t numberOfSCTSharedHits   = 0;

    if(!(track->summaryValue(numberOfPixelHits,       xAOD::numberOfPixelHits)))       continue;
    if(!(track->summaryValue(numberOfSCTHits,         xAOD::numberOfSCTHits)))         continue;
    if(!(track->summaryValue(numberOfPixelHoles,      xAOD::numberOfPixelHoles)))      continue;
    if(!(track->summaryValue(numberOfSCTHoles,        xAOD::numberOfSCTHoles)))        continue;
    if(!(track->summaryValue(numberOfPixelSharedHits, xAOD::numberOfPixelSharedHits))) continue;
    if(!(track->summaryValue(numberOfSCTSharedHits,   xAOD::numberOfSCTSharedHits)))   continue;

    const uint8_t NSiHits   = numberOfPixelHits  + numberOfSCTHits;
    const uint8_t NSiHoles  = numberOfPixelHoles + numberOfSCTHoles;
    const float   NSiShHits = float(numberOfPixelSharedHits) + float(numberOfSCTSharedHits)/2.0;

    if(m_selectTracks) {
      //
      // Kinematic track selection
      //
      if(track->pt()                    < m_minTrackpT)    continue;
      if(abs_eta                        > m_maxTrackEta)   continue;
      if(Z0Sin                          > m_maxTrackZ0Sin) continue;
      
      //
      // Hit quality track selection
      //
      if(NSiHits                        < m_minTrackSilHits)       continue;
      if(NSiShHits                      > m_maxTrackSharedSilHits) continue;
      if(NSiHoles                       > m_maxTrackSilHoles  )    continue;
      if(numberOfPixelHoles             > m_maxTrackPixHoles  )    continue;
    }

    mytracks.push_back(track);
  }

  return mytracks;
}

//=============================================================================
std::vector<xAOD::Vertex*> Prompt::NonPromptLeptonVertexingAlg::prepLepWithTwoTrkSVVec(const FittingInput &input,
										       const xAOD::TrackParticle* tracklep,
										       const std::vector<const xAOD::TrackParticle*> &tracks)
{
  //
  // Decorate lepton with vector of two-track vertices.  
  // Return vector of finding vertices
  //  
  std::vector<xAOD::Vertex*>              twoTrk_vertices;
  std::vector<const xAOD::TrackParticle*> tracks_for_fit; 

  if(!input.priVtx) {
    ATH_MSG_WARNING("prepLepWithTwoTrkSVVec -- invalid primary vertex: nothing to do");
    return twoTrk_vertices;
  }

  for(const xAOD::TrackParticle *selectedtrack: tracks) {
    tracks_for_fit.clear();
    tracks_for_fit.push_back(tracklep);
    tracks_for_fit.push_back(selectedtrack);

    std::pair<xAOD::Vertex *, int> newSecondaryVertex = m_vertexFitterSvc->fitVertexWithPrimarySeed(input, tracks_for_fit, kTwoTrackVtx);

    if(!newSecondaryVertex.first) { 
      ATH_MSG_DEBUG("prepLepWithTwoTrkSVVec -- failed to fit 2-track vertex");            
      continue;
    }

    twoTrk_vertices.push_back(newSecondaryVertex.first);
  }

  return twoTrk_vertices;
}

//=============================================================================
std::vector<xAOD::Vertex*> Prompt::NonPromptLeptonVertexingAlg::prepLepWithMergedSVVec(const FittingInput &input,
										       const xAOD::TrackParticle* tracklep,
										       const std::vector<xAOD::Vertex *> &twoTrk_vertices)
{
  //
  // Merge the two vertices if the distance between them with in 0.5 mm.
  // Re-fit a three-track vertex using the input tracks from the vertices above.   
  //    
  std::vector<xAOD::Vertex*> twoTrk_vertices_pass;
  std::vector<xAOD::Vertex*> twoTrk_vertices_pass_fixed;
  std::vector<xAOD::Vertex*> twoTrk_vertices_merged;
  std::vector<xAOD::Vertex*> result_vertices;

  if(!input.priVtx) {
    ATH_MSG_WARNING("prepLepWithMergedSVVec -- invalid primary vertex: nothing to do");
    return result_vertices;
  }

  for(xAOD::Vertex *vtx: twoTrk_vertices) {
    double chi2OverDoF = -99.;

    if(vtx->numberDoF() > 0 && vtx->chiSquared() > 0) {
      chi2OverDoF = vtx->chiSquared()/double(vtx->numberDoF());
    }
    
    if(chi2OverDoF >= 0.0 && chi2OverDoF < m_mergeChi2OverDoF) {
      twoTrk_vertices_pass      .push_back(vtx);
      twoTrk_vertices_pass_fixed.push_back(vtx);
    }
  }
  
  std::vector<xAOD::Vertex *>::iterator curr_iter = twoTrk_vertices_pass.begin();

  while(curr_iter != twoTrk_vertices_pass.end()) {
    std::vector<xAOD::Vertex *> cluster_vtxs;  
    cluster_vtxs.push_back(*curr_iter);
    
    twoTrk_vertices_pass.erase(curr_iter);

    makeVertexCluster(cluster_vtxs, twoTrk_vertices_pass);
    
    curr_iter = twoTrk_vertices_pass.begin();
    
    //
    // Fit vertex cluster
    //
    std::vector<const xAOD::TrackParticle*> tracks_for_fit;

    for(xAOD::Vertex *vtx: cluster_vtxs) {
      for(unsigned k = 0; k < vtx->nTrackParticles(); ++k) {
        const xAOD::TrackParticle *track  = vtx->trackParticle(k);
	
        if(track) {
          tracks_for_fit.push_back(track);
        }   
      }
    } 
    
    //
    // Ignore standalone vertexes
    //
    if(cluster_vtxs.size() < 2) {
      continue;
    }
  
    //
    // Fit merged vertex
    //
    tracks_for_fit.push_back(tracklep);

    std::pair<xAOD::Vertex *, int> newSecondaryVertex = m_vertexFitterSvc->fitVertexWithPrimarySeed(input, tracks_for_fit, kSimpleMergedVtx);

    if(!newSecondaryVertex.first) { 
      ATH_MSG_DEBUG("DecorateLepWithMergedSVVec -- failed to fit merged vertex");      
      continue;
    }

    result_vertices.push_back(newSecondaryVertex.first);

    for(xAOD::Vertex *vtx: cluster_vtxs) {    
      twoTrk_vertices_merged.push_back(vtx);
    }

    ATH_MSG_DEBUG("DecorateLepWithMergedSVVec -- NTrack of merged vertex = " << newSecondaryVertex.first->nTrackParticles());
  }

  //
  // Include passed 2-track vertexes that were NOT merged
  //
  for(xAOD::Vertex *vtx: twoTrk_vertices_pass_fixed) {
    const std::vector<xAOD::Vertex*>::const_iterator fit = std::find(twoTrk_vertices_merged.begin(),
								     twoTrk_vertices_merged.end(),
								     vtx);

    if(fit == twoTrk_vertices_merged.end()) {
      result_vertices.push_back(vtx);
    }
  }

  return result_vertices;
}

//=============================================================================
void Prompt::NonPromptLeptonVertexingAlg::makeVertexCluster(std::vector<xAOD::Vertex *> &cluster_vtxs, std::vector<xAOD::Vertex *> &input_vtxs)
{
  ATH_MSG_DEBUG("makeVertexCluster - before: cluster_vtxs.size()=" << cluster_vtxs.size() << ", input_vtxs.size()=" << input_vtxs.size());
  
  std::vector<xAOD::Vertex *>::iterator vit = input_vtxs.begin();

  while(vit != input_vtxs.end()) {
    bool pass = false;

    for(std::vector<xAOD::Vertex *>::const_iterator cit = cluster_vtxs.begin(); cit != cluster_vtxs.end(); ++cit) {
      if((*vit) == (*cit)) {
	ATH_MSG_DEBUG("makeVertexCluster - logic error - found the same vertex twice: " << (*vit));
	continue;
      }

      const double vdist = GetDistance((*vit)->position(), (*cit)->position());
      
      ATH_MSG_DEBUG("makeVertexCluster - vdist=" << vdist ); 

      if(vdist < m_mergeMinVtxDist) {
	pass = true;
	break;
      }
    }

    if(pass) {
      cluster_vtxs.push_back(*vit);
      input_vtxs.erase(vit);
      
      vit = input_vtxs.begin();
    }
    else {
      vit++;
    }
  }

  ATH_MSG_DEBUG("makeVertexCluster - after:  cluster_vtxs.size()=" << cluster_vtxs.size() << ", input_vtxs.size()=" << input_vtxs.size());
}

//=============================================================================
void Prompt::NonPromptLeptonVertexingAlg::saveSecondaryVertices(std::vector<xAOD::Vertex *> &vtxs,
                                                                std::vector<int> &index_vector,
                                                                std::vector<ElementLink<xAOD::VertexContainer> > &sv_links,
                                                                xAOD::VertexContainer &SVContainer,
                                                                std::set< xAOD::Vertex* >& svSet)
{
  //
  // Record created xAOD::Vertex in output vertex container
  //
  ATH_MSG_DEBUG("saveSecondaryVertices - will save " << vtxs.size() << " vertexes");

  for(xAOD::Vertex *vtx: vtxs) {
    int index = -99;
    if(GetVar(vtx, index, "SecondaryVertexIndex")) {
      index_vector.push_back(index);
    }
    else {
      ATH_MSG_WARNING("saveSecondaryVertices - missing \"SecondaryVertexIndex\" variable");
    }

    if(svSet.insert(vtx).second) {
      if(!m_vertexFitterSvc->moveVertexPointerToContainer(vtx, SVContainer)) {
	ATH_MSG_ERROR("saveSecondaryVertices - failed to release xAOD::Vertex from fitting service - expect troubles");
	continue;
      }

      //
      // First time seeing this this vertex - record it in output container
      //
      ElementLink<xAOD::VertexContainer> sv_link;
      sv_link.toContainedElement(SVContainer, vtx);
      sv_links.push_back(sv_link);
    }
  }

  ATH_MSG_DEBUG("saveSecondaryVertices - all done");
}


