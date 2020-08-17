/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCaloTagTool
//  AlgTool performing calorimeter tagging of ID tracks for energy depositions
//  compatible with mip. A CaloTag is added to the InDetCandidate object.
//
//  Authors: Nicolo de Groot & Antonio Salvucci
//  based on: CaloTrkMuId
//  
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////


#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCaloTagTool.h"
#include "RecoToolInterfaces/IsolationCommon.h"
#include <vector>

#include "TrackToCalo/CaloCellHelpers.h"

#include "CaloEvent/CaloCellContainer.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

// --- HepMC Includes ---
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "CLHEP/Vector/LorentzVector.h"

#include <cmath>

namespace MuonCombined {
  MuonCaloTagTool::MuonCaloTagTool (const std::string& type, const std::string& name, const IInterface* parent) :


    AthAlgTool(type, name, parent),
    m_nTrueMuons(0),
    m_nTracksTagged(0),
    m_nMuonsTagged(0) {
      declareInterface<IMuonCombinedInDetExtensionTool>(this);
      declareInterface<IMuonCombinedTrigCaloTagExtensionTool>(this);
  }

  StatusCode MuonCaloTagTool::initialize() {
    
    ATH_MSG_INFO("MuonCaloTagTool::initialize()");
    
    if (m_doCosmicTrackSelection && m_doTrkSelection) {
      ATH_MSG_WARNING("Cosmic track selection and detailed track selection both set to true.");
      ATH_MSG_WARNING("Cosmic track selection will be discarded. Fix jobOptions.");
    }
    
    // --- Get an Identifier helper object ---
    if( m_doCaloLR ) ATH_CHECK( m_caloMuonLikelihood.retrieve() );
    else m_caloMuonLikelihood.disable();

    if( m_doCaloMuonScore ) ATH_CHECK( m_caloMuonScoreTool.retrieve() );
    else m_caloMuonScoreTool.disable();

    ATH_CHECK( m_caloMuonTagLoose.retrieve()   );
    ATH_CHECK( m_caloMuonTagTight.retrieve()   );
    ATH_CHECK( m_trkDepositInCalo.retrieve()   );
    if(!m_trackIsolationTool.empty()) ATH_CHECK( m_trackIsolationTool.retrieve() );
    else m_trackIsolationTool.disable();
    ATH_CHECK( m_trkSelTool.retrieve()         );
    ATH_CHECK(m_caloClusterCont.initialize(m_doCaloLR));
    ATH_CHECK(m_caloCellCont.initialize(m_doOldExtrapolation));
    
    return StatusCode::SUCCESS;
  }

  StatusCode MuonCaloTagTool::finalize() {
    ATH_MSG_INFO("Number of true muons               : " << m_nTrueMuons);
    ATH_MSG_INFO("Number of tracks tagged            : " << m_nTracksTagged);
    ATH_MSG_INFO("Number of muons tagged             : " << m_nMuonsTagged);
    return StatusCode::SUCCESS;
  }

  void MuonCaloTagTool::extendWithPRDs( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
					TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments) const {
    //shouldn't need this interface for this tool, I don't think
    if(!prdData.mdtPrds) ATH_MSG_DEBUG("calo-tagging doesn't need PRDs");
    extend(inDetCandidates, tagMap, combTracks, meTracks, segments);
  }

  void MuonCaloTagTool::extend( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, TrackCollection* meTracks,
				Trk::SegmentCollection* segments) const {

    if(combTracks || meTracks || segments) ATH_MSG_DEBUG("track collections passed to MuonCaloTagTool?");
    const xAOD::CaloClusterContainer* caloClusterCont=0;
    const CaloCellContainer* caloCellCont=0;
    if(m_doCaloLR){ //retrieve the xAOD::CaloClusterContainer
      SG::ReadHandle<xAOD::CaloClusterContainer> clusters(m_caloClusterCont);
      if(!clusters.isValid()) ATH_MSG_WARNING("CaloClusterContainer "<<m_caloClusterCont.key()<<" not valid");
      else if(!clusters.isPresent()) ATH_MSG_DEBUG("CaloClusterContainer "<<m_caloClusterCont.key()<<" not present");
      else caloClusterCont=clusters.cptr();
    }
    if(m_doOldExtrapolation){ //retrieve the CaloCellContainer
      SG::ReadHandle<CaloCellContainer> cells(m_caloCellCont);
      if(!cells.isValid()) ATH_MSG_WARNING("CaloCellContainer "<<m_caloCellCont.key()<<" not valid");
      else if(!cells.isPresent()) ATH_MSG_DEBUG("CaloCellContainer "<<m_caloCellCont.key()<<" not present");
      else caloCellCont=cells.cptr();
    }
    extend(inDetCandidates, tagMap, caloCellCont, caloClusterCont);
  }

