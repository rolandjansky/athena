/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetTagAna is an example algorithm which shows how to access the
// b-tagging information from JetTag objects.
//
// To simply access the b-tagging weight for each jet, have a look 
// at the section SECTION::WEIGHTS
//
// To access the Monte-Carlo label (b,c or light) of a jet, have a 
// look at the section SECTION::LABEL
//
// To access detailed information about each tag: SECTION::DETAILS
//
// Author: Laurent Vacavant <vacavant@in2p3.fr>

#include "JetTagAna.h"

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/DataHandle.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TMath.h"

#include "McParticleEvent/TruthParticleContainer.h"
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/JetTagInfoBase.h"
#include "JetTagEvent/TrackAssociation.h"
#include "Particle/TrackParticleContainer.h"

#include "Navigation/NavigationToken.h"
#include "MuonIDEvent/MuonAssociation.h"
#include "egammaEvent/ElectronAssociation.h"


#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"

/// the Electron
#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"


/// The Muon
#include "muonEvent/Muon.h"

#include "JetTagInfo/ITagInfo.h"
#include "JetTagInfo/IPInfoBase.h"
#include "JetTagInfo/IPInfoPlus.h"
#include "JetTagInfo/IPTrackInfo.h"
#include "JetTagInfo/SVInfoBase.h"
#include "JetTagInfo/SVInfoPlus.h"
#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/SoftMuonInfo.h"
#include "JetTagInfo/SMTrackInfo.h"
#include "JetTagInfo/SoftElectronInfo.h"
#include "JetTagInfo/SETrackInfo.h"
#include "JetTagInfo/SoftLeptonTruthInfo.h"
#include "JetTagInfo/SLTrueInfo.h"

//*
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigEFBjetContainer.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/Units.h"

#include <map>
#include <algorithm>
#include <cmath>

using Athena::Units::GeV;

