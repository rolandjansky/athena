/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDAlignMonTruthComparison.cxx
// AUTHORS: Beate Heinemann, Tobias Golling
// **********************************************************************

#include <sstream>
#include <math.h>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMath.h"
#include "TROOT.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"

//#include "HepPDT/ParticleData.hh"
#include "HepMC/GenParticle.h"

//#include "IdDictDetDescr/IdDictManager.h"
// #include "InDetIdentifier/PixelID.h"
// #include "InDetIdentifier/SCT_ID.h"
// #include "InDetIdentifier/TRT_ID.h"
// #include "InDetReadoutGeometry/PixelDetectorManager.h"
// #include "InDetReadoutGeometry/SCT_DetectorManager.h"
// #include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruth.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"


//#include "Particle/TrackParticleContainer.h"
//#include "Particle/TrackParticle.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

// #include "VxVertex/VxContainer.h"
// #include "VxVertex/VxCandidate.h"
// #include "VxVertex/RecVertex.h"
// #include "VxVertex/Vertex.h"
// #include "VxVertex/VxTrackAtVertex.h"

//#include "AthenaMonitoring/AthenaMonManager.h"
#include "IDAlignMonTruthComparison.h"

//#include "TrkTruthToTrack/TruthToTrack.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrackSelectionTool.h"

// *********************************************************************
// Public Methods
// *********************************************************************

IDAlignMonTruthComparison::IDAlignMonTruthComparison( const std::string & type, const std::string & name, const IInterface* parent )
	     :ManagedMonitorToolBase( type, name, parent ),
	      m_Pi(3.14156),
	      m_tracksName("ExtendedTracks"),
	      m_tracksTruthName("ExtendedTracksTruthCollection")
{
  m_trackSelection = ToolHandle< InDetAlignMon::TrackSelectionTool >("InDetAlignMon::TrackSelectionTool");
  m_truthToTrack = ToolHandle<Trk::ITruthToTrack>("Trk::TruthToTrack/InDetTruthToTrack");
  
  declareProperty("tracksName"              , m_tracksName);
  declareProperty("tracksTruthName"         , m_tracksTruthName);  
  declareProperty("CheckRate"               , m_checkrate=1000);
  declareProperty("TruthToTrackTool"        , m_truthToTrack);
  declareProperty("trackSelection"          , m_trackSelection);
}


IDAlignMonTruthComparison::~IDAlignMonTruthComparison() { }


StatusCode IDAlignMonTruthComparison::initialize()
{
  StatusCode sc;                                      
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;
  

  // AG: init truthToTrack
  if (m_truthToTrack.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_truthToTrack << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_truthToTrack << endmsg;
  }

  // AG: init trackSelection
  if (m_trackSelection.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackSelection << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_trackSelection << endmsg;
  }

  return sc;
}