  void MuonCaloTagTool::extend( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap,
                                const CaloCellContainer* caloCellCont,
                                const xAOD::CaloClusterContainer* caloClusterCont) const {


    // --- Retrieve primary vertex (not retrieving for now) ---
    const Trk::Vertex* vertex = 0;
    
    // --- Big loop over all the particles in the container ---                                                                                                              
    //-- make sure CaloCellContainer is there
    if (!caloCellCont){
      ATH_MSG_VERBOSE("Called with no CaloCellContainer in argument");
    }

    for( auto idTP : inDetCandidates ){
      
      // skip track particles which are no complete ID track
      if ( m_ignoreSiAssocated && idTP->isSiliconAssociated() ){
        ATH_MSG_DEBUG("Associated track is just tracklet. Skipping this particle.");
        continue;
      }
      // ensure that the id trackparticle has a track
      const Trk::Track* track = idTP->indetTrackParticle().track(); //->originalTrack()
      const xAOD::TrackParticle* tp =  &(*idTP).indetTrackParticle(); //->originalTrackParticle()
      if( !track ){
	ATH_MSG_DEBUG("Particle with no track associated. Skipping this particle.");
	continue;
      }
      const Trk::TrackParameters* par = getTrackParameters(track);
      if (!par) {
	ATH_MSG_DEBUG("Could not retrieve Trk::TrackParameters from track. Skipping this particle.");
	continue;
      }

      // --- Get pdgId (when requested) ---
      int pdgId = 0;

      ElementLink< xAOD::TruthParticleContainer > truthLink;
      if( tp->isAvailable< ElementLink< xAOD::TruthParticleContainer > > ("truthParticleLink") ){
	truthLink = tp->auxdata< ElementLink< xAOD::TruthParticleContainer >  >("truthParticleLink");
	if(m_doTruth && truthLink.isValid()) {
	  // no decay in flight
	  pdgId = (*truthLink)->pdgId();
	} else {
	  ATH_MSG_DEBUG("No TruthLink available.");
	}
      }

      // --- Track selection ---

      if(!selectTrack(track, vertex)){                                                                                                                           
	// --- in debug mode, monitor the number of muons rejected by track selection ---
	if (m_debugMode) {
	  // --- Truth info == 0 when doTruth is false ---
	  if (abs(pdgId)==13) {
	    //MUST BE DEFINE track pt
	    ATH_MSG_DEBUG("Cutflow MuonCaloTag  >>> Muon rejected by track selection."); //(pt = " << track->pt() << ")" << "<<<"); to update
	  }
	}       
        continue;
      }

      // --- Track isolation --- -> SEE DETAILS IN corresponding method
      if(!applyTrackIsolation(*tp)){
        // --- in debug mode, monitor the number of muons rejected by track selection ---
        if(m_debugMode){
          // --- Truth info == 0 when doTruth is false ---
          if (abs(pdgId)==13) {
            ATH_MSG_DEBUG("Cutflow MuonCaloTag  >>> Muon rejected by track isolation. <<<");
          }                                                                                                                                                              
        }                                                                                                                                                                  
        continue;                                                                                                                                                            
      } 
      
      // --- Show track information when debug mode is set to true ---
      if (m_debugMode) {
	showTrackInfo(par);
	if (pdgId)
	  ATH_MSG_DEBUG("  pdgId = " << pdgId);
      }
	if (abs(pdgId)==13) m_nTrueMuons++;

      // --- Muon tagging ---
      float likelihood = 0;                                                                                                                                                    
      float muon_score = -1;
      int tag = 0;
      std::vector<DepositInCalo> deposits;
      if (m_doCaloMuonTag) {
        if (m_doOldExtrapolation)  {
	   deposits = m_trkDepositInCalo->getDeposits(par, caloCellCont);
        } else {
	   deposits = m_trkDepositInCalo->getDeposits(tp);
        }
	tag = m_caloMuonTagLoose->caloMuonTag(deposits, par->eta(), par->pT());
	tag += 10*m_caloMuonTagTight->caloMuonTag(deposits, par->eta(), par->pT());
      }
      if(m_doCaloLR){
	likelihood = m_caloMuonLikelihood->getLHR(tp, caloClusterCont);
      }
      if(m_doCaloMuonScore){
	muon_score = m_caloMuonScoreTool->getMuonScore(tp);
      }
      ATH_MSG_DEBUG("Track found with tag " << tag << ", LHR " << likelihood << " and calo muon score " << muon_score);
      // --- If all three taggers do not think it's a muon, forget about it ---
      if (tag == 0 && likelihood <= m_CaloLRlikelihoodCut && muon_score < m_CaloMuonScoreCut) {
	continue;                                                                                                                                                            
      }
      // --- Only accept tight tagged muons if pT is below 4 GeV and the muon score is below the threshold---
      if (tag<10 && par->pT()<4000 && muon_score < m_CaloMuonScoreCut ) {
	continue;
      }
      
      // FIXME const-cast  changes object passed in as const
      createMuon(*idTP,  deposits, tag, likelihood, muon_score, tagMap);

      // --- Count number of muons written to container 
      if ( abs(pdgId) == 13 )  m_nMuonsTagged++;
      m_nTracksTagged++;
    }
    
  } //end of the extend method

  
  //getTrackParameters -> SHOULD BE OK
  const Trk::TrackParameters* MuonCaloTagTool::getTrackParameters(const Trk::Track* trk) const{
    
    // --- Retrieve the last measured hit which is closest to the solenoid/calorimeter entrance ---
    const DataVector<const Trk::TrackParameters>* paramvec = trk->trackParameters();
    const Trk::TrackParameters* param = 0;
    if (paramvec) {
      DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
      param = *(--itEnd);
    }

    return param;
    
  }