JetTagAna::JetTagAna(const std::string& name,
		     ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_irun(0),
    m_ievt(0),
    m_trackTES(nullptr),
    m_mcpartTES(nullptr),
    m_histos(nullptr),
    m_h_global_counters(nullptr),
    m_h_global_nprimvtx(nullptr),
    m_h_global_xprimvtx(nullptr),
    m_h_global_yprimvtx(nullptr),
    m_h_global_zprimvtx(nullptr),
    m_h_global_primvtxresx(nullptr),
    m_h_global_primvtxresy(nullptr),
    m_h_global_primvtxresz(nullptr),
    m_h_global_nmcpart(nullptr),
    m_h_global_ntrkpart(nullptr),
    m_h_global_BLayerHits(nullptr),
    m_h_global_BLayerSharedHits(nullptr),
    m_h_global_PixelHits(nullptr),
    m_h_global_PixelLayers(nullptr),
    m_h_global_SiHits(nullptr),
    m_h_global_TRTHits(nullptr),
    m_h_global_nmuon(nullptr),
    m_h_global_nelectron(nullptr),
    m_h_global_njettag(nullptr),
    m_h_truth_bquark_nb(nullptr),
    m_h_truth_bquark_pt(nullptr),
    m_h_truth_bquark_eta(nullptr),
    m_h_truth_cquark_nb(nullptr),
    m_h_truth_cquark_pt(nullptr),
    m_h_truth_cquark_eta(nullptr),
    m_h_truth_bhadr_nb(nullptr),
    m_h_truth_bhadr_pt(nullptr),
    m_h_truth_bhadr_eta(nullptr),
    m_h_truth_chadr_nb(nullptr),
    m_h_truth_chadr_pt(nullptr),
    m_h_truth_chadr_eta(nullptr),
    m_h_jet_ntotal(nullptr),
    m_h_jet_label(nullptr),
    m_h_jet_nlabelb(nullptr),
    m_h_jet_nlabelc(nullptr),
    m_h_jet_nlabelt(nullptr),
    m_h_jet_ntag(nullptr),
    m_h_jet_eta(nullptr),
    m_h_jet_phi(nullptr),
    m_h_jet_et(nullptr),
    m_h_jet_ntracks(nullptr),
    m_h_jet_tracks_pt(nullptr),
    m_h_jet_tracks_BLayerHits(nullptr),
    m_h_jet_tracks_PixelHits(nullptr),
    m_h_jet_tracks_SCTHits(nullptr),
    m_h_jet_nmuons(nullptr),
    m_h_jet_muons_pt(nullptr),
    m_h_jet_nelectrons(nullptr),
    m_h_jet_electrons_pt(nullptr),
    m_h_tag_jetprob_w(nullptr),
    m_h_tag_i2d_w(nullptr),
    m_h_tag_i2d_n(nullptr),
    m_h_tag_i2d_b(nullptr),
    m_h_tag_i2d_u(nullptr),
    m_h_tag_i2d_sig(nullptr),
    m_h_tag_i3d_w(nullptr),
    m_h_tag_i3d_n(nullptr),
    m_h_tag_i3d_b(nullptr),
    m_h_tag_i3d_u(nullptr),
    m_h_tag_i3d_sig(nullptr),
    m_h_tag_sv1_w(nullptr),
    m_h_tag_sv1_b(nullptr),
    m_h_tag_sv1_u(nullptr),
    m_h_tag_sv2_w(nullptr),
    m_h_tag_sv2_b(nullptr),
    m_h_tag_sv2_u(nullptr),
    m_h_tag_sv_n(nullptr),
    m_h_tag_sv_n2t(nullptr),
    m_h_tag_sv_frc(nullptr),
    m_h_tag_sv_m(nullptr),
    m_h_tag_cmb_w(nullptr),
    m_h_tag_lf2d_w(nullptr),
    m_h_tag_svbu_w(nullptr),
    m_h_tag_lhsig_w(nullptr),
    m_h_tag_softm_w(nullptr),
    m_h_tag_softe_w(nullptr),
    m_h_tag_jetfitter_w(nullptr),
    m_h_tag_jetfitcomb_w(nullptr),
    m_h_tag_jetfitternn_w(nullptr),
    m_h_tag_jetfitcombnn_w(nullptr),
    m_h_tag_i2d_l2_w(nullptr),
    m_h_tag_i3d_l2_w(nullptr),
    m_h_tag_i2d_ef_w(nullptr),
    m_h_tag_i3d_ef_w(nullptr),
    m_h_tag_i2d_of_w(nullptr),
    m_h_tag_i3d_of_w(nullptr),
    m_h_tag_IPinfo_ntrk(),
    m_h_tag_IPinfo_trkPt(),
    m_h_tag_IPinfo_d0val(),
    m_h_tag_IPinfo_z0val(),
    m_h_tag_IPinfo_d0sig(),
    m_h_tag_IPinfo_z0sig(),
    m_h_tag_IPinfo_weight2D(),
    m_h_tag_IPinfo_weight3D(),
    m_h_tag_IPinfo_piJP(),
    m_h_tag_IPinfo_fromV0(),
    m_h_tag_IPinfo_grade(),
    m_h_perf_b(),
    m_h_perf_u(),
    m_h_perf_upur(),
    m_h_perf_vxeff_den_b(nullptr),
    m_h_perf_vxeff_num_b(nullptr),
    m_h_perf_vxeff_den_u(nullptr),
    m_h_perf_vxeff_num_u(nullptr),
    m_h_perf_vxeff_den_upur(nullptr),
    m_h_perf_vxeff_num_upur(nullptr),
    m_h_perf_rej10(nullptr),
    m_h_perf_rej50(nullptr),
    m_h_perf_rej60(nullptr),
    m_h_perf_rejpur10(nullptr),
    m_h_perf_rejpur50(nullptr),
    m_h_perf_rejpur60(nullptr),
    m_h_perfWt_10(nullptr),
    m_h_perfWt_50(nullptr),
    m_h_perfWt_60(nullptr),
    m_h_perfWt_pur10(nullptr),
    m_h_perfWt_pur50(nullptr),
    m_h_perfWt_pur60(nullptr),
    m_h_perf_b_ET(),
    m_h_perf_b_eta(),
    m_h_perf_b_phi(),
    m_h_perf_u_ET(),
    m_h_perf_u_eta(),
    m_h_perf_u_phi(),
    m_h_perf_upur_ET(),
    m_h_perf_upur_eta(),
    m_h_perf_upur_phi(),
    m_h_eff_b_ET(),
    m_h_eff_b_eta(),
    m_h_eff_b_phi(),
    m_h_rej_u_ET(),
    m_h_rej_u_eta(),
    m_h_rej_u_phi(),
    m_h_rej_upur_ET(),
    m_h_rej_upur_eta(),
    m_h_rej_upur_phi(),
    m_njet(0),
    m_jet_phi(),
    m_jet_eta(),
    m_jet_et(),
    m_jet_flav(),
    m_jet_isol(),
    m_jet_ne(),
    m_jet_eTrueEle(),
    m_jet_eAnyEle(),
    m_jet_eptr(),
    m_jet_ept(),
    m_jet_ea0(),
    m_jet_ew(),
    m_jet_elh(),
    m_jet_epb(),
    m_jet_epu(),
    m_jet_tag_pid(),
    m_jet_tag_mothpid(),
    m_jet_NtrackEle(),
    m_jet_trackElept(),
    m_jet_trackEle(),
    m_jet_Ntrack(),
    m_jet_sumptTrack(),
    m_nelej(),
    m_ntrackj(),
    m_nmuonj(),
    p_nt1(nullptr),
    m_UseTupleSET(false)
{
  declareProperty("JetContainer", m_particleJetContainerName = "Cone4H1TowerJets");
  declareProperty("MCParticleContainer", m_truthParticleContainerName = "SpclMC");
  declareProperty("TrackParticleContainer", m_trackParticleContainerName = "TrackParticleCandidate");
  declareProperty("MuonContainer",m_muonContainerName = "StacoMuonCollection");
  declareProperty("ElectronContainer", m_electronContainerName = "ElectronAODCollection");
  declareProperty("PrimaryVertexContainer", m_primaryVertexContainerName = "VxPrimaryCandidate");
  declareProperty("MCEventContainer", m_mcEventContainerName = "GEN_AOD");
  declareProperty("SelectBFromRun", m_selectBFromRun = 0);
  declareProperty("SelectUFromRun", m_selectUFromRun = 0);
  declareProperty("PurificationDeltaR", m_purificationDeltaR = 0.8);
  declareProperty("LeptonPurificationDeltaR", m_leptonPurificationDeltaR = 0.7);
  declareProperty("UseTupleSET", m_UseTupleSET = false);
  m_tagger[0] = "IP2D";
  m_tagger[1] = "IP3D";
  m_tagger[2] = "SV1";
  m_tagger[3] = "SV2";
  m_tagger[4] = "IP3DSV1";
  m_tagger[5] = "Lifetime2D";
  m_tagger[6] = "SecVtxTagBU";
  m_tagger[7] = "lhSig";
  m_tagger[8] = "SoftMuonTag";
  m_tagger[9] = "SoftElectronTag";
  m_tagger[10] = "JetFitter";
  m_tagger[11] = "JetFitComb";
  m_tagger[12] = "JetFitterNN";
  m_tagger[13] = "JetFitCombNN";
  m_tagger[14] = "JetProb";
  //*
  m_tagger[15] = "Offline_afterHLT_IP2D";
  m_tagger[16] = "Offline_afterHLT_IP3D";
  m_tagger[17] = "L2_IP2D";
  m_tagger[18] = "L2_IP3D";
  m_tagger[19] = "L2_JetProb";
  m_tagger[20] = "EF_afterL2_IP2D";
  m_tagger[21] = "EF_afterL2_IP3D";
  m_tagger[22] = "EF_IP2D";
  m_tagger[23] = "EF_IP3D";
  m_tagger[24] = "EF_JetProb";
  m_tagger[25] = "EF_MVTX";
  m_tagger[26] = "EF_EVTX";
  m_tagger[27] = "EF_NVTX";

  // for histograms as a function of jet ET
  //
  m_jetET[0] = "Et_0_100";
  m_jetET[1] = "Et_100_200";
  m_jetET[2] = "Et_200_300";
  m_jetET[3] = "Et_300_400";
  m_jetET[4] = "Et_400_500";
  m_jetET[5] = "Et_500";

}

JetTagAna::~JetTagAna() {}

StatusCode JetTagAna::initialize() {
  ATH_CHECK( service("THistSvc", m_histos, true) );

  this->bookHistograms();
  return StatusCode::SUCCESS;
}		 

double phiCorr(double phi){
  double newphi = phi;
  if (phi<-M_PI) newphi += 2*M_PI;
  if (phi> M_PI) newphi -= 2*M_PI;
  return newphi;
}

StatusCode JetTagAna::finalize() {
  int nj  = (int)(m_h_jet_ntotal->GetMean()*m_h_jet_ntotal->Integral());
  int njb = (int)(m_h_jet_nlabelb->GetMean()*m_h_jet_nlabelb->Integral());
  int njc = (int)(m_h_jet_nlabelc->GetMean()*m_h_jet_nlabelc->Integral());
  int njt = (int)(m_h_jet_nlabelt->GetMean()*m_h_jet_nlabelt->Integral());
  ATH_MSG_INFO ( "##### APPROX. NB JETS LABELLED AS B: " << njb << " C: " << njc
                 << " TAU: " << njt << " TOTAL: " << nj );
  this->computeRejections();
  return StatusCode::SUCCESS;
}


StatusCode JetTagAna::execute() {

  ATH_MSG_DEBUG ( "JetTagAna::execute()" );
  StatusCode sc = StatusCode::SUCCESS;
  m_h_global_counters->Fill(1.);

  /** Retrieve Event header: */
  const xAOD::EventInfo* eventInfo;
  sc = evtStore()->retrieve(eventInfo);
  m_irun = 0;
  m_ievt = 0;
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve event info" );
  } else {
    m_irun = eventInfo->runNumber();
    m_ievt = eventInfo->eventNumber();
  }
  ATH_MSG_DEBUG ( "--- Run " << m_irun << " Event " << m_ievt );

  /** Primary vertex: */
  double pvx = 0.;
  double pvy = 0.;
  double pvz = 0.;
  const VxContainer* vxContainer(0);
  int npvx = 0;
  sc = evtStore()->retrieve(vxContainer, m_primaryVertexContainerName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve primary vertex info: " << m_primaryVertexContainerName );
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_VERBOSE ( "Found primary vertex info: " << m_primaryVertexContainerName );
    m_h_global_counters->Fill(2.);
    if(vxContainer) {
      npvx = vxContainer->size();
      VxContainer::const_iterator fz = vxContainer->begin();
      const Trk::RecVertex& primaryVertex = (*fz)->recVertex();
      pvx = primaryVertex.position().x();
      pvy = primaryVertex.position().y();
      pvz = primaryVertex.position().z();
    }
  }
  m_h_global_nprimvtx->Fill((float)npvx);
  m_h_global_xprimvtx->Fill(pvx);
  m_h_global_yprimvtx->Fill(pvy);
  m_h_global_zprimvtx->Fill(pvz);
  ATH_MSG_VERBOSE ( "--- Primary vertex: " << pvx << " " << pvy << " " << pvz );
  // Check for undefined PV (when no PV is reconstructed, a dummy one is stored):
  if( pvx == 0 && pvy == 0 && pvz == 0 ) {
    ATH_MSG_WARNING ( "Primary Vertex is (0,0,0): skipping event." );
    return StatusCode::SUCCESS;
  } else {
    m_h_global_counters->Fill(3.);
  }
  /** True primary vertex: */
  double xpvxt = 0.;
  double ypvxt = 0.;
  double zpvxt = 0.;
  const McEventCollection* gen(0);
  sc = evtStore()->retrieve(gen, m_mcEventContainerName);
  if( sc.isFailure() ) {
    ATH_MSG_WARNING ( "MC Event " << m_mcEventContainerName << " not found." );
  } else {
    ATH_MSG_VERBOSE ( "MC Event " << m_mcEventContainerName << " found with " << gen->size() << " entries." );
    const HepMC::GenEvent* genEvent = *(gen->begin());
    if(genEvent) {
      HepMC::GenEvent::vertex_const_iterator vitr = genEvent->vertices_begin();
      xpvxt = (*vitr)->position().x();
      ypvxt = (*vitr)->position().y();
      zpvxt = (*vitr)->position().z(); 
      m_h_global_primvtxresx->Fill(pvx-xpvxt);
      m_h_global_primvtxresy->Fill(pvy-ypvxt);
      m_h_global_primvtxresz->Fill(pvz-zpvxt);
    }
  }

  /** MC Truth container: */
  sc=evtStore()->retrieve( m_mcpartTES, m_truthParticleContainerName);
  int nmcp = 0;
  if( sc.isFailure() ) {
    ATH_MSG_WARNING ( "TruthParticleContainer " << m_truthParticleContainerName 
                      << " not found." );
  } else {
    nmcp = (*m_mcpartTES).size();
    ATH_MSG_VERBOSE ( "TruthParticleContainer " << m_truthParticleContainerName 
                      << " found with " << nmcp << " entries." );
    m_h_global_counters->Fill(5.);
  }
  m_h_global_nmcpart->Fill((float)nmcp);

  // fill some information about b and c particles:
  if(m_mcpartTES) {
   TruthParticleContainer::const_iterator mcItr  = (*m_mcpartTES).begin();
   TruthParticleContainer::const_iterator mcEnd = (*m_mcpartTES).end();
   int nbbq = 0;
   int nbcq = 0;
   int nbbh = 0;
   int nbch = 0;
   for (; mcItr != mcEnd; ++mcItr) {
     int mpdg = (*mcItr)->pdgId();
     double pt = (*mcItr)->pt();
     double eta = (*mcItr)->eta();
     if(5==abs(mpdg)) {
       nbbq++;
       m_h_truth_bquark_pt->Fill(pt);
       m_h_truth_bquark_eta->Fill(eta);
     }
     if(4==abs(mpdg)) {
       nbcq++;
       m_h_truth_cquark_pt->Fill(pt);
       m_h_truth_cquark_eta->Fill(eta);
     }
     if(this->isBHadron(mpdg)) {
       nbbh++;
       m_h_truth_bhadr_pt->Fill(pt);
       m_h_truth_bhadr_eta->Fill(eta);
     }
     if(this->isDHadron(mpdg)) {
       nbch++;
       m_h_truth_chadr_pt->Fill(pt);
       m_h_truth_chadr_eta->Fill(eta);
     }
   }
   m_h_truth_bquark_nb->Fill((float)nbbq);
   m_h_truth_cquark_nb->Fill((float)nbcq);
   m_h_truth_bhadr_nb->Fill((float)nbbh);
   m_h_truth_chadr_nb->Fill((float)nbch);
  }

  /** TrackParticleTruth container: */
  const TrackParticleTruthCollection* tpTruthColl(0);
  sc = evtStore()->retrieve(tpTruthColl,m_TPTruthContainer);
  if (!sc.isFailure() ) {
    ATH_MSG_VERBOSE ( "Number of TruthTrackParticles in event: " 
                      << tpTruthColl->size() );
  } else {
    ATH_MSG_DEBUG ( "Unable to retrieve TrackParticleTruthCollection" );
  }

  /** TrackParticle container: */
  int ntrkp = 0;
  sc=evtStore()->retrieve( m_trackTES, m_trackParticleContainerName);
  if( sc.isFailure() ) {
    ATH_MSG_WARNING ( "TrackParticleContainer " << m_trackParticleContainerName 
                      << " not found." );
  } else {
    m_h_global_counters->Fill(6.);
    ntrkp = (*m_trackTES).size();
    ATH_MSG_VERBOSE ( "TrackParticleContainer " << m_trackParticleContainerName 
                      << " found with " << ntrkp << " entries." );

    /// iterators over the container 
    Rec::TrackParticleContainer::const_iterator trackItr  = m_trackTES->begin();
    Rec::TrackParticleContainer::const_iterator trackItrE = m_trackTES->end();
    for (; trackItr != trackItrE; ++trackItr) {
      const Rec::TrackParticle * trackParticle = (*trackItr);
      if(trackParticle) {
	if(trackParticle->pt()<1000.) continue; 
        const Trk::TrackSummary* summary = trackParticle->trackSummary();
        if (summary) {
	  m_h_global_BLayerHits->Fill((float) summary->get(Trk::numberOfInnermostPixelLayerHits));
	  m_h_global_BLayerSharedHits->Fill((float) summary->get(Trk::numberOfInnermostPixelLayerSharedHits));
	  m_h_global_PixelHits->Fill((float) summary->get(Trk::numberOfPixelHits));
	  m_h_global_PixelLayers->Fill((float) summary->get(Trk::numberOfContribPixelLayers));
	  m_h_global_TRTHits->Fill((float) summary->get(Trk::numberOfTRTHits));
          int nSiHits = summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfSCTHits);
          m_h_global_SiHits->Fill((float) nSiHits);
	}	
      }
    }

  }
  m_h_global_ntrkpart->Fill((float)ntrkp);

  /** Muon Container: */
  const Analysis::MuonContainer* muonTDS=0;
  int nmuon=0;
  sc=evtStore()->retrieve( muonTDS, m_muonContainerName);
  if( sc.isFailure()  ||  !muonTDS ) {
    ATH_MSG_WARNING ( "No AOD muon container of muons found in TDS");
  }  
  else {
    nmuon = (*muonTDS).size();
    ATH_MSG_DEBUG ( "MuonContainer successfully retrieved, size = " << nmuon );
  } 
  m_h_global_nmuon->Fill((float) nmuon);  

  /** Electron Container: */

  const ElectronContainer* electronTDS=0;
  int nelectron=0;
  sc=evtStore()->retrieve( electronTDS, m_electronContainerName);
  if( sc.isFailure()  ||  !electronTDS ) {
    ATH_MSG_WARNING ( "No AOD electron container of electrons found in TDS" );
  }  
  else {
    nelectron = (*electronTDS).size();
    ATH_MSG_DEBUG ( "ElectronContainer successfully retrieved, size = " << nelectron );
  } 
  m_h_global_nelectron->Fill((float) nelectron);  

  /** Jet container: */
  int njtag = 0;
  const JetCollection* jetTES;
  sc=evtStore()->retrieve( jetTES, m_particleJetContainerName);
  if( sc.isFailure() ) {
    ATH_MSG_ERROR ( "JetContainer " << m_particleJetContainerName 
                    << " not found." );
    return StatusCode::SUCCESS;
  } else {  
    njtag = (*jetTES).size();
    ATH_MSG_VERBOSE ( "JetContainer " << m_particleJetContainerName 
                      << " found with " << njtag << " entries." );
    m_h_global_counters->Fill(7.);
  }
  m_h_global_njettag->Fill((float)njtag);

  // --- iterate over the Jet container 
  JetCollection::const_iterator jetItr = (*jetTES).begin();
  JetCollection::const_iterator jetEnd = (*jetTES).end();
  int ntotal = 0;
  int nlabelb = 0;
  int nlabelc = 0;
  int nlabelt = 0;
  int icount =0;
  for (; jetItr != jetEnd; ++jetItr) {
    CLHEP::HepLorentzVector p4((*jetItr)->px(),
			(*jetItr)->py(),
			(*jetItr)->pz(),
			(*jetItr)->e());
    int ntag = (*jetItr)->jetTagInfoVector().size();
    m_h_jet_ntag->Fill((float)ntag);
    ntotal++;

    // --- get the true label of the jet from MC Truth: SECTION::LABEL
    std::string label("N/A");
    const Analysis::TruthInfo* mcinfo = (*jetItr)->tagInfo<Analysis::TruthInfo>("TruthInfo");
    if(mcinfo) {
      label = mcinfo->jetTruthLabel();
    } else {
      ATH_MSG_VERBOSE ( "could not find TruthInfo for matching jet" );
    }
    int iflav(0);
    if(label=="B") {
      iflav = 5;
      nlabelb++;
    }
    if(label=="C") {
      iflav = 4;
      nlabelc++;
    }
    if(label=="T") {
      iflav = 15;
      nlabelt++;
    }
    m_h_jet_label->Fill((float)iflav);
    // for isolation/purification:
    double dRminToB = mcinfo->deltaRMinTo("B");
    double dRminToC = mcinfo->deltaRMinTo("C");
    double dRminToT = mcinfo->deltaRMinTo("T");

    // --- get jet basic kinematics:
    ATH_MSG_VERBOSE ( "BJet # " << icount << " Eta= " << p4.pseudoRapidity()
                      << " Phi= " << p4.phi() << " pT= " << p4.perp()
                      << "  #Tags= " << ntag << " MCLabel= " << label  );
    icount++;
    m_h_jet_eta->Fill(p4.pseudoRapidity());
    m_h_jet_phi->Fill(p4.phi());
    m_h_jet_et->Fill(p4.et());

    // --- get some information about Track/Muon/Electron Constituents

    int ntrk_j=0, nmu_j=0, nel_j=0;
    const Analysis::TrackAssociation* ta = (*jetItr)->getAssociation<Analysis::TrackAssociation>("Tracks");
    if(ta) ntrk_j = ta->size();
    const Analysis::MuonAssociation* ma = (*jetItr)->getAssociation<Analysis::MuonAssociation>("Muons");
    if(ma) nmu_j = ma->size();
    const Analysis::ElectronAssociation* ea = (*jetItr)->getAssociation<Analysis::ElectronAssociation>("Electrons");
    if(ea) nel_j = ea->size();
    m_h_jet_ntracks->Fill((float) ntrk_j);
    m_h_jet_nmuons->Fill((float) nmu_j);
    m_h_jet_nelectrons->Fill((float) nel_j);

    // loop over tracks in a jet

    if(ta) {

      std::vector<const Rec::TrackParticle*>* trackVector = ta->tracks();
      for(std::vector<const Rec::TrackParticle*>::iterator trkItr = trackVector->begin(); trkItr !=trackVector->end(); ++trkItr) {


	const Rec::TrackParticle* aTemp = *trkItr;

	m_h_jet_tracks_pt->Fill(aTemp->pt()/GeV);

       const Trk::TrackSummary* summary = aTemp->trackSummary();
       if (summary) {
         m_h_jet_tracks_BLayerHits->Fill((float) summary->get(Trk::numberOfInnermostPixelLayerHits));
         m_h_jet_tracks_PixelHits->Fill((float) summary->get(Trk::numberOfPixelHits));
         m_h_jet_tracks_SCTHits->Fill((float) summary->get(Trk::numberOfSCTHits));
       }
      }     

    }

    // loop over muons in a jet
    if(ma) {
      for(Navigable<Analysis::MuonContainer,double>::object_iter it=ma->begin(); it !=ma->end(); ++it) {
        const Analysis::Muon *m = (*it);
        m_h_jet_muons_pt->Fill(m->pt()/GeV);
      }
    }
    // loop over electrons in jet
    if(ea) {
      for(Navigable<ElectronContainer,double>::object_iter it=ea->begin(); it !=ea->end(); ++it) {
        const Analysis::Electron *e = (*it);
        m_h_jet_electrons_pt->Fill(e->pt()/GeV);
      }
    }

    // --- get btagging weights: SECTION::WEIGHTS
    double w_ip2d = (*jetItr)->getFlavourTagWeight("IP2D");
    double w_ip3d = (*jetItr)->getFlavourTagWeight("IP3D");
    double w_sv1  = (*jetItr)->getFlavourTagWeight("SV1");
    double w_sv2  = (*jetItr)->getFlavourTagWeight("SV2");
    double w_cmb  = (*jetItr)->getFlavourTagWeight(); // combination of IP3D and SV1
    double w_lf2d = (*jetItr)->getFlavourTagWeight("LifetimeTag2D");
    double w_svbu = (*jetItr)->getFlavourTagWeight("SecVtxTagBU");
    double w_lhsig= (*jetItr)->getFlavourTagWeight("lhSig");
    double w_softm= (*jetItr)->getFlavourTagWeight("SoftMuonTag");
    double w_softe= (*jetItr)->getFlavourTagWeight("SoftElectronTag");
    double w_jetfit = (*jetItr)->getFlavourTagWeight("JetFitterTag");
    double w_jetfitcomb = (*jetItr)->getFlavourTagWeight("JetFitterCOMB");
    double w_jetfitnn = (*jetItr)->getFlavourTagWeight("JetFitterTagNN");
    double w_jetfitcombnn = (*jetItr)->getFlavourTagWeight("JetFitterCOMBNN");
    double w_jetprob = (*jetItr)->getFlavourTagWeight("JetProb");
    ATH_MSG_VERBOSE ( "-> weights: "
                      << " JetProb: " << w_jetprob
                      << " IP2D: " << w_ip2d
                      << " IP3D: " << w_ip3d
                      << " SV1 : " << w_sv1
                      << " SV2 : " << w_sv2
                      << " SV1+IP3D : " << w_cmb
                      << " | " 
                      << " Lifetime2D : " << w_lf2d
                      << " SecVtxTagBU : " << w_svbu
                      << " lhSig : " << w_lhsig
                      << " | " 
                      << " SoftMuonTag : " << w_softm
                      << " SoftElectronTag : " << w_softe
                      << " | " 
                      << " JetFitterTag : "<< w_jetfit
                      << " JetFitterCOMB : "<< w_jetfitcomb
                      << " JetFitterTagNN : "<< w_jetfitnn
                      << " JetFitterCOMBNN : "<< w_jetfitcombnn
                      );
    m_h_tag_jetprob_w->Fill(w_jetprob);
    m_h_tag_i2d_w->Fill(w_ip2d);
    m_h_tag_i3d_w->Fill(w_ip3d);
    m_h_tag_sv1_w->Fill(w_sv1);
    m_h_tag_sv2_w->Fill(w_sv2);
    m_h_tag_cmb_w->Fill(w_cmb);
    m_h_tag_lf2d_w->Fill(w_lf2d);
    m_h_tag_svbu_w->Fill(w_svbu);
    m_h_tag_lhsig_w->Fill(w_lhsig);
    m_h_tag_softm_w->Fill(w_softm);
    m_h_tag_softe_w->Fill(w_softe);
    m_h_tag_jetfitter_w->Fill(w_jetfit);
    m_h_tag_jetfitcomb_w->Fill(w_jetfitcomb);
    m_h_tag_jetfitternn_w->Fill(w_jetfitnn);
    m_h_tag_jetfitcombnn_w->Fill(w_jetfitcombnn);
    double w[MAX_numTaggers];
    w[0] = w_ip2d;
    w[1] = w_ip3d;
    w[2] = w_sv1;
    w[3] = w_sv2;
    w[4] = w_cmb;
    w[5] = w_lf2d;
    w[6] = w_svbu;
    w[7] = w_lhsig;
    w[8] = w_softm;
    w[9] = w_softe;
    w[10] = w_jetfit;
    w[11] = w_jetfitcomb;
    w[12] = w_jetfitnn;
    w[13] = w_jetfitcombnn;
    w[14] = -TMath::Log10(w_jetprob);

    bool svok = false;
    // --- loop on tags & get detailed information for each tag: SECTION::DETAILS
    std::vector<const JetTagInfoBase*> infoVector = (*jetItr)->jetTagInfoVector();
    for(uint iInfo = 0; iInfo < infoVector.size(); iInfo++) {
      if(0==infoVector[iInfo]) continue;
      bool is2D = (infoVector[iInfo]->infoType()=="IP2D");
      bool is3D = (infoVector[iInfo]->infoType()=="IP3D");
      bool isIPlus = (infoVector[iInfo]->infoType()=="IPInfoPlus");
      bool isS1 = (infoVector[iInfo]->infoType()=="SV1");
      bool isS2 = (infoVector[iInfo]->infoType()=="SV2");
      bool isSPlus = (infoVector[iInfo]->infoType()=="SVInfoPlus");
      bool isSoftElectron = (infoVector[iInfo]->infoType()=="SoftElectronTag");

      // ...... detailed information for impact parameter-based informations ......
      if(is2D) { // impact parameter 2D
	// basic additional information:
	const Analysis::IPInfoBase* infob = dynamic_cast<const Analysis::IPInfoBase*>(infoVector[iInfo]);
	if(infob) {
	  int ntrk = infob->nbTracks();            // number of tracks used for tagging in the jet
	  double pb = infob->tagLikelihood()[0];   // b likelihood
	  double pu = infob->tagLikelihood()[1];   // u likelihood
	  m_h_tag_i2d_n->Fill((float)ntrk);
	  m_h_tag_i2d_b->Fill(pb);
	  m_h_tag_i2d_u->Fill(pu);
          ATH_MSG_VERBOSE ( "-> InfoBase for IP2D:  #tracks= " << ntrk
                            << " Pb= " << pb << " Pu= " << pu );
	}
      }
      if(is3D) { // impact parameter 3D
	// basic additional information:
	const Analysis::IPInfoBase* infob = dynamic_cast<const Analysis::IPInfoBase*>(infoVector[iInfo]);
	if(infob) {
	  int ntrk = infob->nbTracks();            // number of tracks used for tagging in the jet
	  double pb = infob->tagLikelihood()[0];   // b likelihood
	  double pu = infob->tagLikelihood()[1];   // u likelihood
	  m_h_tag_i3d_n->Fill((float)ntrk);
	  m_h_tag_i3d_b->Fill(pb);
	  m_h_tag_i3d_u->Fill(pu);
          ATH_MSG_VERBOSE ( "-> InfoBase for IP3D:  #tracks= " << ntrk
                            << " Pb= " << pb << " Pu= " << pu );
	}
      }
      if(isIPlus) {
	// more detailed information (not always present in any AOD)::
	const Analysis::IPInfoPlus* infop = dynamic_cast<const Analysis::IPInfoPlus*>(infoVector[iInfo]);
	if(infop) {
	  int ntrk = infop->numTrackInfo();
          ATH_MSG_VERBOSE ( "-> InfoPlus for each track in IP2D/IP3D/JetProb:  #tracks= " 
                            << ntrk );
          int ibin;
          ibin = (int) ((p4.et())/GeV)/100;
          if(ibin>5) ibin=5;
          m_h_tag_IPinfo_ntrk[ibin]->Fill((float) ntrk);
	  for(int itinf = 0; itinf < ntrk; itinf++) {
	    Analysis::IPTrackInfo trackInfo = infop->getTrackInfo(itinf);
	    // impact parameters w.r.t. primary vertex: this information is stored 
	    // to avoid calling again the extrapolator which requires the full geometry initialization
	    double d0val = trackInfo.d0Value();
	    double d0sig = trackInfo.d0Significance();
	    double z0val = trackInfo.z0Value();
	    double z0sig = trackInfo.z0Significance();
	    m_h_tag_i2d_sig->Fill(d0sig);
	    m_h_tag_i3d_sig->Fill(z0sig);
	    // individual contribution of this track to the b-tagging weights:
	    double wi2D = trackInfo.trackWeight2D(); // IP2D
	    double wi3D = trackInfo.trackWeight3D(); // IP3D
	    double piJP = trackInfo.trackProbJP();   // JetProb
	    // track quality information:
	    Analysis::TrackGrade grd = trackInfo.trackGrade(); // Good, Shared, ...
	    bool vzero = trackInfo.isFromV0(); // track from V0, interaction in identified material, ...
	    // pointer to actual TrackParticle:
	    const Rec::TrackParticle* trk = trackInfo.track();
            //
            // fill some histograms as a func of jet ET
            //
            m_h_tag_IPinfo_d0val[ibin]->Fill(d0val);
            m_h_tag_IPinfo_z0val[ibin]->Fill(z0val);
            m_h_tag_IPinfo_d0sig[ibin]->Fill(d0sig);
            m_h_tag_IPinfo_z0sig[ibin]->Fill(z0sig);
            m_h_tag_IPinfo_weight2D[ibin]->Fill(wi2D);
            m_h_tag_IPinfo_weight3D[ibin]->Fill(wi3D);
            m_h_tag_IPinfo_piJP[ibin]->Fill(piJP);
            m_h_tag_IPinfo_fromV0[ibin]->Fill((float) vzero);
            m_h_tag_IPinfo_grade[ibin]->Fill((float) grd);

	    // dump:
	    ATH_MSG_VERBOSE ( "    --> track " << itinf
	         << " d0= " << d0val << " Signif(d0)= " << d0sig
	         << " z0= " << z0val << " Signif(z0)= " << z0sig
		 << " weight2D= " << wi2D
		 << " weight3D= " << wi3D
		 << " probJetProb= " << piJP
                              << " grade= " << grd.gradeString()
                              << (vzero ? " fromV0 " : "") );
	    if(trk) {
	      ATH_MSG_VERBOSE ( " track pT= " << trk->pt()
                                << " eta= " << trk->eta()
                                << " phi= " << trk->phi() );
	    }
	  }
	}
      }
      // ...... detailed information for secondary vertex-based informations ......
      if(isS1) {
	// basic additional information:
	const Analysis::SVInfoBase* info = dynamic_cast<const Analysis::SVInfoBase*>(infoVector[iInfo]);
	if(info) {
	  double pb = info->tagLikelihood()[0];
	  double pu = info->tagLikelihood()[1];
	  m_h_tag_sv1_b->Fill(pb);
	  m_h_tag_sv1_u->Fill(pu);
          ATH_MSG_VERBOSE ( "-> InfoBase for SV1: " 
                            << " Pb= " << pb << " Pu= " << pu );
	}
      }
      if(isS2) {
	// basic additional information:
	const Analysis::SVInfoBase* info = dynamic_cast<const Analysis::SVInfoBase*>(infoVector[iInfo]);
	if(info) {
	  double pb = info->tagLikelihood()[0];
	  double pu = info->tagLikelihood()[1];
	  m_h_tag_sv2_b->Fill(pb);
	  m_h_tag_sv2_u->Fill(pu);
          ATH_MSG_VERBOSE ( "-> InfoBase for SV2: " 
                            << " Pb= " << pb << " Pu= " << pu );
	}
      }
      if(isSPlus) {
	// more detailed information (not always present in any AOD)::
	const Analysis::SVInfoPlus* info = dynamic_cast<const Analysis::SVInfoPlus*>(infoVector[iInfo]);
	if(info) {
	  int ntrk = info->getNGTrackInSvx();       // number of tracks in vertex
	  double mass = info->getMass();            // mass of secondary vertex
	  int n2t = info->getN2T();                 // number of two-track vertices
	  svok = (mass>0. && n2t>0 );
	  double efrc = info->getEnergyFraction();  // energy fraction svx/jet
	  m_h_tag_sv_n->Fill((float)ntrk);
	  m_h_tag_sv_n2t->Fill((float)n2t);
	  m_h_tag_sv_frc->Fill(efrc);
	  m_h_tag_sv_m->Fill(mass);
          ATH_MSG_VERBOSE ( "-> InfoPlus for SV1/SV2: " 
                            << " #tracks= " << ntrk
                            << " mass= " << mass
                            << " #n2t= " << n2t
                            << " efrac= " << efrc
                            << " tagOK= " << svok );
	}
      }
     // soft electron b-tagging
      if (isSoftElectron) {
	// basic additional information:
	const Analysis::SoftElectronInfo* info = dynamic_cast<const Analysis::SoftElectronInfo*>(infoVector[iInfo]);
	if(info) {
	  checkSoftElectron(infoVector,iInfo,ntotal,p4,iflav,ea,
			    ntrk_j,nmu_j,electronTDS,tpTruthColl);

	  double pb = info->tagLikelihood()[0];
	  double pu = info->tagLikelihood()[1];
	  //m_h_tag_softe_b->Fill(pb);
	  //m_h_tag_softe_u->Fill(pu);
          ATH_MSG_VERBOSE ( "-> InfoBase for SET: " 
                            << " Pb= " << pb << " Pu= " << pu );
	}
      }
    } // loop on tags

    /////////////////////
    //* HLT b-tagging *//
    /////////////////////

    double deltaR = 9999, phiRoI = -9999, etaRoI = -9999, etRoI = -9999;
 
    double w_ip2d_l2 = -1,  w_ip3d_l2 = -1;
    double w_ip2d_ef = -1,  w_ip3d_ef = -1;
    double w_ip2d_of = -21, w_ip3d_of = -21;

    double w_jetProb_l2 = -1, w_jetProb_ef = -1;
    double w_mVtx_ef = -1, w_eVtx_ef = -1, w_nVtx_ef = -1;

    bool isFoundL2 = true;

    //* Retrieve from StoreGate TrigL2BjetContainer *//
    const DataHandle<TrigL2BjetContainer> trigL2Bjet;
    const DataHandle<TrigL2BjetContainer> lastTrigL2Bjet;
  
    sc = evtStore()->retrieve(trigL2Bjet,lastTrigL2Bjet);
    if (sc.isSuccess()) {

      //* Loop on TrigL2BjetContainer *//
      for (int j=1; trigL2Bjet != lastTrigL2Bjet; ++trigL2Bjet, ++j) {
      
	ATH_MSG_VERBOSE ( "Looking at TrigL2BjetContainer " << j );
    
	TrigL2BjetContainer::const_iterator pL2BjetItr    = trigL2Bjet->begin();
	TrigL2BjetContainer::const_iterator lastL2BjetItr = trigL2Bjet->end();
      
	//* Loop on TrigL2Bjet (expected one per collection) *//
	for (int k=1; pL2BjetItr != lastL2BjetItr; ++pL2BjetItr, ++k ) {
	
	  ATH_MSG_VERBOSE ( "Looking at TrigL2Bjet " << k << "/" << trigL2Bjet->size() );
	
	  ATH_MSG_VERBOSE
            ( "TrigL2Bjet->xIP1D() = " << (*pL2BjetItr)->xIP1D()
	       << "; TrigL2Bjet->xIP2D() = " << (*pL2BjetItr)->xIP2D()
	       << "; TrigL2Bjet->xIP3D() = " << (*pL2BjetItr)->xIP3D() 
	       << "; TrigL2Bjet->xCHI2() = " << (*pL2BjetItr)->xCHI2() 
              );
	
	  //* eta/phi matching w.r.t. offline b-jet *//
	  double dR = sqrt(pow((*jetItr)->eta() - (*pL2BjetItr)->eta(),2) + pow(phiCorr(phiCorr((*jetItr)->phi()) - phiCorr((*pL2BjetItr)->phi())),2));
	
	  if (dR < deltaR) {
	    deltaR = dR;
	    
	    w_ip2d_l2  = (double)(*pL2BjetItr)->xIP2D();
	    w_ip3d_l2  = (double)(*pL2BjetItr)->xIP3D();
	    
	    w_jetProb_l2 = (double)(*pL2BjetItr)->xCHI2();

	    phiRoI = (*pL2BjetItr)->phi();
	    etaRoI = (*pL2BjetItr)->eta();
	  }
	}
      }
    
      ATH_MSG_DEBUG ( "deltaR L2 = " << deltaR );

    } else {

      isFoundL2 = false;
      ATH_MSG_DEBUG ( "No TrigL2BjetContainer found" );

    }

    if (deltaR > 0.1) {
      isFoundL2 = false;
      w_ip2d_l2 = -1; w_ip3d_l2 = -1;
      ATH_MSG_DEBUG ( "TrigL2BjetContainer not matched with offline" );
    }
  
    if (isFoundL2) {

      //If no EF found the chain stopped at L2..
      w_ip2d_ef = 0, w_ip3d_ef = 0; 
      w_jetProb_ef = 0;
      w_mVtx_ef = 0, w_eVtx_ef = 0, w_nVtx_ef = 0;
      deltaR = 9999;
    
      //* Retrieve from StoreGate TrigEFBjetContainer *//
      const DataHandle<TrigEFBjetContainer> trigEFBjet;
      const DataHandle<TrigEFBjetContainer> lastTrigEFBjet;
    
      sc = evtStore()->retrieve(trigEFBjet,lastTrigEFBjet);
      if (sc.isSuccess()) {

	//* Loop on TrigEFBjetContainer *//
	for (int i=0; trigEFBjet != lastTrigEFBjet; ++trigEFBjet, ++i) {
	  
	  ATH_MSG_VERBOSE ( "Looking at TrigEFBjetContainer " << i );
	  
	  TrigEFBjetContainer::const_iterator pEFBjetItr    = trigEFBjet->begin();
	  TrigEFBjetContainer::const_iterator lastEFBjetItr = trigEFBjet->end();

	  //* Loop on TrigEFBjet (expected one per collection) *//    	
	  for (int j=0; pEFBjetItr != lastEFBjetItr; ++pEFBjetItr, ++j ) {
	    
	    ATH_MSG_VERBOSE ( "Looking at TrigEFBjet " << j );
	    
	    ATH_MSG_VERBOSE
              ( "TrigEFBjet->xIP1D() = " << (*pEFBjetItr)->xIP1D()
		 << "; TrigEFBjet->xIP2D() = " << (*pEFBjetItr)->xIP2D()
		 << "; TrigEFBjet->xIP3D() = " << (*pEFBjetItr)->xIP3D() 
		 << "; TrigEFBjet->xCHI2() = " << (*pEFBjetItr)->xCHI2() 
		 << "; TrigEFBjet->xMVtx() = " << (*pEFBjetItr)->xMVtx()
		 << "; TrigEFBjet->xEVtx() = " << (*pEFBjetItr)->xEVtx()
		 << "; TrigEFBjet->xNVtx() = " << (*pEFBjetItr)->xNVtx() 
                );
	    
	    //* eta/phi matching w.r.t. LVL2 b-jet *//	  
	    double dR = sqrt(pow(etaRoI - (*pEFBjetItr)->eta(),2) + pow(phiCorr(phiCorr(phiRoI) - phiCorr((*pEFBjetItr)->phi())),2));
	    
	    if (dR < deltaR){
	      deltaR = dR;
	      
	      w_ip2d_ef = (*pEFBjetItr)->xIP2D();
	      w_ip3d_ef = (*pEFBjetItr)->xIP3D();

	      w_jetProb_ef = (double)(*pEFBjetItr)->xCHI2();

	      w_mVtx_ef = (double)(*pEFBjetItr)->xMVtx();
	      w_eVtx_ef = (double)(*pEFBjetItr)->xEVtx();
	      w_nVtx_ef = (double)(*pEFBjetItr)->xNVtx();
	    }
	  }
	}
  
	ATH_MSG_DEBUG ( "deltaR EF = " << deltaR << " " << w_ip3d_l2 );
      
	if (deltaR > 0.001) {
	  w_ip2d_ef = -1; w_ip3d_ef = -1;
	  w_jetProb_ef = -1;
	  w_mVtx_ef = -1, w_eVtx_ef = -1, w_nVtx_ef = -1;
	  ATH_MSG_DEBUG ( "TrigEFBjetContainer found but not matched with offline (should never happen)" );
	}
      } else {
	ATH_MSG_INFO ( "TrigEFBjetContainer not found" );
      }
   
      deltaR = 9999;

      //* Retrieve from StoreGate LVL1 RoIs *//
      const LVL1_ROI* lvl1RoI = 0; 
      sc = evtStore()->retrieve(lvl1RoI);
      
      if (sc.isFailure() || !lvl1RoI) {
	ATH_MSG_WARNING ( "No L1RoI found in TDS" );
	return StatusCode::SUCCESS;
      } else {
	ATH_MSG_DEBUG ( "L1RoI successfully retrieved in TDS" );
	
	LVL1_ROI::jets_type::const_iterator pL1Jet    = (lvl1RoI->getJetROIs()).begin();
	LVL1_ROI::jets_type::const_iterator lastL1Jet = (lvl1RoI->getJetROIs()).end();
	
	ATH_MSG_VERBOSE ( "get L1 Jet size : " << (lvl1RoI->getJetROIs()).size() );
	
	for (int i=1; pL1Jet != lastL1Jet; ++pL1Jet, ++i) {
	  
	  ATH_MSG_VERBOSE ( "Looking at LVL1 RoI " << i << "/" << (lvl1RoI->getJetROIs()).size() );
	  ATH_MSG_VERBOSE ( "L1 ET8x8 : " << (*pL1Jet).getET8x8() );
	  
	  if ((*pL1Jet).getET8x8() <= 18000) continue;
	  
	  double dR = sqrt(pow((*pL1Jet).eta() - etaRoI,2) + pow(phiCorr(phiCorr((*pL1Jet).phi()) - phiCorr(phiRoI)),2));
	
	  if (dR < deltaR) {
	    deltaR = dR;  
	    etRoI = (*pL1Jet).getET8x8();
	  }
	}
	
	ATH_MSG_DEBUG ( "deltaR L1 = " << deltaR );
      }
    }

    bool jetRoImatched=false;
    //* IP2D offline weight if b-jet has been selected online *// 
    if (w_ip2d_l2 != -1 || w_ip2d_ef != -1) {
      if (w_ip2d_l2 >= 0.57 && w_ip2d_ef >= 0.33)
	w_ip2d_of = (*jetItr)->getFlavourTagWeight("IP2D");
      else 
	w_ip2d_of = -10;
      jetRoImatched=true;
    }

    //* IP3D offline weight if b-jet has been selected online *// 
    if (w_ip3d_l2 != -1 || w_ip3d_ef != -1) {
      if (w_ip3d_l2 >= 0.57 && w_ip3d_ef >= 0.33)
	w_ip3d_of = (*jetItr)->getFlavourTagWeight("IP3D");
      else 
	w_ip3d_of = -10;
      jetRoImatched=true;
    }

    double w_ip2d_ef_afterl2=w_ip2d_ef;
    double w_ip3d_ef_afterl2=w_ip3d_ef;
    if (jetRoImatched){
      if (w_ip2d_l2 <= 0.57)
	  w_ip2d_ef_afterl2 = 0;
      if (w_ip3d_l2 <= 0.57)
	  w_ip3d_ef_afterl2 = 0;
    }

    if (jetRoImatched){
      w[15] = w_ip2d_of;
      w[16] = w_ip3d_of;
      w[17] = w_ip2d_l2;
      w[18] = w_ip3d_l2;
      w[19] = w_jetProb_l2;
      w[20] = w_ip2d_ef_afterl2;
      w[21] = w_ip3d_ef_afterl2;
      w[22] = w_ip2d_ef;
      w[23] = w_ip3d_ef;
      w[24] = w_jetProb_ef;
      w[25] = w_mVtx_ef;
      w[26] = w_eVtx_ef;
      w[27] = w_nVtx_ef;
    }

    ////////////////////////////
    //* end of HLT b-tagging *//
    ////////////////////////////

    // --- fill histos for computing efficiency & rejections:
    // use only jets within ID acceptance and with pT>15 GeV and away from high-pt leptons
    bool leptonPur = isJetFarFromHPTLepton(*jetItr);
    if(fabs(p4.pseudoRapidity())<2.5&&p4.et()>15000.&&leptonPur) {

      // b-jets:
      if(5==iflav && ( m_selectBFromRun==0 || m_selectBFromRun==m_irun)) {
	for(int i=0;i<MAX_numTaggers;i++) {
          if ( i<MAX_numTaggers-MAX_numHLTTaggers || jetRoImatched ){
	    m_h_perf_b[i]->Fill(w[i]);
	  }
	}
	m_h_perf_vxeff_den_b->Fill(0.5);
	if(svok) m_h_perf_vxeff_num_b->Fill(0.5);

	// fill histograms for weights vs. ET, eta and phi
	// will use them to calculate effs and rejs. as a function of these variables
	// VJ Apr 13'2007
	for(int i=0;i<(MAX_numTaggers-MAX_numHLTTaggers)+2;i++) {
	  m_h_perf_b_ET[i]->Fill(p4.et()/GeV,w[i]);
	  m_h_perf_b_eta[i]->Fill(p4.pseudoRapidity(),w[i]);
	  m_h_perf_b_phi[i]->Fill(p4.phi(),w[i]);
	}
	//*
	for(int i=(MAX_numTaggers-MAX_numHLTTaggers)+2;i<MAX_numTaggers;i++) {
          if (jetRoImatched){
	    m_h_perf_b_ET[i]->Fill(etRoI/GeV,w[i]);
	    m_h_perf_b_eta[i]->Fill(etaRoI,w[i]);
	    m_h_perf_b_phi[i]->Fill(phiRoI,w[i]);
	  }
	}
      }

      // light jets:
      if(0==iflav && ( m_selectUFromRun==0 || m_selectUFromRun==m_irun)) {
	for(int i=0;i<MAX_numTaggers;i++) {
          if ( i<MAX_numTaggers-MAX_numHLTTaggers || jetRoImatched ){
	    m_h_perf_u[i]->Fill(w[i]);
	  }
	}
	m_h_perf_vxeff_den_u->Fill(0.5);
	if(svok) m_h_perf_vxeff_num_u->Fill(0.5);

	// fill histograms for weights vs. ET, eta and phi
	// will use them to calculate effs and rejs. as a function of these variables
	for(int i=0;i<(MAX_numTaggers-MAX_numHLTTaggers)+2;i++) {
	  m_h_perf_u_ET[i]->Fill(p4.et()/GeV,w[i]);
	  m_h_perf_u_eta[i]->Fill(p4.pseudoRapidity(),w[i]);
	  m_h_perf_u_phi[i]->Fill(p4.phi(),w[i]);
	}
	//*
	for(int i=(MAX_numTaggers-MAX_numHLTTaggers)+2;i<MAX_numTaggers;i++) {
          if (jetRoImatched){
	    m_h_perf_u_ET[i]->Fill(etRoI/GeV,w[i]);
	    m_h_perf_u_eta[i]->Fill(etaRoI,w[i]);
	    m_h_perf_u_phi[i]->Fill(phiRoI,w[i]);
	  }
	}

	// light jets away from lifetime particles (purification)
	if( this->isLightJetIsolated(p4) || // recomputed on truth particles
            (                               // from pre-filled distances
              dRminToB > m_purificationDeltaR &&
              dRminToC > m_purificationDeltaR &&
              dRminToT > m_purificationDeltaR 
            )
          ) {
	  for(int i=0;i<MAX_numTaggers;i++) {
	    if ( i<MAX_numTaggers-MAX_numHLTTaggers || jetRoImatched ){
	      m_h_perf_upur[i]->Fill(w[i]);
	    }
	  }
	  m_h_perf_vxeff_den_upur->Fill(0.5);
	  if(svok) m_h_perf_vxeff_num_upur->Fill(0.5);

	  // fill histograms for weights vs. ET, eta and phi
	  // will use them to calculate effs and rejs. as a function of these variables
	  for(int i=0;i<(MAX_numTaggers-MAX_numHLTTaggers)+2;i++) {
	    m_h_perf_upur_ET[i]->Fill(p4.et()/GeV,w[i]);
	    m_h_perf_upur_eta[i]->Fill(p4.pseudoRapidity(),w[i]);
	    m_h_perf_upur_phi[i]->Fill(p4.phi(),w[i]);
	  }
	  //*
	  for(int i=(MAX_numTaggers-MAX_numHLTTaggers)+2;i<MAX_numTaggers;i++) {
            if (jetRoImatched){
	      m_h_perf_upur_ET[i]->Fill(etRoI/GeV,w[i]);
	      m_h_perf_upur_eta[i]->Fill(etaRoI,w[i]);
	      m_h_perf_upur_phi[i]->Fill(phiRoI,w[i]);
	    }
	  }
	}
      }
    }
  } // end loop jets

  m_h_jet_ntotal->Fill((float)ntotal);
  m_h_jet_nlabelb->Fill((float)nlabelb);
  m_h_jet_nlabelc->Fill((float)nlabelc);
  m_h_jet_nlabelt->Fill((float)nlabelt);

  // for ntuple
  m_njet = ntotal;
  // Fill ntuple //
  if (m_UseTupleSET) p_nt1->Fill();

  return StatusCode::SUCCESS;
}

