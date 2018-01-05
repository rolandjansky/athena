/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDPerfMonZee.cxx
// AUTHORS: Beate Heinemann, Tobias Golling, Sara Strandberg
// **********************************************************************

#include <sstream>
#include <math.h>
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TLorentzVector.h"


#include "GaudiKernel/IJobOptionsSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "IdDictDetDescr/IdDictManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"


#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

//#include "VxVertex/VxContainer.h"
//#include "VxVertex/VxCandidate.h"
//#include "VxVertex/RecVertex.h"
//#include "VxVertex/Vertex.h"
//#include "VxVertex/VxTrackAtVertex.h"

#include "egammaEvent/egammaParamDefs.h"
#include "egammaEvent/egammaPIDdefsObs.h"


#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "InDetPerformanceMonitoring/IDPerfMonZee.h"

// *********************************************************************
// Public Methods
// *********************************************************************

IDPerfMonZee::IDPerfMonZee( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_triggerChainName("NoTriggerSelection")
{
  declareProperty("tracksName",m_tracksName);
  declareProperty("electronsName",m_electronsName="Electrons");
  declareProperty("photonsName",m_photonsName="Photons");
  declareProperty("VxPrimContainerName",m_VxPrimContainerName="PrimaryVertices");
  declareProperty("emclustersName",m_emclustersName="LArClusterEM");
  declareProperty("metName",m_metName="MET_Reference_AntiKt4LCTopo");
  declareProperty("METFinalName",  m_metRefFinalName= "FinalClus");
  declareProperty("eoverp_standard_min",m_eoverp_standard_min=0.5);
  declareProperty("eoverp_standard_max",m_eoverp_standard_max=4.0);
  declareProperty("eoverp_tight_min",m_eoverp_tight_min=0.7);
  declareProperty("eoverp_tight_max",m_eoverp_tight_max=1.3);
  declareProperty("CheckRate",m_checkrate=1000);
  declareProperty("triggerChainName",m_triggerChainName);
  declareProperty("electronIDLevel",m_electronIDLevel = "Tight");

  m_region_strings.push_back("incl");
  m_region_strings.push_back("barrel");
  m_region_strings.push_back("eca");
  m_region_strings.push_back("ecc");

}


IDPerfMonZee::~IDPerfMonZee() { }


StatusCode IDPerfMonZee::initialize()
{

   m_histosBooked = 0;

  if (m_tracksName.empty() && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " no track collection given" << endmsg;

  StatusCode sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure() && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not initialize ManagedMonitorToolBase" << endmsg;

  //---Electron Likelihood tool---
  m_doIDCuts = true;
  ATH_MSG_INFO("IDPerfMonWenu::Initialize() -- Setting up electron LH tool.");
  m_LHTool2015 = new AsgElectronLikelihoodTool ("m_LHTool2015");
  if((m_LHTool2015->setProperty("primaryVertexContainer",m_VxPrimContainerName)).isFailure())
    ATH_MSG_WARNING("Failure setting primary vertex container " << m_VxPrimContainerName << "in electron likelihood tool");

  //Set up electron LH level
  std::string confDir = "ElectronPhotonSelectorTools/offline/mc15_20150712/";
  if(m_electronIDLevel == ""){
    ATH_MSG_WARNING("electronIDLevel is set to empty!  No electron ID cuts will be applied.");
    m_doIDCuts = false;
  }
  else{
    if((m_electronIDLevel != "Loose") && (m_electronIDLevel != "Medium") && (m_electronIDLevel != "Tight")){
      ATH_MSG_WARNING("Unknown electronIDLevel!! (Accepted values: Loose, Medium, Tight)");
      m_doIDCuts = false;
    }
    else{
      std::string configFile = confDir+"ElectronLikelihood"+m_electronIDLevel+"OfflineConfig2015.conf";
      ATH_MSG_INFO("Likelihood configuration file: " << configFile);
      if((m_LHTool2015->setProperty("ConfigFile",configFile)).isFailure())
	ATH_MSG_WARNING("Failure loading ConfigFile in electron likelihood tool.");
    }
  }
  StatusCode lh = m_LHTool2015->initialize();
  if(lh.isFailure()){
    ATH_MSG_WARNING("Electron likelihood tool initialize() failed!  Turning off electron LH cuts!");
    m_doIDCuts = false;
  }

  return StatusCode::SUCCESS;
}

StatusCode IDPerfMonZee::bookHistograms()
{
  MonGroup al_Zee_mon ( this, "IDPerfMon/Zee/" + m_triggerChainName, run);

  //if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
  //  // book histograms that are only made in the online environment...
  //}

  //if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
  //  // book histograms that are only relevant for cosmics data...
  //}


  //if ( newLowStatFlag() || newLumiBlockFlag() ) {
  //}


  if( newRunFlag() ) {
    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) return StatusCode::SUCCESS;
    // ***********************
    // Book event histograms
    // ***********************

    m_Nevents = new TH1F("Nevents", "Number of events processed", 1, -.5, 0.5);
    RegisterHisto(al_Zee_mon,m_Nevents);

    m_Check = new TH1F("Check","", 4, 0,4);
    RegisterHisto(al_Zee_mon,m_Check);
    m_Zee_invmass = new TH1F("Zee_invmass","Invariant mass of the two leading em clusters", 90, 0.,180.);
    RegisterHisto(al_Zee_mon,m_Zee_invmass);
    m_Zee_invmass_sel = new TH1F("Zee_invmass_sel","Invariant mass of the two leading em clusters after Zee selection", 80, 50.,130.);
    RegisterHisto(al_Zee_mon,m_Zee_invmass_sel);
    m_Zee_trk_invmass = new TH1F("Zee_trk_invmass","Invariant mass of the two tracks", 90, 0.,180.);
    RegisterHisto(al_Zee_mon,m_Zee_trk_invmass);
    m_Zee_trk_invmass_scaled = new TH1F("Zee_trk_invmass_scaled","Invariant mass of the two tracks scaled to per event", 90, 0.,180.);
    RegisterHisto(al_Zee_mon,m_Zee_trk_invmass_scaled);
    m_Zee_trk_invmass_sel = new TH1F("Zee_trk_invmass_sel","Invariant mass of the two tracks after Zee selection", 90, 0.,180.);
    RegisterHisto(al_Zee_mon,m_Zee_trk_invmass_sel);

    m_Zee_Eopasym_perevent = new TH1F("Zee_Eopasym_perevent", "E/p difference (pos-neg)/(pos+neg) per Z event for Zee EM-clusters", 60, -1.5, 1.5);
    RegisterHisto(al_Zee_mon,m_Zee_Eopasym_perevent);
    m_Zee_Eopasym_perevent_central = new TH1F("Zee_Eopasym_perevent_central", "E/p difference (pos-neg)/(pos+neg) per Z event for Zee EM-clusters with E/p between 0.7 and 1.3", 60, -1.5, 1.5);
    RegisterHisto(al_Zee_mon,m_Zee_Eopasym_perevent_central);

    // ***********************
    // Book cluster histograms
    // ***********************

    // first take care to clear all histogram vectors
    m_Zee_Eop.clear();
    m_Zee_Eopdiff.clear();
    m_Zee_Eop_plus.clear();
    m_Zee_Eop_minus.clear();
    m_Zee_eta.clear();
    m_Zee_phi.clear();
    m_Zee_deta.clear();
    m_Zee_deta_vs_eta.clear();
    m_Zee_deta_vs_phi.clear();
    m_Zee_absdeta_vs_eta.clear();
    m_Zee_absdeta_vs_phi.clear();
    m_Zee_dphi.clear();
    m_Zee_dphi_vs_eta.clear();
    m_Zee_dphi_vs_phi.clear();
    m_Zee_absdphi_vs_eta.clear();
    m_Zee_absdphi_vs_phi.clear();
    m_Zee_dphi_vs_phi_2d.clear();
    m_Zee_deta_vs_phi_2d.clear();
    m_Zee_dphi_vs_eta_2d.clear();
    m_Zee_deta_vs_eta_2d.clear();
    m_Zee_trackmatched_eta.clear();
    m_Zee_trackmatched_phi.clear();
    m_Zee_trackmatched_Eopmatched_eta.clear();
    m_Zee_trackmatched_Eopmatched_phi.clear();
    m_Zee_trackmatched_tightEopmatched_eta.clear();
    m_Zee_trackmatched_tightEopmatched_phi.clear();
    m_Zee_trackmatch_eff_vs_eta.clear();
    m_Zee_trackmatch_eff_vs_phi.clear();
    m_Zee_Eopmatch_eff_vs_eta.clear();
    m_Zee_Eopmatch_eff_vs_phi.clear();
    m_Zee_tightEopmatch_eff_vs_eta.clear();
    m_Zee_tightEopmatch_eff_vs_phi.clear();
    m_Zee_meanEop_vs_p.clear();
    m_Zee_meanEop_vs_invp.clear();
    m_Zee_meanEop_vs_E.clear();
    m_Zee_meanEop_vs_chargedp.clear();
    m_Zee_meanEop_vs_chargedE.clear();
    m_Zee_meanEop_vs_phi.clear();
    m_Zee_meanEop_vs_eta.clear();
    m_Zee_meanEop_vs_p_plus.clear();
    m_Zee_meanEop_vs_invp_plus.clear();
    m_Zee_meanEop_vs_E_plus.clear();
    m_Zee_meanEop_vs_phi_plus.clear();
    m_Zee_meanEop_vs_eta_plus.clear();
    m_Zee_meanEop_vs_p_minus.clear();
    m_Zee_meanEop_vs_invp_minus.clear();
    m_Zee_meanEop_vs_E_minus.clear();
    m_Zee_meanEop_vs_phi_minus.clear();
    m_Zee_meanEop_vs_eta_minus.clear();
    m_Zee_Eop_lt1_gt1.clear();
    m_Zee_Eopdiff_vs_p.clear();
    m_Zee_Eopdiff_vs_invp.clear();
    m_Zee_Eopdiff_vs_E.clear();
    m_Zee_Eopdiff_vs_phi.clear();
    m_Zee_Eopdiff_vs_eta.clear();
    m_Zee_Eop_lt1_vs_eta.clear();
    m_Zee_Eop_lt1_vs_phi.clear();
    m_Zee_Eop_gt1_vs_eta.clear();
    m_Zee_Eop_gt1_vs_phi.clear();
    m_Zee_frac_Eop_lt1_vs_eta.clear();
    m_Zee_frac_Eop_lt1_vs_phi.clear();
    m_Zee_Eop_05_25.clear();
    m_Zee_Eop_15_25.clear();
    m_Zee_frac_Eop_05_25_15_25.clear();

    for (int region=0; region!=Nregions; ++region) {
      std::string title;
      std::string name;

      name = "Zee_Eop_" + m_region_strings[region];
      title = "E/p for Zee EM-clusters in " + m_region_strings[region];
      m_Zee_Eop.push_back(new TH1F(name.c_str(),title.c_str(), 60, 0., 10.));
      if (region==incl) RegisterHisto(al_Zee_mon,m_Zee_Eop[region]);
      else RegisterHisto(al_Zee_mon,m_Zee_Eop[region]);

      name = "Zee_Eopdiff_" + m_region_strings[region];
      title = "E/p difference (pos-neg) for Zee EM-clusters in " + m_region_strings[region];
      m_Zee_Eopdiff.push_back(new TH1F(name.c_str(),title.c_str(), 10, 0., 2.));
      if (region==incl) RegisterHisto(al_Zee_mon,m_Zee_Eopdiff[region],true);
      else RegisterHisto(al_Zee_mon,m_Zee_Eopdiff[region],true);

      name = "Zee_Eop_plus_" + m_region_strings[region];
      title = "E/p for pos. charged Zee EM-clusters in " + m_region_strings[region];
      m_Zee_Eop_plus.push_back(new TH1F(name.c_str(),title.c_str(), 10, 0., 2.));
      RegisterHisto(al_Zee_mon,m_Zee_Eop_plus[region],true);

      name = "Zee_Eop_minus_" + m_region_strings[region];
      title = "E/p for neg. charged Zee EM-clusters in " + m_region_strings[region];
      m_Zee_Eop_minus.push_back(new TH1F(name.c_str(),title.c_str(), 10, 0., 2.));
      RegisterHisto(al_Zee_mon,m_Zee_Eop_minus[region],true);

      if (region != incl) continue;

      m_Zee_eta.push_back(new TH1F("Zee_eta","Eta of Zee EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_eta[region]);
      m_Zee_phi.push_back(new TH1F("Zee_phi","Phi of Zee EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_phi[region]);

      m_Zee_deta.push_back(new TH1F("Zee_deta","deltaEta(EM-cluster,track) in Zee events", 50, -0.5, 0.5));
      RegisterHisto(al_Zee_mon,m_Zee_deta[region]);
      m_Zee_deta_vs_eta.push_back(new TProfile("Zee_deta_vs_eta","deltaEta(EM-cluster,track) vs. eta in Zee events", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_deta_vs_eta[region]);
      m_Zee_deta_vs_phi.push_back(new TProfile("Zee_deta_vs_phi","deltaEta(EM-cluster,track) vs. phi in Zee events", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_deta_vs_phi[region]);
      m_Zee_absdeta_vs_eta.push_back(new TProfile("Zee_absdeta_vs_eta","Abs(deltaEta(EM-cluster,track)) vs. eta in Zee events", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_absdeta_vs_eta[region]);
      m_Zee_absdeta_vs_phi.push_back(new TProfile("Zee_absdeta_vs_phi","Abs(deltaEta(EM-cluster,track)) vs. phi in Zee events", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_absdeta_vs_phi[region]);
      m_Zee_dphi.push_back(new TH1F("Zee_dphi","deltaPhi(EM-cluster,track) in Zee events", 50, -0.5, 0.5));
      RegisterHisto(al_Zee_mon,m_Zee_dphi[region]);
      m_Zee_dphi_vs_eta.push_back(new TProfile("Zee_dphi_vs_eta","deltaPhi(EM-cluster,track) vs. eta in Zee events", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_dphi_vs_eta[region]);
      m_Zee_dphi_vs_phi.push_back(new TProfile("Zee_dphi_vs_phi","deltaPhi(EM-cluster,track) vs. phi in Zee events", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_dphi_vs_phi[region]);
      m_Zee_absdphi_vs_eta.push_back(new TProfile("Zee_absdphi_vs_eta","Abs(deltaPhi(EM-cluster,track)) vs. eta in Zee events", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_absdphi_vs_eta[region]);
      m_Zee_absdphi_vs_phi.push_back(new TProfile("Zee_absdphi_vs_phi","Abs(deltaPhi(EM-cluster,track)) vs. phi in Zee events", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_absdphi_vs_phi[region]);

      m_Zee_dphi_vs_phi_2d.push_back(new TH2F("Zee_dphi_vs_phi_2d","deltaPhi(EM-cluster,track) vs. phi in Zee events", 15, -3.15, 3.15 , 64, -0.1, 0.1));
      RegisterHisto(al_Zee_mon,m_Zee_dphi_vs_phi_2d[region]);
      m_Zee_deta_vs_phi_2d.push_back(new TH2F("Zee_deta_vs_phi_2d","deltaPhi(EM-cluster,track) vs. phi in Zee events", 15, -3.15, 3.15 , 64, -0.1, 0.1));
      RegisterHisto(al_Zee_mon,m_Zee_deta_vs_phi_2d[region]);
      m_Zee_dphi_vs_eta_2d.push_back(new TH2F("Zee_dphi_vs_eta_2d","deltaPhi(EM-cluster,track) vs. eta in Zee events", 15, -3., 3. , 64, -0.1, 0.1));
      RegisterHisto(al_Zee_mon,m_Zee_dphi_vs_eta_2d[region]);
      m_Zee_deta_vs_eta_2d.push_back(new TH2F("Zee_deta_vs_eta_2d","deltaPhi(EM-cluster,track) vs. eta in Zee events", 15, -3., 3. , 64, -0.1, 0.1));
      RegisterHisto(al_Zee_mon,m_Zee_deta_vs_eta_2d[region]);

      m_Zee_trackmatched_eta.push_back(new TH1F("Zee_trackmatched_eta","Eta of track-matched Zee EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_trackmatched_eta[region]);
      m_Zee_trackmatched_phi.push_back(new TH1F("Zee_trackmatched_phi","Phi of track-matched Zee EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_trackmatched_phi[region]);
      m_Zee_trackmatched_Eopmatched_eta.push_back(new TH1F("Zee_trackmatched_Eopmatched_eta","Eta of track-matched and E/p-matched Zee EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_trackmatched_Eopmatched_eta[region]);
      m_Zee_trackmatched_Eopmatched_phi.push_back(new TH1F("Zee_trackmatched_Eopmatched_phi","Phi of track-matched and E/p-matched Zee EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_trackmatched_Eopmatched_phi[region]);
      m_Zee_trackmatched_tightEopmatched_eta.push_back(new TH1F("Zee_trackmatched_tightEopmatched_eta","Eta of track-matched and tight E/p-matched Zee EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_trackmatched_tightEopmatched_eta[region]);
      m_Zee_trackmatched_tightEopmatched_phi.push_back(new TH1F("Zee_trackmatched_tightEopmatched_phi","Phi of track-matched and tight E/p-matched Zee EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_trackmatched_tightEopmatched_phi[region]);

      m_Zee_trackmatch_eff_vs_eta.push_back(new TH1F("Zee_trackmatch_eff_vs_eta","Track-match efficiency for Zee EM-clusters vs. eta", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_trackmatch_eff_vs_eta[region],1);
      m_Zee_trackmatch_eff_vs_phi.push_back(new TH1F("Zee_trackmatch_eff_vs_phi","Track-match efficiency for Zee EM-clusters vs. phi", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_trackmatch_eff_vs_phi[region],1);
      m_Zee_Eopmatch_eff_vs_eta.push_back(new TH1F("Zee_Eopmatch_eff_vs_eta","E/p-match efficiency for Zee EM-clusters vs. eta", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_Eopmatch_eff_vs_eta[region],1);
      m_Zee_Eopmatch_eff_vs_phi.push_back(new TH1F("Zee_Eopmatch_eff_vs_phi","E/p-match efficiency for Zee EM-clusters vs. phi", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_Eopmatch_eff_vs_phi[region],1);
      m_Zee_tightEopmatch_eff_vs_eta.push_back(new TH1F("Zee_tightEopmatch_eff_vs_eta","Tight E/p-match efficiency for Zee EM-clusters vs. eta", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_tightEopmatch_eff_vs_eta[region],1);
      m_Zee_tightEopmatch_eff_vs_phi.push_back(new TH1F("Zee_tightEopmatch_eff_vs_phi","Tight E/p-match efficiency for Zee EM-clusters vs. phi", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_tightEopmatch_eff_vs_phi[region],1);

      m_Zee_meanEop_vs_p.push_back(new TProfile("Zee_meanEop_vs_p", "Mean E/p vs p for Zee EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_p[region]);
      m_Zee_meanEop_vs_invp.push_back(new TProfile("Zee_meanEop_vs_invp", "Mean E/p vs 1/p for Zee EM-clusters", 15, 0., 0.05));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_invp[region]);
      m_Zee_meanEop_vs_E.push_back(new TProfile("Zee_meanEop_vs_E", "Mean E/p vs E for Zee EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_E[region]);
      m_Zee_meanEop_vs_chargedp.push_back(new TProfile("Zee_meanEop_vs_chargedp", "Mean E/p vs charge*p for Zee EM-clusters", 30, -300., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_chargedp[region]);
      m_Zee_meanEop_vs_chargedE.push_back(new TProfile("Zee_meanEop_vs_chargedE", "Mean E/p vs charge*E for Zee EM-clusters", 30, -300., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_chargedE[region]);
      m_Zee_meanEop_vs_phi.push_back(new TProfile("Zee_meanEop_vs_phi", "Mean E/p vs phi for Zee EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_phi[region]);
      m_Zee_meanEop_vs_eta.push_back(new TProfile("Zee_meanEop_vs_eta", "Mean E/p vs eta for Zee EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_eta[region]);

      m_Zee_meanEop_vs_p_plus.push_back(new TProfile("Zee_meanEop_vs_p_plus", "Mean E/p vs p for pos. charged Zee EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_p_plus[region]);
      m_Zee_meanEop_vs_invp_plus.push_back(new TProfile("Zee_meanEop_vs_invp_plus", "Mean E/p vs 1/p for pos. charged Zee EM-clusters", 15, 0., 0.05));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_invp_plus[region]);
      m_Zee_meanEop_vs_E_plus.push_back(new TProfile("Zee_meanEop_vs_E_plus", "Mean E/p vs E for pos. charged Zee EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_E_plus[region]);
      m_Zee_meanEop_vs_phi_plus.push_back(new TProfile("Zee_meanEop_vs_phi_plus", "Mean E/p vs phi for pos. charged Zee EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_phi_plus[region]);
      m_Zee_meanEop_vs_eta_plus.push_back(new TProfile("Zee_meanEop_vs_eta_plus", "Mean E/p vs eta for pos. charged Zee EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_eta_plus[region]);

      m_Zee_meanEop_vs_p_minus.push_back(new TProfile("Zee_meanEop_vs_p_minus", "Mean E/p vs p for neg. charged Zee EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_p_minus[region]);
      m_Zee_meanEop_vs_invp_minus.push_back(new TProfile("Zee_meanEop_vs_invp_minus", "Mean E/p vs 1/p for neg. charged Zee EM-clusters", 15, 0., 0.05));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_invp_minus[region]);
      m_Zee_meanEop_vs_E_minus.push_back(new TProfile("Zee_meanEop_vs_E_minus", "Mean E/p vs E for neg. charged Zee EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_E_minus[region]);
      m_Zee_meanEop_vs_phi_minus.push_back(new TProfile("Zee_meanEop_vs_phi_minus", "Mean E/p vs phi for neg. charged Zee EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_phi_minus[region]);
      m_Zee_meanEop_vs_eta_minus.push_back(new TProfile("Zee_meanEop_vs_eta_minus", "Mean E/p vs eta for neg. charged Zee EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_meanEop_vs_eta_minus[region]);

      m_Zee_Eop_lt1_gt1.push_back(new TH1F("Zee_Eop_lt1_gt1", "E/p below/above 1 for Zee EM-clusters", 2, 0.5, 1.5));
      RegisterHisto(al_Zee_mon,m_Zee_Eop_lt1_gt1[region]);

      m_Zee_Eopdiff_vs_p.push_back(new TProfile("Zee_Eopdiff_vs_p", "E/p difference (pos-neg) vs p for Zee EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_Eopdiff_vs_p[region]);
      m_Zee_Eopdiff_vs_invp.push_back(new TProfile("Zee_Eopdiff_vs_invp", "E/p difference (pos-neg) vs 1/p for Zee EM-clusters", 15, 0., 0.05));
      RegisterHisto(al_Zee_mon,m_Zee_Eopdiff_vs_invp[region]);
      m_Zee_Eopdiff_vs_E.push_back(new TProfile("Zee_Eopdiff_vs_E", "E/p difference (pos-neg) vs E for Zee EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Zee_mon,m_Zee_Eopdiff_vs_E[region]);
      m_Zee_Eopdiff_vs_phi.push_back(new TProfile("Zee_Eopdiff_vs_phi", "E/p difference (pos-neg) vs phi for Zee EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_Eopdiff_vs_phi[region]);
      m_Zee_Eopdiff_vs_eta.push_back(new TProfile("Zee_Eopdiff_vs_eta", "E/p difference (pos-neg) vs eta for Zee EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_Eopdiff_vs_eta[region]);

      m_Zee_Eop_lt1_vs_eta.push_back(new TH1F("Zee_Eop_lt1_vs_eta","Eta of Zee EM-clusters with E/p < 1 (not incl. small E/p)", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_Eop_lt1_vs_eta[region]);
      m_Zee_Eop_lt1_vs_phi.push_back(new TH1F("Zee_Eop_lt1_vs_phi","Phi of Zee EM-clusters with E/p < 1 (not incl. small E/p)", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_Eop_lt1_vs_phi[region]);
      m_Zee_Eop_gt1_vs_eta.push_back(new TH1F("Zee_Eop_gt1_vs_eta","Eta of Zee EM-clusters with E/p < 1 (not incl. small E/p)", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_Eop_gt1_vs_eta[region]);
      m_Zee_Eop_gt1_vs_phi.push_back(new TH1F("Zee_Eop_gt1_vs_phi","Phi of Zee EM-clusters with E/p < 1 (not incl. small E/p)", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_Eop_gt1_vs_phi[region]);
      m_Zee_frac_Eop_lt1_vs_eta.push_back(new TH1F("Zee_frac_Eop_lt1_vs_eta","Fraction of Zee EM-clusters with E/p < 1 (not incl. small E/p) vs Eta", 15, -3., 3.));
      RegisterHisto(al_Zee_mon,m_Zee_frac_Eop_lt1_vs_eta[region],1);
      m_Zee_frac_Eop_lt1_vs_phi.push_back(new TH1F("Zee_frac_Eop_lt1_vs_phi","Fraction of Zee EM-clusters with E/p < 1 (not incl. small E/p) vs Phi", 15, -3.15, 3.15));
      RegisterHisto(al_Zee_mon,m_Zee_frac_Eop_lt1_vs_phi[region],1);

      m_Zee_Eop_05_25.push_back(new TH1F("Zee_Eop_05_25","Number of Zee events with 0.5 < E/p 2.5", 1, 0., 1.));
      RegisterHisto(al_Zee_mon,m_Zee_Eop_05_25[region]);
      m_Zee_Eop_15_25.push_back(new TH1F("Zee_Eop_15_25","Number of Zee events with 1.5 < E/p 2.5", 1, 0., 1.));
      RegisterHisto(al_Zee_mon,m_Zee_Eop_15_25[region]);
      m_Zee_frac_Eop_05_25_15_25.push_back(new TH1F("Zee_frac_Eop_05_25_15_25","Fraction of Zee EM-clusters with 1.5 < E/p < 2.5 and 0.5 < E/p < 2.5", 1, 0., 1.));
      RegisterHisto(al_Zee_mon,m_Zee_frac_Eop_05_25_15_25[region],1);
    }

    m_histosBooked++;
  }
  return StatusCode::SUCCESS;
}

void IDPerfMonZee::RegisterHisto(MonGroup& mon, TH1* histo, bool doSumw2) {

  if (doSumw2) histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Cannot book TH1 Histogram:" << endmsg;
  }
}

void IDPerfMonZee::RegisterHisto(MonGroup& mon, TProfile* histo) {

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Cannot book TProfile Histogram:" << endmsg;
  }
}

void IDPerfMonZee::RegisterHisto(MonGroup& mon, TH2* histo, bool doSumw2) {

  if (doSumw2) histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Cannot book TH2 Histogram:" << endmsg;
  }
}

StatusCode IDPerfMonZee::fillHistograms()
{
  ATH_MSG_VERBOSE("In fillHistograms()");
  int nevents = (int) m_Nevents->GetEntries();

  // get electron container from storegate
  const xAOD::ElectronContainer* electrons = 0;
  if (!evtStore()->contains<xAOD::ElectronContainer>(m_electronsName)) {
    if (nevents == 1 && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No Collection with name " << m_electronsName << " found in StoreGate" << endmsg;
    else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No Collection with name " << m_electronsName << " found in StoreGate" << endmsg;
    return StatusCode::SUCCESS;
  }
  else {
    StatusCode sc = evtStore()->retrieve(electrons,m_electronsName);
    if (sc.isFailure()) {
      if(nevents == 1 && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve Collection " << m_electronsName << " from StoreGate" << endmsg;
      else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not retrieve Collection " << m_electronsName << " from StoreGate" << endmsg;
      return StatusCode::SUCCESS;
    }
  }
  ATH_MSG_VERBOSE("This event contains " << electrons->size() << " electrons.");

  // get photon container from storegate
  const xAOD::PhotonContainer* photons = 0;
  if (!evtStore()->contains<xAOD::PhotonContainer>(m_photonsName)) {
    if (nevents == 1 && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No Collection with name " << m_photonsName << " found in StoreGate" << endmsg;
    else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No Collection with name " << m_photonsName << " found in StoreGate" << endmsg;
    return StatusCode::SUCCESS;
  }
  else {
    StatusCode sc = evtStore()->retrieve(photons,m_photonsName);
    if (sc.isFailure()) {
      if(nevents == 1 && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve Collection " << m_photonsName << " from StoreGate" << endmsg;
      else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not retrieve Collection " << m_photonsName << " from StoreGate" << endmsg;
      return StatusCode::SUCCESS;
    }
  }
  //ATH_MSG_DEBUG("This event contains " << photons->size() << " photons.");

  // get emcluster container from storegate
  const xAOD::CaloClusterContainer* emclusters = 0;
  if (!evtStore()->contains<xAOD::CaloClusterContainer>(m_emclustersName)) {
    if (nevents == 1 && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No Collection with name " << m_emclustersName << " found in StoreGate" << endmsg;
    else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No Collection with name " << m_emclustersName << " found in StoreGate" << endmsg;
    return StatusCode::SUCCESS;
  }
  else {
    StatusCode sc = evtStore()->retrieve(emclusters,m_emclustersName);
    if (sc.isFailure()) {
      if(nevents == 1 && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve Collection " << m_emclustersName << " from StoreGate" << endmsg;
      else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not retrieve Collection " << m_emclustersName << " from StoreGate" << endmsg;
      return StatusCode::SUCCESS;
    }
  }

  // get track container from storegate
  const xAOD::TrackParticleContainer* tracks = 0;
  if (!evtStore()->contains<xAOD::TrackParticleContainer>(m_tracksName)) {
    if (nevents == 1 && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No Collection with name " << m_tracksName << " found in StoreGate" << endmsg;
    else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No Collection with name " << m_tracksName << " found in StoreGate" << endmsg;
    return StatusCode::SUCCESS;
  }
  else {
    StatusCode sc = evtStore()->retrieve(tracks,m_tracksName);
    if (sc.isFailure()) {
      if(nevents == 1) msg(MSG::WARNING) << "Could not retrieve Collection " << m_tracksName << " from StoreGate" << endmsg;
      else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not retrieve Collection " << m_tracksName << " from StoreGate" << endmsg;
      return StatusCode::SUCCESS;
    }
  }

  //  // get met container from storegate
  if (msgLvl(MSG::DEBUG)) {
     const xAOD::MissingETContainer* final_met = 0;
     if (!evtStore()->contains<xAOD::MissingETContainer>(m_metName)) {
       if (nevents == 1 && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No Collection with name " << m_metName << " found in StoreGate" << endmsg;
       else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No Collection with name " << m_metName << " found in StoreGate" << endmsg;
       return StatusCode::SUCCESS;
     }
     else {
       StatusCode sc = evtStore()->retrieve(final_met,m_metName);
       if (sc.isFailure()) {
         if(nevents == 1 && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve Collection " << m_metName << " from StoreGate" << endmsg;
         else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not retrieve Collection " << m_metName << " from StoreGate" << endmsg;
         return StatusCode::SUCCESS;
       }
     }
     // ******************
     // Get the missing ET
     // ******************
     const xAOD::MissingET *met;
     met = (*final_met)[m_metRefFinalName];
     if (met) {
         msg(MSG::DEBUG) << "MET = " << met->met() << endmsg;   
     }
  }

  m_Nevents->Fill(0.);

  // *******************
  // Look at EM clusters
  // *******************

  const xAOD::CaloCluster* LeadingEMcluster = getLeadingEMcluster(photons, electrons);
  const xAOD::CaloCluster* SecondLeadingEMcluster = getLeadingEMcluster(photons, electrons, LeadingEMcluster);
  
  if((LeadingEMcluster != 0) && (SecondLeadingEMcluster != 0))
    ATH_MSG_DEBUG("Event has a leading and second leading EM cluster!");

  if (LeadingEMcluster != 0 && SecondLeadingEMcluster != 0) {
    int leading_eta_region = etaRegion(LeadingEMcluster->etaBE(2));
    int second_leading_eta_region = etaRegion(SecondLeadingEMcluster->etaBE(2));
    double leading_dPhi = electronTrackMatchEta(tracks,LeadingEMcluster);
    double leading_dEta = electronTrackMatchPhi(tracks,LeadingEMcluster);
    double second_leading_dPhi = electronTrackMatchEta(tracks,SecondLeadingEMcluster);
    double second_leading_dEta = electronTrackMatchPhi(tracks,SecondLeadingEMcluster);
    const xAOD::TrackParticle* track_leading_emcluster = electronTrackMatch(tracks,LeadingEMcluster);
    const xAOD::TrackParticle* track_second_leading_emcluster = electronTrackMatch(tracks,SecondLeadingEMcluster);

    if( (track_leading_emcluster != 0) && (track_second_leading_emcluster != 0) )
      ATH_MSG_DEBUG("Event has a tracks matched to both clusters!");

    // *********************
    // Fill event histograms
    // *********************

    // get cluster, track invariant masses and fill pre-selection histograms
    double cluster_invmass = InvMass(LeadingEMcluster,SecondLeadingEMcluster);
     ATH_MSG_DEBUG("Cluster invariant mass: " << cluster_invmass);
    if (cluster_invmass > 0.) m_Zee_invmass->Fill(cluster_invmass);
    double track_invmass = 0.;
    if (track_leading_emcluster != 0 && track_second_leading_emcluster != 0) {
      track_invmass = InvMass(track_leading_emcluster,track_second_leading_emcluster);
      ATH_MSG_DEBUG("Track invariant mass: " << track_invmass);
      if (track_invmass > 0.) m_Zee_trk_invmass->Fill(track_invmass);
    }

    // does the event pass the Zee selection?
    int selected = isZee(LeadingEMcluster,SecondLeadingEMcluster,tracks);
    ATH_MSG_DEBUG("Event passed " << 3-selected << "/3 Zee cuts");

    // fill histograms only with selected events
    if(selected == 0){
    //if ( selected <= 1 ) {
      ATH_MSG_DEBUG("Event passed selection -- filling histograms");
      if (cluster_invmass > 0.) m_Zee_invmass_sel->Fill(cluster_invmass);
      if (track_invmass > 0.) m_Zee_trk_invmass_sel->Fill(track_invmass);
      //} // selected <= 1 loop used to end here

      // fill e/p histos from SELECTED Zee events
      if (track_leading_emcluster != 0 && track_second_leading_emcluster !=0) {
	double eoverp_pos = -99.;
	double eoverp_neg = -99.;
	double track_leading_emcluster_p = track_leading_emcluster->pt()*cosh(track_leading_emcluster->eta());
	double track_second_leading_emcluster_p = track_second_leading_emcluster->pt()*cosh(track_second_leading_emcluster->eta());

	if (track_leading_emcluster->charge() == 1. && track_second_leading_emcluster->charge() == -1.) {
	  eoverp_pos = LeadingEMcluster->e()/track_leading_emcluster_p;
	  eoverp_neg = SecondLeadingEMcluster->e()/track_second_leading_emcluster_p;
	}
	else if (track_leading_emcluster->charge() == -1. && track_second_leading_emcluster->charge() == 1.) {
	  eoverp_neg = LeadingEMcluster->e()/track_leading_emcluster_p;
	  eoverp_pos = SecondLeadingEMcluster->e()/track_second_leading_emcluster_p;
	}

	double eoverpasym = -99.;
	if (eoverp_pos+eoverp_neg != 0.) {
	  eoverpasym =  (eoverp_pos-eoverp_neg) / (eoverp_pos+eoverp_neg);
	  m_Zee_Eopasym_perevent->Fill(eoverpasym);
	  if ( (eoverp_pos > m_eoverp_tight_min) && (eoverp_pos < m_eoverp_tight_max) && (eoverp_neg > m_eoverp_tight_min) && (eoverp_neg < m_eoverp_tight_max) ) {
	    m_Zee_Eopasym_perevent_central->Fill(eoverpasym);
	  }
	}
      }
    }

    // ***********************
    // Fill cluster histograms
    // ***********************
    if (selected == 0) {
      // ************************
      // Fill for leading cluster
      // ************************
      FillHistosPerCluster(LeadingEMcluster, track_leading_emcluster, leading_eta_region, leading_dEta, leading_dPhi);
      FillHistosPerCluster(LeadingEMcluster, track_leading_emcluster, incl, leading_dEta, leading_dPhi);
      // *******************************
      // Fill for second leading cluster
      // *******************************
      FillHistosPerCluster(SecondLeadingEMcluster, track_second_leading_emcluster, second_leading_eta_region, second_leading_dEta, second_leading_dPhi);
      FillHistosPerCluster(SecondLeadingEMcluster, track_second_leading_emcluster, incl, second_leading_dEta, second_leading_dPhi);
    }
  }

  return StatusCode::SUCCESS;
}

void IDPerfMonZee::makeEffHisto(TH1F* h_num, TH1F* h_denom, TH1F* h_eff) {
  h_eff->Divide(h_num,h_denom,1.,1.,"B");
  //   int Nbins;
  //   Nbins = h_num->GetNbinsX();
  //   for (int bin=0; bin!=Nbins; ++bin) {
  //     int Npass = int(h_num->GetBinContent(bin+1));
  //     int Nfail = int(h_denom->GetBinContent(bin+1)) - Npass;
  //     double x = h_denom->GetBinCenter(bin+1);
  //     for (int pass=0; pass!=Npass; ++pass) {
  //       h_eff->Fill(x,1.);
  //     }
  //     for (int fail=0; fail!=Nfail; ++fail) {
  //       h_eff->Fill(x,0.);
  //     }
  //   }
}

StatusCode IDPerfMonZee::procHistograms()
{

  //if( endOfLowStatFlag() || endOfLumiBlockFlag() ) {
  //
  //}

  if( endOfRunFlag() ) {

    // PostProcess Zee histograms

    for (int region=0; region!=1; ++region) {
      makeEffHisto(m_Zee_trackmatched_eta[region],m_Zee_eta[region],m_Zee_trackmatch_eff_vs_eta[region]);
      makeEffHisto(m_Zee_trackmatched_phi[region],m_Zee_phi[region],m_Zee_trackmatch_eff_vs_phi[region]);
      makeEffHisto(m_Zee_trackmatched_Eopmatched_eta[region],m_Zee_trackmatched_eta[region],m_Zee_Eopmatch_eff_vs_eta[region]);
      makeEffHisto(m_Zee_trackmatched_Eopmatched_phi[region],m_Zee_trackmatched_phi[region],m_Zee_Eopmatch_eff_vs_phi[region]);
      makeEffHisto(m_Zee_trackmatched_tightEopmatched_eta[region],m_Zee_trackmatched_eta[region],m_Zee_tightEopmatch_eff_vs_eta[region]);
      makeEffHisto(m_Zee_trackmatched_tightEopmatched_phi[region],m_Zee_trackmatched_phi[region],m_Zee_tightEopmatch_eff_vs_phi[region]);

      // these were commented out
      m_Zee_Eopdiff[region]->Add(m_Zee_Eop_plus[region],m_Zee_Eop_minus[region],1.,-1);
      m_Zee_Eopdiff_vs_p[region]->Add(m_Zee_meanEop_vs_p_plus[region],m_Zee_meanEop_vs_p_minus[region],1.,-1);
      m_Zee_Eopdiff_vs_invp[region]->Add(m_Zee_meanEop_vs_invp_plus[region],m_Zee_meanEop_vs_invp_minus[region],1.,-1);
      m_Zee_Eopdiff_vs_E[region]->Add(m_Zee_meanEop_vs_E_plus[region],m_Zee_meanEop_vs_E_minus[region],1.,-1);
      m_Zee_Eopdiff_vs_phi[region]->Add(m_Zee_meanEop_vs_phi_plus[region],m_Zee_meanEop_vs_phi_minus[region],1.,-1);
      m_Zee_Eopdiff_vs_eta[region]->Add(m_Zee_meanEop_vs_eta_plus[region],m_Zee_meanEop_vs_eta_minus[region],1.,-1);

      makeEffHisto(m_Zee_Eop_lt1_vs_eta[region],m_Zee_eta[region],m_Zee_frac_Eop_lt1_vs_eta[region]);
      makeEffHisto(m_Zee_Eop_lt1_vs_phi[region],m_Zee_phi[region],m_Zee_frac_Eop_lt1_vs_phi[region]);
      makeEffHisto(m_Zee_Eop_15_25[region],m_Zee_Eop_05_25[region],m_Zee_frac_Eop_05_25_15_25[region]);
    }

  }
  return StatusCode::SUCCESS;
}


const xAOD::CaloCluster* IDPerfMonZee::getLeadingEMcluster(const xAOD::CaloClusterContainer* clusters, const xAOD::CaloCluster* omitCluster) const {
  // iterators over the emcluster container
  xAOD::CaloClusterContainer::const_iterator Itr = clusters->begin();
  xAOD::CaloClusterContainer::const_iterator ItrEnd = clusters->end();

  const xAOD::CaloCluster* leading_emcluster = 0;

  float max_pt = 0.;
  for (; Itr != ItrEnd; ++Itr) {
    const xAOD::CaloCluster* cl = (*Itr);
    if (cl == omitCluster) continue;
    double deltaR = sqrt(pow(fabs(cl->phi() - omitCluster->phi()),2) + pow(fabs(cl->eta() - omitCluster->eta()),2));
    if(deltaR < 0.005) continue;
    if (cl->pt()/Gaudi::Units::GeV < 10.) continue;
    if (cl->pt() > max_pt) {
      leading_emcluster = cl;
      max_pt = cl->pt();
    }
  }
  return leading_emcluster;

}

const xAOD::CaloCluster* IDPerfMonZee::getLeadingEMcluster(const xAOD::PhotonContainer* photons, const xAOD::ElectronContainer* electrons, const xAOD::CaloCluster* omitCluster) const {

  // iterators over the photon container
  xAOD::PhotonContainer::const_iterator photonItr = photons->begin();
  xAOD::PhotonContainer::const_iterator photonItrEnd = photons->end();

  // iterators over the electron container
  xAOD::ElectronContainer::const_iterator electronItr = electrons->begin();
  xAOD::ElectronContainer::const_iterator electronItrEnd = electrons->end();

  const xAOD::CaloCluster* leading_emcluster = 0;
  bool LHSel;
  float max_pt = 0.;

  for (; electronItr != electronItrEnd; ++electronItr) {
    ATH_MSG_DEBUG("Checking likelihood");
    const xAOD::Electron* em = (*electronItr);
    // check ID
    if(m_doIDCuts){
      LHSel = false;
      LHSel = m_LHTool2015->accept(em);
      if(!LHSel) continue;
      ATH_MSG_DEBUG("Electron passes " << m_electronIDLevel << " likelihood selection");
    }

    const xAOD::CaloCluster* cl = em->caloCluster();
    if (cl == omitCluster) continue;
    double deltaR = !omitCluster ? 1.0 : sqrt(pow(fabs(cl->phi() - omitCluster->phi()),2) + pow(fabs(cl->eta() - omitCluster->eta()),2));
    if(deltaR < 0.005) continue;
    if (cl->pt()/Gaudi::Units::GeV < 20.) continue;
    if (cl->pt() > max_pt) {
      leading_emcluster = cl;
      max_pt = cl->pt();
    }
  }

  ATH_MSG_DEBUG("leading_emcluster: " << leading_emcluster); 

  return leading_emcluster;

}


const xAOD::TrackParticle* IDPerfMonZee::electronTrackMatch(const xAOD::TrackParticleContainer* tracks, const xAOD::CaloCluster* cluster, double dEta, double dPhi) const {

 // iterators over the track container
  xAOD::TrackParticleContainer::const_iterator Itr = tracks->begin();
  xAOD::TrackParticleContainer::const_iterator ItrEnd = tracks->end();

  const xAOD::TrackParticle* matched_track = 0;
  double min_dR = 1.0e+20;

  for (; Itr!=ItrEnd; ++Itr) {
    const xAOD::TrackParticle* track = (*Itr);
    double deta = cluster->etaBE(2)-track->eta();
    double dphi = cluster->phi()-track->phi();
    double dr = TMath::Sqrt(deta*deta + dphi*dphi);
    if (dr < min_dR && TMath::Abs(deta) < dEta && TMath::Abs(dphi) < dPhi) {
      min_dR = dr;
      matched_track = track;
    }
  }

  return matched_track;

}

double IDPerfMonZee::electronTrackMatchEta(const xAOD::TrackParticleContainer* tracks, const xAOD::CaloCluster* cluster, double dEta) const {

 // iterators over the track container
  xAOD::TrackParticleContainer::const_iterator Itr = tracks->begin();
  xAOD::TrackParticleContainer::const_iterator ItrEnd = tracks->end();

  const xAOD::TrackParticle* matched_track = 0;
  double min_dEta = 1.0e+20;

  for (; Itr!=ItrEnd; ++Itr) {
    const xAOD::TrackParticle* track = (*Itr);
    double deta = TMath::Abs(cluster->etaBE(2)-track->eta());
    if (deta < min_dEta && deta < dEta) {
      min_dEta = deta;
      matched_track = track;
    }
  }

  double dPhi = 1.0e+20;
  if (matched_track != 0) dPhi = signedDeltaPhi(cluster->phi(),matched_track->phi());

  return dPhi;

}

double IDPerfMonZee::electronTrackMatchPhi(const xAOD::TrackParticleContainer* tracks, const xAOD::CaloCluster* cluster, double dPhi) const {

 // iterators over the track container
  xAOD::TrackParticleContainer::const_iterator Itr = tracks->begin();
  xAOD::TrackParticleContainer::const_iterator ItrEnd = tracks->end();

  const xAOD::TrackParticle* matched_track = 0;
  double min_dPhi = 1.0e+20;

  for (; Itr!=ItrEnd; ++Itr) {
    const xAOD::TrackParticle* track = (*Itr);
    double dphi = TMath::Abs(signedDeltaPhi(cluster->phi(),track->phi()));
    if (dphi < min_dPhi && dphi < dPhi) {
      min_dPhi = dphi;
      matched_track = track;
    }
  }

  double dEta = 1.0e+20;
  if (matched_track != 0) dEta = cluster->etaBE(2)-matched_track->eta();

  return dEta;

}

double IDPerfMonZee::InvMass(const xAOD::CaloCluster* EM1, const xAOD::CaloCluster* EM2) const {

  if (EM1 == 0 || EM2 == 0) return -99.;

  double invmass = 0.;
  if (EM1->pt() != 0 && EM2->pt() != 0.) {
    TLorentzVector particle1;
    TLorentzVector particle2;
    particle1.SetPtEtaPhiE(EM1->pt()/Gaudi::Units::GeV,EM1->eta(),EM1->phi(),EM1->e()/Gaudi::Units::GeV);
    particle2.SetPtEtaPhiE(EM2->pt()/Gaudi::Units::GeV,EM2->eta(),EM2->phi(),EM2->e()/Gaudi::Units::GeV);
    invmass = (particle1+particle2).Mag();
  }

   return invmass;

}

double IDPerfMonZee::InvMass(const xAOD::TrackParticle* trk1, const xAOD::TrackParticle* trk2) const {

  if (trk1 == 0 || trk2 == 0) return -99.;


  double invmass = 0.;
  if (trk1->pt() != 0 && trk2->pt() != 0.) {
    TLorentzVector particle1;
    TLorentzVector particle2;
    particle1.SetPtEtaPhiE(trk1->pt()/Gaudi::Units::GeV,trk1->eta(),trk1->phi(),trk1->e()/Gaudi::Units::GeV);
    particle2.SetPtEtaPhiE(trk2->pt()/Gaudi::Units::GeV,trk2->eta(),trk2->phi(),trk2->e()/Gaudi::Units::GeV);
    invmass = (particle1+particle2).Mag();
  }

  return invmass;

}

double IDPerfMonZee::TransMass(const xAOD::CaloCluster* EM, const xAOD::MissingET* met) const {

  if (EM == 0 || met == 0) return -99.;

  double transmass = 0.;
  float dphi = signedDeltaPhi(EM->phi(),met->phi());
  transmass = TMath::Sqrt(2.*EM->et()*met->met()*(1.-TMath::Cos(dphi)));

  return transmass;

}

double IDPerfMonZee::deltaR(const xAOD::CaloCluster* cluster, const xAOD::TrackParticle* track) const {
  double dr =-999.;
  if (cluster == 0 || track == 0) return dr;

  double deta = cluster->etaBE(2)-track->eta();
  double dphi = cluster->phi()-track->phi();
  dr = TMath::Sqrt(deta*deta + dphi*dphi);

  return dr;

}

int IDPerfMonZee::isZee(const xAOD::CaloCluster* em1, const xAOD::CaloCluster* em2, const xAOD::TrackParticleContainer* tracks) const {

  int selected = 3;
  
  //are the two electrons oppositely charged?
  const xAOD::TrackParticle* track_leading_emcluster = electronTrackMatch(tracks,em1);
  const xAOD::TrackParticle* track_second_leading_emcluster = electronTrackMatch(tracks,em2);
  if(!track_leading_emcluster || !track_second_leading_emcluster){
    ATH_MSG_DEBUG("Don't have 2 matched tracks!  Skipping charge check...");
    --selected;
  }
  else if(track_leading_emcluster->charge() != track_second_leading_emcluster->charge())
    --selected;

  double invmass = InvMass(em1,em2); // given in GeV
  if (em1->pt()/Gaudi::Units::GeV > 20. &&
      em2->pt()/Gaudi::Units::GeV > 20.) --selected;

  if (invmass > 70. &&
      invmass < 110.) --selected;

  return selected;

}

double IDPerfMonZee::signedDeltaPhi(double phi1, double phi2) const {
  double phia=phi1;
  if(phi1>Gaudi::Units::pi) phia=phi1-2.*Gaudi::Units::pi;
  double phib=phi2;
  if(phi2>Gaudi::Units::pi) phib=phi2-2.*Gaudi::Units::pi;
  double dphi=phia-phib;
  if(dphi>Gaudi::Units::pi) dphi-=2.*Gaudi::Units::pi;
  if(dphi<-Gaudi::Units::pi) dphi+=2.*Gaudi::Units::pi;
  return dphi;
}

int IDPerfMonZee::etaRegion(double eta) {

  int region = -99;

  if (TMath::Abs(eta) <= 1.) region = barrel;
  else if (eta > 1.) region = eca; // eca
  else if (eta < -1.) region = ecc; // ecc

  return region;

}

void IDPerfMonZee::FillHistosPerCluster(const xAOD::CaloCluster* cluster, const xAOD::TrackParticle* track, int region, float dEta, float dPhi) {

  if (cluster == 0) return;
  // THERE IS A CLUSTER
  if (region == incl) { // inclusive only
    m_Zee_eta[region]->Fill(cluster->etaBE(2));
    m_Zee_phi[region]->Fill(cluster->phi());
    // match in eta and phi separately and make dEta and dPhi plots
    if (dEta < 1.0e+20) {
      m_Zee_deta[region]->Fill(dEta);
      if (TMath::Abs(dEta) < 0.05) { // calculate mean only for those in matching window
	m_Zee_deta_vs_eta[region]->Fill(cluster->etaBE(2),dEta);
	m_Zee_deta_vs_phi[region]->Fill(cluster->phi(),dEta);
	m_Zee_deta_vs_eta_2d[region]->Fill(cluster->etaBE(2),dEta);
	m_Zee_deta_vs_phi_2d[region]->Fill(cluster->phi(),dEta);
	m_Zee_absdeta_vs_eta[region]->Fill(cluster->etaBE(2),TMath::Abs(dEta));
	m_Zee_absdeta_vs_phi[region]->Fill(cluster->phi(),TMath::Abs(dEta));
      }
    }
    if (dPhi < 1.0e+20) {
      m_Zee_dphi[region]->Fill(dPhi);
      if (TMath::Abs(dPhi) < 0.1) { // calculate mean only for those in matching window
	m_Zee_dphi_vs_eta[region]->Fill(cluster->etaBE(2),dPhi);
	m_Zee_dphi_vs_phi[region]->Fill(cluster->phi(),dPhi);
	m_Zee_dphi_vs_eta_2d[region]->Fill(cluster->etaBE(2),dPhi);
	m_Zee_dphi_vs_phi_2d[region]->Fill(cluster->phi(),dPhi);
	m_Zee_absdphi_vs_eta[region]->Fill(cluster->etaBE(2),TMath::Abs(dPhi));
	m_Zee_absdphi_vs_phi[region]->Fill(cluster->phi(),TMath::Abs(dPhi));
      }
    }

  } // end inclusive only

  if (track == 0) return;
  // TRACK-MATCHED

  float eoverp = 0.;
  float track_p = track->pt()*cosh(track->eta());
  if (track_p != 0.) eoverp = cluster->e()/track_p;

  m_Zee_Eop[region]->Fill(eoverp);

  if (track->charge() == 1.) {
    m_Zee_Eop_plus[region]->Fill(eoverp);
  }
  else if (track->charge() == -1.) {
    m_Zee_Eop_minus[region]->Fill(eoverp);
  }

  if (region == incl) { // inclusive only

    m_Zee_trackmatched_eta[region]->Fill(cluster->etaBE(2));
    m_Zee_trackmatched_phi[region]->Fill(cluster->phi());
    if (eoverp > m_eoverp_tight_min && eoverp < m_eoverp_tight_max) {
      m_Zee_meanEop_vs_p[region]->Fill(track_p/Gaudi::Units::GeV,eoverp);
      m_Zee_meanEop_vs_invp[region]->Fill(1./(track_p/Gaudi::Units::GeV),eoverp);
      m_Zee_meanEop_vs_E[region]->Fill(cluster->e()/Gaudi::Units::GeV,eoverp);
      m_Zee_meanEop_vs_chargedp[region]->Fill(double(track->charge())*track_p/Gaudi::Units::GeV,eoverp);
      m_Zee_meanEop_vs_chargedE[region]->Fill(double(track->charge())*cluster->e()/Gaudi::Units::GeV,eoverp);
      m_Zee_meanEop_vs_phi[region]->Fill(track->phi(),eoverp);
      m_Zee_meanEop_vs_eta[region]->Fill(track->eta(),eoverp);
    }
    if (track->charge() == 1.) {
      if (eoverp > m_eoverp_tight_min && eoverp < m_eoverp_tight_max) {
	m_Zee_meanEop_vs_p_plus[region]->Fill(track_p/Gaudi::Units::GeV,eoverp);
	m_Zee_meanEop_vs_invp_plus[region]->Fill(1./(track_p/Gaudi::Units::GeV),eoverp);
	m_Zee_meanEop_vs_E_plus[region]->Fill(cluster->e()/Gaudi::Units::GeV,eoverp);
	m_Zee_meanEop_vs_phi_plus[region]->Fill(track->phi(),eoverp);
	m_Zee_meanEop_vs_eta_plus[region]->Fill(track->eta(),eoverp);
      }
    }
    else if (track->charge() == -1.) {
      if (eoverp > m_eoverp_tight_min && eoverp < m_eoverp_tight_max) {
	m_Zee_meanEop_vs_p_minus[region]->Fill(track_p/Gaudi::Units::GeV,eoverp);
	m_Zee_meanEop_vs_invp_minus[region]->Fill(1./(track_p/Gaudi::Units::GeV),eoverp);
	m_Zee_meanEop_vs_E_minus[region]->Fill(cluster->e()/Gaudi::Units::GeV,eoverp);
	m_Zee_meanEop_vs_phi_minus[region]->Fill(track->phi(),eoverp);
	m_Zee_meanEop_vs_eta_minus[region]->Fill(track->eta(),eoverp);
      }
    }
    if (eoverp < m_eoverp_standard_max && eoverp > m_eoverp_standard_min) {
      m_Zee_trackmatched_Eopmatched_eta[region]->Fill(cluster->etaBE(2));
      m_Zee_trackmatched_Eopmatched_phi[region]->Fill(cluster->phi());
    }
    if (eoverp < m_eoverp_tight_max && eoverp > m_eoverp_tight_min) {
      m_Zee_trackmatched_tightEopmatched_eta[region]->Fill(cluster->etaBE(2));
      m_Zee_trackmatched_tightEopmatched_phi[region]->Fill(cluster->phi());
    }
    if (eoverp > m_eoverp_tight_min && eoverp < 1.) {
      m_Zee_Eop_lt1_vs_eta[region]->Fill(cluster->etaBE(2));
      m_Zee_Eop_lt1_vs_phi[region]->Fill(cluster->phi());
      m_Zee_Eop_lt1_gt1[region]->Fill(0.75);
    }
    else if (eoverp > 1. && eoverp < m_eoverp_tight_max) {
      m_Zee_Eop_gt1_vs_eta[region]->Fill(cluster->etaBE(2));
      m_Zee_Eop_gt1_vs_phi[region]->Fill(cluster->phi());
      m_Zee_Eop_lt1_gt1[region]->Fill(1.25);
    }
    if (eoverp > 0.5 && eoverp < 2.5) m_Zee_Eop_05_25[region]->Fill(0.5);
    if (eoverp > 1.5 && eoverp < 2.5) m_Zee_Eop_15_25[region]->Fill(0.5);

  } // end inclusive only

}