  //selectTrack -> TO CHECK track p momenta
  bool MuonCaloTagTool::selectTrack(const Trk::Track* trk, const Trk::Vertex* vertex) const{
    
    if (m_doTrkSelection) {
      bool result = (m_trkSelTool->decision(*trk, vertex)) && (trk->perigeeParameters()->momentum().mag()>2000); //;p()>2000); to check
      
      if (!result&&m_showCutFlow)
	ATH_MSG_DEBUG("Track selection wrt primary vertex: Veto on this track");
      return result;
    }
    if (m_doCosmicTrackSelection) {
      bool result = selectCosmic(trk);
      if (!result&&m_showCutFlow)
	ATH_MSG_DEBUG("Track selection in commissioning mode: Veto on this track");
      return result;
    }
    
    // --- Return true when no track selection options are set ---
    return true;
    
  }

  // cosmicTrackSelection
  bool MuonCaloTagTool::selectCosmic(const Trk::Track* ptcl) const{
    
    // --- Only ask some hits with eta information ---
    if(ptcl->perigeeParameters()->momentum().mag()>2000) 
      return false;
    const Trk::TrackSummary* trkSummary = ptcl->trackSummary();
    if (trkSummary) {
      int nHitsSCT   = trkSummary->get(Trk::numberOfSCTHits);
      int nHitsPixel = trkSummary->get(Trk::numberOfPixelHits);
      if ((nHitsSCT + nHitsPixel) > 2)
	return true;
    }
    else {
      ATH_MSG_WARNING("Trk::TrackSummary was not retrieved from TrackParticle.");
    }
    
    return false;
    
  }
  
  // applyTrackIsolation check 
  