// ============================================================
void
JetTagAna::checkSoftElectron(const std::vector<const JetTagInfoBase*> infoVector, uint iInfo, int ntotal, 
                             CLHEP::HepLorentzVector p4, int iflav, 
                             const Analysis::ElectronAssociation *ea, int ntrk_j, int nmu_j, 
                             const ElectronContainer* electronTDS, 
                             const TrackParticleTruthCollection* tpTruthColl) 
{
  //
  // check information for soft electron b-tagging
  // in particular look for jets which indeed contain 
  // an electron with good quality track in order 
  // to estimate performance only on this subset
  // add information in a tuple
  //
  
  const Analysis::SoftElectronInfo* info = dynamic_cast<const Analysis::SoftElectronInfo*>(infoVector[iInfo]);
  
  if (!info) return;
  if (!electronTDS) return;

  ATH_MSG_DEBUG ( name() << " SoftElectronTag info ! " 
                  << iInfo << " "  << infoVector[iInfo] 
                  << " " << infoVector[iInfo]->infoType() << " " << ntotal );
  
  // ntuple variables initialisation 
  m_jet_eptr[ntotal-1]        = 0;
  m_jet_ea0[ntotal-1]         = 0;
  m_jet_tag_pid[ntotal-1]     = 0;
  m_jet_tag_mothpid[ntotal-1] = 0;
  
  m_jet_eTrueEle[ntotal-1]     = 0 ;
  m_jet_eAnyEle[ntotal-1]      = 0 ;
  m_jet_ne[ntotal-1]           = 0 ;
  m_jet_epb[ntotal-1]          = 0.;
  m_jet_epu[ntotal-1]          = 0.;
  m_jet_elh[ntotal-1]          = -40.;
  m_jet_ew[ntotal-1]           = -40.;
  m_jet_Ntrack[ntotal-1]       = 0 ;
  m_jet_sumptTrack[ntotal-1]   = 0 ;
  m_jet_NtrackEle[ntotal-1]    = 0 ; 
  
  for (int i=0; i<10;i++) { //fix me! hardcoded number
    m_jet_trackEle[ntotal-1][i]   = 0;
    m_jet_trackElept[ntotal-1][i] = 0;
  }
  
  int jetele    = 0;
  int jetanyele = 0;
  
  for (int ii=0; ii<  m_maxjet   ;ii++) {
    m_jet_eTrueEle[ii]= 0;
    m_jet_eAnyEle[ii] = 0;
  }
  
  m_jet_phi[ntotal-1]  = (float)p4.phi();
  m_jet_eta[ntotal-1]  = p4.pseudoRapidity();
  m_jet_et[ntotal-1]   = p4.et();
  m_jet_flav[ntotal-1] = iflav;
  if( this->isLightJetIsolated(p4)) m_jet_isol[ntotal-1]=1;
  else                              m_jet_isol[ntotal-1]=0;
  
  if(ea) m_nelej[ntotal-1]  = ea->size();
  else   m_nelej[ntotal-1]  = 0;
  
  m_ntrackj[ntotal-1]       = ntrk_j;
  m_nmuonj[ntotal-1]        = nmu_j;

  unsigned int ntrkp = info->numTrackInfo();
  //mlog << MSG::INFO << "N good ele " << ntrkp << endmsg;
  
  m_jet_ne[ntotal-1] = ntrkp; 
  m_jet_ew[ntotal-1] = info->weight(); // softe standard weight
  
  // get total LH
  if (info->tagLikelihood().size() >= 2) {
    m_jet_epb[ntotal-1] = info->tagLikelihood()[0];//sig part of J-B LH
    m_jet_epu[ntotal-1] = info->tagLikelihood()[1];//bkg part of J-B LH
    m_jet_elh[ntotal-1]=getTotWeight(m_jet_epb[ntotal-1],m_jet_epu[ntotal-1]);
  }
  ATH_MSG_VERBOSE ( "Elec xcheck " << m_jet_ew[ntotal-1] 
                    << " " << m_jet_elh[ntotal-1]);

  // Flag the jet as containing ELECTRON if DR <= 0.4
  // and e mother pid is between 400 and 600 or 4000 and 6000 	      
  ATH_MSG_VERBOSE ( " in container you have electrons : " 
                    << electronTDS->size() );
  // loop on electron container
  ElectronContainer::const_iterator elecItr  = electronTDS->begin();
  ElectronContainer::const_iterator elecItrE = electronTDS->end();
  for (; elecItr != elecItrE; ++elecItr) { 
    // only soft e
    if ( !(*elecItr)->author(egammaParameters::AuthorSofte)) continue; 
    
    CLHEP::HepLorentzVector p4ele = (*elecItr)->hlv();
    double drie = p4ele.deltaR(p4);
    //double dptr = (p4ele.et()-p4.et())/p4.et();
    
    // check the parent id
    int parentID =0;
    //int grandparentID=0;
    int ID = 0;
    
    if( !(*elecItr)->trackParticle() ) continue; 
    
    const Rec::TrackParticle *myTrackParticle = (*elecItr)->trackParticle();
    // take truth info for that particle
    const HepMC::GenParticle* thePart1 = getTruth((myTrackParticle),tpTruthColl,m_trackTES);
    if (thePart1!=0) {
      //std::cout << "PDG of the track is " << thePart1->pdg_id()<<std::endl;
      ID= thePart1->pdg_id();
      HepMC::GenVertex::particle_iterator firstParent, lastParent, thisParent;
      
      firstParent =thePart1->production_vertex()->particles_begin(HepMC::parents);
      lastParent  =thePart1->production_vertex()->particles_end(HepMC::parents);
      for(thisParent = firstParent; thisParent != lastParent++; ++thisParent)  {
        parentID = (*thisParent)->pdg_id();
      }
    }
    
    // REMOVE ME!      
    //std::cout<< " MW drie parentID "<<drie<<" "<<parentID<<std::endl;
    if (std::abs(drie) <= 0.4  && std::abs(ID)==11 && 
        (isBHadron(parentID) || isDHadron(parentID))) jetele = 1;
    //if (fabs(drie)    <= 0.4  && fabs(ID)==11        && 
    //((fabs(parentID)>= 400  && fabs(parentID)<600) ||
    // (fabs(parentID)>= 4000 && fabs(parentID)<6000))) jetele = 1;  
    
    if (std::abs(drie) <= 0.4 && std::abs(ID)==11) jetanyele = 1;
  }
  
  // REMOVE ME!
  //std::cout<< " is electron in jet "<<jetanyele<<" "<<jetele<<std::endl;
  m_jet_eTrueEle[ntotal-1] = jetele;
  m_jet_eAnyEle[ntotal-1]  = jetanyele;
  //----------------------------------------------
  // Flag the jet as containing ELECTRON TRACK    
  //-----------------------------------------------
  int itrack      = 0;
  double ptsum    = 0.;
  //int jettrackele[10]; // this is never used in the code
  //for (int i=0; i< 10;i++) jettrackele[i]= 0;
  
  int nele       = 0;
  double trackpt = 0.;
  
  // iterators over the trackparticle container
  Rec::TrackParticleContainer::const_iterator trackItr  = m_trackTES->begin();
  Rec::TrackParticleContainer::const_iterator trackItrE = m_trackTES->end();
  for (; trackItr != trackItrE; ++trackItr) { 
    // check track quality
    if (!checkTrackqualforSET(trackItr,&trackpt).isSuccess()) continue;
    //int goodtrack = 0;  
    CLHEP::HepLorentzVector p4trk = (*trackItr)->hlv();
    double drie = p4trk.deltaR(p4);
    //double dptr = (p4trk.et()-p4.et())/p4.et();
    //std::cout << " HLV = " << p4trk << " " << drie << " " << dptr << std::endl;
    
    // check the parent id
    int parentID      = 0;              
    //int grandparentID = 0;
    int ID            = 0;
    
    const Rec::TrackParticle *myTrackParticle = (*trackItr);
    //take truth info for that particle
    const HepMC::GenParticle* thePart1 =getTruth(myTrackParticle,tpTruthColl,m_trackTES);
    if (thePart1!=0) {
      //std::cout << "PDG of the trackparticle is " << thePart1->pdg_id()<<std::endl;
      ID= thePart1->pdg_id();
      HepMC::GenVertex::particle_iterator firstParent, lastParent, thisParent;
      
      firstParent =thePart1->production_vertex()->particles_begin(HepMC::parents);
      lastParent  =thePart1->production_vertex()->particles_end(HepMC::parents);
      for(thisParent = firstParent; thisParent != lastParent++; ++thisParent) {
        parentID = (*thisParent)->pdg_id();
      }
    }
    
    //if GQ track inside jet 
    int codeEle = 0;
    if (fabs(drie) <= 0.4) {
      if (std::abs(ID)==11) { 
        codeEle = 1;
	
        if (isDHadron(parentID)) codeEle = 4;
        //if ((fabs(parentID)>=400 && fabs(parentID)<500) ||
        //  (fabs(parentID)>=4000 && fabs(parentID)<5000)) codeEle = 4;
	
        if (isBHadron(parentID)) codeEle = 5;
        //if ((fabs(parentID)>=500 && fabs(parentID)<600) ||
        //  (fabs(parentID)>=5000 && fabs(parentID)<6000)) codeEle = 5;
	
        if (std::abs(parentID)==22 || std::abs(parentID)==111) codeEle = 2;  
	
        //jettrackele[nele]            = codeEle;                  
        m_jet_trackEle[ntotal-1][nele]   = codeEle;
        m_jet_trackElept[ntotal-1][nele] = fabs(trackpt);
	
        nele = nele + 1;
      }
      ptsum = ptsum + fabs(trackpt);
      itrack++; 
    }
  }

  
  //if (m_UseTupleSET) {
  m_jet_NtrackEle[ntotal-1]  = nele;
  m_jet_sumptTrack[ntotal-1] = ptsum;
  m_jet_Ntrack[ntotal-1]     = itrack;
  //}
  //std::cout << " number of found ele tracks in jet  "
  //    << nele<<std::endl;
  
  int beid     = 0;
  int mothbeid = 0; 
  
  for(unsigned int i=0;i<ntrkp;i++) {
    const Analysis::SETrackInfo ipteinfo = info->getTrackInfo(i);
    //mlog << MSG::INFO << "Elec " << i << endmsg;
    //mlog << MSG::INFO << ipteinfo << endmsg;
    
    beid     = 0;
    mothbeid = 0; 
    
    if (i == 0) {
      m_jet_eptr[ntotal-1]   = ipteinfo.pTrel();
      //std::cout << " pT = " << ipteinfo.pTrel() << std::endl;
      m_jet_ea0[ntotal-1]    = ipteinfo.d0Value();

      // Does it correspond to a true electron ?
      const egamma* bestSoftE = ipteinfo.electron();
      //      const Analysis::Electron* bestSoftE = ipteinfo.electron();
      if (bestSoftE) {
        //if (m_UseTupleSET) {
        m_jet_ept[ntotal-1]    = bestSoftE->pt();
        //}
        //info about pid and moth pid of tagging particle
        if( bestSoftE->trackParticle() ) {
          const Rec::TrackParticle *myTrackParticle = bestSoftE->trackParticle();
          //take truth info for that particle
          const HepMC::GenParticle* thePartBE =getTruth(myTrackParticle,tpTruthColl,m_trackTES);
          if (thePartBE!=0) {
            beid= thePartBE->pdg_id();
            //std::cout << "PDG of the best ele is " << beid<<std::endl;
	    
            HepMC::GenVertex::particle_iterator firstParent, lastParent, thisParent;
	    
            firstParent =thePartBE->production_vertex()->particles_begin(HepMC::parents);
            lastParent  =thePartBE->production_vertex()->particles_end(HepMC::parents);
            for(thisParent = firstParent; thisParent != lastParent++; ++thisParent) {
              mothbeid = (*thisParent)->pdg_id();
              //std::cout<< "best ele parent id " << mothbeid <<std::endl;
            }
          }
        }
	
        //if (m_UseTupleSET) {
        m_jet_tag_pid[ntotal-1]     = beid     ;
        m_jet_tag_mothpid[ntotal-1] = mothbeid ;
        //}
        // Flag the jet as containing electron if DR <= 0.4
        // and e mother pid is between 400 and 600 or 4000 and 6000 	
        jetele    = 0;
        jetanyele = 0;
	
      }
    }
  }
}

