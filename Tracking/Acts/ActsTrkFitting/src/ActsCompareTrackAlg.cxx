/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsCompareTrackAlg.h"

#include "Gaudi/Property.h"  /*no forward decl: typedef*/

#include "TrkTrack/TrackCollection.h"

#include "TROOT.h"
#include "TTree.h"
#include "TH1F.h"
#include "TString.h"

#include "GaudiKernel/ITHistSvc.h"

// ATHENA
#include "GaudiKernel/ListItem.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"

// STL
#include <vector>
#include <fstream>


ActsCompareTrackAlg::ActsCompareTrackAlg(const std::string &name,
                                         ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_atlasTrackName("ResolvedTracks_TrackParticle"),
      m_actsTrackName("ReFitted_Tracks_TrackParticle"),
      m_validationTreeName("Track_Comparison"),
      m_validationTreeDescription("Comparison of ATLAS tracks and refitted Acts tracks"),
      m_validationTreeFolder("/val/TrackComp"),
      m_validationTree(nullptr)
{
  // Get parameter values from jobOptions file
  declareProperty("ATLASTrackName", m_atlasTrackName, "collection name for ATLAS tracks");
  declareProperty("ActsTrackName", m_actsTrackName, "collection name for refitted acts tracks");
}

StatusCode ActsCompareTrackAlg::initialize() {

  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);
  m_treeData = std::make_unique<TreeData>();
  
  // create the new Tree
  m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());

  m_validationTree->Branch("Tracknb",   &m_treeData->m_track_nb,  "tracknb/I");

  m_validationTree->Branch("TrkD0",      m_treeData->m_trk_d0,    "trkd0[tracknb]/F");
  m_validationTree->Branch("TrkZ0",      m_treeData->m_trk_z0,    "trkz0[tracknb]/F");
  m_validationTree->Branch("TrkPhi",     m_treeData->m_trk_phi,   "trkPhi[tracknb]/F");
  m_validationTree->Branch("TrkTheta",   m_treeData->m_trk_theta, "trkTheta[tracknb]/F");
  m_validationTree->Branch("TrkQoverP",  m_treeData->m_trk_q_p,   "trkTheta[tracknb]/F");

  m_validationTree->Branch("TrkSigmaD0",      m_treeData->m_trk_sigma_d0,    "trkSigmad0[tracknb]/F");
  m_validationTree->Branch("TrkSigmaZ0",      m_treeData->m_trk_sigma_z0,    "trkSigmaz0[tracknb]/F");
  m_validationTree->Branch("TrkSigmaPhi",     m_treeData->m_trk_sigma_phi,   "trkSigmaPhi[tracknb]/F");
  m_validationTree->Branch("TrkSigmaTheta",   m_treeData->m_trk_sigma_theta, "trkSigmaTheta[tracknb]/F");
  m_validationTree->Branch("TrkSigmaQoverP",  m_treeData->m_trk_sigma_q_p,   "trkSigmaTheta[tracknb]/F");

  m_validationTree->Branch("TrkHits",        m_treeData->m_trk_pixelhits,     "TrkHits[tracknb]/I");
  m_validationTree->Branch("TrkHoles",       m_treeData->m_trk_pixelholes,    "TrkHoles[tracknb]/I");
  m_validationTree->Branch("TrkOutliers",    m_treeData->m_trk_pixeloutliers, "TrkOutliers[tracknb]/I");
  m_validationTree->Branch("TrkSCTHits",     m_treeData->m_trk_scthits,       "TrkSCTHits[tracknb]/I");
  m_validationTree->Branch("TrkSCTHoles",    m_treeData->m_trk_sctholes,      "TrkSCTHoles[tracknb]/I");
  m_validationTree->Branch("TrkSCTOutliers", m_treeData->m_trk_sctoutliers,   "TrkSCTOutliers[tracknb]/I");

  m_validationTree->Branch("TrkX",       m_treeData->m_trk_x,     "trkX[tracknb]/F");
  m_validationTree->Branch("TrkY",       m_treeData->m_trk_y,     "trkY[tracknb]/F");
  m_validationTree->Branch("TrkZ",       m_treeData->m_trk_z,     "trkZ[tracknb]/F");
  m_validationTree->Branch("TrkEta",     m_treeData->m_trk_eta,   "trkEta[tracknb]/F");
  m_validationTree->Branch("TrkPT",      m_treeData->m_trk_pt,    "trkPt[tracknb]/F");

  m_validationTree->Branch("ActsD0",     m_treeData->m_acts_d0,     "actsd0[tracknb]/F");
  m_validationTree->Branch("ActsZ0",     m_treeData->m_acts_z0,     "actsz0[tracknb]/F");
  m_validationTree->Branch("ActsPhi",    m_treeData->m_acts_phi,    "actsPhi[tracknb]/F");
  m_validationTree->Branch("ActsTheta",  m_treeData->m_acts_theta,  "actsTheta[tracknb]/F");
  m_validationTree->Branch("ActsQOverP", m_treeData->m_acts_q_p,    "actsEta[tracknb]/F");

  m_validationTree->Branch("ActsSigmaD0",     m_treeData->m_acts_sigma_d0,     "actsSigmad0[tracknb]/F");
  m_validationTree->Branch("ActsSigmaZ0",     m_treeData->m_acts_sigma_z0,     "actsSigmaz0[tracknb]/F");
  m_validationTree->Branch("ActsSigmaPhi",    m_treeData->m_acts_sigma_phi,    "actsSigmaPhi[tracknb]/F");
  m_validationTree->Branch("ActsSigmaTheta",  m_treeData->m_acts_sigma_theta,  "actsSigmaTheta[tracknb]/F");
  m_validationTree->Branch("ActsSigmaQOverP", m_treeData->m_acts_sigma_q_p,    "actsSigmaEta[tracknb]/F");

  m_validationTree->Branch("ActsHits",        m_treeData->m_acts_pixelhits,     "actsHits[tracknb]/I");
  m_validationTree->Branch("ActsHoles",       m_treeData->m_acts_pixelholes,    "actsHoles[tracknb]/I");
  m_validationTree->Branch("ActsOutliers",    m_treeData->m_acts_pixeloutliers, "actsOutliers[tracknb]/I");
  m_validationTree->Branch("ActsSCTHits",     m_treeData->m_acts_scthits,       "actsSCTHits[tracknb]/I");
  m_validationTree->Branch("ActsSCTHoles",    m_treeData->m_acts_sctholes,      "actsSCTHoles[tracknb]/I");
  m_validationTree->Branch("ActsSCTOutliers", m_treeData->m_acts_sctoutliers,   "actsSCTOutliers[tracknb]/I");

  m_validationTree->Branch("ActsX",      m_treeData->m_acts_x,     "actsX[tracknb]/F");
  m_validationTree->Branch("ActsY",      m_treeData->m_acts_y,     "actsY[tracknb]/F");
  m_validationTree->Branch("ActsZ",      m_treeData->m_acts_z,     "actsZ[tracknb]/F");
  m_validationTree->Branch("ActsEta",    m_treeData->m_acts_eta,   "actsEta[tracknb]/F");
  m_validationTree->Branch("ActsPT",     m_treeData->m_acts_pt,    "actsPt[tracknb]/F");

  m_diff_d0 = new TH1F("Diff_d0", "Diff_d0  ;(d0_{trk}-d0_{acts})  ;Number of entries", 100, -0.2, 0.2);
  m_diff_z0 = new TH1F("Diff_z0", "Diff_z0  ;(z0_{trk}-z0_{acts})  ;Number of entries", 100, -0.2, 0.2);
  m_diff_phi = new TH1F("Diff_phi", "Diff_phi  ;(#phi_{trk}-#phi_{acts})  ;Number of entries", 100, -0.01, 0.01);
  m_diff_theta = new TH1F("Diff_theta", "Diff_theta  ;(#theta_{trk}-#theta_{acts})  ;Number of entries", 100, -0.002, 0.002);
  m_diff_q_p = new TH1F("Diff_q_p", "Diff_q_p  ;(qOp_{trk}-qOp_{acts})  ;Number of entries", 100, -0.001, 0.001);

  m_sigma_d0 = new TH1F("Sigma_d0_diff", "Sigma_d0_diff  ;(#sigma_{d0_{trk}}-#sigma_{d0_{acts}})/#sigma_{d0_{trk}}  ;Number of entries", 100, -1, 1);
  m_sigma_z0 = new TH1F("Sigma_z0_diff", "Sigma_z0_diff  ;(#sigma_{z0_{trk}}-#sigma_{z0_{acts}})/#sigma_{z0_{trk}}  ;Number of entries", 100, -1, 1);
  m_sigma_phi = new TH1F("Sigma_phi_diff", "Sigma_phi_diff  ;(#sigma_{phi_{trk}}-#sigma_{phi_{acts}})/#sigma_{phi_{trk}}  ;Number of entries", 100, -1, 1);
  m_sigma_theta = new TH1F("Sigma_theta_diff", "Sigma_theta_diff  ;(#sigma_{theta_{trk}}-#sigma_{theta_{acts}})/#sigma_{theta_{trk}}  ;Number of entries", 100, -1, 1);
  m_sigma_q_p = new TH1F("Sigma_q_p_diff", "Sigma_q_p_diff  ;(#sigma_{qOp_{trk}}-#sigma_{qOp_{acts}})/#sigma_{qOp_{trk}}  ;Number of entries", 100, -1, 1);

  // now register the Tree
  ITHistSvc* tHistSvc = 0;
  if (service("THistSvc",tHistSvc).isFailure()){
    ATH_MSG_ERROR( "Could not find Hist Service -> Switching ValidationMode Off !" );
    delete m_validationTree; m_validationTree = 0;
  }
  if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the validation Tree -> Switching ValidationMode Off !" );
    delete m_validationTree; m_validationTree = 0;
  }
  if ((tHistSvc->regHist("/val/Diff_d0", m_diff_d0)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the diff d0 hist" );
  }
    if ((tHistSvc->regHist("/val/Diff_z0", m_diff_z0)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the diff z0 hist" );
  }
    if ((tHistSvc->regHist("/val/Diff_phi", m_diff_phi)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the diff phi hist" );
  }
    if ((tHistSvc->regHist("/val/Diff_theta", m_diff_theta)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the diff theta hist" );
  }
    if ((tHistSvc->regHist("/val/Diff_q_p", m_diff_q_p)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the diff q over p hist" );
  }
    if ((tHistSvc->regHist("/val/Sigma_d0_diff", m_sigma_d0)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the sigma d0 hist" );
  }
    if ((tHistSvc->regHist("/val/Sigma_z0_diff", m_sigma_z0)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the sigma z0 hist" );
  }
    if ((tHistSvc->regHist("/val/Sigma_phi_diff", m_sigma_phi)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the sigma phi hist" );
  }
    if ((tHistSvc->regHist("/val/Sigma_theta_diff", m_sigma_theta)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the signa theta hist" );
  }
    if ((tHistSvc->regHist("/val/Sigma_q_p_diff", m_sigma_q_p)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the sigma q over p hist" );
  }

  m_cov_comparison = new TH1F*[25];
  for(int j=0; j<5; j++){
    for(int i=0; i<5; i++){
      int pos = i+j*5;
      std::string covariance = "Covariance_" + std::to_string(i) + "_" + std::to_string(j);
      m_cov_comparison[pos] = new TH1F(TString(covariance), TString(covariance + " ;(" + covariance + "_{trk}-" + covariance + "_{acts})/" + covariance + "_{trk} ;Number of entries"), 100, -10, 10);
      if ((tHistSvc->regHist("/val/"+covariance, m_cov_comparison[pos])).isFailure()) {
        ATH_MSG_ERROR( "Could not register the covariance hist" );
      }
    }
  } 

  ATH_MSG_INFO("initialize() successful" );
  return StatusCode::SUCCESS;
}

StatusCode ActsCompareTrackAlg::execute() {
  ATH_MSG_DEBUG ("ActsCompareTrackAlg::execute()");
  const xAOD::TrackParticleContainer*  atlasTracks = evtStore()->tryConstRetrieve<xAOD::TrackParticleContainer>(m_atlasTrackName );
  const xAOD::TrackParticleContainer*  actsTracks = evtStore()->tryConstRetrieve<xAOD::TrackParticleContainer>(m_actsTrackName );

  if (!atlasTracks){
    msg(MSG::ERROR) <<"Track collection named " << m_atlasTrackName
		    << " not found, exit ReFitTrack." << endmsg;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ("Tracks collection '" << m_atlasTrackName << "' retrieved from EventStore.");
  }

  if (!actsTracks){
    msg(MSG::ERROR) <<"Track collection named " << m_actsTrackName
		    << " not found, exit ReFitTrack." << endmsg;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ("Tracks collection '" << m_actsTrackName << "' retrieved from EventStore.");
  }

  if(atlasTracks->size() != actsTracks->size()){
    msg(MSG::ERROR) <<"Different number of ATLAS and Acts tracks skiping event" << endmsg;
    return StatusCode::SUCCESS;
  }

  m_treeData->m_track_nb = 0;
  auto actsTr = actsTracks->begin();
  auto atlasTr = atlasTracks->begin();


  while(actsTr != actsTracks->end() && atlasTr != atlasTracks->end()){

    auto covAtlasDiag = (*atlasTr)->definingParametersCovMatrixDiagVec();
    auto covActsDiag = (*actsTr)->definingParametersCovMatrixDiagVec();

    auto covAtlas = (*atlasTr)->definingParametersCovMatrix();
    auto covActs = (*actsTr)->definingParametersCovMatrix();

    m_treeData->m_trk_d0[m_treeData->m_track_nb]    =  (*atlasTr)->d0();
    m_treeData->m_trk_z0[m_treeData->m_track_nb]    =  (*atlasTr)->z0();
    m_treeData->m_trk_phi[m_treeData->m_track_nb]   =  (*atlasTr)->phi();
    m_treeData->m_trk_theta[m_treeData->m_track_nb] =  (*atlasTr)->theta();
    m_treeData->m_trk_q_p[m_treeData->m_track_nb]   =  (*atlasTr)->qOverP();

    m_treeData->m_trk_sigma_d0[m_treeData->m_track_nb]    =  sqrt(covAtlasDiag.at(0));
    m_treeData->m_trk_sigma_z0[m_treeData->m_track_nb]    =  sqrt(covAtlasDiag.at(1));
    m_treeData->m_trk_sigma_phi[m_treeData->m_track_nb]   =  sqrt(covAtlasDiag.at(2));
    m_treeData->m_trk_sigma_theta[m_treeData->m_track_nb] =  sqrt(covAtlasDiag.at(3));
    m_treeData->m_trk_sigma_q_p[m_treeData->m_track_nb]   =  sqrt(covAtlasDiag.at(4));
      
    uint8_t trk_pixelhits = 0;
    uint8_t trk_pixelholes = 0;
    uint8_t trk_pixeloutliers = 0;
    uint8_t trk_scthits = 0;
    uint8_t trk_sctholes = 0;
    uint8_t trk_sctoutliers = 0;

    (*atlasTr)->summaryValue(trk_pixelhits, xAOD::numberOfPixelHits);
    (*atlasTr)->summaryValue(trk_pixelholes, xAOD::numberOfPixelHoles);
    (*atlasTr)->summaryValue(trk_pixeloutliers, xAOD::numberOfPixelOutliers);
    (*atlasTr)->summaryValue(trk_scthits, xAOD::numberOfSCTHits);
    (*atlasTr)->summaryValue(trk_sctholes, xAOD::numberOfSCTHoles);
    (*atlasTr)->summaryValue(trk_sctoutliers, xAOD::numberOfSCTOutliers);

    m_treeData->m_trk_pixelhits[m_treeData->m_track_nb] = trk_pixelhits;
    m_treeData->m_trk_pixelholes[m_treeData->m_track_nb] = trk_pixelholes;
    m_treeData->m_trk_pixeloutliers[m_treeData->m_track_nb] = trk_pixeloutliers;
    m_treeData->m_trk_scthits[m_treeData->m_track_nb] = trk_scthits;
    m_treeData->m_trk_sctholes[m_treeData->m_track_nb] = trk_sctholes;
    m_treeData->m_trk_sctoutliers[m_treeData->m_track_nb] = trk_sctoutliers;

    m_treeData->m_trk_x[m_treeData->m_track_nb]     =  (*atlasTr)->vx();
    m_treeData->m_trk_y[m_treeData->m_track_nb]     =  (*atlasTr)->vy();
    m_treeData->m_trk_z[m_treeData->m_track_nb]     =  (*atlasTr)->vz();
    m_treeData->m_trk_eta[m_treeData->m_track_nb]   =  (*atlasTr)->eta();
    m_treeData->m_trk_pt[m_treeData->m_track_nb]    =  (*atlasTr)->pt();

    m_treeData->m_acts_d0[m_treeData->m_track_nb]    =  (*actsTr)->d0();
    m_treeData->m_acts_z0[m_treeData->m_track_nb]    =  (*actsTr)->z0();
    m_treeData->m_acts_phi[m_treeData->m_track_nb]   =  (*actsTr)->phi();
    m_treeData->m_acts_theta[m_treeData->m_track_nb] =  (*actsTr)->theta();
    m_treeData->m_acts_q_p[m_treeData->m_track_nb]   =  (*actsTr)->qOverP();

    m_treeData->m_acts_sigma_d0[m_treeData->m_track_nb]    =  sqrt(covActsDiag.at(0));
    m_treeData->m_acts_sigma_z0[m_treeData->m_track_nb]    =  sqrt(covActsDiag.at(1));
    m_treeData->m_acts_sigma_phi[m_treeData->m_track_nb]   =  sqrt(covActsDiag.at(2));
    m_treeData->m_acts_sigma_theta[m_treeData->m_track_nb] =  sqrt(covActsDiag.at(3));
    m_treeData->m_acts_sigma_q_p[m_treeData->m_track_nb]   =  sqrt(covActsDiag.at(4));

    uint8_t acts_pixelhits = 0;
    uint8_t acts_pixelholes = 0;
    uint8_t acts_pixeloutliers = 0;
    uint8_t acts_scthits = 0;
    uint8_t acts_sctholes = 0;
    uint8_t acts_sctoutliers = 0;

    (*actsTr)->summaryValue(acts_pixelhits, xAOD::numberOfPixelHits);
    (*actsTr)->summaryValue(acts_pixelholes, xAOD::numberOfPixelHoles);
    (*actsTr)->summaryValue(acts_pixeloutliers, xAOD::numberOfPixelOutliers);
    (*actsTr)->summaryValue(acts_scthits, xAOD::numberOfSCTHits);
    (*actsTr)->summaryValue(acts_sctholes, xAOD::numberOfSCTHoles);
    (*actsTr)->summaryValue(acts_sctoutliers, xAOD::numberOfSCTOutliers);

    m_treeData->m_acts_pixelhits[m_treeData->m_track_nb] = acts_pixelhits;
    m_treeData->m_acts_pixelholes[m_treeData->m_track_nb] = acts_pixelholes;
    m_treeData->m_acts_pixeloutliers[m_treeData->m_track_nb] = acts_pixeloutliers;
    m_treeData->m_acts_scthits[m_treeData->m_track_nb] = acts_scthits;
    m_treeData->m_acts_sctholes[m_treeData->m_track_nb] = acts_sctholes;
    m_treeData->m_acts_sctoutliers[m_treeData->m_track_nb] = acts_sctoutliers;

    m_treeData->m_acts_x[m_treeData->m_track_nb]     =  (*actsTr)->vx();
    m_treeData->m_acts_y[m_treeData->m_track_nb]     =  (*actsTr)->vy();
    m_treeData->m_acts_z[m_treeData->m_track_nb]     =  (*actsTr)->vz();
    m_treeData->m_acts_eta[m_treeData->m_track_nb]   =  (*actsTr)->eta();
    m_treeData->m_acts_pt[m_treeData->m_track_nb]    =  (*actsTr)->pt();

    m_diff_d0->Fill(((*atlasTr)->d0() - (*actsTr)->d0()));
    m_diff_z0->Fill(((*atlasTr)->z0() - (*actsTr)->z0()));
    m_diff_phi->Fill(((*atlasTr)->phi() - (*actsTr)->phi()));
    m_diff_theta->Fill(((*atlasTr)->theta() - (*actsTr)->theta()));
    m_diff_q_p->Fill(((*atlasTr)->qOverP() - (*actsTr)->qOverP()));

    m_sigma_d0->Fill( (sqrt(covAtlasDiag.at(0)) - sqrt(covActsDiag.at(0)))/sqrt(covAtlasDiag.at(0)) );
    m_sigma_z0->Fill( (sqrt(covAtlasDiag.at(1)) - sqrt(covActsDiag.at(1)))/sqrt(covAtlasDiag.at(1)) );
    m_sigma_phi->Fill( (sqrt(covAtlasDiag.at(2)) - sqrt(covActsDiag.at(2)))/sqrt(covAtlasDiag.at(2)) );
    m_sigma_theta->Fill( (sqrt(covAtlasDiag.at(3)) - sqrt(covActsDiag.at(3)))/sqrt(covAtlasDiag.at(3)) );
    m_sigma_q_p->Fill( (sqrt(covAtlasDiag.at(4)) - sqrt(covActsDiag.at(4)))/sqrt(covAtlasDiag.at(4)) );

    for(int j=0; j<5; j++){
      for(int i=0; i<5; i++){
        int pos = i+j*5;
        m_cov_comparison[pos]->Fill( (covAtlas(i,j) - covActs(i,j))/covAtlas(i,j) );
      }
    } 

    ++m_treeData->m_track_nb;
    actsTr++;
    atlasTr++;

  }

  // fill the validation tree
  m_validationTree->Fill();

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode ActsCompareTrackAlg::finalize()
{
  msg(MSG::INFO) << "ActsCompareTrackAlg::finalize()" << endmsg;
  return StatusCode::SUCCESS;
}