StatusCode IDAlignMonTruthComparison::bookHistograms()
{
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  std::string outputDirName = "IDAlignMon/" + m_tracksName + "_NoTriggerSelection/TruthComparison";
  MonGroup al_mon ( this, outputDirName, run );

  //if ( newLowStatFlag() ) {    }
  //if ( newLumiBlockFlag() ) {   }
  if( newRunFlag() ) { 
    
    // increase d0 and z0 range for cosmics
    if (AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
      m_dz0_barrel = new TH1F("dz0_barrel","Delta z0(Rec - truth)",1000,-2000,2000);  
      m_dz0_eca = new TH1F("dz0_eca","Delta z0(Rec - truth)",1000,-2000,2000);  
      m_dz0_ecc = new TH1F("dz0_ecc","Delta z0(Rec - truth)",1000,-2000,2000); 
      m_dd0_barrel = new TH1F("dd0_barrel","Delta d0(Rec - truth)",1000,-1000,1000);  
      m_dd0_eca = new TH1F("dd0_eca","Delta d0(Rec - truth)",1000,-1000,1000); 
      m_dd0_ecc = new TH1F("dd0_ecc","Delta d0(Rec - truth)",1000,-1000,1000);   
    } else {
      m_dz0_barrel = new TH1F("dz0_barrel","Delta z0(Rec - truth)",100,-2,2);  
      m_dz0_eca = new TH1F("dz0_eca","Delta z0(Rec - truth)",100,-2,2);  
      m_dz0_ecc = new TH1F("dz0_ecc","Delta z0(Rec - truth)",100,-2,2); 
      m_dd0_barrel = new TH1F("dd0_barrel","Delta d0(Rec - truth)",100,-1,1);  
      m_dd0_eca = new TH1F("dd0_eca","Delta d0(Rec - truth)",100,-1,1); 
      m_dd0_ecc = new TH1F("dd0_ecc","Delta d0(Rec - truth)",100,-1,1);    
    }
    
    m_truthpT = new TH1F("truthpT","pT truth",100,0,100); 
    RegisterHisto(al_mon,m_truthpT) ;  
    m_truthphi = new TH1F("truthphi","phi truth",100,0,6.3);  
    RegisterHisto(al_mon,m_truthphi) ; 
    m_trutheta = new TH1F("trutheta","eta truth",100,-5,5);  
    RegisterHisto(al_mon,m_trutheta) ; 

    m_dqopt_barrel = new TH1F("dqopt_barrel","Delta QopT(Rec - truth)",500,-0.05,0.05);   
    RegisterHisto(al_mon,m_dqopt_barrel) ;
    m_dqopt_eca = new TH1F("dqopt_eca","Delta QopT(Rec - truth)",500,-0.05,0.05);  
    RegisterHisto(al_mon,m_dqopt_eca) ;  
    m_dqopt_ecc = new TH1F("dqopt_ecc","Delta QopT(Rec - truth)",500,-0.05,0.05);  
    RegisterHisto(al_mon,m_dqopt_ecc) ;  

    m_deta_barrel = new TH1F("deta_barrel","Delta eta(Rec - truth)",100,-0.02,0.02);  
    RegisterHisto(al_mon,m_deta_barrel) ;  
    m_deta_eca = new TH1F("deta_eca","Delta eta(Rec - truth)",100,-0.02,0.02);  
    RegisterHisto(al_mon,m_deta_eca) ;  
    m_deta_ecc = new TH1F("deta_ecc","Delta eta(Rec - truth)",100,-0.02,0.02);  
    RegisterHisto(al_mon,m_deta_ecc) ; 

    m_dphi_barrel = new TH1F("dphi_barrel","Delta phi(Rec - truth)",100,-0.01,0.01);  
    RegisterHisto(al_mon,m_dphi_barrel) ;  
    m_dphi_barrel_vs_phi = new TProfile("dphi_barrel_vs_phi","Delta phi(Rec - truth) vs truth phi, eta < 1.0",60,0,6.28,-0.1,0.1);
    RegisterHisto(al_mon,m_dphi_barrel_vs_phi) ;  
    m_dphi_eca = new TH1F("dphi_eca","Delta phi(Rec - truth)",100,-0.01,0.01);  
    RegisterHisto(al_mon,m_dphi_eca) ;  
    m_dphi_ecc = new TH1F("dphi_ecc","Delta phi(Rec - truth)",100,-0.01,0.01);  
    RegisterHisto(al_mon,m_dphi_ecc) ;  
    m_dphi_vs_eta = new TProfile("dphi_vs_eta","Delta phi(Rec - truth) vs truth eta",60,-2.5,2.5,-0.1,0.1);
    RegisterHisto(al_mon,m_dphi_vs_eta) ;  

    RegisterHisto(al_mon,m_dz0_barrel) ;  
    RegisterHisto(al_mon,m_dz0_eca) ;  
    RegisterHisto(al_mon,m_dz0_ecc) ;   

    RegisterHisto(al_mon,m_dd0_barrel) ;  
    RegisterHisto(al_mon,m_dd0_eca) ;  
    RegisterHisto(al_mon,m_dd0_ecc) ;  


    // Deta vs eta
    m_Deta_vs_eta = new TH2F("Deta_vs_eta","eta(Rec - truth) vs. eta",20,-2.5,2.5,100,-0.05,0.05);  
    RegisterHisto(al_mon,m_Deta_vs_eta) ; 
    m_deta_vs_eta_1 = new TH1F("deta_vs_eta_1","Mean eta(Rec - truth) vs. eta",20,-2.5,2.5);  
    RegisterHisto(al_mon,m_deta_vs_eta_1) ;  
    m_deta_vs_eta_2 = new TH1F("deta_vs_eta_2","RMS eta(Rec - truth) vs. eta",20,-2.5,2.5);   
    //RegisterHisto(al_mon,m_deta_vs_eta_2) ;  
    m_deta_vs_eta_chi2 = new TH1F("deta_vs_eta_chi2","Chi2/NDOF eta(Rec - truth) vs. eta",20,-2.5,2.5);  
    RegisterHisto(al_mon,m_deta_vs_eta_chi2) ; 


    // vs pT
    m_Dqopt_vs_pt_barrel = new TH2F("Dqopt_vs_pt_barrel","QopT(Rec - truth) vs. signed pT (Barrel)",50,-100,100,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_pt_barrel) ; 
    m_dqopt_vs_pt_barrel_1 = new TH1F("dqopt_vs_pt_barrel_1","Mean QopT(Rec - truth) vs. signed pT (Barrel)",50,-100,100);  
    RegisterHisto(al_mon,m_dqopt_vs_pt_barrel_1) ;  
    m_dqopt_vs_pt_barrel_2 = new TH1F("dqopt_vs_pt_barrel_2","RMS QopT(Rec - truth) vs. signed pT (Barrel)",50,-100,100);  
    //RegisterHisto(al_mon,m_dqopt_vs_pt_barrel_2) ; 
    m_dqopt_vs_pt_barrel_chi2 = new TH1F("dqopt_vs_pt_barrel_chi2","Chi2/NDOF QopT(Rec - truth) vs. signed pT (Barrel)",50,-100,100);  
    RegisterHisto(al_mon,m_dqopt_vs_pt_barrel_chi2) ; 
    m_dpt_vs_truthpt_barrel = new TProfile("dpt_vs_truthpt_barrel","pT(Rec/truth) vs. pT truth (Barrel)",100,-50,50,-20.0,20.0);  
    RegisterHisto(al_mon,m_dpt_vs_truthpt_barrel) ; 


    m_Dqopt_vs_pt_eca = new TH2F("Dqopt_vs_pt_eca","QopT(Rec - truth) vs. signed pT (Eca)",50,-100,100,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_pt_eca) ; 
    m_dqopt_vs_pt_eca_1 = new TH1F("dqopt_vs_pt_eca_1","Mean QopT(Rec - truth) vs. signed pT (Eca)",50,-100,100);  
    RegisterHisto(al_mon,m_dqopt_vs_pt_eca_1) ;  
    m_dqopt_vs_pt_eca_2 = new TH1F("dqopt_vs_pt_eca_2","RMS QopT(Rec - truth) vs. signed pT (Eca)",50,-100,100);  
    //RegisterHisto(al_mon,m_dqopt_vs_pt_eca_2) ; 
    m_dqopt_vs_pt_eca_chi2 = new TH1F("dqopt_vs_pt_eca_chi2","Chi2/NDOF QopT(Rec - truth) vs. signed pT (Eca)",50,-100,100);  
    RegisterHisto(al_mon,m_dqopt_vs_pt_eca_chi2) ; 
    m_dpt_vs_truthpt_eca = new TProfile("dpt_vs_truthpt_eca","pT(Rec/truth) vs. pT truth (Eca)",100,-50,50,-20.0,20.0);  
    RegisterHisto(al_mon,m_dpt_vs_truthpt_eca) ; 

    m_Dqopt_vs_pt_ecc = new TH2F("Dqopt_vs_pt_ecc","QopT(Rec - truth) vs. signed pT (Ecc)",50,-100,100,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_pt_ecc) ; 
    m_dqopt_vs_pt_ecc_1 = new TH1F("dqopt_vs_pt_ecc_1","Mean QopT(Rec - truth) vs. signed pT (Ecc)",50,-100,100);  
    RegisterHisto(al_mon,m_dqopt_vs_pt_ecc_1) ;  
    m_dqopt_vs_pt_ecc_2 = new TH1F("dqopt_vs_pt_ecc_2","RMS QopT(Rec - truth) vs. signed pT (Ecc)",50,-100,100);  
    //RegisterHisto(al_mon,m_dqopt_vs_pt_ecc_2) ; 
    m_dqopt_vs_pt_ecc_chi2 = new TH1F("dqopt_vs_pt_ecc_chi2","Chi2/NDOF QopT(Rec - truth) vs. signed pT (Ecc)",50,-100,100);  
    RegisterHisto(al_mon,m_dqopt_vs_pt_ecc_chi2) ; 
    m_dpt_vs_truthpt_ecc = new TProfile("dpt_vs_truthpt_ecc","pT(Rec/truth) vs. pT truth (Ecc)",100,-50,50,-20.0,20.0);  
    RegisterHisto(al_mon,m_dpt_vs_truthpt_ecc) ; 

    // vs eta
    m_Dqopt_vs_eta_highpt = new TH2F("Dqopt_vs_eta_highpt","QopT(Rec - truth) vs. eta (high pT)",20,-2.5,2.5,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_eta_highpt) ; 
    m_dqopt_vs_eta_highpt_1 = new TH1F("dqopt_vs_eta_highpt_1","Mean QopT(Rec - truth) vs. eta (high pT)",20,-2.5,2.5);  
    RegisterHisto(al_mon,m_dqopt_vs_eta_highpt_1) ;  
    m_dqopt_vs_eta_highpt_2 = new TH1F("dqopt_vs_eta_highpt_2","RMS QopT(Rec - truth) vs. eta (high pT)",20,-2.5,2.5);  
    //RegisterHisto(al_mon,m_dqopt_vs_eta_highpt_2) ; 
    m_dqopt_vs_eta_highpt_chi2 = new TH1F("dqopt_vs_eta_highpt_chi2","Chi2/NDOF QopT(Rec - truth) vs. eta (high pT)",20,-2.5,2.5);  
    RegisterHisto(al_mon,m_dqopt_vs_eta_highpt_chi2) ; 

    m_Dqopt_vs_eta_lowpt = new TH2F("Dqopt_vs_eta_lowpt","QopT(Rec - truth) vs. eta (low pT)",20,-2.5,2.5,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_eta_lowpt) ; 
    m_dqopt_vs_eta_lowpt_1 = new TH1F("dqopt_vs_eta_lowpt_1","Mean QopT(Rec - truth) vs. eta (low pT)",20,-2.5,2.5);  
    RegisterHisto(al_mon,m_dqopt_vs_eta_lowpt_1) ;  
    m_dqopt_vs_eta_lowpt_2 = new TH1F("dqopt_vs_eta_lowpt_2","RMS QopT(Rec - truth) vs. eta (low pT)",20,-2.5,2.5);  
    //RegisterHisto(al_mon,m_dqopt_vs_eta_lowpt_2) ; 
    m_dqopt_vs_eta_lowpt_chi2 = new TH1F("dqopt_vs_eta_lowpt_chi2","Chi2/NDOF QopT(Rec - truth) vs. eta (low pT)",20,-2.5,2.5);  
    RegisterHisto(al_mon,m_dqopt_vs_eta_lowpt_chi2) ; 

    // vs phi
    m_Dqopt_vs_phi_highpt_barrel = new TH2F("Dqopt_vs_phi_highpt_barrel","QopT(Rec - truth) vs. phi (highpt, Barrel)",20,0,2*m_Pi,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_phi_highpt_barrel) ; 
    m_dqopt_vs_phi_highpt_barrel_1 = new TH1F("dqopt_vs_phi_highpt_barrel_1","Mean QopT(Rec - truth) vs. phi (highpt, Barrel)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_highpt_barrel_1) ;  
    m_dqopt_vs_phi_highpt_barrel_2 = new TH1F("dqopt_vs_phi_highpt_barrel_2","RMS QopT(Rec - truth) vs. phi (highpt, Barrel)",20,0,2*m_Pi);  
    //RegisterHisto(al_mon,m_dqopt_vs_phi_highpt_barrel_2) ; 
    m_dqopt_vs_phi_highpt_barrel_chi2 = new TH1F("dqopt_vs_phi_highpt_barrel_chi2","Chi2/NDOF QopT(Rec - truth) vs. phi (highpt, Barrel)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_highpt_barrel_chi2) ; 

    m_Dqopt_vs_phi_highpt_eca = new TH2F("Dqopt_vs_phi_highpt_eca","QopT(Rec - truth) vs. phi (highpt, Eca)",20,0,2*m_Pi,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_phi_highpt_eca) ; 
    m_dqopt_vs_phi_highpt_eca_1 = new TH1F("dqopt_vs_phi_highpt_eca_1","Mean QopT(Rec - truth) vs. phi (highpt, Eca)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_highpt_eca_1) ;  
    m_dqopt_vs_phi_highpt_eca_2 = new TH1F("dqopt_vs_phi_highpt_eca_2","RMS QopT(Rec - truth) vs. phi (highpt, Eca)",20,0,2*m_Pi);  
    //RegisterHisto(al_mon,m_dqopt_vs_phi_highpt_eca_2) ; 
    m_dqopt_vs_phi_highpt_eca_chi2 = new TH1F("dqopt_vs_phi_highpt_eca_chi2","Chi2/NDOF QopT(Rec - truth) vs. phi (highpt, Eca)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_highpt_eca_chi2) ; 

    m_Dqopt_vs_phi_highpt_ecc = new TH2F("Dqopt_vs_phi_highpt_ecc","QopT(Rec - truth) vs. phi (highpt, Ecc)",20,0,2*m_Pi,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_phi_highpt_ecc) ; 
    m_dqopt_vs_phi_highpt_ecc_1 = new TH1F("dqopt_vs_phi_highpt_ecc_1","Mean QopT(Rec - truth) vs. phi (highpt, Ecc)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_highpt_ecc_1) ;  
    m_dqopt_vs_phi_highpt_ecc_2 = new TH1F("dqopt_vs_phi_highpt_ecc_2","RMS QopT(Rec - truth) vs. phi (highpt, Ecc)",20,0,2*m_Pi);  
    //RegisterHisto(al_mon,m_dqopt_vs_phi_highpt_ecc_2) ; 
    m_dqopt_vs_phi_highpt_ecc_chi2 = new TH1F("dqopt_vs_phi_highpt_ecc_chi2","Chi2/NDOF QopT(Rec - truth) vs. phi (highpt, Ecc)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_highpt_ecc_chi2) ; 

    // vs phi
    m_Dqopt_vs_phi_lowpt_barrel = new TH2F("Dqopt_vs_phi_lowpt_barrel","QopT(Rec - truth) vs. phi (lowpt, Barrel)",20,0,2*m_Pi,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_phi_lowpt_barrel) ; 
    m_dqopt_vs_phi_lowpt_barrel_1 = new TH1F("dqopt_vs_phi_lowpt_barrel_1","Mean QopT(Rec - truth) vs. phi (lowpt, Barrel)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_lowpt_barrel_1) ;  
    m_dqopt_vs_phi_lowpt_barrel_2 = new TH1F("dqopt_vs_phi_lowpt_barrel_2","RMS QopT(Rec - truth) vs. phi (lowpt, Barrel)",20,0,2*m_Pi);  
    //RegisterHisto(al_mon,m_dqopt_vs_phi_lowpt_barrel_2) ; 
    m_dqopt_vs_phi_lowpt_barrel_chi2 = new TH1F("dqopt_vs_phi_lowpt_barrel_chi2","Chi2/NDOF QopT(Rec - truth) vs. phi (lowpt, Barrel)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_lowpt_barrel_chi2) ; 

    m_Dqopt_vs_phi_lowpt_eca = new TH2F("Dqopt_vs_phi_lowpt_eca","QopT(Rec - truth) vs. phi (lowpt, Eca)",20,0,2*m_Pi,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_phi_lowpt_eca) ; 
    m_dqopt_vs_phi_lowpt_eca_1 = new TH1F("dqopt_vs_phi_lowpt_eca_1","Mean QopT(Rec - truth) vs. phi (lowpt, Eca)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_lowpt_eca_1) ;  
    m_dqopt_vs_phi_lowpt_eca_2 = new TH1F("dqopt_vs_phi_lowpt_eca_2","RMS QopT(Rec - truth) vs. phi (lowpt, Eca)",20,0,2*m_Pi);  
    //RegisterHisto(al_mon,m_dqopt_vs_phi_lowpt_eca_2) ; 
    m_dqopt_vs_phi_lowpt_eca_chi2 = new TH1F("dqopt_vs_phi_lowpt_eca_chi2","Chi2/NDOF cQopT(Rec - truth) vs. phi (lowpt, Eca)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_lowpt_eca_chi2) ; 

    m_Dqopt_vs_phi_lowpt_ecc = new TH2F("Dqopt_vs_phi_lowpt_ecc","QopT(Rec - truth) vs. phi (lowpt, Ecc)",20,0,2*m_Pi,20,-0.1,0.1);  
    RegisterHisto(al_mon,m_Dqopt_vs_phi_lowpt_ecc) ; 
    m_dqopt_vs_phi_lowpt_ecc_1 = new TH1F("dqopt_vs_phi_lowpt_ecc_1","Mean QopT(Rec - truth) vs. phi (lowpt, Ecc)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_lowpt_ecc_1) ;  
    m_dqopt_vs_phi_lowpt_ecc_2 = new TH1F("dqopt_vs_phi_lowpt_ecc_2","RMS QopT(Rec - truth) vs. phi (lowpt, Ecc)",20,0,2*m_Pi);  
    //RegisterHisto(al_mon,m_dqopt_vs_phi_lowpt_ecc_2) ; 
    m_dqopt_vs_phi_lowpt_ecc_chi2 = new TH1F("dqopt_vs_phi_lowpt_ecc_chi2","Chi2/NDOF QopT(Rec - truth) vs. phi (lowpt, Ecc)",20,0,2*m_Pi);  
    RegisterHisto(al_mon,m_dqopt_vs_phi_lowpt_ecc_chi2) ;   

    m_Zmumu = new TH1F("Zmumu","Zmumu",50,0,150);  
    RegisterHisto(al_mon,m_Zmumu) ;  
    m_Zmumu_truth = new TH1F("Zmumu_truth","Zmumu_truth",50,0,150);  
    RegisterHisto(al_mon,m_Zmumu_truth) ; 
    m_dZmumu = new TH1F("Delta_Zmumu","Delta Zmumu",100,-20,20);  
    RegisterHisto(al_mon,m_dZmumu) ;   

    m_dZmumu_barrel = new TH1F("Delta_Zmumu_barrel","#DeltaZmumu Both Legs Barrel",100,-20,20);  
    RegisterHisto(al_mon,m_dZmumu_barrel) ;   
    m_dZmumu_eca = new TH1F("Delta_Zmumu_eca","#DeltaZmumu Both Legs ECA",100,-20,20);  
    RegisterHisto(al_mon,m_dZmumu_eca) ;   
    m_dZmumu_ecc = new TH1F("Delta_Zmumu_ecc","#DeltaZmumu Both Legs ECC",100,-20,20);  
    RegisterHisto(al_mon,m_dZmumu_ecc) ;
    m_dZmumu_barrel_eca = new TH1F("Delta_Zmumu_barrel_eca","#DeltaZmumu One Leg Barrel One Leg ECA",100,-20,20);  
    RegisterHisto(al_mon,m_dZmumu_barrel_eca) ;   
    m_dZmumu_barrel_ecc = new TH1F("Delta_Zmumu_barrel_ecc","#DeltaZmumu One Leg Barrel One Leg ECC",100,-20,20);  
    RegisterHisto(al_mon,m_dZmumu_barrel_ecc) ;   


  }

  return StatusCode::SUCCESS;
}

void IDAlignMonTruthComparison::RegisterHisto(MonGroup& mon, TH1* histo) {

  histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH1 Histogram:" << endmsg;
  }
}

void IDAlignMonTruthComparison::RegisterHisto(MonGroup& mon, TProfile* histo) {

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TProfile Histogram:" << endmsg;
  }
}

void IDAlignMonTruthComparison::RegisterHisto(MonGroup& mon, TH2* histo) {
  
  histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH2 Histogram:" << endmsg;
  }
}


StatusCode IDAlignMonTruthComparison::fillHistograms()
{

  //get TrackCollection using TrackSelectionTool
  const DataVector<Trk::Track>* trks = m_trackSelection->selectTracks(m_tracksName);
  
  //although we select tracks using the TrackSelectionTool, we still need to get a complete TrackCollection
  //from StoreGate for use in the track-truth map, otherwise the track-truth matching is screwed up
  const TrackCollection       * RecCollection = NULL;
  StatusCode sc = StatusCode :: SUCCESS;
  sc = evtStore()->retrieve(RecCollection, m_tracksName);  
  if (sc.isFailure()) {
    if (msgLvl(MSG::VERBOSE)) msg() <<"Track collection \"" << m_tracksName << "\" not found." << endmsg;
    return StatusCode::SUCCESS;
  }
  if (RecCollection)  
    {
      if (msgLvl(MSG::VERBOSE)) 
	msg() << "Retrieved " << m_tracksName << " with size " << RecCollection->size() << " reconstructed tracks from storegate" << endmsg;
    }
  else 
    {
      if (msgLvl(MSG::VERBOSE)) msg()<<"Problem in retrieving " << m_tracksName << endmsg;
      return StatusCode::SUCCESS;
    }

  // get TrackTruthCollection
  const TrackTruthCollection  * TruthMap  = NULL;
  if (StatusCode::SUCCESS!=evtStore()->retrieve(TruthMap,m_tracksTruthName)) {
    if (msgLvl(MSG::VERBOSE)) msg() << "Cannot find " << m_tracksTruthName  << endmsg;
    return StatusCode::SUCCESS;
  } else {
    if (msgLvl(MSG::VERBOSE)) msg() << "Track Truth Collection with name " << m_tracksTruthName << " with size " << TruthMap->size() <<" found in StoreGate" << endmsg;
  }
    
  
  bool z_true = false;
  float z_E[2]={0.}, z_px[2]={0.}, z_py[2]={0.}, z_pz[2]={0.};
  float z_eta[2]={0.};
  float zMC_E[2]={0.}, zMC_px[2]={0.}, zMC_py[2]={0.}, zMC_pz[2]={0.};
  float ptlast = 0;
  int chargefirst = 0;

  int nTracks=0;  
  DataVector<Trk::Track>::const_iterator trksItr  = trks->begin();
  DataVector<Trk::Track>::const_iterator trksItrE = trks->end();
  for (; trksItr != trksItrE; ++trksItr) {
    nTracks++;  
    
    float trkd0          = -999;
    float trkz0          = -999;
    float trkphi         = -999;
    float trktheta       = -999;
    float trketa         = -999;
    float qOverPt        = -999;
    float trkpt          = -999;
    float charge         = 0;
 
    // get fit quality and chi2 probability of track
    // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
    const Trk::Perigee* measPer = (*trksItr)->perigeeParameters();
    const AmgSymMatrix(5)* covariance = measPer ? measPer->covariance() : NULL;
    
    if (covariance==0) {
      msg(MSG::WARNING) << "No measured perigee parameters assigned to the track" << endmsg; 
    }
    else{  
      AmgVector(5) perigeeParams = measPer->parameters();    
      trkd0    = perigeeParams[Trk::d0];  
      trkz0    = perigeeParams[Trk::z0];    
      trkphi   = perigeeParams[Trk::phi0];  
      trktheta = perigeeParams[Trk::theta];
      trketa   = measPer->eta(); 
      qOverPt  = perigeeParams[Trk::qOverP]*1000./sin(trktheta);  
      trkpt    = measPer->pT()/1000.;  
      if (qOverPt<0) charge=-1;
      else charge=+1; 
    }
    if (trkphi<0) trkphi+=2*m_Pi;

    if (msgLvl(MSG::VERBOSE)) msg() << "Found good track with phi, PT = " << trkphi << ", " << trkpt << endmsg; 
    
    if (TruthMap) {	

      ElementLink<TrackCollection> tracklink;
      tracklink.setElement(const_cast<Trk::Track*>(*trksItr));
      tracklink.setStorableObject(*RecCollection);
      const ElementLink<TrackCollection> tracklink2=tracklink;
           
      TrackTruthCollection::const_iterator found = TruthMap->find(tracklink2);
      if (found != TruthMap->end())
	{
	  TrackTruth trtruth = found->second;
	  HepMcParticleLink HMPL = trtruth.particleLink();
          
	  if ( HMPL.isValid()) 
	    {
	      const HepMC::GenParticle *genparptr = HMPL.cptr(); 

	      if (genparptr) {
		if (genparptr->production_vertex()) { 
		  
		  if(genparptr->pdg_id() == 0){
		    msg(MSG::WARNING) <<" Particle with PDG ID = 0! Status "<<endmsg; 
		    //msg(MSG::WARNING) <<" Particle with PDG ID = 0! Status "<<genparptr->status()<<" mass "<< genparptr->momentum().m() <<" pt "<<genparptr->momentum().et()<<" eta "
		    // <<genparptr->momentum().eta()<<" phi "<<genparptr->momentum().phi()<<" Gen Vertex barcode "<<genparptr->production_vertex()->barcode()<<"Gen Vertex Position x"
		    // <<genparptr->production_vertex()->position().x()<< " y "<<genparptr->production_vertex()->position().y()<<" z "<<genparptr->production_vertex()->position().z()<<endmsg;                    
		  }else{
		    
		    const Trk::TrackParameters* generatedTrackPerigee = m_truthToTrack->makePerigeeParameters(genparptr);
		    if (!generatedTrackPerigee)   msg(MSG::WARNING) <<  "Unable to extrapolate genparticle to perigee!" << endmsg;
		    
		    if ( generatedTrackPerigee) {
		      float track_truth_qoverpt      = 1000. * generatedTrackPerigee->parameters()[Trk::qOverP]/sin(generatedTrackPerigee->parameters()[Trk::theta]);
		      float track_truth_phi          = generatedTrackPerigee->parameters()[Trk::phi0];       
		      float track_truth_d0           = generatedTrackPerigee->parameters()[Trk::d0];
		      float track_truth_z0           = generatedTrackPerigee->parameters()[Trk::z0];
		      float track_truth_theta        = generatedTrackPerigee->parameters()[Trk::theta];
		      float track_truth_eta          = generatedTrackPerigee->eta();
		      delete  generatedTrackPerigee; 		      
		      float track_truth_pt           = 1./fabs(track_truth_qoverpt);  
		      float track_truth_charge       = 1; 
		      if(track_truth_qoverpt<0) track_truth_charge = -1;
		      if (track_truth_phi<0) track_truth_phi+=2*m_Pi;
		      if (msgLvl(MSG::VERBOSE)) msg() << "Found matched truth track with phi, PT = " << track_truth_phi << ", " << track_truth_pt << endmsg; 

		      m_truthpT->Fill(track_truth_pt);
		      m_truthphi->Fill(track_truth_phi);
		      m_trutheta->Fill(track_truth_eta);

		      if(trkpt > ptlast){
			z_E[0] = fabs(trkpt / sin(trktheta));
			z_pz[0] = trkpt / tan(trktheta);
			z_px[0] = trkpt * sin(trkphi);
			z_py[0] = trkpt * cos(trkphi);
			z_eta[0] = trketa;
			zMC_E[0] = fabs(track_truth_pt / sin(track_truth_theta));
			zMC_pz[0] = track_truth_pt / tan(track_truth_theta);
			zMC_px[0] = track_truth_pt * sin(track_truth_phi);
			zMC_py[0] = track_truth_pt * cos(track_truth_phi);
			ptlast = trkpt;
			chargefirst = (int)charge;
		      }
		      
		      // Fill hitos
		      m_Deta_vs_eta -> Fill(track_truth_eta, trketa-track_truth_eta);
		      m_dphi_vs_eta -> Fill(track_truth_eta,trkphi-track_truth_phi);

		      float eta_barrel = 1.;		      
		      if (fabs(track_truth_eta) < eta_barrel) {
			m_dpt_vs_truthpt_barrel -> Fill(track_truth_charge*track_truth_pt,trkpt/track_truth_pt);
			m_Dqopt_vs_pt_barrel -> Fill(track_truth_charge*track_truth_pt, qOverPt-track_truth_qoverpt);
			m_dqopt_barrel -> Fill(qOverPt-track_truth_qoverpt);
			m_deta_barrel -> Fill(trketa-track_truth_eta);
			m_dphi_barrel -> Fill(trkphi-track_truth_phi);
			m_dphi_barrel_vs_phi -> Fill(track_truth_phi,trkphi-track_truth_phi);
			m_dz0_barrel -> Fill(trkz0-track_truth_z0);
			m_dd0_barrel -> Fill(trkd0-track_truth_d0);
		      } else if (track_truth_eta > eta_barrel) {
			m_dpt_vs_truthpt_eca -> Fill(track_truth_charge*track_truth_pt,trkpt/track_truth_pt);
			m_Dqopt_vs_pt_eca -> Fill(track_truth_charge*track_truth_pt, qOverPt-track_truth_qoverpt);
			m_dqopt_eca -> Fill(qOverPt-track_truth_qoverpt);
			m_deta_eca -> Fill(trketa-track_truth_eta);
			m_dphi_eca -> Fill(trkphi-track_truth_phi);
			m_dz0_eca -> Fill(trkz0-track_truth_z0);
			m_dd0_eca -> Fill(trkd0-track_truth_d0);
		      } else {
			m_dpt_vs_truthpt_ecc -> Fill(track_truth_charge*track_truth_pt,trkpt/track_truth_pt);
			m_Dqopt_vs_pt_ecc -> Fill(track_truth_charge*track_truth_pt, qOverPt-track_truth_qoverpt);
			m_dqopt_ecc -> Fill(qOverPt-track_truth_qoverpt);
			m_deta_ecc -> Fill(trketa-track_truth_eta);
			m_dphi_ecc -> Fill(trkphi-track_truth_phi);
			m_dz0_ecc -> Fill(trkz0-track_truth_z0);
			m_dd0_ecc -> Fill(trkd0-track_truth_d0);
		      }

		      float highpt = 10.;
		      if(track_truth_pt > highpt) {
			m_Dqopt_vs_eta_highpt -> Fill(track_truth_eta, qOverPt-track_truth_qoverpt);
		      
			// vs phi
			if (fabs(track_truth_eta) < eta_barrel) {
			  m_Dqopt_vs_phi_highpt_barrel -> Fill(track_truth_phi, qOverPt-track_truth_qoverpt);
			} else if (track_truth_eta > eta_barrel) {
			  m_Dqopt_vs_phi_highpt_eca -> Fill(track_truth_phi, qOverPt-track_truth_qoverpt);
			} else {
			  m_Dqopt_vs_phi_highpt_ecc -> Fill(track_truth_phi, qOverPt-track_truth_qoverpt);
			}

		      } else {
			m_Dqopt_vs_eta_lowpt -> Fill(track_truth_eta, qOverPt-track_truth_qoverpt);
		      
			// vs phi
			if (fabs(track_truth_eta) < eta_barrel) {
			  m_Dqopt_vs_phi_lowpt_barrel -> Fill(track_truth_phi, qOverPt-track_truth_qoverpt);
			} else if (track_truth_eta > eta_barrel) {
			  m_Dqopt_vs_phi_lowpt_eca -> Fill(track_truth_phi, qOverPt-track_truth_qoverpt);
			} else {
			  m_Dqopt_vs_phi_lowpt_ecc -> Fill(track_truth_phi, qOverPt-track_truth_qoverpt);
			}
		      }


		    }
		  }
		} else msg(MSG::WARNING) << " no genparptr->production_vertex() " << endmsg; 
	      } else msg(MSG::WARNING) << " no genparptr found " << endmsg; 
	    } else msg(MSG::WARNING) << " HMPL not Valid " << endmsg; 
	}
    } else msg(MSG::WARNING) << " No TruthMap found " << endmsg; 
  }
  
  float ptfirst = ptlast;
  ptlast = 0;
  trksItr  = trks->begin();
  for (; trksItr != trksItrE; ++trksItr) {     
    float trkphi         = -999;
    float trktheta       = -999;
    float trkpt          = -999;
    float qOverP         = -999;
    float trketa         = -999;
    float charge         = 0;
    
    // get fit quality and chi2 probability of track
    // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
    const Trk::Perigee* measPer = (*trksItr)->perigeeParameters();
    const AmgSymMatrix(5)* covariance = measPer ? measPer->covariance() : NULL;
    
    if (covariance==0) {
      msg(MSG::WARNING) << "No measured perigee parameters assigned to the track" << endmsg; 
    }
    else{  
      AmgVector(5) perigeeParams = measPer->parameters(); 
      trkphi   = perigeeParams[Trk::phi0];  
      trktheta = perigeeParams[Trk::theta];
      trkpt    = measPer->pT()/1000.;  
      trketa   = measPer->eta(); 
      qOverP   = perigeeParams[Trk::qOverP]*1000.;
      if (qOverP<0) charge=-1;
      else charge=+1; 
    }  
    
    if (TruthMap) {
           

      ElementLink<TrackCollection> tracklink;
      tracklink.setElement(const_cast<Trk::Track*>(*trksItr));
      tracklink.setStorableObject(*RecCollection);
      const ElementLink<TrackCollection> tracklink2=tracklink;
      
      TrackTruthCollection::const_iterator found = TruthMap->find(tracklink2);
      if (found != TruthMap->end())
	{
	  TrackTruth trtruth = found->second;
	  HepMcParticleLink HMPL = trtruth.particleLink();
          
	  if ( HMPL.isValid()) 
	    {
	      const HepMC::GenParticle *genparptr = HMPL.cptr(); 
	      
	      if (genparptr) {
		if (genparptr->production_vertex()) { 
		  
		  if(genparptr->pdg_id() == 0){
		    msg(MSG::WARNING) <<" Particle with PDG ID = 0! Status "<<endmsg;              
		  }else{
		    
		    const Trk::TrackParameters* generatedTrackPerigee = m_truthToTrack->makePerigeeParameters(genparptr);
		    if (!generatedTrackPerigee)   msg(MSG::WARNING) <<  "Unable to extrapolate genparticle to perigee!" << endmsg;
		    
		    if ( generatedTrackPerigee) {
		      float track_truth_qoverpt      = 1000. * generatedTrackPerigee->parameters()[Trk::qOverP]/sin(generatedTrackPerigee->parameters()[Trk::theta]);
		      float track_truth_phi          = generatedTrackPerigee->parameters()[Trk::phi0];  
		      float track_truth_theta        = generatedTrackPerigee->parameters()[Trk::theta];
		      delete  generatedTrackPerigee; 		      
		      float track_truth_pt           = 1./fabs(track_truth_qoverpt);  
		      //float track_truth_charge       = 1; 
		      //if(track_truth_qoverpt<0) track_truth_charge = -1;
		      if (track_truth_phi<0) track_truth_phi+=2*m_Pi;
		      if (msgLvl(MSG::VERBOSE)) msg() << "Found matched truth track with phi, PT = " << track_truth_phi << ", " << track_truth_pt << endmsg; 


		      if(trkpt > ptlast && trkpt < ptfirst && chargefirst*charge < 0 && trkpt > 15){
			z_E[1] = fabs(trkpt / sin(trktheta));
			z_pz[1] = trkpt / tan(trktheta);
			z_px[1] = trkpt * sin(trkphi);
			z_py[1] = trkpt * cos(trkphi);
			z_eta[1] = trketa;
			zMC_E[1] = fabs(track_truth_pt / sin(track_truth_theta));
			zMC_pz[1] = track_truth_pt / tan(track_truth_theta);
			zMC_px[1] = track_truth_pt * sin(track_truth_phi);
			zMC_py[1] = track_truth_pt * cos(track_truth_phi);
			ptlast = trkpt;
			z_true = true;
		      } 
		    }
		  }
		}
	      }
	    }
	}
    }
  }

  if(z_true){
    // build invariant mass of two highest pT tracks
    // M = sqrt(E1*E2 - px1*px2 - py1*py2 - pz1*pz2);
    float M = sqrt((z_E[0]+z_E[1])*(z_E[0]+z_E[1]) - (z_px[0]+z_px[1])*(z_px[0]+z_px[1]) - (z_py[0]+z_py[1])*(z_py[0]+z_py[1]) - (z_pz[0]+z_pz[1])*(z_pz[0]+z_pz[1]));
    float MMC = sqrt((zMC_E[0]+zMC_E[1])*(zMC_E[0]+zMC_E[1]) - (zMC_px[0]+zMC_px[1])*(zMC_px[0]+zMC_px[1]) - (zMC_py[0]+zMC_py[1])*(zMC_py[0]+zMC_py[1]) - (zMC_pz[0]+zMC_pz[1])*(zMC_pz[0]+zMC_pz[1]));
    m_Zmumu -> Fill(M);
    m_Zmumu_truth -> Fill(MMC);
    m_dZmumu -> Fill(M-MMC);

    if(fabs(z_eta[0]) < 1.0 && fabs(z_eta[1]) < 1.0) m_dZmumu_barrel->Fill(M-MMC);
    if(z_eta[0] > 1.0 && z_eta[1] > 1.0) m_dZmumu_eca->Fill(M-MMC);
    if(z_eta[0] < -1.0 && z_eta[1] < -1.0) m_dZmumu_ecc->Fill(M-MMC);
    
    if((fabs(z_eta[0]) < 1.0 && z_eta[1] > 1.0) ||
       (z_eta[0] > 1.0 && fabs(z_eta[1]) < 1.0)) m_dZmumu_barrel_eca->Fill(M-MMC);

    if((fabs(z_eta[0]) < 1.0 && z_eta[1] < -1.0) ||
       (z_eta[0] < -1.0 && fabs(z_eta[1]) < 1.0)) m_dZmumu_barrel_ecc->Fill(M-MMC);
  }      
    
  delete trks;
  return StatusCode::SUCCESS;
}


StatusCode IDAlignMonTruthComparison::procHistograms()
{
  //if( endOfLowStatFlag() ) {  }
  //if( endOfLumiBlockFlag() ) {  }
  if( endOfRunFlag() ) {

    // deta vs eta
    m_Deta_vs_eta->FitSlicesY(0,1,0,10);
    TH1F* Deta_vs_eta_1 = (TH1F*)gDirectory->Get("Deta_vs_eta_1");
    TH1F* Deta_vs_eta_2 = (TH1F*)gDirectory->Get("Deta_vs_eta_2");
    TH1F* Deta_vs_eta_chi2 = (TH1F*)gDirectory->Get("Deta_vs_eta_chi2");

    // vs pt
    m_Dqopt_vs_pt_barrel->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_pt_barrel_1 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_barrel_1");
    TH1F* Dqopt_vs_pt_barrel_2 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_barrel_2");
    TH1F* Dqopt_vs_pt_barrel_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_barrel_chi2");

    m_Dqopt_vs_pt_eca->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_pt_eca_1 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_eca_1");
    TH1F* Dqopt_vs_pt_eca_2 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_eca_2");
    TH1F* Dqopt_vs_pt_eca_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_eca_chi2");

    m_Dqopt_vs_pt_ecc->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_pt_ecc_1 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_ecc_1");
    TH1F* Dqopt_vs_pt_ecc_2 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_ecc_2");
    TH1F* Dqopt_vs_pt_ecc_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_ecc_chi2");

    // vs eta
    m_Dqopt_vs_eta_highpt->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_eta_highpt_1 = (TH1F*)gDirectory->Get("Dqopt_vs_eta_highpt_1");
    TH1F* Dqopt_vs_eta_highpt_2 = (TH1F*)gDirectory->Get("Dqopt_vs_eta_highpt_2");
    TH1F* Dqopt_vs_eta_highpt_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_eta_highpt_chi2");

    m_Dqopt_vs_eta_lowpt->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_eta_lowpt_1 = (TH1F*)gDirectory->Get("Dqopt_vs_eta_lowpt_1");
    TH1F* Dqopt_vs_eta_lowpt_2 = (TH1F*)gDirectory->Get("Dqopt_vs_eta_lowpt_2");
    TH1F* Dqopt_vs_eta_lowpt_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_eta_lowpt_chi2");

    // vs phi
    m_Dqopt_vs_phi_highpt_barrel->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_phi_highpt_barrel_1 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_barrel_1");
    TH1F* Dqopt_vs_phi_highpt_barrel_2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_barrel_2");
    TH1F* Dqopt_vs_phi_highpt_barrel_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_barrel_chi2");

    m_Dqopt_vs_phi_highpt_eca->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_phi_highpt_eca_1 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_eca_1");
    TH1F* Dqopt_vs_phi_highpt_eca_2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_eca_2");
    TH1F* Dqopt_vs_phi_highpt_eca_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_eca_chi2");

    m_Dqopt_vs_phi_highpt_ecc->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_phi_highpt_ecc_1 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_ecc_1");
    TH1F* Dqopt_vs_phi_highpt_ecc_2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_ecc_2");
    TH1F* Dqopt_vs_phi_highpt_ecc_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_ecc_chi2");

    // vs phi
    m_Dqopt_vs_phi_lowpt_barrel->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_phi_lowpt_barrel_1 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_barrel_1");
    TH1F* Dqopt_vs_phi_lowpt_barrel_2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_barrel_2");
    TH1F* Dqopt_vs_phi_lowpt_barrel_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_barrel_chi2");

    m_Dqopt_vs_phi_lowpt_eca->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_phi_lowpt_eca_1 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_eca_1");
    TH1F* Dqopt_vs_phi_lowpt_eca_2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_eca_2");
    TH1F* Dqopt_vs_phi_lowpt_eca_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_eca_chi2");

    m_Dqopt_vs_phi_lowpt_ecc->FitSlicesY(0,1,0,10);
    TH1F* Dqopt_vs_phi_lowpt_ecc_1 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_ecc_1");
    TH1F* Dqopt_vs_phi_lowpt_ecc_2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_ecc_2");
    TH1F* Dqopt_vs_phi_lowpt_ecc_chi2 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_ecc_chi2");


    // deta vs eta
    for (int i=1;i<=Deta_vs_eta_1->GetNbinsX();i++){
      double tmp=Deta_vs_eta_1->GetBinContent(i);
      m_deta_vs_eta_1->SetBinContent(i,tmp);
      tmp=Deta_vs_eta_2->GetBinContent(i);
      m_deta_vs_eta_2->SetBinContent(i,tmp);
      m_deta_vs_eta_1->SetBinError(i,tmp);
      tmp=Deta_vs_eta_chi2->GetBinContent(i);
      m_deta_vs_eta_chi2->SetBinContent(i,tmp);
    }


    // vs pT
    for (int i=1;i<=Dqopt_vs_pt_barrel_1->GetNbinsX();i++){
      double tmp=Dqopt_vs_pt_barrel_1->GetBinContent(i);
      m_dqopt_vs_pt_barrel_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_pt_barrel_2->GetBinContent(i);
      m_dqopt_vs_pt_barrel_2->SetBinContent(i,tmp);
      m_dqopt_vs_pt_barrel_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_pt_barrel_chi2->GetBinContent(i);
      m_dqopt_vs_pt_barrel_chi2->SetBinContent(i,tmp);

      tmp=Dqopt_vs_pt_eca_1->GetBinContent(i);
      m_dqopt_vs_pt_eca_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_pt_eca_2->GetBinContent(i);
      m_dqopt_vs_pt_eca_2->SetBinContent(i,tmp);
      m_dqopt_vs_pt_eca_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_pt_eca_chi2->GetBinContent(i);
      m_dqopt_vs_pt_eca_chi2->SetBinContent(i,tmp);

      tmp=Dqopt_vs_pt_ecc_1->GetBinContent(i);
      m_dqopt_vs_pt_ecc_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_pt_ecc_2->GetBinContent(i);
      m_dqopt_vs_pt_ecc_2->SetBinContent(i,tmp);
      m_dqopt_vs_pt_ecc_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_pt_ecc_chi2->GetBinContent(i);
      m_dqopt_vs_pt_ecc_chi2->SetBinContent(i,tmp);
    }
    
    // vs eta
    for (int i=1;i<=Dqopt_vs_eta_highpt_1->GetNbinsX();i++){
      double tmp=Dqopt_vs_eta_highpt_1->GetBinContent(i);
      m_dqopt_vs_eta_highpt_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_eta_highpt_2->GetBinContent(i);
      m_dqopt_vs_eta_highpt_2->SetBinContent(i,tmp);
      m_dqopt_vs_eta_highpt_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_eta_highpt_chi2->GetBinContent(i);
      m_dqopt_vs_eta_highpt_chi2->SetBinContent(i,tmp);

      tmp=Dqopt_vs_eta_lowpt_1->GetBinContent(i);
      m_dqopt_vs_eta_lowpt_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_eta_lowpt_2->GetBinContent(i);
      m_dqopt_vs_eta_lowpt_2->SetBinContent(i,tmp);
      m_dqopt_vs_eta_lowpt_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_eta_lowpt_chi2->GetBinContent(i);
      m_dqopt_vs_eta_lowpt_chi2->SetBinContent(i,tmp);
    }

    // vs phi
    for (int i=1;i<=Dqopt_vs_phi_highpt_barrel_1->GetNbinsX();i++){
      double tmp=Dqopt_vs_phi_highpt_barrel_1->GetBinContent(i);
      m_dqopt_vs_phi_highpt_barrel_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_phi_highpt_barrel_2->GetBinContent(i);
      m_dqopt_vs_phi_highpt_barrel_2->SetBinContent(i,tmp);
      m_dqopt_vs_phi_highpt_barrel_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_phi_highpt_barrel_chi2->GetBinContent(i);
      m_dqopt_vs_phi_highpt_barrel_chi2->SetBinContent(i,tmp);

      tmp=Dqopt_vs_phi_highpt_eca_1->GetBinContent(i);
      m_dqopt_vs_phi_highpt_eca_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_phi_highpt_eca_2->GetBinContent(i);
      m_dqopt_vs_phi_highpt_eca_2->SetBinContent(i,tmp);
      m_dqopt_vs_phi_highpt_eca_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_phi_highpt_eca_chi2->GetBinContent(i);
      m_dqopt_vs_phi_highpt_eca_chi2->SetBinContent(i,tmp);

      tmp=Dqopt_vs_phi_highpt_ecc_1->GetBinContent(i);
      m_dqopt_vs_phi_highpt_ecc_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_phi_highpt_ecc_2->GetBinContent(i);
      m_dqopt_vs_phi_highpt_ecc_2->SetBinContent(i,tmp);
      m_dqopt_vs_phi_highpt_ecc_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_phi_highpt_ecc_chi2->GetBinContent(i);
      m_dqopt_vs_phi_highpt_ecc_chi2->SetBinContent(i,tmp);

      tmp=Dqopt_vs_phi_lowpt_barrel_1->GetBinContent(i);
      m_dqopt_vs_phi_lowpt_barrel_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_phi_lowpt_barrel_2->GetBinContent(i);
      m_dqopt_vs_phi_lowpt_barrel_2->SetBinContent(i,tmp);
      m_dqopt_vs_phi_lowpt_barrel_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_phi_lowpt_barrel_chi2->GetBinContent(i);
      m_dqopt_vs_phi_lowpt_barrel_chi2->SetBinContent(i,tmp);

      tmp=Dqopt_vs_phi_lowpt_eca_1->GetBinContent(i);
      m_dqopt_vs_phi_lowpt_eca_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_phi_lowpt_eca_2->GetBinContent(i);
      m_dqopt_vs_phi_lowpt_eca_2->SetBinContent(i,tmp);
      m_dqopt_vs_phi_lowpt_eca_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_phi_lowpt_eca_chi2->GetBinContent(i);
      m_dqopt_vs_phi_lowpt_eca_chi2->SetBinContent(i,tmp);

      tmp=Dqopt_vs_phi_lowpt_ecc_1->GetBinContent(i);
      m_dqopt_vs_phi_lowpt_ecc_1->SetBinContent(i,tmp);
      tmp=Dqopt_vs_phi_lowpt_ecc_2->GetBinContent(i);
      m_dqopt_vs_phi_lowpt_ecc_2->SetBinContent(i,tmp);
      m_dqopt_vs_phi_lowpt_ecc_1->SetBinError(i,tmp);
      tmp=Dqopt_vs_phi_lowpt_ecc_chi2->GetBinContent(i);
      m_dqopt_vs_phi_lowpt_ecc_chi2->SetBinContent(i,tmp);
    } 

    TH1F* Deta_vs_eta_0 = (TH1F*)gDirectory->Get("Deta_vs_eta_0");
    delete Deta_vs_eta_0;
    delete Deta_vs_eta_1;
    delete Deta_vs_eta_2;
    delete Deta_vs_eta_chi2;

    TH1F* Dqopt_vs_pt_barrel_0 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_barrel_0");
    delete Dqopt_vs_pt_barrel_0;
    delete Dqopt_vs_pt_barrel_1;
    delete Dqopt_vs_pt_barrel_2;
    delete Dqopt_vs_pt_barrel_chi2;
    TH1F* Dqopt_vs_pt_eca_0 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_eca_0");
    delete Dqopt_vs_pt_eca_0;
    delete Dqopt_vs_pt_eca_1;
    delete Dqopt_vs_pt_eca_2;
    delete Dqopt_vs_pt_eca_chi2;
    TH1F* Dqopt_vs_pt_ecc_0 = (TH1F*)gDirectory->Get("Dqopt_vs_pt_ecc_0");
    delete Dqopt_vs_pt_ecc_0;
    delete Dqopt_vs_pt_ecc_1;
    delete Dqopt_vs_pt_ecc_2;
    delete Dqopt_vs_pt_ecc_chi2;

    TH1F* Dqopt_vs_eta_highpt_0 = (TH1F*)gDirectory->Get("Dqopt_vs_eta_highpt_0");
    delete Dqopt_vs_eta_highpt_0;
    delete Dqopt_vs_eta_highpt_1;
    delete Dqopt_vs_eta_highpt_2;
    delete Dqopt_vs_eta_highpt_chi2;
    TH1F* Dqopt_vs_eta_lowpt_0 = (TH1F*)gDirectory->Get("Dqopt_vs_eta_lowpt_0");
    delete Dqopt_vs_eta_lowpt_0;
    delete Dqopt_vs_eta_lowpt_1;
    delete Dqopt_vs_eta_lowpt_2;
    delete Dqopt_vs_eta_lowpt_chi2;

    TH1F* Dqopt_vs_phi_highpt_barrel_0 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_barrel_0");
    delete Dqopt_vs_phi_highpt_barrel_0;
    delete Dqopt_vs_phi_highpt_barrel_1;
    delete Dqopt_vs_phi_highpt_barrel_2;
    delete Dqopt_vs_phi_highpt_barrel_chi2;
    TH1F* Dqopt_vs_phi_highpt_eca_0 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_eca_0");
    delete Dqopt_vs_phi_highpt_eca_0;
    delete Dqopt_vs_phi_highpt_eca_1;
    delete Dqopt_vs_phi_highpt_eca_2;
    delete Dqopt_vs_phi_highpt_eca_chi2;
    TH1F* Dqopt_vs_phi_highpt_ecc_0 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_highpt_ecc_0");
    delete Dqopt_vs_phi_highpt_ecc_0;
    delete Dqopt_vs_phi_highpt_ecc_1;
    delete Dqopt_vs_phi_highpt_ecc_2;
    delete Dqopt_vs_phi_highpt_ecc_chi2;

    TH1F* Dqopt_vs_phi_lowpt_barrel_0 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_barrel_0");
    delete Dqopt_vs_phi_lowpt_barrel_0;
    delete Dqopt_vs_phi_lowpt_barrel_1;
    delete Dqopt_vs_phi_lowpt_barrel_2;
    delete Dqopt_vs_phi_lowpt_barrel_chi2;
    TH1F* Dqopt_vs_phi_lowpt_eca_0 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_eca_0");
    delete Dqopt_vs_phi_lowpt_eca_0;
    delete Dqopt_vs_phi_lowpt_eca_1;
    delete Dqopt_vs_phi_lowpt_eca_2;
    delete Dqopt_vs_phi_lowpt_eca_chi2;
    TH1F* Dqopt_vs_phi_lowpt_ecc_0 = (TH1F*)gDirectory->Get("Dqopt_vs_phi_lowpt_ecc_0");
    delete Dqopt_vs_phi_lowpt_ecc_0;
    delete Dqopt_vs_phi_lowpt_ecc_1;
    delete Dqopt_vs_phi_lowpt_ecc_2;
    delete Dqopt_vs_phi_lowpt_ecc_chi2;

    delete m_deta_vs_eta_2;
    delete m_dqopt_vs_pt_barrel_2;
    delete m_dqopt_vs_pt_eca_2;
    delete m_dqopt_vs_pt_ecc_2;
    delete m_dqopt_vs_eta_highpt_2;
    delete m_dqopt_vs_eta_lowpt_2;
    delete m_dqopt_vs_phi_highpt_barrel_2;
    delete m_dqopt_vs_phi_highpt_eca_2;
    delete m_dqopt_vs_phi_highpt_ecc_2;
    delete m_dqopt_vs_phi_lowpt_barrel_2;
    delete m_dqopt_vs_phi_lowpt_eca_2;
    delete m_dqopt_vs_phi_lowpt_ecc_2;
  }
  
  return StatusCode::SUCCESS;
}