// ============================================================
StatusCode JetTagAna::checkTrackqualforSET(Rec::TrackParticleContainer::const_iterator trackItr, double *trackpt) {
  //
  // check track quality for soft electrons
  // Beware that requirements could be slightly different 
  // than for other taggers (in particular for TRT)
  //

  // good quality track cuts
  double cutA0       = 1;
  int cutBL          = 1;
  int cutPi          = 2;
  int cutSi          = 7;
  int cutNTRHits     = 1;
  int cutNTRTHits    = 20;
  double cutPt       = 2000.;

  int nBL   = 0;
  int nPix  = 0;
  int nSi   = 0;
  int nTR   = 0;
  int nTRT  = 0; 
    
  const Trk::TrackSummary* summary = (*trackItr)->trackSummary();
  if (summary) {
    // number of b-layer hits
    nBL = summary->get(Trk::numberOfInnermostPixelLayerHits);
    // number of pixel hits
    nPix = summary->get(Trk::numberOfPixelHits);
    // number of Si hits
    nSi = summary->get(Trk::numberOfPixelHits) + 
      summary->get(Trk::numberOfSCTHits);
    // number of high threshold hits in TRT (with outliers)
    nTR = summary->get(Trk::numberOfTRTHighThresholdHits)+
      summary->get(Trk::numberOfTRTHighThresholdOutliers);
    // number of TRT hits (with outliers)
    nTRT = summary->get(Trk::numberOfTRTHits)+
    summary->get(Trk::numberOfTRTOutliers);
  }
    
  // access to the perigee parameters
  const Trk::Perigee* perigee = (*trackItr)->measuredPerigee();
    
  double A0Vert     = 0.;
  double PTInvVert  = 0.;
    
  if (perigee) {
    const auto & parameters = perigee->parameters();
    // transverse impact parameter
    A0Vert  = parameters[Trk::d0];
    // pT of the track
    PTInvVert = parameters[Trk::qOverP];
    if ( PTInvVert != 0. ) {
      *trackpt = 1./PTInvVert;
    } else { *trackpt = 0.;}
  }
  
  // basic cuts on good quality tracks
  if (   A0Vert   <= cutA0       &&
	 nBL      >= cutBL       &&
	 nPix     >= cutPi       &&
	 nSi      >= cutSi       &&
	 nTR      >= cutNTRHits  &&
	 nTRT     >= cutNTRTHits &&
	 fabs(*trackpt) > cutPt      ) 
    return StatusCode::SUCCESS; 

  return StatusCode::FAILURE;  
}

