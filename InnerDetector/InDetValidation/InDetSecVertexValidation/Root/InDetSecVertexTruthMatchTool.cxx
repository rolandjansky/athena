/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaKernel/Units.h"

#include "InDetSecVertexValidation/InDetSecVertexTruthMatchTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"

using namespace InDetSecVertexTruthMatchUtils;
using Athena::Units::GeV;

InDetSecVertexTruthMatchTool::InDetSecVertexTruthMatchTool( const std::string & name ) : asg::AsgTool(name) {
  declareProperty("trackMatchProb", m_trkMatchProb = 0.5 );
  declareProperty("vertexMatchWeight", m_vxMatchWeight = 0.5 );
  declareProperty("trackPtCut", m_trkPtCut = 1000. );
  declareProperty("pdgIds", m_pdgIds = "36" );
  declareProperty("fillHist", m_fillHist = true );
}

StatusCode InDetSecVertexTruthMatchTool::initialize() {
  ATH_MSG_INFO("Initializing InDetSecVertexTruthMatchTool");

  // build the vector of pdgIds from the input string
  std::stringstream ss(m_pdgIds);
  for (int i; ss >> i;) {
      m_pdgIdList.push_back(i);    
      if (ss.peek() == ',')
          ss.ignore();
  }

  // histograms
  ATH_CHECK( service("THistSvc",m_thistSvc) );

  ////////////////////////////////////////////
  ////// Seconvery Vertex Histograms /////////
  ////////////////////////////////////////////
  m_matchType                 = new TH1F("reco_matchType","Vertex Match Type",6,-0.5,5.5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/matchType", m_matchType));

  m_recoX                     = new TH1F("reco_X","Reco vertex x [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/X", m_recoX));
  m_recoY                     = new TH1F("reco_Y","Reco vertex y [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Y", m_recoY));
  m_recoZ                     = new TH1F("reco_Z","Reco vertex z [mm]",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Z", m_recoZ));
  m_recoR                     = new TH1F("reco_R","Reco vertex r [mm]",500,0,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/R", m_recoR));
  m_recodistFromPV            = new TH1F("reco_distFromPV","Reco vertex distFromPV [mm]",500,0,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/distFromPV", m_recodistFromPV));
  m_recoPt                    = new TH1F("reco_Pt","Reco vertex Pt [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Pt", m_recoPt));
  m_recoEta                   = new TH1F("reco_Eta","Reco vertex Eta ",100,-5,5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Eta", m_recoEta));
  m_recoPhi                   = new TH1F("reco_Phi","Reco vertex Phi ",64,-3.2,3.2);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Phi", m_recoPhi));
  m_recoMass                  = new TH1F("reco_Mass","Reco vertex Mass [GeV]",500,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Mass", m_recoMass));
  m_recoMu                    = new TH1F("reco_Mu","Reco vertex Red. Mass [GeV]",500,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Mu", m_recoMu));
  m_recoChi2                  = new TH1F("reco_Chi2","Reco vertex recoChi2",100,0,10); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Chi2", m_recoChi2));
  m_recoDir                   = new TH1F("reco_Dir","Reco vertex recoDirection",100,-1,1);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Dir", m_recoDir));
  m_recoCharge                = new TH1F("reco_Charge","Reco vertex recoCharge",20,-10,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Charge", m_recoCharge));
  m_recoH                     = new TH1F("reco_H","Reco vertex H [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/H", m_recoH));
  m_recoHt                    = new TH1F("reco_Ht","Reco vertex Mass [GeV]",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Ht", m_recoHt));
  m_recoMinOpAng              = new TH1F("reco_MinOpAng","Reco vertex minOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/MinOpAng", m_recoMinOpAng));
  m_recoMaxOpAng              = new TH1F("reco_MaxOpAng","Reco vertex MaxOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/MaxOpAng", m_recoMaxOpAng));
  m_recoMaxDR                 = new TH1F("reco_MaxDR","Reco vertex maxDR",100,0,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/MaxDR", m_recoMaxDR));
  m_recoMinD0                 = new TH1F("reco_MinD0","Reco vertex min d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/MinD0", m_recoMinD0));
  m_recoMaxD0                 = new TH1F("reco_MaxD0","Reco vertex max d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/MaxD0", m_recoMaxD0));
  m_recoNtrk                  = new TH1F("reco_Ntrk","Reco vertex n tracks",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Ntrk", m_recoNtrk));

  // splitting up by match type
  m_positionRes_R             = new TH1F("reco_positionRes_R","Position resolution for vertices matched to truth LLP decays",400,-20,20);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/positionRes_R", m_positionRes_R));
  m_positionRes_Z             = new TH1F("reco_positionRes_z","Position resolution for vertices matched to truth LLP decays",400,-20,20);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/positionRes_Z", m_positionRes_Z));
  m_matchScore_weight          = new TH1F("matchScore_weight","Vertex Match Score (weight)",101,0,1.01);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/matchScore_weight", m_matchScore_weight));
  m_matchScore_pt             = new TH1F("matchScore_pt","Vertex Match Score (pT)",101,0,1.01);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/matchScore_pt", m_matchScore_pt));
  m_matchedTruthID            = new TH1F("matchedTruthID","Vertex Truth Match ID",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/matchedTruthID", m_matchedTruthID));

  m_recoR_LLP                  = new TH1F("reco_R_LLP","Reco vertex r [mm]",500,0,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/R", m_recoR_LLP));
  m_recoPt_LLP                 = new TH1F("reco_Pt_LLP","Reco vertex Pt [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Pt", m_recoPt_LLP));
  m_recoEta_LLP                = new TH1F("reco_Eta_LLP","Reco vertex Eta ",100,-5,5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Eta", m_recoEta_LLP));
  m_recoPhi_LLP                = new TH1F("reco_Phi_LLP","Reco vertex Phi ",64,-3.2,3.2);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Phi", m_recoPhi_LLP));
  m_recoMass_LLP               = new TH1F("reco_Mass_LLP","Reco vertex Mass [GeV]",500,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Mass", m_recoMass_LLP));
  m_recoMu_LLP                 = new TH1F("reco_Mu_LLP","Reco vertex Red. Mass [GeV]",500,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Mu", m_recoMu_LLP));
  m_recoChi2_LLP               = new TH1F("reco_Chi2_LLP","Reco vertex recoChi2",100,0,10); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Chi2", m_recoChi2_LLP));
  m_recoDir_LLP                = new TH1F("reco_Dir_LLP","Reco vertex recoDirection",100,-1,1);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Dir", m_recoDir_LLP));
  m_recoCharge_LLP             = new TH1F("reco_Charge_LLP","Reco vertex recoCharge",20,-10,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Charge", m_recoCharge_LLP));
  m_recoH_LLP                  = new TH1F("reco_H_LLP","Reco vertex H [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/H", m_recoH_LLP));
  m_recoHt_LLP                 = new TH1F("reco_Ht_LLP","Reco vertex Mass [GeV]",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Ht", m_recoHt_LLP));
  m_recoMinOpAng_LLP           = new TH1F("reco_MinOpAng_LLP","Reco vertex minOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/MinOpAng", m_recoMinOpAng_LLP));
  m_recoMaxOpAng_LLP           = new TH1F("reco_MaxOpAng_LLP","Reco vertex MaxOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/MaxOpAng", m_recoMaxOpAng_LLP));
  m_recoMaxDR_LLP              = new TH1F("reco_MaxDR_LLP","Reco vertex maxDR",100,0,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/MaxDR", m_recoMaxDR_LLP));
  m_recoMinD0_LLP              = new TH1F("reco_MinD0_LLP","Reco vertex min d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/MinD0", m_recoMinD0_LLP));
  m_recoMaxD0_LLP              = new TH1F("reco_MaxD0_LLP","Reco vertex max d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/MaxD0", m_recoMaxD0_LLP));
  m_recoNtrk_LLP               = new TH1F("reco_Ntrk_LLP","Reco vertex n tracks",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/LLP/Ntrk", m_recoNtrk_LLP));


  m_recoR_Fake                  = new TH1F("reco_R_Fake","Reco vertex r [mm]",500,0,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/R", m_recoR_Fake));
  m_recoPt_Fake                 = new TH1F("reco_Pt_Fake","Reco vertex Pt [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Pt", m_recoPt_Fake));
  m_recoEta_Fake                = new TH1F("reco_Eta_Fake","Reco vertex Eta ",100,-5,5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Eta", m_recoEta_Fake));
  m_recoPhi_Fake                = new TH1F("reco_Phi_Fake","Reco vertex Phi ",64,-3.2,3.2);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Phi", m_recoPhi_Fake));
  m_recoMass_Fake               = new TH1F("reco_Mass_Fake","Reco vertex Mass [GeV]",500,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Mass", m_recoMass_Fake));
  m_recoMu_Fake                 = new TH1F("reco_Mu_Fake","Reco vertex Red. Mass [GeV]",500,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Mu", m_recoMu_Fake));
  m_recoChi2_Fake               = new TH1F("reco_Chi2_Fake","Reco vertex recoChi2",100,0,10); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Chi2", m_recoChi2_Fake));
  m_recoDir_Fake                = new TH1F("reco_Dir_Fake","Reco vertex recoDirection",100,-1,1);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Dir", m_recoDir_Fake));
  m_recoCharge_Fake             = new TH1F("reco_Charge_Fake","Reco vertex recoCharge",20,-10,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Charge", m_recoCharge_Fake));
  m_recoH_Fake                  = new TH1F("reco_H_Fake","Reco vertex H [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/H", m_recoH_Fake));
  m_recoHt_Fake                 = new TH1F("reco_Ht_Fake","Reco vertex Mass [GeV]",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Ht", m_recoHt_Fake));
  m_recoMinOpAng_Fake           = new TH1F("reco_MinOpAng_Fake","Reco vertex minOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/MinOpAng", m_recoMinOpAng_Fake));
  m_recoMaxOpAng_Fake           = new TH1F("reco_MaxOpAng_Fake","Reco vertex MaxOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/MaxOpAng", m_recoMaxOpAng_Fake));
  m_recoMaxDR_Fake              = new TH1F("reco_MaxDR_Fake","Reco vertex maxDR",100,0,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/MaxDR", m_recoMaxDR_Fake));
  m_recoMinD0_Fake              = new TH1F("reco_MinD0_Fake","Reco vertex min d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/MinD0", m_recoMinD0_Fake));
  m_recoMaxD0_Fake              = new TH1F("reco_MaxD0_Fake","Reco vertex max d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/MaxD0", m_recoMaxD0_Fake));
  m_recoNtrk_Fake               = new TH1F("reco_Ntrk_Fake","Reco vertex n tracks",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Fake/Ntrk", m_recoNtrk_Fake));


  m_recoR_Split                  = new TH1F("reco_R_Split","Reco vertex r [mm]",500,0,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/R", m_recoR_Split));
  m_recoPt_Split                 = new TH1F("reco_Pt_Split","Reco vertex Pt [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Pt", m_recoPt_Split));
  m_recoEta_Split                = new TH1F("reco_Eta_Split","Reco vertex Eta ",100,-5,5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Eta", m_recoEta_Split));
  m_recoPhi_Split                = new TH1F("reco_Phi_Split","Reco vertex Phi ",64,-3.2,3.2);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Phi", m_recoPhi_Split));
  m_recoMass_Split               = new TH1F("reco_Mass_Split","Reco vertex Mass [GeV]",500,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Mass", m_recoMass_Split));
  m_recoMu_Split                 = new TH1F("reco_Mu_Split","Reco vertex Red. Mass [GeV]",500,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Mu", m_recoMu_Split));
  m_recoChi2_Split               = new TH1F("reco_Chi2_Split","Reco vertex recoChi2",100,0,10); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Chi2", m_recoChi2_Split));
  m_recoDir_Split                = new TH1F("reco_Dir_Split","Reco vertex recoDirection",100,-1,1);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Dir", m_recoDir_Split));
  m_recoCharge_Split             = new TH1F("reco_Charge_Split","Reco vertex recoCharge",20,-10,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Charge", m_recoCharge_Split));
  m_recoH_Split                  = new TH1F("reco_H_Split","Reco vertex H [GeV]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/H", m_recoH_Split));
  m_recoHt_Split                 = new TH1F("reco_Ht_Split","Reco vertex Mass [GeV]",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Ht", m_recoHt_Split));
  m_recoMinOpAng_Split           = new TH1F("reco_MinOpAng_Split","Reco vertex minOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/MinOpAng", m_recoMinOpAng_Split));
  m_recoMaxOpAng_Split           = new TH1F("reco_MaxOpAng_Split","Reco vertex MaxOpAng",100,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/MaxOpAng", m_recoMaxOpAng_Split));
  m_recoMaxDR_Split              = new TH1F("reco_MaxDR_Split","Reco vertex maxDR",100,0,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/MaxDR", m_recoMaxDR_Split));
  m_recoMinD0_Split              = new TH1F("reco_MinD0_Split","Reco vertex min d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/MinD0", m_recoMinD0_Split));
  m_recoMaxD0_Split              = new TH1F("reco_MaxD0_Split","Reco vertex max d0 [mm]",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/MaxD0", m_recoMaxD0_Split));
  m_recoNtrk_Split               = new TH1F("reco_Ntrk_Split","Reco vertex n tracks",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Split/Ntrk", m_recoNtrk_Split));

  // tracks
  m_recoTrk_qOverP            = new TH1F("reco_Trk_qOverP","Reco track qOverP ",100,0,.01);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_qOverP", m_recoTrk_qOverP));
  m_recoTrk_theta             = new TH1F("reco_Trk_theta","Reco track theta ",64,0,3.2);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_theta", m_recoTrk_theta));
  m_recoTrk_E                 = new TH1F("reco_Trk_E","Reco track E ",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_E", m_recoTrk_E));    
  m_recoTrk_M                 = new TH1F("reco_Trk_M","Reco track M ",100,0,10);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_M", m_recoTrk_M));
  m_recoTrk_Pt                = new TH1F("reco_Trk_Pt","Reco track Pt ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_Pt", m_recoTrk_Pt));
  m_recoTrk_Px                = new TH1F("reco_Trk_Px","Reco track Px ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_Px", m_recoTrk_Px));
  m_recoTrk_Py                = new TH1F("reco_Trk_Py","Reco track Py ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_Py", m_recoTrk_Py));
  m_recoTrk_Pz                = new TH1F("reco_Trk_Pz","Reco track Pz ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_Pz", m_recoTrk_Pz));
  m_recoTrk_Eta               = new TH1F("reco_Trk_Eta","Reco track Eta ",100,-5,5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_Eta", m_recoTrk_Eta));
  m_recoTrk_Phi               = new TH1F("reco_Trk_Phi","Reco track Phi ",63,-3.2,3.2); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_Phi", m_recoTrk_Phi));
  m_recoTrk_D0                = new TH1F("reco_Trk_D0","Reco track D0 ",300,-300,300); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_D0", m_recoTrk_D0));
  m_recoTrk_Z0                = new TH1F("reco_Trk_Z0","Reco track Z0 ",500,-500,500); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_Z0", m_recoTrk_Z0));
  m_recoTrk_errD0             = new TH1F("reco_Trk_errD0","Reco track errD0 ",300,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_errD0", m_recoTrk_errD0));
  m_recoTrk_errZ0             = new TH1F("reco_Trk_errZ0","Reco track errZ0 ",500,0,50);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_errZ0", m_recoTrk_errZ0));
  m_recoTrk_Chi2              = new TH1F("reco_Trk_Chi2","Reco track Chi2 ",100,0,10); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_Chi2", m_recoTrk_Chi2));
  m_recoTrk_nDoF              = new TH1F("reco_Trk_nDoF","Reco track nDoF ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_nDoF", m_recoTrk_nDoF));
  m_recoTrk_charge            = new TH1F("reco_Trk_charge","Reco track charge ",3,-1.5,1.5); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/SecondaryVertex/Trk_charge", m_recoTrk_charge));


  ////////////////////////////////////////////
  //////// Truth Vertex Histograms ///////////
  ////////////////////////////////////////////
  m_truthX                    = new TH1F("truth_X","truth vertex x [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthX", m_truthX));
  m_truthY                    = new TH1F("truth_Y","truth vertex y [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthY", m_truthY));
  m_truthZ                    = new TH1F("truth_Z","truth vertex z [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthZ", m_truthZ));
  m_truthR                    = new TH1F("truth_R","truth vertex r [mm]",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthR", m_truthR));
  m_truthdistFromPV           = new TH1F("truth_distFromPV","truth vertex distFromPV [mm]",500,0,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthdistFromPV", m_truthdistFromPV));
  m_truthEta                  = new TH1F("truth_Eta","truth veEtatex Eta ",100,-5,5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthEta", m_truthEta));
  m_truthPhi                  = new TH1F("truth_Phi","truth vePhitex Phi ",64,-3.2,3.2); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthPhi", m_truthPhi));
  m_truthNtrk_out             = new TH1F("truth_Ntrk_out","truth vertex n outgoing tracks",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthNtrk_out", m_truthNtrk_out));
  m_truthParent_E             = new TH1F("truth_Parent_E","Reco track E ",100,0,100); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParent_E", m_truthParent_E));    
  m_truthParent_M             = new TH1F("truth_Parent_M","Reco track M ",500,0,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParent_M", m_truthParent_M));
  m_truthParent_Pt            = new TH1F("truth_Parent_Pt","Reco track Pt ",100,0,100);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParent_Pt", m_truthParent_Pt));
  m_truthParent_Eta           = new TH1F("truth_Parent_Eta","Reco track Eta ",100,-5,5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParent_Eta", m_truthParent_Eta));
  m_truthParent_Phi           = new TH1F("truth_Parent_Phi","Reco track Phi ",63,-3.2,3.2); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParent_Phi", m_truthParent_Phi));
  m_truthParent_charge        = new TH1F("truth_Parent_charge","Reco track charge ",3,-1,1); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParent_charge", m_truthParent_charge));
  m_truthParentProdX          = new TH1F("truth_ParentProdX","truthParentProd vertex x [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParentProdX", m_truthParentProdX));
  m_truthParentProdY          = new TH1F("truth_ParentProdY","truthParentProd vertex y [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParentProdY", m_truthParentProdY));
  m_truthParentProdZ          = new TH1F("truth_ParentProdZ","truthParentProd vertex z [mm]",500,-500,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParentProdZ", m_truthParentProdZ));
  m_truthParentProdR          = new TH1F("truth_ParentProdR","truthParentProd vertex r [mm]",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParentProdR", m_truthParentProdR));
  m_truthParentProddistFromPV = new TH1F("truth_ParentProddistFromPV","truthParentProd vertex distFromPV [mm]",500,0,500);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParentProddistFromPV", m_truthParentProddistFromPV));
  m_truthParentProdEta        = new TH1F("truth_ParentProdEta","truthParentProd veEtatex Eta ",100,-5,5);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParentProdEta", m_truthParentProdEta));
  m_truthParentProdPhi        = new TH1F("truth_ParentProdPhi","truthParentProd vePhitex Phi ",64,-3.2,3.2); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/truthParentProdPhi", m_truthParentProdPhi));

  m_truthInclusive_r            = new TH1F("truth_R_Inclusive","Reconstructable Truth Vertices",6000,0,600); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/Inclusive/R", m_truthInclusive_r));
  m_truthReconstructable_r      = new TH1F("truth_R_Reconstructable","Truth Vertices in detector acceptance",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/Reconstructable/R", m_truthReconstructable_r));
  m_truthAccepted_r             = new TH1F("truth_R_Accepted","Truth Vertices in detector acceptance",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/Accepted/R", m_truthAccepted_r));
  m_truthSeeded_r               = new TH1F("truth_R_Seeded","Seedable Truth Vertices",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/Seeded/R", m_truthSeeded_r));
  m_truthReconstructed_r        = new TH1F("truth_R_Reconstructed","Vertex with Match Score > 0.5",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/Reconstructed/R", m_truthReconstructed_r));
  m_truthSplit_r                = new TH1F("truth_R_Split","Split Vertex",6000,0,600);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/Split/R", m_truthSplit_r));

  // TODO: implement these plots
  m_truth_Ntrk                  = new TH1F("truth_Ntrk","Truth vertex n track pass tracks",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/Inclusive/Ntrk", m_truth_Ntrk));
  m_truthReconstructable_trkSel = new TH1F("truth_Ntrk_Seeded","Seedable Truth Vertices",30,0,30); 
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/Reconstructable/Ntrk", m_truthReconstructable_trkSel));
  m_truthReconstructed_trkSel   = new TH1F("truth_Ntrk_Reconstructed","Vertex with Match Score > 0.5",30,0,30);  
  ATH_CHECK( m_thistSvc->regHist("/VTXPLOTS/TruthVertex/Reconstructed/Ntrk", m_truthReconstructed_trkSel));

  return StatusCode::SUCCESS;
}

StatusCode InDetSecVertexTruthMatchTool::finalize()
{

  ATH_MSG_INFO("Finalizing InDetSecVertexTruthMatchTool");
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

    float totalPt = 0; 

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
          //truth particle failed cuts
        }
      } else {
        //not valid or low matching probability
        ATH_MSG_DEBUG("Invalid or low prob truth link!");
      }
    }//end loop over tracks in vertex

    if (vx_n_bad_links>0) {
       ++n_vx_with_bad_links;
    }

    // normalize by total weight and pT
    std::for_each( matchinfo.begin(), matchinfo.end(), [&](VertexTruthMatchInfo& link)
    {
      std::get<1>(link) /= totalWeight;
      std::get<2>(link) /= totalPt;
    });

    // TODO: sort match info first
    matchInfoDecor( *vtx ) = matchinfo;
  }

  //After first loop, all vertices have been decorated with their vector of match info (link to TruthVertex paired with weight)
  //now we want to use that information from the whole collection to assign types
  //keep track of whether a type is assigned
  //useful since looking for splits involves a double loop, and then setting types ahead in the collection
  std::vector<bool> assignedType( vtxContainer.size(), false );
  static const xAOD::TruthVertex::Decorator<bool> isMatched("VertexMatchedToTruth");
  static const xAOD::TruthVertex::Decorator<bool> isSplit("VertexSplit");

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
    } else if ( std::get<2>(info[0]) > m_vxMatchWeight ) { // one truth matched vertex, sufficient weight
      ATH_MSG_DEBUG("One true decay vertices matched. Vertex is matched.");
      matchTypeDecor( *vtxContainer[i] ) = MATCHED;
      isMatched(**std::get<0>(info[0])) = true;
    } else {                                               // one truth matched vertex, insufficient weight 
      ATH_MSG_DEBUG("One true decay vertices matched with insufficient score. Vertex is fake.");
      matchTypeDecor( *vtxContainer[i] ) = FAKE;
    }

    //check for splitting
    // TODO: decorate linked truth vertices with isSplit
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
          isSplit(**std::get<0>(info[0])) = true;
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
  xAOD::TruthVertex::Decorator<bool> isSplit("VertexSplit");

  for(const xAOD::TruthVertex* truthVtx : truthVtxContainer) {

    if(truthVtx->nIncomingParticles() != 1){continue;}
    const xAOD::TruthParticle* truthPart = truthVtx->incomingParticle(0);
    if(not truthPart) continue;
    if(std::find(m_pdgIdList.begin(), m_pdgIdList.end(), std::abs(truthPart->pdgId())) == m_pdgIdList.end()) continue;
    
    ATH_MSG_DEBUG("Analysing Truth Vertex " << truthVtx->barcode() );
    std::vector<const xAOD::TruthParticle*> reconstructibleParticles;
    countReconstructibleDescendentParticles( *truthVtx, reconstructibleParticles );

    // temporary solution for keeping track of particles in the vertex
    std::vector<int> particleInfo = {0,0,0};
    std::vector<int> vertexInfo = {0,0,0};

    for(size_t n = 0; n < reconstructibleParticles.size(); n++){
      ATH_MSG_DEBUG("Checking daughter no. " << n);
      const xAOD::TruthParticle* outPart = reconstructibleParticles.at(n);
      
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
        
        // skip first value in the tuple, we already counted it
        // in the first loop
        for(size_t h = 1; h < particleInfo.size(); h++){
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
    if(isSplit(*truthVtx)){
      ATH_MSG_DEBUG("Vertex is matched to multiple secVtx");
      matchTypeDecor(*truthVtx) = RECONSTRUCTEDSPLIT;
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
    ATH_MSG_DEBUG("Insufficient pt to reconstruct the particle");
    return {0,0,0};
  }
  else{

    for(const xAOD::TrackParticle* trkPart : trkCont){
      const ElementLink<xAOD::TruthParticleContainer> & truthPartLink = trk_truthPartAcc( *trkPart );
      float matchProb = trk_truthProbAcc( *trkPart );

      if(truthPartLink.isValid() && matchProb > m_trkMatchProb) {
        const xAOD::TruthParticle& tmpPart = **truthPartLink;
        if(tmpPart.barcode() == truthPart.barcode()) {
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
    ATH_MSG_DEBUG("Particle has enough pt.");
    return {1,0,0};
    
  }
  return {0,0,0};
}

StatusCode InDetSecVertexTruthMatchTool::fillRecoPlots( const xAOD::Vertex& secVtx ) const {
  
  // set of accessors for tracks and weights
  xAOD::Vertex::ConstAccessor<xAOD::Vertex::TrackParticleLinks_t> trkAcc("trackParticleLinks");
  xAOD::Vertex::ConstAccessor<std::vector<float> > weightAcc("trackWeights");
  
  // set of decorators for truth matching info
  static const xAOD::Vertex::Decorator<VertexMatchType> matchTypeDecor("VertexMatchType");
  static const xAOD::Vertex::Decorator<std::vector<VertexTruthMatchInfo> > matchInfoDecor("TruthVertexMatchingInfos");

  m_matchType->Fill(matchTypeDecor(secVtx));

  m_recoX->Fill(secVtx.x());
  m_recoY->Fill(secVtx.y());
  m_recoZ->Fill(secVtx.z());

  TVector3 reco_pos(secVtx.x(), secVtx.y(), secVtx.z());
  float reco_r = reco_pos.Perp();


  size_t ntracks;
  const xAOD::Vertex::TrackParticleLinks_t & trkParts = trkAcc( secVtx );
  ntracks = trkParts.size();

  TLorentzVector sumP4(0,0,0,0);
  double H = 0.0;
  double HT = 0.0;
  int charge = 0;
  double minOpAng = -1.0* 1.e10;
  double maxOpAng =  1.0* 1.e10;
  double minD0 = 1.0* 1.e10;
  double maxD0 = 0.0;
  double maxDR = 0.0;

  xAOD::TrackParticle::ConstAccessor< std::vector< float > > accCovMatrixDiag( "definingParametersCovMatrixDiag" );

  ATH_MSG_DEBUG("Loop over tracks");
  for(size_t t = 0; t < ntracks; t++){
    const xAOD::TrackParticle & trk = **trkParts[t];

    m_recoTrk_errD0->Fill(trk.definingParametersCovMatrix()(0,0));
    m_recoTrk_errZ0->Fill(trk.definingParametersCovMatrix()(1,1));
    double trk_d0 = std::abs(trk.definingParameters()[0]);
    double trk_z0 = std::abs(trk.definingParameters()[1]);

    m_recoTrk_theta->Fill(trk.definingParameters()[3]);
    m_recoTrk_qOverP->Fill(trk.definingParameters()[4]);
    m_recoTrk_E->Fill(trk.e()/GeV);
    m_recoTrk_M->Fill(trk.m()/GeV);
    m_recoTrk_Pt->Fill(trk.pt()/GeV);

    m_recoTrk_Px->Fill(trk.p4().Px()/GeV);
    m_recoTrk_Py->Fill(trk.p4().Py()/GeV);
    m_recoTrk_Pz->Fill(trk.p4().Pz()/GeV);

    m_recoTrk_Eta->Fill(trk.eta());
    m_recoTrk_Phi->Fill(trk.phi0());

    m_recoTrk_D0->Fill(trk_d0);
    m_recoTrk_Z0->Fill(trk_z0);

    if(trk_d0 < minD0){ minD0 = trk_d0; }
    if(trk_d0 > maxD0){ maxD0 = trk_d0; }

    TLorentzVector vv;
    // TODO: use values computed w.r.t SV
    vv.SetPtEtaPhiM(trk.pt(),trk.eta(), trk.phi0(), trk.m());
    sumP4 += vv;
    H += vv.Vect().Mag();
    HT += vv.Pt();

    TLorentzVector v_minus_iv(0,0,0,0);
    for(size_t j = 0; j < ntracks; j++){
      if (j == t){ continue; }

      const xAOD::TrackParticle & trk_2 = **trkParts[j];

      TLorentzVector tmp;
      // TODO: use values computed w.r.t. SV
      tmp.SetPtEtaPhiM(trk_2.pt(),trk_2.eta(), trk_2.phi0(), trk_2.m());
      v_minus_iv += tmp;

      if( j > t ) {
        double tm = vv * tmp / ( vv.Mag() * tmp.Mag() );
        if( minOpAng < tm ) minOpAng = tm;
        if( maxOpAng > tm ) maxOpAng = tm;
      }
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

  const double dir  = sumP4.Vect().Dot( reco_pos ) / sumP4.Vect().Mag() / reco_pos.Mag();

  xAOD::Vertex::ConstAccessor<float> Chi2("chiSquared");
  xAOD::Vertex::ConstAccessor<float> nDoF("numberDoF");

  m_recoR->Fill(reco_r);
  m_recoNtrk->Fill(ntracks);
  m_recoPt->Fill(sumP4.Pt() / GeV);
  m_recoEta->Fill(sumP4.Eta());
  m_recoPhi->Fill(sumP4.Phi());
  m_recoMass->Fill(sumP4.M() / GeV);
  m_recoMu->Fill(sumP4.M()/maxDR / GeV);
  m_recoChi2->Fill(Chi2(secVtx)/nDoF(secVtx));
  m_recoDir->Fill(dir);
  m_recoCharge->Fill(charge);
  m_recoH->Fill(H / GeV);
  m_recoHt->Fill(HT / GeV);
  m_recoMinOpAng->Fill(minOpAng);
  m_recoMaxOpAng->Fill(maxOpAng); 
  m_recoMinD0->Fill(minD0);
  m_recoMaxD0->Fill(maxD0); 
  m_recoMaxDR->Fill(maxDR); 

  // fill the conditional hists
  if(matchTypeDecor(secVtx) == SPLIT) {
    m_recoR_Split->Fill(reco_r);
    m_recoNtrk_Split->Fill(ntracks);
    m_recoPt_Split->Fill(sumP4.Pt() / GeV);
    m_recoEta_Split->Fill(sumP4.Eta());
    m_recoPhi_Split->Fill(sumP4.Phi());
    m_recoMass_Split->Fill(sumP4.M() / GeV);
    m_recoMu_Split->Fill(sumP4.M()/maxDR / GeV);
    m_recoChi2_Split->Fill(Chi2(secVtx)/nDoF(secVtx));
    m_recoDir_Split->Fill(dir);
    m_recoCharge_Split->Fill(charge);
    m_recoH_Split->Fill(H / GeV);
    m_recoHt_Split->Fill(HT / GeV);
    m_recoMinOpAng_Split->Fill(minOpAng);
    m_recoMaxOpAng_Split->Fill(maxOpAng); 
    m_recoMinD0_Split->Fill(minD0);
    m_recoMaxD0_Split->Fill(maxD0); 
    m_recoMaxDR_Split->Fill(maxDR); 
  }
  else if(matchTypeDecor(secVtx) == FAKE)  {
    m_recoR_Fake->Fill(reco_r);
    m_recoNtrk_Fake->Fill(ntracks);
    m_recoPt_Fake->Fill(sumP4.Pt() / GeV);
    m_recoEta_Fake->Fill(sumP4.Eta());
    m_recoPhi_Fake->Fill(sumP4.Phi());
    m_recoMass_Fake->Fill(sumP4.M() / GeV);
    m_recoMu_Fake->Fill(sumP4.M()/maxDR / GeV);
    m_recoChi2_Fake->Fill(Chi2(secVtx)/nDoF(secVtx));
    m_recoDir_Fake->Fill(dir);
    m_recoCharge_Fake->Fill(charge);
    m_recoH_Fake->Fill(H / GeV);
    m_recoHt_Fake->Fill(HT / GeV);
    m_recoMinOpAng_Fake->Fill(minOpAng);
    m_recoMaxOpAng_Fake->Fill(maxOpAng); 
    m_recoMinD0_Fake->Fill(minD0);
    m_recoMaxD0_Fake->Fill(maxD0); 
    m_recoMaxDR_Fake->Fill(maxDR); 
  }
  else if(matchTypeDecor(secVtx) == MATCHED) {
    m_recoR_LLP->Fill(reco_r);
    m_recoNtrk_LLP->Fill(ntracks);
    m_recoPt_LLP->Fill(sumP4.Pt() / GeV);
    m_recoEta_LLP->Fill(sumP4.Eta());
    m_recoPhi_LLP->Fill(sumP4.Phi());
    m_recoMass_LLP->Fill(sumP4.M() / GeV);
    m_recoMu_LLP->Fill(sumP4.M()/maxDR / GeV);
    m_recoChi2_LLP->Fill(Chi2(secVtx)/nDoF(secVtx));
    m_recoDir_LLP->Fill(dir);
    m_recoCharge_LLP->Fill(charge);
    m_recoH_LLP->Fill(H / GeV);
    m_recoHt_LLP->Fill(HT / GeV);
    m_recoMinOpAng_LLP->Fill(minOpAng);
    m_recoMaxOpAng_LLP->Fill(maxOpAng); 
    m_recoMinD0_LLP->Fill(minD0);
    m_recoMaxD0_LLP->Fill(maxD0); 
    m_recoMaxDR_LLP->Fill(maxDR); 
  }
  std::vector<VertexTruthMatchInfo> truthmatchinfo;
  truthmatchinfo = matchInfoDecor(secVtx);

  // This includes all matched vertices, including splits
  if(not truthmatchinfo.empty()){
    float matchScore_weight = std::get<1>(truthmatchinfo.at(0));
    float matchScore_pt     = std::get<2>(truthmatchinfo.at(0));

    m_matchScore_weight->Fill(matchScore_weight);
    m_matchScore_pt->Fill(matchScore_pt);

    ATH_MSG_DEBUG("Match Score and probability: " << matchScore_weight << " " << matchScore_pt/0.01);

    const ElementLink<xAOD::TruthVertexContainer>& truthVertexLink = std::get<0>(truthmatchinfo.at(0));
    const xAOD::TruthVertex& truthVtx = **truthVertexLink ;
    xAOD::TruthVertex::Decorator<int> isReconstructed("VertexReconstructed");

    m_positionRes_R->Fill(reco_r - truthVtx.perp());
    m_positionRes_Z->Fill(secVtx.z() - truthVtx.z());
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

  m_truthParent_E->Fill(truthPart.e() / GeV);
  m_truthParent_M->Fill(truthPart.m() / GeV);
  m_truthParent_Pt->Fill(truthPart.pt() / GeV);
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
  if(matchTypeDecor(truthVtx) >= RECONSTRUCTEDSPLIT){
    m_truthSplit_r->Fill(truthVtx.perp());
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
      
      auto isInside  = []( TVector3& v ) { return ( v.Perp() < 300. && std::abs( v.z() ) < 1500. ); };
      auto isOutside = []( TVector3& v ) { return ( v.Perp() > 563. || std::abs( v.z() ) > 2720. ); };
      
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
