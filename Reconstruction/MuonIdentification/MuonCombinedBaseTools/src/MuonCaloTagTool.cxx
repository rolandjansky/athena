/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//<<<<<< INCLUDES                                                       >>>>>>

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
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "CLHEP/Vector/LorentzVector.h"


namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuonCaloTagTool::MuonCaloTagTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
	m_nTrueMuons(0),
	m_nTracksTagged(0),
	m_nMuonsTagged(0),
	m_caloMuonLikelihood("CaloMuonLikelihoodTool/CaloMuonLikelihoodTool",this),
        m_caloMuonTagLoose("CaloMuonTag/CaloMuonTagLoose"),
        m_caloMuonTagTight("CaloMuonTag/CaloMuonTag"),
        m_trkDepositInCalo("TrackDepositInCaloTool/TrackDepositInCaloTool"),
	m_trackIsolationTool("xAOD::TrackIsolationTool"),
        m_trkSelTool("InDet::InDetDetailedTrackSelectorTool/CaloTrkMuIdAlgTrackSelectorTool")
  {
    declareInterface<IMuonCombinedInDetExtensionTool>(this);
    declareInterface<IMuonCombinedTrigCaloTagExtensionTool>(this);
    // --- Collections ---
    declareProperty("TrackParticleName",                 m_TrackParticleName             =  "TrackParticleCandidate"    );
    declareProperty("VertexContainerName",               m_VertexContainerName           =  "VxPrimaryCandidate"        );
    
    // --- Muon Dressing ---    //declareProperty("CoreDeltaR",                        m_coreDR       =  0.05                );
    //declareProperty("doMuonDressing",                    m_doDressing   =  true                );
    declareProperty("doCosmicTrackSelection",            m_doCosmicTrackSelection  = false     );
    
    // --- Track Preselection Cuts ---
    declareProperty("doTrkSelection",                    m_doTrkSelection            =  true   );
    declareProperty("TrackEIsoBarrelCut",                m_eIsoBarrelCut             =  15000. );
    declareProperty("TrackEIsoTransitionCut",            m_eIsoTransitionCut         =  8000.  );
    declareProperty("TrackEIsoEndCapCut",                m_eIsoEndCapCut             =  12000. );
    declareProperty("TrackPtIsoPtRatioCut",              m_ptIsoPtRatioCut           =  5.0    );
    declareProperty("TrackEIsoPtRatioBarrelCut",         m_eIsoPtRatioBarrelCut      =  2.5    );
    declareProperty("TrackEIsoPtRatioTransitionCut",     m_eIsoPtRatioTransitionCut  =  1.25   );
    declareProperty("TrackEIsoPtRatioEndCapCut",         m_eIsoPtRatioEndCapCut      =  1.6    );
    declareProperty("TrackIsoConeSize",                  m_trackIsoCone              =  0.45   );
    declareProperty("EnergyIsoConeSize",                 m_energyIsoCone             =  0.4    );
    
    // --- Calorimeter ID Tools ---
    declareProperty("CaloMuonTagLoose",                  m_caloMuonTagLoose             );
    declareProperty("CaloMuonTagTight",                  m_caloMuonTagTight             );
    declareProperty("doCaloMuonTag",                     m_doCaloMuonTag        =  true );
    declareProperty("doCaloLR",                          m_doCaloLR             =  true );
    declareProperty("ShowTruth",                         m_doTruth              =  true);
    declareProperty("DebugMode",                         m_debugMode            =  false);
    declareProperty("doOldExtrapolation",                m_doOldExtrapolation   =  false);
    declareProperty("IgnoreSiAssociatedCandidates",      m_ignoreSiAssocated    =  true );
    declareProperty("ShowCutFlow",                       m_showCutFlow          =  true);
    declareProperty("CaloMuonLikelihoodTool",            m_caloMuonLikelihood           );
    declareProperty("CaloLRLikelihoodCut",               m_CaloLRlikelihoodCut  =  0.5  );  //Likelihood ratio hard cut
    
    // --- Track in Calo Tools ---
    declareProperty("TrackDepositInCaloTool",            m_trkDepositInCalo  );
    declareProperty("TrackIsolationTool",                m_trackIsolationTool );
    declareProperty("TrackSelectorTool",                 m_trkSelTool        );
  }

  MuonCaloTagTool::~MuonCaloTagTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuonCaloTagTool::initialize() {
    
    ATH_MSG_INFO("MuonCaloTagTool::initialize()");
    
    if (m_doCosmicTrackSelection && m_doTrkSelection) {
      ATH_MSG_WARNING("Cosmic track selection and detailed track selection both set to true.");
      ATH_MSG_WARNING("Cosmic track selection will be discarded. Fix jobOptions.");
    }
    
    // --- Get an Identifier helper object ---
    //StoreGateSvc* detStore(0);
    //if (service("DetectorStore", detStore).isFailure()) {
    //  msg(MSG::ERROR) << "Detector service not found !" << endmsg;
    //  return StatusCode::FAILURE;
    //}
    if( true ){
      if( m_doCaloLR ) ATH_CHECK( m_caloMuonLikelihood.retrieve() );
      ATH_CHECK( m_caloMuonTagLoose.retrieve()   );
      ATH_CHECK( m_caloMuonTagTight.retrieve()   );
      ATH_CHECK( m_trkDepositInCalo.retrieve()   );
      if(!m_trackIsolationTool.empty()) ATH_CHECK( m_trackIsolationTool.retrieve() );
      ATH_CHECK( m_trkSelTool.retrieve()         );
    }
    if(m_doTrkSelection && m_doCosmicTrackSelection) {
      ATH_MSG_WARNING("Inconsistent setup: track selection for collisions AND cosmics requested.");
    } 
    
    return StatusCode::SUCCESS;
  }

  StatusCode MuonCaloTagTool::finalize() {
    
    
    ATH_MSG_INFO("Number of true muons               : " << m_nTrueMuons);
    ATH_MSG_INFO("Number of tracks tagged            : " << m_nTracksTagged);
    ATH_MSG_INFO("Number of muons tagged             : " << m_nMuonsTagged);
    
    return StatusCode::SUCCESS;
  
  }

  void MuonCaloTagTool::extendWithPRDs( const InDetCandidateCollection& inDetCandidates, const Muon::MdtPrepDataContainer* mdtPRDs, const Muon::CscPrepDataContainer* cscPRDs,
					const Muon::RpcPrepDataContainer* rpcPRDs, const Muon::TgcPrepDataContainer *tgcPRDs, const Muon::sTgcPrepDataContainer* stgcPRDs,
					const Muon::MMPrepDataContainer* mmPRDs ) {
    //shouldn't need this interface for this tool, I don't think
    if(mdtPRDs && cscPRDs && rpcPRDs && tgcPRDs && stgcPRDs && mmPRDs) extend(inDetCandidates);
  }

  void MuonCaloTagTool::extend( const InDetCandidateCollection& inDetCandidates ) {
    extend(inDetCandidates, nullptr, nullptr);
  }

  void MuonCaloTagTool::extend( const InDetCandidateCollection& inDetCandidates,
                                const CaloCellContainer* caloCellCont,
                                const xAOD::CaloClusterContainer* caloClusterCont) {


    //return;
    //
    // --- Retrieve primary vertex ---
    const Trk::Vertex* vertex = 0;// retrievePrimaryVertex();
//    const Trk::TrackParticleOrigin prtOrigin =  Trk::TrackParticleOrigin::PriVtx; // we only consider trks form primary vtx
    
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
      //const xAOD::TrackParticle& tp =  (*idTP).indetTrackParticle(); //->originalTrackParticle()
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

//      bool truthMuon = false;
      ElementLink< xAOD::TruthParticleContainer > truthLink;
      // const xAOD::TrackParticle* tp  = ElementLink< xAOD::TruthParticleContainer > truthLink;
      if( tp->isAvailable< ElementLink< xAOD::TruthParticleContainer > > ("truthParticleLink") ){
	truthLink = tp->auxdata< ElementLink< xAOD::TruthParticleContainer >  >("truthParticleLink");
	if(m_doTruth && truthLink.isValid()) {
	  //         truthMuon = (  abs((*truthLink)->pdgId())==13 && ((*truthLink)->auxdata<int>("truthType") == 6 ||
	  //           (*truthLink)->auxdata<int>("truthType") == 7 )  && // prompt muon
	  //           (*truthLink)->auxdata<int>("truthOrigin") > 0 && (*truthLink)->auxdata<int>("truthOrigin") <= 34 );
	  // no decay in flight
	  pdgId = (*truthLink)->pdgId();
	} else {
	  ATH_MSG_DEBUG("No TruthLink available.");
	}
      }

      // --- Track selection ---
//      if(!selectTrack(track, vertex)) continue;

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
      if(m_doCaloLR)
	likelihood = m_caloMuonLikelihood->getLHR(par, caloClusterCont);
	ATH_MSG_DEBUG("Track found with tag " << tag << " and LHR " << likelihood);
      // --- If both the taggers do not think it's a muon, forget about it ---
      if (tag == 0 && likelihood <= m_CaloLRlikelihoodCut) {
	continue;                                                                                                                                                            
      }
      // --- Only accept tight tagged muons if pT is below 4 GeV ---
      if (tag<10&& par->pT()<4000) {
	continue;
      }
      
      // FIXME const-cast  changes object passed in as const
      createMuon(const_cast<InDetCandidate&>(*idTP),  deposits, tag, likelihood);

      // --- Count number of muons written to container 
      if ( abs(pdgId) == 13 )  m_nMuonsTagged++;
      m_nTracksTagged++;
//        delete trackParticle;
    }
    
  } //end of the extend method

  
  //retrievePrimaryVertex -> SHOULD BE OK
  const Trk::Vertex* MuonCaloTagTool::retrievePrimaryVertex() const {
    
    // --- No need to retrieve the primary vertex when no track selection is applied ---
    if (!m_doTrkSelection) 
      return 0;
    
    // --- Retrieve vertex container ---
    const VxContainer* vxContainer = 0;
    StatusCode sc = evtStore()->retrieve(vxContainer, m_VertexContainerName);
    if(sc.isFailure() || vxContainer==0) {
      ATH_MSG_DEBUG("Vertex container " << m_VertexContainerName << " not found in StoreGate!");
      return 0;
    }
    // --- Retrieve vertex ---
    const Trk::Vertex* vertex = 0;
    if(vxContainer->size()> 0)
      vertex = &((*vxContainer)[0]->recVertex());
    else{
      ATH_MSG_DEBUG("No vertices in vertex container.");
    }
  
    return vertex;
    
  }
  
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
      //bool result = trk->perigeeParameters()->momentum().perp()>5000; //;p()>2000); to check
      
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
    if(ptcl->perigeeParameters()->momentum().mag()>2000) //cl->p() < 2000)
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
  
  bool MuonCaloTagTool::applyTrackIsolation(const xAOD::TrackParticle& tp ) {

    if( m_trackIsolationTool.empty() ) return true;

    //std::vector<xAOD::Iso::IsolationType> ptcones = { xAOD::Iso::ptcone45 };
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
    ATH_MSG_VERBOSE("  position:  r = " << r  << ", eta = " << eta    << ", phi = " << phi/3.1415);
    ATH_MSG_DEBUG("  momentum: pt = " << pt << ", eta = " << momEta << ", phi = " << momPhi/3.1415);
    
    return;
    
  }
  
  /*
  //applyEnergyIsolation -> DON'T WORK with Trk::Track (require particle)
  
  bool MuonCaloTagTool::applyEnergyIsolation(const xAOD::IParticle* muon){
    
    double eIso = m_muonIsolationTool->isolationEnergy(muon, m_energyIsoCone);
    double eIsoRatio = -9999.;
    double pt = muon->pt();
    double eta= muon->eta();
    if (eIso>0) {
      if (pt>0) {
  	eIsoRatio  = eIso/pt;
      }
    }
    
    double eIsoCut = 0;
    double ratioCutEt = 0;
    if( fabs(eta)<1.5 ) {
      eIsoCut     =   m_eIsoBarrelCut;
      ratioCutEt  =   m_eIsoPtRatioBarrelCut;
    }
    else if (fabs(eta) > 1.5 && fabs(eta) < 1.8) {
      eIsoCut     =   m_eIsoTransitionCut;
      ratioCutEt  =   m_eIsoPtRatioTransitionCut;
    }
    else {
      eIsoCut     =   m_eIsoEndCapCut;
      ratioCutEt  =   m_eIsoPtRatioEndCapCut;
    } 
    
    if (m_showCutFlow) {
      ATH_MSG_DEBUG("Isolation Energy in " << m_energyIsoCone << " cone is: " << eIso << ", eta: " << eta << ",  pt: " << pt);
      ATH_MSG_DEBUG("Et Iso: " << eIso << " Et Iso over pt: " << eIsoRatio);
    }
    
    if( eIso < eIsoCut && eIsoRatio < ratioCutEt ) {
      if (m_showCutFlow)
  	ATH_MSG_DEBUG("EtIso and EtIso over pt cut passed.");
      return true;
    }
    else
      return false;
  
  }
  */

  
  void MuonCaloTagTool::createMuon(InDetCandidate& muonCandidate,
                                   const std::vector<DepositInCalo>& deposits, int tag, float likelihood) const {
    
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
    }
    else if (likelihood > m_CaloLRlikelihoodCut)
      caloTag = new CaloTag(xAOD::Muon::CaloLikelihood, eLoss, 0); 
    if( caloTag ){
      caloTag->set_deposits(deposits);
      caloTag->set_caloMuonIdTag(tag);
      caloTag->set_caloLRLikelihood(likelihood);
      
//      eLoss = m_muonIsolationTool->summedCellEnergy(ptcl, m_coreDR);
//      caloTag->set_etCore(eLoss);
      muonCandidate.addTag(*caloTag);
    }
  }


}	// end of namespace