// ====================================================================
void JetTagAna::computeRejections() {
  MsgStream mlog( msgSvc(), name() );
  double r10, e10, w10, r50, e50, w50, r60, e60, w60;
  int nbj = (int)m_h_perf_b[0]->Integral();
  int nbu = (int)m_h_perf_u[0]->Integral();
  ATH_MSG_INFO ( "##### STANDARD REJECTIONS #####  #B-JETS: " << nbj << " #U-JETS: " << nbu );
  for(int i=0;i<MAX_numTaggers;i++) {
    if(i>4&&i<8) continue;
    this->getRej(m_h_perf_u[i], m_h_perf_b[i], r10, e10, w10, r50, e50, w50, r60, e60, w60);
    ATH_MSG_INFO ( "- Tag " << m_tagger[i] << ": light-jet rejection" );
    if(i<8 || i>9) { 
        ATH_MSG_INFO ( "   for a 50% b-tag efficiency (w>" << w50 << "): " << r50 << "+-" << e50 );
        ATH_MSG_INFO ( "   for a 60% b-tag efficiency (w>" << w60 << "): " << r60 << "+-" << e60 );

	// fill some plots here - VJ Apr. 2007
	//
	m_h_perfWt_50->SetBinContent(i+1,w50); // histogram givs the weight (for each tagger) for the 50% eff point
	if(r50<100000) {
	  m_h_perf_rej50->SetBinContent(i+1,r50); m_h_perf_rej50->SetBinError(i+1,e50);
	}
	m_h_perfWt_60->SetBinContent(i+1,w60); // histogram givs the weight (for each tagger) for the 50% eff point
	if(r60<100000) {
	  m_h_perf_rej60->SetBinContent(i+1,r60); m_h_perf_rej60->SetBinError(i+1,e60);
	}
	// now get efficiencies as a function of et/eta/phi
	// w50 was determined by averaging over all et/eta/phi. see how well it does in bins of et/eta/phi

	int nYbins = m_h_perf_b_ET[i]->GetNbinsY();
	int y50bin = m_h_perf_b[i]->FindBin(w50);
	//    int y60bin = m_h_perf_b[i]->FindBin(w60);
	//
	// first do for ET
	for(int iet=1;iet<51;iet++) {

	  //eff
	  double integ = m_h_perf_b_ET[i]->Integral(iet,iet,1,nYbins+1);
	  double integ50 = m_h_perf_b_ET[i]->Integral(iet,iet,y50bin,nYbins+1);
	  if(integ>0) {
	    double eff= integ50/integ;
	    m_h_eff_b_ET[i]->SetBinContent(iet,eff);
	    m_h_eff_b_ET[i]->SetBinError(iet,sqrt(eff*(1-eff)/integ));
	  }

	  // std. rejection
	  integ = m_h_perf_u_ET[i]->Integral(iet,iet,1,nYbins+1);
	  integ50 = m_h_perf_u_ET[i]->Integral(iet,iet,y50bin,nYbins+1);
	  if(integ50>0) {
	    m_h_rej_u_ET[i]->SetBinContent(iet,integ/integ50);
	    double invrej = integ50/integ;
	    double err_invrej = sqrt(invrej*(1-invrej)/integ);
	    double err_rej = err_invrej/invrej/invrej;
	    m_h_rej_u_ET[i]->SetBinError(iet,err_rej);
	  }
	  // purified rejection
	  integ = m_h_perf_upur_ET[i]->Integral(iet,iet,1,nYbins+1);
	  integ50 = m_h_perf_upur_ET[i]->Integral(iet,iet,y50bin,nYbins+1);
	  if(integ50>0) {
	    m_h_rej_upur_ET[i]->SetBinContent(iet,integ/integ50);
	    double invrej = integ50/integ;
	    double err_invrej = sqrt(invrej*(1-invrej)/integ);
	    double err_rej = err_invrej/invrej/invrej;
	    m_h_rej_upur_ET[i]->SetBinError(iet,err_rej);
	  }

	} // end of ET

	// now do for eta
	for(int ieta=1;ieta<51;ieta++) {

	  //eff
	  double integ = m_h_perf_b_eta[i]->Integral(ieta,ieta,1,nYbins+1);
	  double integ50 = m_h_perf_b_eta[i]->Integral(ieta,ieta,y50bin,nYbins+1);
	  if(integ>0) {
	    double eff= integ50/integ;
	    m_h_eff_b_eta[i]->SetBinContent(ieta,eff);
	    m_h_eff_b_eta[i]->SetBinError(ieta,sqrt(eff*(1-eff)/integ));
	  }

	  // std. rejection
	  integ = m_h_perf_u_eta[i]->Integral(ieta,ieta,1,nYbins+1);
	  integ50 = m_h_perf_u_eta[i]->Integral(ieta,ieta,y50bin,nYbins+1);
	  if(integ50>0) {
	    m_h_rej_u_eta[i]->SetBinContent(ieta,integ/integ50);
	    double invrej = integ50/integ;
	    double err_invrej = sqrt(invrej*(1-invrej)/integ);
	    double err_rej = err_invrej/invrej/invrej;
	    m_h_rej_u_eta[i]->SetBinError(ieta,err_rej);
	  }
	  // purified rejection
	  integ = m_h_perf_upur_eta[i]->Integral(ieta,ieta,1,nYbins+1);
	  integ50 = m_h_perf_upur_eta[i]->Integral(ieta,ieta,y50bin,nYbins+1);
	  if(integ50>0) {
	    m_h_rej_upur_eta[i]->SetBinContent(ieta,integ/integ50);
	    double invrej = integ50/integ;
	    double err_invrej = sqrt(invrej*(1-invrej)/integ);
	    double err_rej = err_invrej/invrej/invrej;
	    m_h_rej_upur_eta[i]->SetBinError(ieta,err_rej);
	  }

	} // end of eta
	// now do for phi
	for(int iphi=1;iphi<61;iphi++) {

	  //eff
	  double integ = m_h_perf_b_phi[i]->Integral(iphi,iphi,1,nYbins+1);
	  double integ50 = m_h_perf_b_phi[i]->Integral(iphi,iphi,y50bin,nYbins+1);
	  if(integ>0) {
	    double eff= integ50/integ;
	    m_h_eff_b_phi[i]->SetBinContent(iphi,eff);
	    m_h_eff_b_phi[i]->SetBinError(iphi,sqrt(eff*(1-eff)/integ));
	  }

	  // std. rejection
	  integ = m_h_perf_u_phi[i]->Integral(iphi,iphi,1,nYbins+1);
	  integ50 = m_h_perf_u_phi[i]->Integral(iphi,iphi,y50bin,nYbins+1);
	  if(integ50>0) {
	    m_h_rej_u_phi[i]->SetBinContent(iphi,integ/integ50);
	    double invrej = integ50/integ;
	    double err_invrej = sqrt(invrej*(1-invrej)/integ);
	    double err_rej = err_invrej/invrej/invrej;
	    m_h_rej_u_phi[i]->SetBinError(iphi,err_rej);
	  }
	  // purified rejection
	  integ = m_h_perf_upur_phi[i]->Integral(iphi,iphi,1,nYbins+1);
	  integ50 = m_h_perf_upur_phi[i]->Integral(iphi,iphi,y50bin,nYbins+1);
	  if(integ50>0) {
	    m_h_rej_upur_phi[i]->SetBinContent(iphi,integ/integ50);
	    double invrej = integ50/integ;
	    double err_invrej = sqrt(invrej*(1-invrej)/integ);
	    double err_rej = err_invrej/invrej/invrej;
	    m_h_rej_upur_phi[i]->SetBinError(iphi,err_rej);
	  }

	} // end of phi      

      }
    else
      {
	ATH_MSG_INFO ( "   for a 10% b-tag efficiency (w>" << w10 << "): " << r10 << "+-" << e10 );

        m_h_perfWt_10->SetBinContent(i-7,w10);// start with bin 1
        if(r10<100000) {
	  m_h_perf_rej10->SetBinContent(i-7,r10); // start with bin 1
	  m_h_perf_rej10->SetBinError(i-7,e10); // start with bin 1
	}

      }
    //
  }
  int nbupur = (int)m_h_perf_upur[0]->Integral();
  ATH_MSG_INFO ( "##### REJECTIONS AFTER PURIFICATION #####  #B-JETS: " << nbj << " #U-JETS: " << nbupur );
  for(int i=0;i<MAX_numTaggers;i++) {
    if(i>4&&i<8) continue;
    this->getRej(m_h_perf_upur[i], m_h_perf_b[i], r10, e10, w10, r50, e50, w50, r60, e60, w60);
    ATH_MSG_INFO ( "- Tag " << m_tagger[i] << ": light-jet rejection" );
    if(i<8 || i>9) { 
        ATH_MSG_INFO ( "   for a 50% b-tag efficiency (w>" << w50 << "): " << r50 << "+-" << e50 );
        ATH_MSG_INFO ( "   for a 60% b-tag efficiency (w>" << w60 << "): " << r60 << "+-" << e60 );
	// fill some plots - VJ Apr. 2007
	m_h_perfWt_pur50->SetBinContent(i+1,w50);
	if(r50<100000) {
	  m_h_perf_rejpur50->SetBinContent(i+1,r50); m_h_perf_rejpur50->SetBinError(i+1,e50);
	}
	m_h_perfWt_pur60->SetBinContent(i+1,w60);
	if(r60<100000) {
	  m_h_perf_rejpur60->SetBinContent(i+1,r60); m_h_perf_rejpur60->SetBinError(i+1,e60);
	}


      }
    else
      {
	ATH_MSG_INFO ( "   for a 10% b-tag efficiency (w>" << w10 << "): " << r10 << "+-" << e10 );
        m_h_perfWt_pur10->SetBinContent(i-7,w10);// start with bin 1
        if(r10<100000) {
	  m_h_perf_rejpur10->SetBinContent(i-7,r10); // start with bin 1
	  m_h_perf_rejpur10->SetBinError(i-7,e10); // start with bin 1
	}
      }
  }
}

