/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSecVertexValidation/InDetSecVertexTruthMatchTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"

#include "InDetSecVertexValidation/InDetSecVertexTruthMatchUtils.h"
using namespace InDetSecVertexTruthMatchUtils;

InDetSecVertexTruthMatchTool::InDetSecVertexTruthMatchTool( const std::string & name ) : asg::AsgTool(name) {
  declareProperty("trackMatchProb", m_trkMatchProb = 0.5 );
  declareProperty("vertexMatchWeight", m_vxMatchWeight = 0.5 );
  declareProperty("trackPtCut", m_trkPtCut = 1000. );
  declareProperty("pdgIds", m_pdgIds = "36" );
  declareProperty("fillHist", m_fillHist = true );
}

StatusCode InDetSecVertexTruthMatchTool::initialize() {
  ATH_MSG_INFO("Initializing");

  // build the vector of pdgIds from the input string
  std::stringstream ss(m_pdgIds);
  for (int i; ss >> i;) {
      m_pdgIdList.push_back(i);    
      if (ss.peek() == ',')
          ss.ignore();
  }

  // histograms
  ATH_CHECK( service("THistSvc",m_thistSvc) );
  m_matchType                 = new TH1F("matchType","Vertex Match Type",8,-0.5,7.5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_matchtype", m_matchType));

  m_positionRes_R             = new TH1F("positionRes_r","Position resolution for vertices matched to truth LLP decays",40,-10,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/positionRes_R", m_positionRes_R));
  m_positionRes_Z             = new TH1F("positionRes_z","Position resolution for vertices matched to truth LLP decays",40,-10,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/positionRes_Z", m_positionRes_Z));

  m_truthInclusive_r          = new TH1F("truthInclusive_r","Reconstructable Truth Vertices",6000,0,600); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthInclusive_r", m_truthInclusive_r));
  m_truthReconstructable_r    = new TH1F("truthReconstructable_r","Truth Vertices in detector acceptance",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthReconstructable_r", m_truthReconstructable_r));
  m_truthAccepted_r    = new TH1F("truthAccepted_r","Truth Vertices in detector acceptance",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthAccepted_r", m_truthAccepted_r));
  m_truthSeeded_r             = new TH1F("truthSeeded_r","Seedable Truth Vertices",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthSeeded_r", m_truthSeeded_r));
  m_truthReconstructed_r      = new TH1F("truthReconstructed_r","Vertex with Match Score > 0.5",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthReconstructed_r", m_truthReconstructed_r));
  m_seedPosition_r            = new TH1F("seedPosition_r","Seed Positions ",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_seedPosition_r", m_seedPosition_r));
  m_seedPosition_z            = new TH1F("seedPosition_z","Seed Positions ",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_seedPosition_z", m_seedPosition_z));
  m_seedBadPosition_r         = new TH1F("seedBadPosition_r","Seed Positions ",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_seedBadPosition_r", m_seedBadPosition_r));
  m_seedBadPosition_z         = new TH1F("seedBadPosition_z","Seed Positions ",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_seedBadPosition_z", m_seedBadPosition_z));
  m_seedAllPosition_r         = new TH1F("seedAllPosition_r","Seed Positions ",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_seedAllPosition_r", m_seedAllPosition_r));
  m_seedAllPosition_z         = new TH1F("seedAllPosition_z","Seed Positions ",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_seedAllPosition_z", m_seedAllPosition_z));
  m_truthReconstructable_trkSel = new TH1F("m_truthReconstructable_trkSel","Seedable Truth Vertices",10,0,30); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthReconstructable_trkSel", m_truthReconstructable_trkSel));
  m_truthReconstructed_trkSel = new TH1F("truthReconstructed","Vertex with Match Score > 0.5",10,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/truthReconstructed_trkSel", m_truthReconstructed_trkSel));

  //// plots concearning Reco vetices and their tracks
  m_recoX                     = new TH1F("m_recoX","Reco vertex x [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoX", m_recoX));
  m_recoY                     = new TH1F("m_recoY","Reco vertex y [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoY", m_recoY));
  m_recoZ                     = new TH1F("m_recoZ","Reco vertex z [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoZ", m_recoZ));
  m_recoR                     = new TH1F("m_recoR","Reco vertex r [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoR", m_recoR));
  m_recoR_LLP                 = new TH1F("m_recoR_LLP","Reco vertex r [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoR_LLP", m_recoR_LLP));
  m_recoR_Fake                = new TH1F("m_recoR_Fake","Reco vertex r [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoR_Fake", m_recoR_Fake));
  m_recoR_Clean               = new TH1F("m_recoR_Clean","Reco vertex r [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoR_Clean", m_recoR_Clean));
  m_recoR_Split               = new TH1F("m_recoR_Split","Reco vertex r [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoR_Split", m_recoR_Split));
  m_recoR_NonLLP              = new TH1F("m_recoR_NonLLP","Reco vertex r [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoR_NonLLP", m_recoR_NonLLP));
  m_recodistFromPV            = new TH1F("m_recodistFromPV","Reco vertex distFromPV [mm]",500,0,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recodistFromPV", m_recodistFromPV));
  m_recoPt                    = new TH1F("m_recoPt","Reco vePttex Pt [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoPt", m_recoPt));
  m_recoEta                   = new TH1F("m_recoEta","Reco veEtatex Eta ",100,-5,5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoEta", m_recoEta));
  m_recoPhi                   = new TH1F("m_recoPhi","Reco vePhitex Phi ",64,-3.2,3.2);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoPhi", m_recoPhi));
  m_recoMass                  = new TH1F("m_recoMass","Reco vertex Mass [GeV]",500,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMass", m_recoMass));
  m_recoMass_LLP              = new TH1F("m_recoMass_LLP","Reco vertex Mass [GeV]",500,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMass_LLP", m_recoMass_LLP));
  m_recoMass_Fake             = new TH1F("m_recoMass_Fake","Reco vertex Mass [GeV]",500,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMass_Fake", m_recoMass_Fake));
  m_recoMu                    = new TH1F("m_recoMu","Reco vertex Red. Mass [GeV]",500,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMu", m_recoMu));
  m_recoMu_LLP                = new TH1F("m_recoMu_LLP","Reco vertex Red. Mass [GeV]",500,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMu_LLP", m_recoMu_LLP));
  m_recoMu_Fake               = new TH1F("m_recoMu_Fake","Reco vertex Red. Mass [GeV]",500,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMu_Fake", m_recoMu_Fake));
  m_yield                     = new TH1F("m_yield_","Pass selection",2,-0.5,1.5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_yield", m_yield));
  m_yield_LLP                 = new TH1F("m_yield_LLP","Pass selection",2,-0.5,1.5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_yield_LLP", m_yield_LLP));
  m_yield_Fake                = new TH1F("m_yield_Fake","Pass selection",2,-0.5,1.5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_yield_Fake", m_yield_Fake));
  m_recoChi2                  = new TH1F("m_recoChi2","Reco vertex recoChi2",100,0,10); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoChi2", m_recoChi2));
  m_recoDir                   = new TH1F("m_recoDir","Reco vertex recoDirection",100,-1,1);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoDir", m_recoDir));
  m_recoCharge                = new TH1F("m_recoCharge","Reco vertex recoCharge",20,-10,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoCharge", m_recoCharge));
  m_recoH                     = new TH1F("m_recoH","Reco vertex H [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoH", m_recoH));
  m_recoHt                    = new TH1F("m_recoHt","Reco vertex Mass [GeV]",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoHt", m_recoHt));
  m_recoMinOpAng              = new TH1F("m_recoMinOpAng","Reco vertex minOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMinOpAng", m_recoMinOpAng));
  m_recoMaxOpAng              = new TH1F("m_recoMaxOpAng","Reco vertex MaxOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMaxOpAng", m_recoMaxOpAng));
  m_recoMaxDR                 = new TH1F("m_recoMaxDR","Reco vertex maxDR",100,0,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMaxDR", m_recoMaxDR));
  m_recoMinD0                 = new TH1F("m_recoMinD0","Reco vertex min d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMinD0", m_recoMinD0));
  m_recoMaxD0                 = new TH1F("m_recoMaxD0","Reco vertex max d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoMaxD0", m_recoMaxD0));
  m_recoNtrk                  = new TH1F("m_recoNtrk","Reco vertex n tracks",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoNtrk", m_recoNtrk));
  m_truth_Ntrk                = new TH1F("m_truth_Ntrk","Truth vertex n track pass tracks",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truth_Ntrk", m_truth_Ntrk));
  m_recoNtrk_LLP              = new TH1F("m_recoNtrk_LLP","Reco vertex n tracks",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoNtrk_LLP", m_recoNtrk_LLP));
  m_recoNtrk_Fake             = new TH1F("m_recoNtrk_Fake","Reco vertex n tracks",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoNtrk_Fake", m_recoNtrk_Fake));

  //////reco tracks
  m_recoTrk_qOverP            = new TH1F("m_recoTrk_qOverP","Reco track qOverP ",100,0,1);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_qOverP", m_recoTrk_qOverP));
  m_recoTrk_theta             = new TH1F("m_recoTrk_theta","Reco track theta ",64,0,3.2);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_theta", m_recoTrk_theta));
  m_recoTrk_E                 = new TH1F("m_recoTrk_E","Reco track E ",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_E", m_recoTrk_E));    
  m_recoTrk_M                 = new TH1F("m_recoTrk_M","Reco track M ",100,0,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_M", m_recoTrk_M));
  m_recoTrk_Pt                = new TH1F("m_recoTrk_Pt","Reco track Pt ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_Pt", m_recoTrk_Pt));
  m_recoTrk_Px                = new TH1F("m_recoTrk_Px","Reco track Px ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_Px", m_recoTrk_Px));
  m_recoTrk_Py                = new TH1F("m_recoTrk_Py","Reco track Py ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_Py", m_recoTrk_Py));
  m_recoTrk_Pz                = new TH1F("m_recoTrk_Pz","Reco track Pz ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_Pz", m_recoTrk_Pz));
  m_recoTrk_Eta               = new TH1F("m_recoTrk_Eta","Reco track Eta ",100,-5,5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_Eta", m_recoTrk_Eta));
  m_recoTrk_Phi               = new TH1F("m_recoTrk_Phi","Reco track Phi ",63,-3.2,3.2); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_Phi", m_recoTrk_Phi));
  m_recoTrk_D0                = new TH1F("m_recoTrk_D0","Reco track D0 ",300,-300,300); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_D0", m_recoTrk_D0));
  m_recoTrk_Z0                = new TH1F("m_recoTrk_Z0","Reco track Z0 ",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_Z0", m_recoTrk_Z0));
  m_recoTrk_errD0             = new TH1F("m_recoTrk_errD0","Reco track errD0 ",300,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_errD0", m_recoTrk_errD0));
  m_recoTrk_errZ0             = new TH1F("m_recoTrk_errZ0","Reco track errZ0 ",500,0,50);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_errZ0", m_recoTrk_errZ0));
  m_recoTrk_Chi2              = new TH1F("m_recoTrk_Chi2","Reco track Chi2 ",100,0,10); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_Chi2", m_recoTrk_Chi2));
  m_recoTrk_nDoF              = new TH1F("m_recoTrk_nDoF","Reco track nDoF ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_nDoF", m_recoTrk_nDoF));
  m_recoTrk_charge            = new TH1F("m_recoTrk_charge","Reco track charge ",3,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_recoTrk_charge", m_recoTrk_charge));

  //////////// matched truth vertex
  m_matchScore                = new TH1F("matchScore","Vertex Match Score",100,0,1);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_matchScore", m_matchScore));
  m_matchedTruthID            = new TH1F("matchedTruthID","Vertex Match Score",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_matchedTruthID", m_matchedTruthID));
  m_matchedTruthX             = new TH1F("matchedTruthX","Vertex Match Score",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_matchedTruthX", m_matchedTruthX));
  m_matchedTruthY             = new TH1F("matchedTruthY","Vertex Match Score",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_matchedTruthY", m_matchedTruthY));
  m_matchedTruthZ             = new TH1F("matchedTruthZ","Vertex Match Score",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_matchedTruthZ", m_matchedTruthZ));
  m_matchedTruthR             = new TH1F("matchedTruthR","Vertex Match Score",500,0,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_matchedTruthR", m_matchedTruthR));
  m_matchedTruthEta           = new TH1F("matchedTruthEta","Vertex Match Score",100,-5,5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_matchedTruthEta", m_matchedTruthEta));
  m_matchedTruthPhi           = new TH1F("matchedTruthPhi","Vertex Match Score",64,-3.2,3.2);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_matchedTruthPhi", m_matchedTruthPhi));

  ///////truth vertex plots
  m_truthX                    = new TH1F("m_truthX","truth vertex x [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthX", m_truthX));
  m_truthY                    = new TH1F("m_truthY","truth vertex y [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthY", m_truthY));
  m_truthZ                    = new TH1F("m_truthZ","truth vertex z [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthZ", m_truthZ));
  m_truthR                    = new TH1F("m_truthR","truth vertex r [mm]",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthR", m_truthR));
  m_truthdistFromPV           = new TH1F("m_truthdistFromPV","truth vertex distFromPV [mm]",500,0,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthdistFromPV", m_truthdistFromPV));
  m_truthEta                  = new TH1F("m_truthEta","truth veEtatex Eta ",100,-5,5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthEta", m_truthEta));
  m_truthPhi                  = new TH1F("m_truthPhi","truth vePhitex Phi ",64,-3.2,3.2); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthPhi", m_truthPhi));
  m_truthNtrk_out             = new TH1F("m_truthNtrk_out","truth vertex n outgoing tracks",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthNtrk_out", m_truthNtrk_out));
  m_truthParent_E             = new TH1F("m_truthParent_E","Reco track E ",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParent_E", m_truthParent_E));    
  m_truthParent_M             = new TH1F("m_truthParent_M","Reco track M ",100,0,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParent_M", m_truthParent_M));
  m_truthParent_Pt            = new TH1F("m_truthParent_Pt","Reco track Pt ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParent_Pt", m_truthParent_Pt));
  m_truthParent_Eta           = new TH1F("m_truthParent_Eta","Reco track Eta ",100,-5,5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParent_Eta", m_truthParent_Eta));
  m_truthParent_Phi           = new TH1F("m_truthParent_Phi","Reco track Phi ",63,-3.2,3.2); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParent_Phi", m_truthParent_Phi));
  m_truthParent_charge        = new TH1F("m_truthParent_charge","Reco track charge ",3,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParent_charge", m_truthParent_charge));
  m_truthParentProdX          = new TH1F("m_truthParentProdX","truthParentProd vertex x [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParentProdX", m_truthParentProdX));
  m_truthParentProdY          = new TH1F("m_truthParentProdY","truthParentProd vertex y [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParentProdY", m_truthParentProdY));
  m_truthParentProdZ          = new TH1F("m_truthParentProdZ","truthParentProd vertex z [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParentProdZ", m_truthParentProdZ));
  m_truthParentProdR          = new TH1F("m_truthParentProdR","truthParentProd vertex r [mm]",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParentProdR", m_truthParentProdR));
  m_truthParentProddistFromPV = new TH1F("m_truthParentProddistFromPV","truthParentProd vertex distFromPV [mm]",500,0,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParentProddistFromPV", m_truthParentProddistFromPV));
  m_truthParentProdEta        = new TH1F("m_truthParentProdEta","truthParentProd veEtatex Eta ",100,-5,5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParentProdEta", m_truthParentProdEta));
  m_truthParentProdPhi        = new TH1F("m_truthParentProdPhi","truthParentProd vePhitex Phi ",64,-3.2,3.2); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/m_truthParentProdPhi", m_truthParentProdPhi));

  return StatusCode::SUCCESS;
}

StatusCode InDetSecVertexTruthMatchTool::finalize()
{
  return StatusCode::SUCCESS;
}

namespace {
//Helper methods for this file only

//In the vector of match info, find the element corresponding to link and return its index; create a new one if necessary
size_t indexOfMatchInfo( std::vector<VertexTruthMatchInfo> & matches, const ElementLink<xAOD::TruthVertexContainer> & link ) {
  for ( size_t i = 0; i < matches.size(); ++i ) {
    if ( link.key() == std::get<0>(matches[i]).key() && link.index() == std::get<0>(matches[i]).index() )
      return i;
  }
  // This is the first time we've seen this truth vertex, so make a new entry
  matches.push_back(  std::make_tuple( link, 0., 0. ) );
  return matches.size() - 1;
}

}

StatusCode InDetSecVertexTruthMatchTool::matchVertices( const xAOD::VertexContainer & vtxContainer,
                                                        const xAOD::TruthVertexContainer & truthVtxContainer) const {

  ATH_MSG_DEBUG("Start vertex matching");


  /*
  ATH_MSG_DEBUG("Starting Loop on Truth Vertices");

  std::map<int, std::vector<xAOD::Vertex*>> truthRecoMap;
  for(const xAOD::TruthVertex* truthVtx : truthVtxContainer) {
    // only consider 1->N decay vertices
    if(truthVtx->nIncomingParticles() != 1) continue;

    const xAOD::TruthParticle* truthPart = truthVtx->incomingParticle(0);
    // skip if invalid incoming particle link
    if(not truthPart) continue;
    // skip if the vertex is not an LLP decay
    if(std::find(m_pdgIdList.begin(), m_pdgIdList.end(), truthPart->pdgId()) == m_pdgIdList.end()) continue;

    truthRecoMap[truthPart->barcode()] = std::vector<xAOD::Vertex*>();
  }
  */

  //setup decorators for truth matching info
  static const xAOD::Vertex::Decorator<std::vector<VertexTruthMatchInfo> > matchInfoDecor("TruthVertexMatchingInfos");
  static const xAOD::Vertex::Decorator<VertexMatchType> matchTypeDecor("VertexMatchType");
  static const xAOD::Vertex::Decorator<std::vector<ElementLink<xAOD::VertexContainer> > > splitPartnerDecor("SplitPartners");
  static const xAOD::Vertex::Decorator<ElementLink<xAOD::TruthVertexContainer> > backLinkDecor("RecoToTruthLink");

  //setup accessors
  // can switch to built in method in xAOD::Vertex once don't have to deal with changing names anymore
  xAOD::Vertex::ConstAccessor<xAOD::Vertex::TrackParticleLinks_t> trkAcc("trackParticleLinks");
  xAOD::Vertex::ConstAccessor<std::vector<float> > weightAcc("trackWeights");

  xAOD::TrackParticle::ConstAccessor<ElementLink<xAOD::TruthParticleContainer> > trk_truthPartAcc("truthParticleLink");
  xAOD::TrackParticle::ConstAccessor<float> trk_truthProbAcc("truthMatchProbability");

  //some variables to store
  size_t ntracks;
  xAOD::VxType::VertexType vtxType;

  ATH_MSG_DEBUG("Starting Loop on Vertices");

  //=============================================================================
  //First loop over vertices: get tracks, then TruthParticles, and store relative
  //weights from each TruthVertex
  //=============================================================================
  size_t vtxEntry = 0;
  unsigned int n_bad_links = 0;
  unsigned int n_links = 0;
  unsigned int n_vx_with_bad_links = 0;

  for ( const xAOD::Vertex* vtx : vtxContainer ) {

    vtxEntry++;
    ATH_MSG_DEBUG("Matching vertex number: " << vtxEntry << ".");

    vtxType = static_cast<xAOD::VxType::VertexType>( vtx->vertexType() );

    if(vtxType != xAOD::VxType::SecVtx ){
      ATH_MSG_DEBUG("Vertex not labeled as secondary");
      continue;
    }

    //create the vector we will add as matching info decoration later
    std::vector<VertexTruthMatchInfo> matchinfo;

    const xAOD::Vertex::TrackParticleLinks_t & trkParts = trkAcc( *vtx );
    ntracks = trkParts.size();
    const std::vector<float> & trkWeights = weightAcc( *vtx );

    //if don't have track particles
    if (!trkAcc.isAvailable(*vtx) || !weightAcc.isAvailable(*vtx) ) {
      ATH_MSG_WARNING("trackParticles or trackWeights not available, vertex is missing info");
      continue;
    }
    if ( trkWeights.size() != ntracks ) {
      ATH_MSG_WARNING("Vertex without same number of tracks and trackWeights, vertex is missing info");
      continue;
    }

    ATH_MSG_DEBUG("Matching new vertex at (" << vtx->x() << ", " << vtx->y() << ", " << vtx->z() << ")" << " with " << ntracks << " tracks, at index: " << vtx->index());

    float totalWeight = 0.;
    float totalFake = 0.;

    float totalPt = 0; 
    float otherPt = 0;
    float fakePt = 0;

    unsigned vx_n_bad_links = 0;
    //loop over the tracks in the vertex
    for ( size_t t = 0; t < ntracks; ++t ) {

      ATH_MSG_DEBUG("Checking track number " << t);

      if (!trkParts[t].isValid()) {
         ++vx_n_bad_links;
         ATH_MSG_DEBUG("Track " << t << " is bad!");
         continue;
      }
      const xAOD::TrackParticle & trk = **trkParts[t];

      // store the contribution to total weight and pT
      totalWeight += trkWeights[t];
      totalPt += trk.pt();

      // get the linked truth particle
      const ElementLink<xAOD::TruthParticleContainer> & truthPartLink = trk_truthPartAcc( trk );
      float prob = trk_truthProbAcc( trk );
      ATH_MSG_DEBUG("Truth prob: " << prob);

      // check the truth particle origin
      if (truthPartLink.isValid()  and prob > m_trkMatchProb) {
        const xAOD::TruthParticle & truthPart = **truthPartLink;

        int barcode = -1;
        barcode = checkProduction(truthPart);

        //check if the truth particle is "good"
        if ( barcode != -1 ) {
          //track in vertex is linked to LLP descendant
          //create link to truth vertex and add to matchInfo
          auto it = std::find_if(truthVtxContainer.begin(), truthVtxContainer.end(), 
            [&](const auto& ele){ return ele->barcode() == barcode;} );

          if(it == truthVtxContainer.end()) {
            ATH_MSG_DEBUG("Truth vertex with barcode " << barcode << " not found!");
          }

          // get index for the elementLink
          size_t index = it - truthVtxContainer.begin();

          const ElementLink<xAOD::TruthVertexContainer> elLink = ElementLink<xAOD::TruthVertexContainer>( truthVtxContainer, index );

          size_t matchIdx = indexOfMatchInfo( matchinfo, elLink );

          std::get<1>(matchinfo[matchIdx]) += trkWeights[t];
          std::get<2>(matchinfo[matchIdx]) += trk.pt();
        } else {
          //truth particle failed cuts -> add to other
          otherPt += trk.pt();
        }
      } else {
        //not valid or low matching probability -> add to fakes
        ATH_MSG_DEBUG("Invalid or low prob truth link!");
        totalFake += trkWeights[t];
        fakePt += trk.pt();
      }
    }//end loop over tracks in vertex

    n_links     += ntracks;
    n_bad_links += vx_n_bad_links;
    if (vx_n_bad_links>0) {
       ++n_vx_with_bad_links;
    }

    // TODO: normalize by total pT
    matchInfoDecor( *vtx ) = matchinfo;
  }

  //After first loop, all vertices have been decorated with their vector of match info (link to TruthVertex paired with weight)
  //now we want to use that information from the whole collection to assign types
  //keep track of whether a type is assigned
  //useful since looking for splits involves a double loop, and then setting types ahead in the collection
  std::vector<bool> assignedType( vtxContainer.size(), false );
  static const xAOD::TruthVertex::Decorator<bool> isMatched("VertexMatchedToTruth");

  for ( size_t i = 0; i < vtxContainer.size(); ++i ) {

    if ( assignedType[i] ) {
      ATH_MSG_DEBUG("Vertex already defined as split.");
      if(m_fillHist) {
        ATH_CHECK( fillRecoPlots( *vtxContainer[i] ) );
      }
      continue; // make sure we don't reclassify vertices already found in the split loop below
    }

    std::vector<VertexTruthMatchInfo> & info = matchInfoDecor( *vtxContainer[i] );

    if (info.size() == 0) {                                // no true decay vertices matched 
      ATH_MSG_DEBUG("No true decay vertices matched. Vertex is fake.");
      matchTypeDecor( *vtxContainer[i] ) = FAKE;
    } else if (info.size() >= 2 ) {                        // more than one true deacy vertices matched
      ATH_MSG_DEBUG("Multiple true decay vertices matched. Vertex is merged.");
      matchTypeDecor( *vtxContainer[i] ) = MERGED;
      std::for_each( info.begin(), info.end(), [](VertexTruthMatchInfo& link)
      {
        isMatched(**std::get<0>(link)) = true;
      });
    } else if ( std::get<1>(info[0]) > m_vxMatchWeight ) { // one truth matched vertex, sufficient weight
      ATH_MSG_DEBUG("One true decay vertices matched. Vertex is matched.");
      matchTypeDecor( *vtxContainer[i] ) = MATCHED;
      isMatched(**std::get<0>(info[0])) = true;
    } else {                                               // one truth matched vertex, insufficient weight 
      ATH_MSG_DEBUG("One true decay vertices matched with insufficient score. Vertex is fake.");
      matchTypeDecor( *vtxContainer[i] ) = FAKE;
    }

    //check for splitting
    //TODO: decorate linked truth vertices with isSplit
    if ( matchTypeDecor( *vtxContainer[i] ) == MATCHED || matchTypeDecor( *vtxContainer[i] ) == MERGED ) {
      std::vector<size_t> foundSplits;
      for ( size_t j = i + 1; j < vtxContainer.size(); ++j ) {
        std::vector<VertexTruthMatchInfo> & info2 = matchInfoDecor( *vtxContainer[j] );
        //check second vertex is not dummy or fake, and that it has same elementlink as first vertex
        //equality test is in code but doesnt seem to work for ElementLinks that I have?
        //so i am just checking that the contianer key hash and the index are the same
        if (matchTypeDecor( *vtxContainer[j] ) == FAKE) continue;
        if (info2.size() > 0 && std::get<0>(info2[0]).isValid() && std::get<0>(info[0]).key() == std::get<0>(info2[0]).key() && std::get<0>(info[0]).index() == std::get<0>(info2[0]).index() ) {
          //add split links; first between first one found and newest one
          splitPartnerDecor( *vtxContainer[i] ).push_back( ElementLink<xAOD::VertexContainer>( vtxContainer, j ) );
          splitPartnerDecor( *vtxContainer[j] ).push_back( ElementLink<xAOD::VertexContainer>( vtxContainer, i ) );
          //then between any others we found along the way
          for ( auto k : foundSplits ) { //k is a size_t in the vector of splits
            splitPartnerDecor( *vtxContainer[k] ).push_back( ElementLink<xAOD::VertexContainer>( vtxContainer, j ) );
            splitPartnerDecor( *vtxContainer[j] ).push_back( ElementLink<xAOD::VertexContainer>( vtxContainer, k ) );
          }
          //then keep track that we found this one
          foundSplits.push_back(j);
          matchTypeDecor( *vtxContainer[i] ) = SPLIT;
          matchTypeDecor( *vtxContainer[j] ) = SPLIT;
          assignedType[j] = true;
        } //if the two vertices match to same TruthVertex
      }//inner loop over vertices
    } //if matched or merged

    if(m_fillHist) {
      ATH_CHECK( fillRecoPlots( *vtxContainer[i] ) );
    }
  } //outer loop

  return StatusCode::SUCCESS;

}

StatusCode InDetSecVertexTruthMatchTool::labelTruthVertices( const xAOD::TruthVertexContainer & truthVtxContainer) const {
  
  ATH_MSG_DEBUG("Labeling truth vertices");

  const xAOD::TrackParticleContainer * trackPartCont = nullptr;
  const xAOD::TrackParticleContainer * largeD0TrackPartCont = nullptr;

  ATH_CHECK( evtStore()->retrieve( trackPartCont, "InDetTrackParticles" ) );
  if ( evtStore()->contains<xAOD::TrackParticleContainer>( "InDetLargeD0TrackParticles" ) )
    ATH_CHECK( evtStore()->retrieve( largeD0TrackPartCont, "InDetLargeD0TrackParticles" ) );
  else
    ATH_MSG_WARNING("No LRT container in input! Using standard tracks only.");


  static const xAOD::Vertex::Decorator<TruthVertexMatchType> matchTypeDecor("TruthVertexMatchType");
  xAOD::TruthVertex::Decorator<bool> isMatched("VertexMatchedToTruth");

  for(const xAOD::TruthVertex* truthVtx : truthVtxContainer) {

    if(truthVtx->nIncomingParticles() != 1){continue;}
    const xAOD::TruthParticle* truthPart = truthVtx->incomingParticle(0);
    if(not truthPart) continue;
    if(std::find(m_pdgIdList.begin(), m_pdgIdList.end(), std::abs(truthPart->pdgId())) == m_pdgIdList.end()) continue;
    
    ATH_MSG_DEBUG("Analysing Truth Vertex " << truthVtx->barcode() );
    std::vector<const xAOD::TruthParticle*> reconstructibleParticles;
    countReconstructibleDescendentParticles( *truthVtx, reconstructibleParticles );

    std::vector<int> particleInfo = {0,0,0}; // binary for 
    std::vector<int> vertexInfo = {0,0,0};

    for(size_t n = 0; n < reconstructibleParticles.size(); n++){
      ATH_MSG_DEBUG("Checking daughter no. " << n);
      const xAOD::TruthParticle* outPart = reconstructibleParticles.at(n);
      
      // TODO: here we are double counting
      // need to rethink how we do this
      if (trackPartCont){
        particleInfo = checkParticle(*outPart, *trackPartCont);
        ATH_MSG_DEBUG(particleInfo);
      
        for(size_t h = 0; h < particleInfo.size(); h++){
          vertexInfo.at(h) += particleInfo.at(h);
        }
      }
      if (largeD0TrackPartCont){
        particleInfo = checkParticle(*outPart, *largeD0TrackPartCont);
        ATH_MSG_DEBUG(particleInfo);
      
        for(size_t h = 0; h < particleInfo.size(); h++){
          vertexInfo.at(h) += particleInfo.at(h);
        }
      }
    }
      
    ATH_MSG_DEBUG("Info for this LLP decay: " << vertexInfo);

    matchTypeDecor(*truthVtx) = INCLUSIVE;
    if( vertexInfo.at(0) > 1 &&  truthVtx->perp() <  563 && abs(truthVtx->z()) < 2720){
      ATH_MSG_DEBUG("Vertex is reconstructable and in Inner Det region");
      matchTypeDecor(*truthVtx) = RECONSTRUCTABLE;
    }
    if( vertexInfo.at(1) > 1){
      ATH_MSG_DEBUG("Vertex is Reconstructable");
      matchTypeDecor(*truthVtx) = ACCEPTED;
    }
    if(vertexInfo.at(2) > 1){
      ATH_MSG_DEBUG("Vertex is has at least two tracks passing track selection: " << vertexInfo.at(2));
      matchTypeDecor(*truthVtx) = SEEDED;
    }
    if(isMatched(*truthVtx)){
      ATH_MSG_DEBUG("Vertex is matched to a reconstructed secVtx");
      matchTypeDecor(*truthVtx) = RECONSTRUCTED;
    }

    if(m_fillHist) {
      ATH_CHECK( fillTruthPlots(*truthVtx) );
    }

  }
  ATH_MSG_DEBUG("Done labeling truth vertices");

  return StatusCode::SUCCESS;
}

std::vector<int> InDetSecVertexTruthMatchTool::checkParticle(const xAOD::TruthParticle &truthPart, const xAOD::TrackParticleContainer &trkCont) const {

  xAOD::TrackParticle::ConstAccessor<char>  trackPass("is_selected");
  xAOD::TrackParticle::ConstAccessor<ElementLink<xAOD::TruthParticleContainer> > trk_truthPartAcc("truthParticleLink");
  xAOD::TrackParticle::ConstAccessor<float> trk_truthProbAcc("truthMatchProbability");

  if(truthPart.pt() < m_trkPtCut){
    ATH_MSG_INFO("Insufficient pt to reconstruct the particle");
    return {0,0,0};
  }
  else{
    bool found = false;

    for(const xAOD::TrackParticle* trkPart : trkCont){
      const ElementLink<xAOD::TruthParticleContainer> & truthPartLink = trk_truthPartAcc( *trkPart );
      float matchProb = trk_truthProbAcc( *trkPart );

      if(truthPartLink.isValid() && matchProb > m_trkMatchProb) {
        const xAOD::TruthParticle& tmpPart = **truthPartLink;
        if(tmpPart.barcode() == truthPart.barcode()) {
          found = true;
          if(trackPass.isAvailable( *trkPart ) and trackPass( *trkPart )) {
            ATH_MSG_DEBUG("Particle has a track that passes track selection.");
            return {1,1,1};
          }
          else {
            ATH_MSG_DEBUG("Particle has a track, but did not pass track selection.");
            return {1,1,0};
          }
        }
      }
    }
    if(!found){
      ATH_MSG_DEBUG("Particle has enough pt.");
      return {1,0,0};
    }
    
  }
  return {0,0,0};
}

StatusCode InDetSecVertexTruthMatchTool::fillRecoPlots( const xAOD::Vertex& secVtx ) const {
  
  // set of accessors for tracks and weights
  xAOD::Vertex::ConstAccessor<xAOD::Vertex::TrackParticleLinks_t> trkAcc("trackParticleLinks");
  xAOD::Vertex::ConstAccessor<std::vector<float> > weightAcc("trackWeights");
  
  // set of decorators for truth matching info
  static const xAOD::Vertex::Decorator<VertexMatchType> matchTypeDecor("VertexMatchType");
  static const xAOD::Vertex::Decorator<std::vector<VertexTruthMatchInfo> > matchInfoDecor("TruthVertexMatchInfo");

  m_matchType->Fill(matchTypeDecor(secVtx));

  m_recoX->Fill(secVtx.x());
  m_recoY->Fill(secVtx.y());
  m_recoZ->Fill(secVtx.z());

  TVector3 reco_pos(secVtx.x(), secVtx.y(), secVtx.z());
  float reco_r = reco_pos.Perp();

  m_recoR->Fill(reco_r);

  size_t ntracks;
  const xAOD::Vertex::TrackParticleLinks_t & trkParts = trkAcc( secVtx );
  ntracks = trkParts.size();

  TLorentzVector sumP4(0,0,0,0);
  double maxDR = 0.0;
  double H = 0.0;
  double HT = 0.0;
  int charge = 0;
  double MinOpAng = -1.0* 1.e10;
  double MaxOpAng = -1.0* 1.e10;
  double minD0;
  double maxD0;

  xAOD::TrackParticle::ConstAccessor< std::vector< float > > accCovMatrixDiag( "definingParametersCovMatrixDiag" );
  m_recoNtrk->Fill(ntracks);

  ATH_MSG_DEBUG("Loop over tracks");
  for(size_t t = 0; t < ntracks; t++){
    const xAOD::TrackParticle & trk = **trkParts[t];

    m_recoTrk_errD0->Fill(trk.definingParametersCovMatrix()(0,0));
    m_recoTrk_errZ0->Fill(trk.definingParametersCovMatrix()(1,1));
    double trk_d0 = trk.definingParameters()[0];
    double trk_z0 = trk.definingParameters()[1];

    m_recoTrk_qOverP->Fill(trk.definingParameters()[4]);
    m_recoTrk_theta->Fill(trk.definingParameters()[3]);
    m_recoTrk_E->Fill(trk.e());
    m_recoTrk_M->Fill(trk.m());
    m_recoTrk_Pt->Fill(trk.pt());

    m_recoTrk_Px->Fill(trk.p4().Px());
    m_recoTrk_Py->Fill(trk.p4().Py());
    m_recoTrk_Pz->Fill(trk.p4().Pz());

    m_recoTrk_Eta->Fill(trk.eta());
    m_recoTrk_Phi->Fill(trk.phi0());

    m_recoTrk_D0->Fill(trk_d0);
    m_recoTrk_Z0->Fill(trk_z0);

    if (t==0){
      minD0 = trk_d0;
      maxD0 = trk_d0;
    } else{
      if(minD0 > trk_d0){minD0 = trk_d0;}
      if(maxD0 < trk_d0){maxD0 = trk_d0;}
    }

    TLorentzVector vv;
    vv.SetPtEtaPhiM(trk.pt(),trk.eta(), trk.phi0(), trk.m());
    sumP4 += vv;
    H += vv.Vect().Mag();
    HT += vv.Pt();

    TLorentzVector v_minus_iv(0,0,0,0);
    for(size_t j = 0; j < ntracks; j++){
      const xAOD::TrackParticle & trk_2 = **trkParts[j];

      TLorentzVector tmp;
      tmp.SetPtEtaPhiM(trk_2.pt(),trk_2.eta(), trk_2.phi0(), trk_2.m());

      double tm = vv * tmp / ( vv.Mag() * tmp.Mag() );
      if( MinOpAng < tm ) MinOpAng = tm;
      if( MaxOpAng > tm ) MaxOpAng = tm;
      if (trk_2.definingParameters() == trk.definingParameters()){ continue; }
      v_minus_iv += tmp;
    }

    double DR = vv.DeltaR(v_minus_iv);
    if( DR > maxDR ){ maxDR = DR;}

    charge += trk.charge();
    m_recoTrk_charge->Fill(trk.charge());

    xAOD::TrackParticle::ConstAccessor<float> Trk_Chi2("chiSquared");
    xAOD::TrackParticle::ConstAccessor<float> Trk_nDoF("numberDoF");

    if ( Trk_Chi2(trk) && Trk_nDoF(trk) )  {
      m_recoTrk_Chi2->Fill(trk.chiSquared() / trk.numberDoF());
      m_recoTrk_nDoF->Fill(trk.numberDoF()); 
    }
  } // end loop over tracks

  m_recoPt->Fill(sumP4.Pt());
  m_recoEta->Fill(sumP4.Eta());
  m_recoPhi->Fill(sumP4.Phi());
  m_recoMass->Fill(sumP4.M() / 1000);
  m_recoMu->Fill(sumP4.M()/maxDR / 1000);

  // fill the conditional hists
  // TODO: fill ntrk,R,mu,mass for all categories
  if(matchTypeDecor(secVtx) == MATCHED)   m_recoR_Clean  ->Fill(reco_r);
  if(matchTypeDecor(secVtx) == SPLIT)     m_recoR_Split  ->Fill(reco_r);
  if(matchTypeDecor(secVtx) == FAKE) {
    m_recoNtrk_Fake ->Fill(ntracks);
    m_recoR_Fake    ->Fill(reco_r);
    m_recoMass_Fake ->Fill(sumP4.M() / 1000);
    m_recoMu_Fake   ->Fill(sumP4.M()/maxDR / 1000);
  }
  if(matchTypeDecor(secVtx) == MATCHED or 
     matchTypeDecor(secVtx) == SPLIT) {
    m_recoNtrk_LLP ->Fill(ntracks);
    m_recoR_LLP    ->Fill(reco_r);
    m_recoMass_LLP ->Fill(sumP4.M() / 1000);
    m_recoMu_LLP   ->Fill(sumP4.M()/maxDR / 1000);
  }

  if(ntracks > 2 and (sumP4.M()/maxDR / 1000) > 3) {
    m_yield ->Fill(1.0);

    if(matchTypeDecor(secVtx) == MATCHED or 
       matchTypeDecor(secVtx) == SPLIT) {

      m_yield_LLP ->Fill(1.0);
    }
    if(matchTypeDecor(secVtx) == FAKE) {
      m_yield_Fake ->Fill(1.0);
    }

  }
  else {
    m_yield ->Fill(0.0);

    if(matchTypeDecor(secVtx) == MATCHED or 
       matchTypeDecor(secVtx) == SPLIT) {

      m_yield_LLP ->Fill(0.0);
    }
    if(matchTypeDecor(secVtx) == FAKE) {
      m_yield_Fake ->Fill(0.0);
    }
  }

  xAOD::Vertex::ConstAccessor<float> Chi2("chiSquared");
  xAOD::Vertex::ConstAccessor<float> nDoF("numberDoF");

  m_recoChi2->Fill(Chi2(secVtx)/nDoF(secVtx));
  const double dir  = sumP4.Vect().Dot( reco_pos ) / sumP4.Vect().Mag() / reco_pos.Mag();
  m_recoDir->Fill(dir);
  m_recoCharge->Fill(charge);
  m_recoH->Fill(H);
  m_recoHt->Fill(HT);
  m_recoMinOpAng->Fill(MinOpAng);
  m_recoMaxOpAng->Fill(MaxOpAng); 
  
  std::vector<VertexTruthMatchInfo> truthmatchinfo;
  truthmatchinfo = matchInfoDecor(secVtx);

  if(not truthmatchinfo.empty()){
    float prob = std::get<2>(truthmatchinfo.at(0));
    float matchScore = std::get<1>(truthmatchinfo.at(0));

    m_matchScore->Fill(matchScore);

    ATH_MSG_DEBUG("Match Score and probability: " << matchScore << " " << prob/0.01);

    const ElementLink<xAOD::TruthVertexContainer>& truthVertexLink = std::get<0>(truthmatchinfo.at(0));
    const xAOD::TruthVertex& truthVert = **truthVertexLink ;
    xAOD::TruthVertex::Decorator<int> isReconstructed("VertexReconstructed");

    TVector3 truth_pos(truthVert.x(), truthVert.y(), truthVert.z());

    m_matchedTruthX->Fill(truthVert.x());
    m_matchedTruthY->Fill(truthVert.y());
    m_matchedTruthZ->Fill(truthVert.z());

    float truth_r = truthVert.perp();
    m_matchedTruthR->Fill(truth_r);

    m_matchedTruthEta->Fill(truthVert.eta());
    m_matchedTruthPhi->Fill(truthVert.phi());

    //float pos_r = (reco_pos - truth_pos).Perp();
    //float pos_z = (reco_pos - truth_pos).z();
    //int n_trkSel = isReconstructed(truthVert);

    m_positionRes_R->Fill(reco_r - truth_r);
    m_positionRes_Z->Fill(secVtx.z() - truthVert.z());
  }
  
  return StatusCode::SUCCESS;
}

StatusCode InDetSecVertexTruthMatchTool::fillTruthPlots( const xAOD::TruthVertex& truthVtx) const {
  
  ATH_MSG_DEBUG("Plotting truth vertex");

  m_truthX->Fill(truthVtx.x());
  m_truthY->Fill(truthVtx.y());
  m_truthZ->Fill(truthVtx.z());
  m_truthR->Fill(truthVtx.perp());
  m_truthEta->Fill(truthVtx.eta());
  m_truthPhi->Fill(truthVtx.phi());
  m_truthNtrk_out->Fill(truthVtx.nOutgoingParticles());

  ATH_MSG_DEBUG("Plotting truth parent");
  const xAOD::TruthParticle& truthPart = *truthVtx.incomingParticle(0);

  m_truthParent_E->Fill(truthPart.e());
  m_truthParent_M->Fill(truthPart.m());
  m_truthParent_Pt->Fill(truthPart.pt());
  m_truthParent_Phi->Fill(truthPart.phi());
  m_truthParent_Eta->Fill(truthPart.eta());
  m_truthParent_charge->Fill(truthPart.charge());

  ATH_MSG_DEBUG("Plotting truth prod vtx");
  if(truthPart.hasProdVtx()){
    const xAOD::TruthVertex & vertex = *truthPart.prodVtx();

    m_truthParentProdX->Fill(vertex.x());
    m_truthParentProdY->Fill(vertex.y());
    m_truthParentProdZ->Fill(vertex.z());
    m_truthParentProdR->Fill(vertex.perp());
    m_truthParentProdEta->Fill(vertex.eta());
    m_truthParentProdPhi->Fill(vertex.phi());
  }

  ATH_MSG_DEBUG("Plotting match types");
  static const xAOD::Vertex::Decorator<TruthVertexMatchType> matchTypeDecor("TruthVertexMatchType");
  
  m_truthInclusive_r->Fill(truthVtx.perp());

  if(matchTypeDecor(truthVtx) >= RECONSTRUCTABLE){
    m_truthReconstructable_r->Fill(truthVtx.perp());
  }
  if(matchTypeDecor(truthVtx) >= ACCEPTED){
    m_truthAccepted_r->Fill(truthVtx.perp());
  }
  if(matchTypeDecor(truthVtx) >= SEEDED){
    m_truthSeeded_r->Fill(truthVtx.perp());
  }
  if(matchTypeDecor(truthVtx) >= RECONSTRUCTED){
    m_truthReconstructed_r->Fill(truthVtx.perp());
  }
   
  return StatusCode::SUCCESS;
}


// check if truth particle originated from decay of particle in the pdgIdList
int InDetSecVertexTruthMatchTool::checkProduction( const xAOD::TruthParticle & truthPart ) const {

  if (truthPart.nParents() == 0){
    ATH_MSG_DEBUG("Particle has no parents (end of loop)");
    return -1;
  } 
  else{
    const xAOD::TruthParticle * parent = truthPart.parent(0);
    if(not parent) {
      ATH_MSG_DEBUG("Particle parent is null");
      return -1;
    }
    ATH_MSG_DEBUG("Parent ID: " << parent->pdgId());
        
    if(std::find(m_pdgIdList.begin(), m_pdgIdList.end(), std::abs(parent->pdgId())) != m_pdgIdList.end()) {
        ATH_MSG_DEBUG("Found LLP decay.");
        const xAOD::TruthVertex* vertex = parent->decayVtx();
        return vertex->barcode();
    }
     
    // recurse on parent
    return checkProduction(*parent);
  }
  return -1;
}

void InDetSecVertexTruthMatchTool::countReconstructibleDescendentParticles(const xAOD::TruthVertex& signalTruthVertex,
                                                                           std::vector<const xAOD::TruthParticle*>& set) const {
  for( size_t itrk = 0; itrk < signalTruthVertex.nOutgoingParticles(); itrk++) {
    const auto* particle = signalTruthVertex.outgoingParticle( itrk );
    if( !particle ) continue;
    // Recursively add descendents
    if( particle->hasDecayVtx() ) {
      
      TVector3 decayPos( particle->decayVtx()->x(), particle->decayVtx()->y(), particle->decayVtx()->z() );
      TVector3 prodPos ( particle->prodVtx()->x(),  particle->prodVtx()->y(),  particle->prodVtx()->z()  );
      
      auto isInside  = []( TVector3& v ) { return ( v.Perp() < 300. && fabs( v.z() ) < 1500. ); };
      auto isOutside = []( TVector3& v ) { return ( v.Perp() > 563. || fabs( v.z() ) > 2720. ); };
      
      const auto distance = (decayPos - prodPos).Mag();
      
      // consider track reconstructible if it travels at least 10mm
      if( distance < 10.0 ) {
        countReconstructibleDescendentParticles( *particle->decayVtx(), set );
      } else if( isInside ( prodPos  )  && isOutside( decayPos )  && particle->isCharged() ) {
        set.push_back( particle );
      } else if( particle->isElectron() || particle->isMuon() ) {
        set.push_back( particle );
      }
    } else {
      if( !(particle->isCharged()) ) continue;
      set.push_back( particle );
    }
  }
  
  return;
}