  bool MuonCaloTagTool::applyTrackIsolation(const xAOD::TrackParticle& tp ) const {

    if( m_trackIsolationTool.empty() ) return true;

    xAOD::TrackCorrection corrlist;
    corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::IsolationTrackCorrection::coreTrackPtr));
    std::vector<xAOD::Iso::IsolationType> ptcones = { xAOD::Iso::ptcone40 };
    xAOD::TrackIsolation trackIsolation;
    if( !m_trackIsolationTool->trackIsolation( trackIsolation, tp,ptcones,corrlist ) ) {
      ATH_MSG_WARNING(" Calculation of TrackIsolation failed");
      return false;
    }
    ATH_MSG_VERBOSE("Got track isolation " << trackIsolation.ptcones[0]);
    double ptIso = trackIsolation.ptcones[0];
    double ptIsoRatio = -9999;
    if (ptIso>0) {
      double pt = tp.pt();
      if (pt>0) // ---> just for safety (pt can never be 0 actually)
  	ptIsoRatio = ptIso/pt;
    } 
    if( ptIso==0 || ptIsoRatio < m_ptIsoPtRatioCut ) {
      return true;
    }
    else {
      if (m_showCutFlow)
  	ATH_MSG_DEBUG("Did not pass track isolation cut.");
      return false;
    }
  }

  
  //showTrackInfo -> SHOULD BE OK
  void MuonCaloTagTool::showTrackInfo(const Trk::TrackParameters* par) const{
    
    double r      = par->position().perp();
    double eta    = par->position().eta();
    double phi    = par->position().phi();
    double pt     = par->momentum().perp();
    double momEta = par->momentum().eta();
    double momPhi = par->momentum().phi();
    ATH_MSG_DEBUG("*** Analysing track with parameters: ***");
    ATH_MSG_VERBOSE("  position:  r = " << r  << ", eta = " << eta    << ", phi = " << phi/M_PI);
    ATH_MSG_DEBUG("  momentum: pt = " << pt << ", eta = " << momEta << ", phi = " << momPhi/M_PI);
    
    return;
    
  }
  
  void MuonCaloTagTool::createMuon(const InDetCandidate& muonCandidate,
                                   const std::vector<DepositInCalo>& deposits, int tag, float likelihood, float muonScore, InDetCandidateToTagMap* tagMap) const {
    
    std::vector<DepositInCalo>::const_iterator deposit  = deposits.begin();
    std::vector<DepositInCalo>::const_iterator depositE = deposits.end();
    double eLoss = 0; //Energy Loss as measured in the cell closest to the track in each sample
    CaloTag* caloTag = 0;
    for(; deposit != depositE; deposit++)
      eLoss+=deposit->energyDeposited();

    if (tag>0) {
      caloTag = new CaloTag(xAOD::Muon::CaloTag, eLoss, 0); //set eLoss, sigmaEloss is set to 0.
      if(likelihood > m_CaloLRlikelihoodCut)
        caloTag->set_author2(xAOD::Muon::CaloLikelihood);
      
      if (muonScore > m_CaloMuonScoreCut && likelihood > m_CaloLRlikelihoodCut)
	caloTag->set_author3(xAOD::Muon::CaloScore);
      else if (muonScore > m_CaloMuonScoreCut)
	caloTag->set_author2(xAOD::Muon::CaloScore);

    }
    else if (likelihood > m_CaloLRlikelihoodCut){
      caloTag = new CaloTag(xAOD::Muon::CaloLikelihood, eLoss, 0); 
      if (muonScore > m_CaloMuonScoreCut) 
	caloTag->set_author2(xAOD::Muon::CaloScore);
    }
    else if (muonScore > m_CaloMuonScoreCut){
      caloTag = new CaloTag(xAOD::Muon::CaloScore, eLoss, 0);
    }

    if( caloTag ){
      caloTag->set_deposits(deposits);
      caloTag->set_caloMuonIdTag(tag);
      caloTag->set_caloLRLikelihood(likelihood);
      caloTag->set_caloMuonScore(muonScore);
      tagMap->addEntry(&muonCandidate,caloTag);
    }

  }


}	// end of namespace