void JetTagAna::getRej(TH1F* uw, TH1F* bw,
		       double& ru10, double& eru10, double& w10, 
		       double& ru50, double& eru50, double& w50, 
		       double& ru60, double& eru60, double& w60
		       ) {
  if(uw==0||bw==0) return;
  const int n_max_bins = 1000;
  int    nb   = uw->GetNbinsX()+1;
  if(nb>n_max_bins) return;
  int    bi10 = 0;
  int    bi50 = 0;
  int    bi60 = 0;
  double ru[n_max_bins],efb[n_max_bins],eru[n_max_bins]; //,eefb[n_max_bins];
  const double xbi  = (double) bw->Integral(0,nb); // also includes overflows
  const double xui  = (double) uw->Integral(0,nb);
  const double inv_xbi = xbi != 0 ? 1. / xbi : 1;
  const double inv_xui = xui != 0 ? 1. / xui : 1;
  for (int ib = 1;ib<nb;ib++) {
    efb[ib-1] = ((double) bw->Integral(ib,nb)) * inv_xbi;
    if(ib>1) {
      if (efb[ib-1] <= 0.1 && efb[ib-2] >= 0.1) {bi10 = ib-1;}
      if (efb[ib-1] <= 0.5 && efb[ib-2] >= 0.5) {bi50 = ib-1;}
      if (efb[ib-1] <= 0.6 && efb[ib-2] >= 0.6) {bi60 = ib-1;}
    }
    //eefb[ib-1] = sqrt(efb[ib-1]*(1.-efb[ib-1])/xbi); // nothing done with that 
    if(uw->Integral(ib,nb)>0.) {
	ru[ib-1]   = xui/((double) uw->Integral(ib,nb));
	eru[ib-1]  = ru[ib-1]*sqrt((ru[ib-1]-1.) * inv_xui);
    } else {
	ru[ib-1]   = 0.;
	eru[ib-1]  = 0.;
    }
  }

  if (bi10 > 0) {
    w10 = uw->GetBinCenter(bi10);
    ru10  = (ru[bi10]*(efb[bi10-1]-0.1)+ru[bi10-1]*(0.1-efb[bi10]))/(efb[bi10-1]-efb[bi10]);
    eru10 = (eru[bi10]*(efb[bi10-1]-0.1)+eru[bi10-1]*(0.1-efb[bi10]))/(efb[bi10-1]-efb[bi10]);
  }
  else {
    w10 = 0;
    ru10 = 0;
    eru10 = 0;
  }

  if (bi50 > 0) {
    w50 = uw->GetBinCenter(bi50);
    ru50  = (ru[bi50]*(efb[bi50-1]-0.5)+ru[bi50-1]*(0.5-efb[bi50]))/(efb[bi50-1]-efb[bi50]);
    eru50 = (eru[bi50]*(efb[bi50-1]-0.5)+eru[bi50-1]*(0.5-efb[bi50]))/(efb[bi50-1]-efb[bi50]);
  }
  else {
    w50 = 0;
    ru50 = 0;
    eru50 = 0;
  }

  if (bi60 > 0) {
    w60 = uw->GetBinCenter(bi60);
    ru60  = (ru[bi60]*(efb[bi60-1]-0.6)+ru[bi60-1]*(0.6-efb[bi60]))/(efb[bi60-1]-efb[bi60]);
    eru60 = (eru[bi60]*(efb[bi60-1]-0.6)+eru[bi60-1]*(0.6-efb[bi60]))/(efb[bi60-1]-efb[bi60]);
  }
  else {
    w60 = 0;
    ru60 = 0;
    eru60 = 0;
  }
}

void JetTagAna::bookHistograms() {
  if (m_h_global_counters) return;
  m_h_global_counters = new TH1F("global_counters","Counters",100,0.,100.);
  m_h_global_nprimvtx = new TH1F("global_nprimvtx","N primary vertex",10,0.,10.);
  m_h_global_xprimvtx = new TH1F("global_xprimvtx","X primary vertex",100,-0.1,0.1);
  m_h_global_yprimvtx = new TH1F("global_yprimvtx","Y primary vertex",100,-0.1,0.1);
  m_h_global_zprimvtx = new TH1F("global_zprimvtx","Z primary vertex",100,-250.,250.);
  m_h_global_primvtxresx = new TH1F("global_primvtxresx","reso X primary vertex",100,-0.1,0.1);
  m_h_global_primvtxresy = new TH1F("global_primvtxresy","reso Y primary vertex",100,-0.1,0.1);
  m_h_global_primvtxresz = new TH1F("global_primvtxresz","reso Z primary vertex",100,-0.5,0.5);
  m_h_global_nmcpart  = new TH1F("global_nmcpart","N MC particles",100,0.,1500.);
  m_h_global_ntrkpart = new TH1F("global_ntrkpart","N TrackParticles",100,0.,200.);
  m_h_global_BLayerHits = new TH1F("global_BLayerHits","No. of BLayer Hits on TrackP",5,0.,5.);
  m_h_global_BLayerSharedHits = new TH1F("global_BLayerSharedHits","No. of BLayer shared hits on TrackP",5,0.,5.);
  m_h_global_PixelHits = new TH1F("global_PixelHits","No. of Pixel Hits on TrackP",10,0.,10.);
  m_h_global_PixelLayers = new TH1F("global_PixelLayers","No. of Pixel Layers on TrackP",10,0.,10.);
  m_h_global_SiHits = new TH1F("global_SiHits","No. of Pixel+SCT Hits on TrackP",25,0.,25.);
  m_h_global_TRTHits = new TH1F("global_TRTHits","No. of TRT Hits on TrackP",100,0.,100.);
  m_h_global_nmuon = new TH1F("global_nmuon","N Muons",10,0.,10.);
  m_h_global_nelectron = new TH1F("global_nelectron","N Electron",10,0.,10.);
  m_h_global_njettag  = new TH1F("global_njettag","N JetTag",30,0.,30.);
  m_h_truth_bquark_nb = new TH1F("truth_bquark_nb","N quark b",10,0.,10.);
  m_h_truth_bquark_pt = new TH1F("truth_bquark_pt","b quark pt",100,0.,100000.);
  m_h_truth_bquark_eta= new TH1F("truth_bquark_eta","b quark eta",100,-5.,5.);
  m_h_truth_cquark_nb = new TH1F("truth_cquark_nb","N quark c",10,0.,10.);
  m_h_truth_cquark_pt = new TH1F("truth_cquark_pt","c quark pt",100,0.,100000.);
  m_h_truth_cquark_eta= new TH1F("truth_cquark_eta","c quark eta",100,-5.,5.);
  m_h_truth_bhadr_nb  = new TH1F("truth_bhadr_nb","N hadr b",10,0.,10.);
  m_h_truth_bhadr_pt  = new TH1F("truth_bhadr_pt","b hadr pt",100,0.,100000.);
  m_h_truth_bhadr_eta = new TH1F("truth_bhadr_eta","b hadr eta",100,-5.,5.);
  m_h_truth_chadr_nb  = new TH1F("truth_chadr_nb","N hadr c",10,0.,10.);
  m_h_truth_chadr_pt  = new TH1F("truth_chadr_pt","c hadr pt",100,0.,100000.);
  m_h_truth_chadr_eta = new TH1F("truth_chadr_eta","c hadr eta",100,-5.,5.);
  m_h_jet_ntotal      = new TH1F("jet_ntotal","N jets",30,0.,30.);
  m_h_jet_label       = new TH1F("jet_label","MC jet label",20,0.,20.);
  m_h_jet_nlabelb     = new TH1F("jet_nlabelb","N jets label b",10,0.,10.);
  m_h_jet_nlabelc     = new TH1F("jet_nlabelc","N jets label c",10,0.,10.);
  m_h_jet_nlabelt     = new TH1F("jet_nlabelt","N jets label tau",10,0.,10.);
  m_h_jet_ntag        = new TH1F("jet_ntag","N tags",20,0.,20.);
  m_h_jet_eta         = new TH1F("jet_eta","Jet eta",100,-5.,5.);
  m_h_jet_phi         = new TH1F("jet_phi","Jet phi",100,-3.15,3.15);
  m_h_jet_et          = new TH1F("jet_et","Jet et",100,0.,500000.);
  m_h_jet_ntracks     = new TH1F("jet_ntracks","# tracks in a Jet",50,0,50.);
  m_h_jet_tracks_pt   = new TH1F("jet_tracks_pt","pT of tracks in a Jet",100,0,50.);
  m_h_jet_nmuons      = new TH1F("jet_nmuons","# muons in a Jet",10,0,10.);
  m_h_jet_muons_pt    = new TH1F("jet_muons_pt","pT of muons in a Jet",100,0,100.);
  m_h_jet_nelectrons  = new TH1F("jet_nelectrons","# electrons in a Jet",10,0,10.);
  m_h_jet_electrons_pt= new TH1F("jet_electrons_pt","pT of electrons in a Jet",100,0,100.);
  m_h_jet_tracks_BLayerHits = new TH1F("jet_tracks_BLayerHits","BLay hits",10,0.,10.);
  m_h_jet_tracks_PixelHits = new TH1F("jet_tracks_PixelHits","Pixel hits",10,0.,10.);
  m_h_jet_tracks_SCTHits = new TH1F("jet_tracks_SCTHits","SCT hits",20,0.,20.);
  m_h_tag_jetprob_w   = new TH1F("tag_jetprob_w","JetProb: weight",100,0.,1.);
  m_h_tag_i2d_w       = new TH1F("tag_i2d_w","IP2D: weight",120,-20.,40.);
  m_h_tag_i2d_n       = new TH1F("tag_i2d_n","IP2D: Ntrack",20,0.,20.);
  m_h_tag_i2d_b       = new TH1F("tag_i2d_b","IP2D: Pb",10,0.,1.);
  m_h_tag_i2d_u       = new TH1F("tag_i2d_u","IP2D: Pu",10,0.,1.);
  m_h_tag_i2d_sig     = new TH1F("tag_i2d_sig","IP2D: signed IP",100,-5.,5.);
  m_h_tag_i3d_w       = new TH1F("tag_i3d_w","IP3D: weight",120,-20.,40.);
  m_h_tag_i3d_n       = new TH1F("tag_i3d_n","IP3D: Ntrack",20,0.,20.);
  m_h_tag_i3d_b       = new TH1F("tag_i3d_b","IP3D: Pb",10,0.,1.);
  m_h_tag_i3d_u       = new TH1F("tag_i3d_u","IP3D: Pu",10,0.,1.);
  m_h_tag_i3d_sig     = new TH1F("tag_i3d_sig","IP3D: signed IP",100,-5.,5.);
  m_h_tag_sv1_w       = new TH1F("tag_sv1_w","SV1: weight",120,-20.,40.);
  m_h_tag_sv1_b       = new TH1F("tag_sv1_b","SV1: Pb",10,0.,1.);
  m_h_tag_sv1_u       = new TH1F("tag_sv1_u","SV1: Pu",10,0.,1.);
  m_h_tag_sv_n       = new TH1F("tag_sv_n","SV1: Ntrack",10,-1.,9.);
  m_h_tag_sv_n2t     = new TH1F("tag_sv_n2t","SV1: N2T",20,0.,20.);
  m_h_tag_sv_frc     = new TH1F("tag_sv_frc","SV1: Frc",100,0.,1.);
  m_h_tag_sv_m       = new TH1F("tag_sv_m","SV1: mass",100,0.,10.);
  m_h_tag_sv2_w       = new TH1F("tag_sv2_w","SV2: weight",120,-20.,40.);
  m_h_tag_sv2_b       = new TH1F("tag_sv2_b","SV2: Pb",10,0.,1.);
  m_h_tag_sv2_u       = new TH1F("tag_sv2_u","SV2: Pu",10,0.,1.);
  m_h_tag_cmb_w       = new TH1F("tag_cmb_w","Weight combined",120,-20.,40.);
  m_h_tag_lf2d_w      = new TH1F("tag_lf2d_w","Lifetime2D: weight",120,-20.,40.);
  m_h_tag_svbu_w      = new TH1F("tag_svbu_w","SecVtxTagBU: weight",120,-20.,40.);
  m_h_tag_lhsig_w     = new TH1F("tag_lhsig_w","lhSig: weight",120,-20.,40.);
  m_h_tag_softm_w     = new TH1F("tag_softm_w","SoftMuonTag: weight",120,-20.,40.);
  m_h_tag_softe_w     = new TH1F("tag_softe_w","SoftElectronTag: weight",120,-20.,40.);
  m_h_tag_jetfitter_w     = new TH1F("tag_jetfitter_w","JetFitterTag: weight",120,-20.,40.);
  m_h_tag_jetfitcomb_w     = new TH1F("tag_jetfitcomb_w","JetFitterCombTag: weight",120,-20.,40.);
  m_h_tag_jetfitternn_w     = new TH1F("tag_jetfitternn_w","JetFitterTagNN: weight",120,-20.,40.);
  m_h_tag_jetfitcombnn_w     = new TH1F("tag_jetfitcombnn_w","JetFitterCombTagNN: weight",120,-20.,40.);

  //*
  m_h_tag_i2d_of_w = new TH1F("tag_i2d_of_w","IP2D offline weight if selected by trigger",120,-20.,40.);
  m_h_tag_i3d_of_w = new TH1F("tag_i3d_of_w","IP3D offline weight if selected by trigger",120,-20.,40.);
  m_h_tag_i2d_l2_w = new TH1F("tag_i2d_l2_w","IP2D LVL2 weight",50,0.,1.);
  m_h_tag_i3d_l2_w = new TH1F("tag_i3d_l2_w","IP3D LVL2 weight",50,0.,1.);
  m_h_tag_i2d_ef_w = new TH1F("tag_i2d_ef_w","IP2D EF weight",50,0.,1.);
  m_h_tag_i3d_ef_w = new TH1F("tag_i3d_ef_w","IP3D EF weight",50,0.,1.);

  // plots as a function of jet ET
  for(int i=0; i<6; ++i) {
    TString htit(m_jetET[i]);
    m_h_tag_IPinfo_ntrk[i] = new TH1F(htit+"_tag_IPinfo_ntrk",htit+" ntrk in jet",50,0.,50.);
    m_h_tag_IPinfo_trkPt[i] = new TH1F(htit+"_tag_IPinfo_trkPt",htit+" trk Pt in jet",100,0.,25.);
    m_h_tag_IPinfo_d0val[i] = new TH1F(htit+"_tag_IPinfo_d0val",htit+" IP info - d0val",120,-0.2,0.4);
    m_h_tag_IPinfo_z0val[i] = new TH1F(htit+"_tag_IPinfo_z0val",htit+" IP info - z0val",120,-0.5,1.);
    m_h_tag_IPinfo_d0sig[i] = new TH1F(htit+"_tag_IPinfo_d0sig",htit+" IP info - d0sig",100,-5.,5.);
    m_h_tag_IPinfo_z0sig[i] = new TH1F(htit+"_tag_IPinfo_z0sig",htit+" IP info - z0sig",100,-5.,5.);
    m_h_tag_IPinfo_weight2D[i] = new TH1F(htit+"_tag_IPinfo_weight2D",htit+" weight2D for each track",100,-2.,6.);
    m_h_tag_IPinfo_weight3D[i] = new TH1F(htit+"_tag_IPinfo_weight3D",htit+" weight3D for each track",100,-2.,6.);
    m_h_tag_IPinfo_piJP[i] = new TH1F(htit+"_tag_IPinfo_piJP",htit+" piJP for each track",100,-1.,1.);
    m_h_tag_IPinfo_fromV0[i] = new TH1F(htit+"_tag_IPinfo_fromV0",htit+" fromV0: for each track",3,-1,2.);
    m_h_tag_IPinfo_grade[i] = new TH1F(htit+"_tag_IPinfo_grade",htit+" grade: for each track",5,-1,4.);
  }

  int hnb[MAX_numTaggers];
  float hmin[MAX_numTaggers], hmax[MAX_numTaggers];

  for(int i=0;i<(MAX_numTaggers-MAX_numHLTTaggers)+2;i++) {
    hnb[i] = 560;
    hmin[i] = -20.;
    hmax[i] = +50.;
    if(m_tagger[i]=="JetProb") {
      hnb[i] = 100;
      hmin[i] = 0.;
      hmax[i] = 10.;
    }
  }
  for(int i=(MAX_numTaggers-MAX_numHLTTaggers)+2;i<MAX_numTaggers;i++) {
    hnb[i] = 50;
    hmin[i] = 0.;
    hmax[i] = 1.;
  }
  for(int i=0;i<MAX_numTaggers;i++) {
    TString tit("perf_"); tit+=m_tagger[i]; tit.ToLower();
    TString htit(m_tagger[i]);
    m_h_perf_b[i] = new TH1F(tit+"_b",htit+": weight for selected b jets",hnb[i],hmin[i],hmax[i]);
    m_h_perf_u[i] = new TH1F(tit+"_u",htit+": weight for selected u jets",hnb[i],hmin[i],hmax[i]);
    m_h_perf_upur[i] = new TH1F(tit+"_upur",htit+": weight for selected purified u jets",hnb[i],hmin[i],hmax[i]);
    //
    // hists for eff/rej as a function of ET/eta/phi
    //
    m_h_perf_b_ET[i] = new TH2F(tit+"_b_ET",htit+":weight for selected b jets v jet ET",50,0.,1000.,hnb[i],hmin[i],hmax[i]);
    m_h_perf_b_eta[i] = new TH2F(tit+"_b_eta",htit+":weight for selected b jets v jet eta",50,-5.,5,hnb[i],hmin[i],hmax[i]);
    m_h_perf_b_phi[i] = new TH2F(tit+"_b_phi",htit+":weight for selected b jets v jet phi",60,-3.15,3.15,hnb[i],hmin[i],hmax[i]);
    m_h_perf_u_ET[i] = new TH2F(tit+"_u_ET",htit+":weight for selected u jets v jet ET",50,0.,1000.,hnb[i],hmin[i],hmax[i]);
    m_h_perf_u_eta[i] = new TH2F(tit+"_u_eta",htit+":weight for selected u jets v jet eta",50,-5.,5.,hnb[i],hmin[i],hmax[i]);
    m_h_perf_u_phi[i] = new TH2F(tit+"_u_phi",htit+":weight for selected u jets v jet phi",60,-3.15,3.15,hnb[i],hmin[i],hmax[i]);
    m_h_perf_upur_ET[i] = new TH2F(tit+"_upur_ET",htit+":weight for selected u purified jets v jet ET",50,0.,1000.,hnb[i],hmin[i],hmax[i]);
    m_h_perf_upur_eta[i] = new TH2F(tit+"_upur_eta",htit+":weight for selected u purified jets v jet eta",50,-5.,5.,hnb[i],hmin[i],hmax[i]);
    m_h_perf_upur_phi[i] = new TH2F(tit+"_upur_phi",htit+":weight for selected u purified jets v jet phi",60,-3.15,3.15,hnb[i],hmin[i],hmax[i]);
    //
    TString tit1("eff_"); tit1+=m_tagger[i]; tit1.ToLower();
    m_h_eff_b_ET[i] = new TH1F(tit1+"_b_ET",htit+":efficiency of 50% point for selected b jets v jet ET",50,0.,1000.);
    m_h_eff_b_ET[i]->Sumw2();
    m_h_eff_b_eta[i] = new TH1F(tit1+"_b_eta",htit+":efficiency of 50% point for selected b jets v jet eta",50,-5.,5.);
    m_h_eff_b_eta[i]->Sumw2();
    m_h_eff_b_phi[i] = new TH1F(tit1+"_b_phi",htit+":efficiency of 50% point for selected b jets v jet phi",60,-3.15,3.15);
    m_h_eff_b_phi[i]->Sumw2();
    TString tit2("rej_"); tit2+=m_tagger[i]; tit2.ToLower();
    m_h_rej_u_ET[i] = new TH1F(tit2+"_u_ET",htit+":rejection of 50% point for selected u jets v jet ET",50,0.,1000.);
    m_h_rej_u_ET[i]->Sumw2();
    m_h_rej_u_eta[i] = new TH1F(tit2+"_u_eta",htit+":rejection of 50% point for selected u jets v jet eta",50,-5.,5.);
    m_h_rej_u_eta[i]->Sumw2();
    m_h_rej_u_phi[i] = new TH1F(tit2+"_u_phi",htit+":rejection of 50% point for selected u jets v jet phi",60,-3.15,3.15);
    m_h_rej_u_phi[i]->Sumw2();
    //
    m_h_rej_upur_ET[i] = new TH1F(tit2+"_upur_ET",htit+":rejection of 50% point for selected u purified jets v jet ET",50,0.,1000.);
    m_h_rej_upur_ET[i]->Sumw2();
    m_h_rej_upur_eta[i] = new TH1F(tit2+"_upur_eta",htit+":rejection of 50% point for selected u purified jets v jet eta",50,-5.,5.);
    m_h_rej_upur_eta[i]->Sumw2();
    m_h_rej_upur_phi[i] = new TH1F(tit2+"_upur_phi",htit+":rejection of 50% point for selected u purified jets v jet phi",60,-3.15,3.15);
    m_h_rej_upur_phi[i]->Sumw2();

  }
  m_h_perf_vxeff_den_b   = new TH1F("perf_vxeff_den_b","SV efficiency (den) b",1,0.,1.);
  m_h_perf_vxeff_num_b  = new TH1F("perf_vxeff_num_b","SV efficiency (num) b",1,0.,1.);
  m_h_perf_vxeff_den_u   = new TH1F("perf_vxeff_den_u","SV efficiency (den) u",1,0.,1.);
  m_h_perf_vxeff_num_u  = new TH1F("perf_vxeff_num_u","SV efficiency (num) u",1,0.,1.);
  m_h_perf_vxeff_den_upur   = new TH1F("perf_vxeff_den_upur","SV efficiency (den) u",1,0.,1.);
  m_h_perf_vxeff_num_upur  = new TH1F("perf_vxeff_num_upur","SV efficiency (num) u",1,0.,1.);
  m_h_perf_rej10      = new TH1F("perf_rej10","Rejections for soft muon/electron (eff=0.1)",15,0.,15.);
  m_h_perf_rej10->Sumw2();
  m_h_perf_rej50      = new TH1F("perf_rej50","Rejections (eff=0.5)",15,0.,15.);
  m_h_perf_rej50->Sumw2();
  m_h_perf_rej60      = new TH1F("perf_rej60","Rejections (eff=0.6)",15,0.,15.);
  m_h_perf_rej60->Sumw2();
  m_h_perf_rejpur10      = new TH1F("perf_rejpur10","Rejections for soft muon/electron (eff=0.1 purification)",15,0.,15.);
  m_h_perf_rejpur10->Sumw2();
  m_h_perf_rejpur50   = new TH1F("perf_rejpur50","Rejections (eff=0.5, purification)",15,0.,15.);
  m_h_perf_rejpur50->Sumw2();
  m_h_perf_rejpur60   = new TH1F("perf_rejpur60","Rejections (eff=0.6, purification)",15,0.,15.);
  m_h_perf_rejpur60->Sumw2();
  m_h_perfWt_10      = new TH1F("perfWt_10","Weight for which (eff=0.1) soft muon/electron",15,0.,15.);
  m_h_perfWt_50      = new TH1F("perfWt_50","Weight for which (eff=0.5)",15,0.,15.);
  m_h_perfWt_60      = new TH1F("perfWt_60","Weight for which (eff=0.6)",15,0.,15.);
  m_h_perfWt_pur10   = new TH1F("perfWt_pur10","Weight for which (eff=0.1, purification) soft muon/electron",15,0.,15.);
  m_h_perfWt_pur50   = new TH1F("perfWt_pur50","Weight for which (eff=0.5, purification)",15,0.,15.);
  m_h_perfWt_pur60   = new TH1F("perfWt_pur60","Weight for which (eff=0.6, purification)",15,0.,15.);
  std::string histDir("/fileJetTagAna/");
  StatusCode sc;
  sc = m_histos->regHist(histDir+"global_counters",m_h_global_counters);
  sc = m_histos->regHist(histDir+"global_nprimvtx",m_h_global_nprimvtx);
  sc = m_histos->regHist(histDir+"global_xprimvtx",m_h_global_xprimvtx);
  sc = m_histos->regHist(histDir+"global_yprimvtx",m_h_global_yprimvtx);
  sc = m_histos->regHist(histDir+"global_zprimvtx",m_h_global_zprimvtx);
  sc = m_histos->regHist(histDir+"global_primvtxresx",m_h_global_primvtxresx);
  sc = m_histos->regHist(histDir+"global_primvtxresy",m_h_global_primvtxresy);
  sc = m_histos->regHist(histDir+"global_primvtxresz",m_h_global_primvtxresz);
  sc = m_histos->regHist(histDir+"global_nmcpart", m_h_global_nmcpart);
  sc = m_histos->regHist(histDir+"global_ntrkpart",m_h_global_ntrkpart);
  sc = m_histos->regHist(histDir+"global_BLayerHits",m_h_global_BLayerHits);
  sc = m_histos->regHist(histDir+"global_BLayerSharedHits",m_h_global_BLayerSharedHits);
  sc = m_histos->regHist(histDir+"global_PixelHits",m_h_global_PixelHits);
  sc = m_histos->regHist(histDir+"global_PixelLayers",m_h_global_PixelLayers);
  sc = m_histos->regHist(histDir+"global_SiHits",m_h_global_SiHits);
  sc = m_histos->regHist(histDir+"global_TRTHits",m_h_global_TRTHits);
  sc = m_histos->regHist(histDir+"global_muon",m_h_global_nmuon);
  sc = m_histos->regHist(histDir+"global_electron",m_h_global_nelectron);
  sc = m_histos->regHist(histDir+"global_njettag", m_h_global_njettag);
  sc = m_histos->regHist(histDir+"truth_bquark_nb",m_h_truth_bquark_nb);
  sc = m_histos->regHist(histDir+"truth_bquark_pt",m_h_truth_bquark_pt);
  sc = m_histos->regHist(histDir+"truth_bquark_eta",m_h_truth_bquark_eta);
  sc = m_histos->regHist(histDir+"truth_cquark_nb",m_h_truth_cquark_nb);
  sc = m_histos->regHist(histDir+"truth_cquark_pt",m_h_truth_cquark_pt);
  sc = m_histos->regHist(histDir+"truth_cquark_eta",m_h_truth_cquark_eta);
  sc = m_histos->regHist(histDir+"truth_bhadr_nb",m_h_truth_bhadr_nb);
  sc = m_histos->regHist(histDir+"truth_bhadr_pt",m_h_truth_bhadr_pt);
  sc = m_histos->regHist(histDir+"truth_bhadr_eta",m_h_truth_bhadr_eta);
  sc = m_histos->regHist(histDir+"truth_chadr_nb",m_h_truth_chadr_nb);
  sc = m_histos->regHist(histDir+"truth_chadr_pt",m_h_truth_chadr_pt);
  sc = m_histos->regHist(histDir+"truth_chadr_eta",m_h_truth_chadr_eta);
  sc = m_histos->regHist(histDir+"jet_ntotal", m_h_jet_ntotal);
  sc = m_histos->regHist(histDir+"jet_label", m_h_jet_label);
  sc = m_histos->regHist(histDir+"jet_nlabelb",m_h_jet_nlabelb);
  sc = m_histos->regHist(histDir+"jet_nlabelc",m_h_jet_nlabelc);
  sc = m_histos->regHist(histDir+"jet_nlabelt",m_h_jet_nlabelt);
  sc = m_histos->regHist(histDir+"jet_ntag",   m_h_jet_ntag);
  sc = m_histos->regHist(histDir+"jet_eta",    m_h_jet_eta);
  sc = m_histos->regHist(histDir+"jet_ntracks",    m_h_jet_ntracks);
  sc = m_histos->regHist(histDir+"jet_tracks_pt",    m_h_jet_tracks_pt);
  sc = m_histos->regHist(histDir+"jet_nmuons",    m_h_jet_nmuons);
  sc = m_histos->regHist(histDir+"jet_muons_pt",    m_h_jet_muons_pt);
  sc = m_histos->regHist(histDir+"jet_nelectrons",    m_h_jet_nelectrons);
  sc = m_histos->regHist(histDir+"jet_electrons_pt",    m_h_jet_electrons_pt);
  sc = m_histos->regHist(histDir+"jet_phi",    m_h_jet_phi);
  sc = m_histos->regHist(histDir+"jet_et",     m_h_jet_et);
  sc = m_histos->regHist(histDir+"tag_jetprob_w",m_h_tag_jetprob_w);
  sc = m_histos->regHist(histDir+"tag_i2d_w",m_h_tag_i2d_w);
  sc = m_histos->regHist(histDir+"tag_i2d_n",m_h_tag_i2d_n);
  sc = m_histos->regHist(histDir+"tag_i2d_b",m_h_tag_i2d_b);
  sc = m_histos->regHist(histDir+"tag_i2d_u",m_h_tag_i2d_u);
  sc = m_histos->regHist(histDir+"tag_i2d_sig",m_h_tag_i2d_sig);
  sc = m_histos->regHist(histDir+"tag_i3d_w",m_h_tag_i3d_w);
  sc = m_histos->regHist(histDir+"tag_i3d_n",m_h_tag_i3d_n);
  sc = m_histos->regHist(histDir+"tag_i3d_b",m_h_tag_i3d_b);
  sc = m_histos->regHist(histDir+"tag_i3d_u",m_h_tag_i3d_u);
  sc = m_histos->regHist(histDir+"tag_i3d_sig",m_h_tag_i3d_sig);
  sc = m_histos->regHist(histDir+"tag_sv1_w",m_h_tag_sv1_w);
  sc = m_histos->regHist(histDir+"tag_sv1_b",m_h_tag_sv1_b);
  sc = m_histos->regHist(histDir+"tag_sv1_u",m_h_tag_sv1_u);
  sc = m_histos->regHist(histDir+"tag_sv_n",m_h_tag_sv_n);
  sc = m_histos->regHist(histDir+"tag_sv_n2t",m_h_tag_sv_n2t);
  sc = m_histos->regHist(histDir+"tag_sv_frc",m_h_tag_sv_frc);
  sc = m_histos->regHist(histDir+"tag_sv_m",m_h_tag_sv_m);
  sc = m_histos->regHist(histDir+"tag_sv2_w",m_h_tag_sv2_w);
  sc = m_histos->regHist(histDir+"tag_sv2_b",m_h_tag_sv2_b);
  sc = m_histos->regHist(histDir+"tag_sv2_u",m_h_tag_sv2_u);
  sc = m_histos->regHist(histDir+"tag_cmb_w",m_h_tag_cmb_w);
  sc = m_histos->regHist(histDir+"tag_lf2d_w",m_h_tag_lf2d_w);
  sc = m_histos->regHist(histDir+"tag_svbu_w",m_h_tag_svbu_w);
  sc = m_histos->regHist(histDir+"tag_lhsig_w",m_h_tag_lhsig_w);
  sc = m_histos->regHist(histDir+"tag_softm_w",m_h_tag_softm_w);
  sc = m_histos->regHist(histDir+"tag_jetfitter_w",m_h_tag_jetfitter_w);
  sc = m_histos->regHist(histDir+"tag_jetfitcomb_w",m_h_tag_jetfitcomb_w);
  sc = m_histos->regHist(histDir+"tag_jetfitternn_w",m_h_tag_jetfitternn_w);
  sc = m_histos->regHist(histDir+"tag_jetfitcombnn_w",m_h_tag_jetfitcombnn_w);
  //*
  sc = m_histos->regHist(histDir+"tag_i2d_of_w",m_h_tag_i2d_of_w);
  sc = m_histos->regHist(histDir+"tag_i3d_of_w",m_h_tag_i3d_of_w);
  sc = m_histos->regHist(histDir+"tag_i2d_l2_w",m_h_tag_i2d_l2_w);
  sc = m_histos->regHist(histDir+"tag_i3d_l2_w",m_h_tag_i3d_l2_w);
  sc = m_histos->regHist(histDir+"tag_i2d_ef_w",m_h_tag_i2d_ef_w);
  sc = m_histos->regHist(histDir+"tag_i3d_ef_w",m_h_tag_i3d_ef_w);
  //
  for(int i=0; i<6; ++i) {
    TString htit(m_jetET[i]);
    htit.ToLower();

    std::string htit2 = htit.Data(); htit2+="_tag_IPinfo_ntrk";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_ntrk[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_trkPt";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_trkPt[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_d0val";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_d0val[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_z0val";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_z0val[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_d0sig";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_d0sig[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_z0sig";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_z0sig[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_weight2D";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_weight2D[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_weight3D";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_weight3D[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_piJP";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_piJP[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_fromV0";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_fromV0[i]);

    htit2 = htit.Data(); htit2+="_tag_IPinfo_grade";
    sc = m_histos->regHist(histDir+htit2,m_h_tag_IPinfo_grade[i]);
  }
  //
  for(int i=0;i<MAX_numTaggers;i++) {
    TString tit("perf_"); tit+=m_tagger[i]; tit.ToLower();
    std::string tit2 = tit.Data(); tit2+="_b";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_b[i]);
    tit2 = tit.Data(); tit2+="_u";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_u[i]);
    tit2 = tit.Data(); tit2+="_upur";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_upur[i]);
    //
    tit2=tit.Data(); tit2+="_b_ET";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_b_ET[i]);    
    tit2=tit.Data(); tit2+="_b_eta";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_b_eta[i]);    
    tit2=tit.Data(); tit2+="_b_phi";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_b_phi[i]);    
    tit2=tit.Data(); tit2+="_u_ET";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_u_ET[i]);    
    tit2=tit.Data(); tit2+="_u_eta";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_u_eta[i]);    
    tit2=tit.Data(); tit2+="_u_phi";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_u_phi[i]);    
    tit2=tit.Data(); tit2+="_upur_ET";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_upur_ET[i]);    
    tit2=tit.Data(); tit2+="_upur_eta";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_upur_eta[i]);    
    tit2=tit.Data(); tit2+="_upur_phi";
    sc = m_histos->regHist(histDir+tit2,m_h_perf_upur_phi[i]);    
    //
    TString titA("eff_"); titA+=m_tagger[i]; titA.ToLower();
    tit2=titA.Data(); tit2+="_b_ET";
    sc = m_histos->regHist(histDir+tit2,m_h_eff_b_ET[i]);    
    tit2=titA.Data(); tit2+="_b_eta";
    sc = m_histos->regHist(histDir+tit2,m_h_eff_b_eta[i]);    
    tit2=titA.Data(); tit2+="_b_phi";
    sc = m_histos->regHist(histDir+tit2,m_h_eff_b_phi[i]);    
    //
    TString titB("rej_"); titB+=m_tagger[i]; titB.ToLower();
    tit2=titB.Data(); tit2+="_u_ET";
    sc = m_histos->regHist(histDir+tit2,m_h_rej_u_ET[i]);    
    tit2=titB.Data(); tit2+="_u_eta";
    sc = m_histos->regHist(histDir+tit2,m_h_rej_u_eta[i]);    
    tit2=titB.Data(); tit2+="_u_phi";
    sc = m_histos->regHist(histDir+tit2,m_h_rej_u_phi[i]);    
    //
    tit2=titB.Data(); tit2+="_upur_ET";
    sc = m_histos->regHist(histDir+tit2,m_h_rej_upur_ET[i]);    
    tit2=titB.Data(); tit2+="_upur_eta";
    sc = m_histos->regHist(histDir+tit2,m_h_rej_upur_eta[i]);    
    tit2=titB.Data(); tit2+="_upur_phi";
    sc = m_histos->regHist(histDir+tit2,m_h_rej_upur_phi[i]);    

  }
  sc = m_histos->regHist(histDir+"perf_vxeff_den_b",m_h_perf_vxeff_den_b);
  sc = m_histos->regHist(histDir+"perf_vxeff_num_b",m_h_perf_vxeff_num_b);
  sc = m_histos->regHist(histDir+"perf_vxeff_den_u",m_h_perf_vxeff_den_u);
  sc = m_histos->regHist(histDir+"perf_vxeff_num_u",m_h_perf_vxeff_num_u);
  sc = m_histos->regHist(histDir+"perf_vxeff_den_upur",m_h_perf_vxeff_den_upur);
  sc = m_histos->regHist(histDir+"perf_vxeff_num_upur",m_h_perf_vxeff_num_upur);
  sc = m_histos->regHist(histDir+"perf_rej10",m_h_perf_rej10);
  sc = m_histos->regHist(histDir+"perf_rej50",m_h_perf_rej50);
  sc = m_histos->regHist(histDir+"perf_rej60",m_h_perf_rej60);
  sc = m_histos->regHist(histDir+"perf_rejpur10",m_h_perf_rejpur10);
  sc = m_histos->regHist(histDir+"perf_rejpur50",m_h_perf_rejpur50);
  sc = m_histos->regHist(histDir+"perf_rejpur60",m_h_perf_rejpur60);
  sc = m_histos->regHist(histDir+"perfWt_10",m_h_perfWt_10);
  sc = m_histos->regHist(histDir+"perfWt_50",m_h_perfWt_50);
  sc = m_histos->regHist(histDir+"perfWt_60",m_h_perfWt_60);
  sc = m_histos->regHist(histDir+"perfWt_pur10",m_h_perfWt_pur10);
  sc = m_histos->regHist(histDir+"perfWt_pur50",m_h_perfWt_pur50);
  sc = m_histos->regHist(histDir+"perfWt_pur60",m_h_perfWt_pur60);
  // in case a tuple is written
  if (m_UseTupleSET) {
    p_nt1 = new TTree("JetTagAna","JetTagAna");
    std::string TreeDir("/fileJetTagAna/JetTagAna");
    sc = m_histos->regTree(TreeDir,p_nt1);
    if (sc.isSuccess()) {
      p_nt1->Branch("event",        &m_ievt,         "event/I");
      p_nt1->Branch("run",          &m_irun,         "run/I");
      p_nt1->Branch("njetse",       &m_njet,         "njetse/I");
      p_nt1->Branch("jetse_phi",    &m_jet_phi,      "jetse_phi[njetse]/F");
      p_nt1->Branch("jetse_eta",    &m_jet_eta,      "jetse_eta[njetse]/F");
      p_nt1->Branch("jetse_et",     &m_jet_et,       "jetse_et[njetse]/F");
      p_nt1->Branch("jetse_flav",   &m_jet_flav,     "jetse_flav[njetse]/I");
      p_nt1->Branch("jetse_isol",   &m_jet_isol,     "jetse_isol[njetse]/I");  
      p_nt1->Branch("jetse_nelej",  &m_nelej,        "jetse_nelej[njetse]/I"); 
      p_nt1->Branch("jetse_ntrackj",&m_ntrackj,      "jetse_ntrackj[njetse]/I");
      p_nt1->Branch("jetse_nmuonj", &m_nmuonj,       "jetse_nmuonj[njetse]/I");
      //
      p_nt1->Branch("jetse_ne",   &m_jet_ne,   "jetse_ne[njetse]/I");
      p_nt1->Branch("jetse_eptr", &m_jet_eptr, "jetse_eptr[njetse]/F");
      p_nt1->Branch("jetse_ea0",  &m_jet_ea0,  "jetse_ea0[njetse]/F");
      p_nt1->Branch("jetse_ept",  &m_jet_ept,  "jetse_ept[njetse]/F");
      
      p_nt1->Branch("jetse_ew",       &m_jet_ew,       "jetse_ew[njetse]/F");
      p_nt1->Branch("jetse_elh",      &m_jet_elh,      "jetse_elh[njetse]/F");
      p_nt1->Branch("jetse_eTrueEle", &m_jet_eTrueEle, "jetse_eTrueEle[njetse]/I");     
      p_nt1->Branch("jetse_eAnyEle",  &m_jet_eAnyEle,  "jetse_eAnyEle[njetse]/I"); 
      
      p_nt1->Branch("jetse_epb", &m_jet_epb, "jetse_epb[njetse]/F");
      p_nt1->Branch("jetse_epu", &m_jet_epu, "jetse_epu[njetse]/F");
      
      p_nt1->Branch("jetse_tag_pid",     &m_jet_tag_pid,     "jetse_tag_pid[njetse]/I");
      p_nt1->Branch("jetse_tag_mothpid", &m_jet_tag_mothpid, "jetse_tag_mothpid[njetse]/I");
      p_nt1->Branch("jetse_trackEle",    &m_jet_trackEle,    "jetse_trackEle[njetse][10]/I");
      p_nt1->Branch("jetse_NtrackEle",   &m_jet_NtrackEle,   "jetse_NtrackEle[njetse]/I");
      p_nt1->Branch("jetse_trackElept",  &m_jet_trackElept,  "jetse_trackElept[njetse][10]/F");
      p_nt1->Branch("jetse_Ntrack",      &m_jet_Ntrack,      "jetse_Ntrack[njetse]/I");
      p_nt1->Branch("jetse_sumptTrack",  &m_jet_sumptTrack,  "jetse_sumptTrack[njetse]/F");
     
    } 
  }


  if(sc.isFailure()) {}; 
}

bool JetTagAna::isBHadron(int pdg) {
  int mpdg = abs(pdg);
  return (   ( 500 < mpdg && mpdg < 599 )   ||
           ( 10500 < mpdg && mpdg < 10599 ) ||
           (  5000 < mpdg && mpdg < 5999  ) ||
           ( 20500 < mpdg && mpdg < 20599 ) );
}

bool JetTagAna::isDHadron(int pdg) {
  int mpdg = abs(pdg);
  return (   ( 400 < mpdg && mpdg < 499 )   || 
           ( 10400 < mpdg && mpdg < 10499 ) ||
           (  4000 < mpdg && mpdg < 4999  ) ||
           ( 20400 < mpdg && mpdg < 20499 ) );
}

bool JetTagAna::isLightJetIsolated(const CLHEP::HepLorentzVector& p) {
  if(0==m_mcpartTES) return false;
  TruthParticleContainer::const_iterator mcItr  = (*m_mcpartTES).begin();
  TruthParticleContainer::const_iterator mcEnd = (*m_mcpartTES).end();
  bool isol = true;
  for (; mcItr != mcEnd; ++mcItr) {
    if( p.deltaR( (*mcItr)->hlv() ) < m_purificationDeltaR ) {
      int mpdg = abs( (*mcItr)->pdgId() );
      if( this->isBHadron(mpdg) || 
	  this->isDHadron(mpdg) ||
	  5==mpdg ||
	  4==mpdg ||
	  15==mpdg ) {
	isol = false;
	break;
      }
    }
  }
  return isol;
}

bool JetTagAna::isJetFarFromHPTLepton(const Jet* jet) {
  const Analysis::SoftLeptonTruthInfo* sltinfo = jet->tagInfo<Analysis::SoftLeptonTruthInfo>("SoftLeptonTruthInfo");
  if (sltinfo) {
    int nslt = sltinfo->numSLTrueInfo();
    ATH_MSG_VERBOSE ( "SoftLeptonTruthInfo exists. Found " << nslt << " true leptons in jet" );
    for (int islt = 0; islt < nslt; islt++) {
      const Analysis::SLTrueInfo slt = sltinfo->getSLTrueInfo(islt);
      ATH_MSG_VERBOSE ( "SLT info " << slt.pdgId() 
                        << " " << slt.momentum().perp() 
                        << " " << slt.FromB() << " " << slt.FromD() << " " << slt.FromGH()
                        );
      if ( (abs(slt.pdgId()) == 13 || abs(slt.pdgId()) == 11 || abs(slt.pdgId()) == 15 ) && // Electron Or Muon from direct decay of W/Z/H
           !(slt.FromB()) &&
           !(slt.FromD()) &&
           (abs(slt.pdgIdMother())<100) && // not from light hadron decay-in-flight
           slt.FromGH()
           ) 
        {
          auto eigenv = AmgVector(3) { jet->hlv().x(), jet->hlv().y(), jet->hlv().z() };

          if( slt.momentum().deltaR( eigenv ) < m_leptonPurificationDeltaR ) {
            return false;
          } 
      }
    }
  }
  else {
    ATH_MSG_DEBUG("Could not find SoftLeptonTruthInfo: no lepton purification can be done." );
  } 
  return true;
}

// =========================================================
double JetTagAna::getTotWeight(double epro,double ppro)
{
 double totalpro;

 if ( ppro > 0. )
   totalpro = epro/ppro;
 else if (epro > 0. ) // epro > 0 ppro == 0
   totalpro = 1.0e20;
 else // both == 0
   totalpro = 0.;

 // calculate log  (safely)
 if ( totalpro <= 1.0e-18 ) {
    totalpro = -39.99;
 } else {
    totalpro = log(totalpro);
 }

 return  totalpro;
}

// ===============================================
const HepMC::GenParticle* JetTagAna::getTruth(const Rec::TrackParticle* myTrackParticle,
					      const TrackParticleTruthCollection* mcpartTES,
					      const Rec::TrackParticleContainer* trackTES)
{
  //this method returns pointer to truth particle linked to given track

 const HepMC::GenParticle* GenPart =0;

 ElementLink<Rec::TrackParticleContainer> trackPrtlink;
 trackPrtlink.setElement(const_cast<Rec::TrackParticle*>(myTrackParticle));
 trackPrtlink.setStorableObject(*trackTES);
 //typedef std::map<ElementLink<Rec::TrackParticleContainer>,TrackParticleTruth> truthMap;

 std::map<Rec::TrackParticleTruthKey,TrackParticleTruth>::const_iterator tempTrackPrtTruthItr = mcpartTES->find(trackPrtlink);

 //truthMap::const_iterator tempTrackPrtTruthItr;
 //tempTrackPrtTruthItr = mcpartTES->find(trackPrtlink);
 if (tempTrackPrtTruthItr != mcpartTES->end())
 {
   const HepMcParticleLink & temHepMcLink =
                           (*tempTrackPrtTruthItr).second.particleLink();
   long theBarcode=temHepMcLink.barcode();
   if (theBarcode!=0) GenPart = temHepMcLink.cptr();
 }


  return GenPart;
}
