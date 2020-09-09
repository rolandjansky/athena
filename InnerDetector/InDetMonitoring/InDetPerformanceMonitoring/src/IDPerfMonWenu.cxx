/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDPerfMonWenu.cxx
// AUTHORS: Beate Heinemann, Tobias Golling, Sara Strandberg
// **********************************************************************

#include <cmath>
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
//#include "TMath.h"
#include "TLorentzVector.h"

#include "StoreGate/StoreGateSvc.h"

#include "IdDictDetDescr/IdDictManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"

#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "egammaEvent/egammaParamDefs.h"
#include "egammaEvent/egammaPIDdefsObs.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "InDetPerformanceMonitoring/IDPerfMonWenu.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <stdexcept>

// *********************************************************************
// Public Methods
// *********************************************************************

namespace{
template <class T>
  const T *
  getCollectionWithCheck(const ServiceHandle<StoreGateSvc> & evtStore,const std::string &contName){
    const T * container{};
    if (evtStore->contains<T>(contName)){
      //retrieve
      if(evtStore->retrieve(container,contName).isFailure()) return nullptr;
    }
    return container;
  }
}

IDPerfMonWenu::IDPerfMonWenu( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_region_strings{"incl", "barrel", "eca", "ecc"},
   m_triggerChainName("NoTriggerSelection")
{
  declareProperty("tracksName",m_tracksName);
  declareProperty("electronsName",m_electronsName="Electrons");
  declareProperty("photonsName",m_photonsName="Photons");
  declareProperty("VxPrimContainerName",m_VxPrimContainerName="PrimaryVertices");
  declareProperty("emclustersName",m_emclustersName="LArClusterEM");
  declareProperty("metName",m_metName="MET_Reference_AntiKt4LCTopo");
  declareProperty("METFinalName",m_metRefFinalName="FinalClus");
  declareProperty("eoverp_standard_min",m_eoverp_standard_min=0.5);
  declareProperty("eoverp_standard_max",m_eoverp_standard_max=4.0);
  declareProperty("eoverp_tight_min",m_eoverp_tight_min=0.7);
  declareProperty("eoverp_tight_max",m_eoverp_tight_max=1.3);
  declareProperty("CheckRate",m_checkrate=1000);
  declareProperty("triggerChainName",m_triggerChainName);
  declareProperty("rejectSecondCluster",m_rejectSecondCluster = true);
  declareProperty("electronIDLevel",m_electronIDLevel = "Tight");
}

IDPerfMonWenu::~IDPerfMonWenu() { }

StatusCode IDPerfMonWenu::initialize(){
  m_histosBooked = 0;
  if (m_tracksName.empty() ){
    ATH_MSG_WARNING( " no track collection given" );
  }
  StatusCode sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()){
    ATH_MSG_WARNING( "Could not initialize ManagedMonitorToolBase" );
  }
  //---Electron Likelihood tool---
  ATH_MSG_DEBUG("IDPerfMonWenu::Initialize() -- Setting up electron LH tool.");
  m_LHTool2015=std::make_unique<AsgElectronLikelihoodTool> ("m_LHTool2015");
  if((m_LHTool2015->setProperty("primaryVertexContainer",m_VxPrimContainerName)).isFailure()){
    ATH_MSG_WARNING("Failure setting primary vertex container " << m_VxPrimContainerName << "in electron likelihood tool");
  }
  //Set up electron LH level
  m_doIDCuts = true;
  std::string confDir = "ElectronPhotonSelectorTools/offline/mc15_20150712/";
  if(m_electronIDLevel .empty()){
    ATH_MSG_WARNING("electronIDLevel is set to empty!  No electron ID cuts will be applied.");
    m_doIDCuts = false;
  } else {
    if((m_electronIDLevel != "Loose") && (m_electronIDLevel != "Medium") && (m_electronIDLevel != "Tight")){
      ATH_MSG_WARNING("Unknown electronIDLevel!! (Accepted values: Loose, Medium, Tight)");
      m_doIDCuts = false;
    } else {
      std::string configFile = confDir+"ElectronLikelihood"+m_electronIDLevel+"OfflineConfig2015.conf";
      ATH_MSG_INFO("Likelihood configuration file: " << configFile);
      if((m_LHTool2015->setProperty("ConfigFile",configFile)).isFailure()){
	      ATH_MSG_WARNING("Failure loading ConfigFile in electron likelihood tool.");
	    }
    }
  }
  StatusCode lh = m_LHTool2015->initialize();
  if(lh.isFailure()){
    ATH_MSG_WARNING("Electron likelihood tool initialize() failed!  Turning off electron LH cuts!");
    m_doIDCuts = false;
  }
  return StatusCode::SUCCESS;
}

StatusCode IDPerfMonWenu::bookHistograms()
{
  MonGroup al_Wenu_mon ( this, "IDPerfMon/Wenu/" + m_triggerChainName, run);
  if( newRunFlag() ) {
    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user)      return StatusCode::SUCCESS;

    // ***********************
    // Book event histograms
    // ***********************

    m_Nevents = new TH1F("Nevents", "Number of events processed", 1, -.5, 0.5);
    RegisterHisto(al_Wenu_mon,m_Nevents);
    m_Wenu_met_sel = new TH1F("Wenu_met_sel","met", 50, 0., 100.);
    RegisterHisto(al_Wenu_mon,m_Wenu_met_sel);
    m_Wenu_transmass_sel = new TH1F("Wenu_transmass_sel","Transverse mass of the leading em cluster and the met", 90, 0., 180.);
    RegisterHisto(al_Wenu_mon,m_Wenu_transmass_sel);
    m_Wenu_trk_transmass_sel = new TH1F("Wenu_trk_transmass_sel","Transverse mass of the track and the met", 90, 0., 180.);
    RegisterHisto(al_Wenu_mon,m_Wenu_trk_transmass_sel);
    m_Wenu_trk_transmass_sel_scaled = new TH1F("Wenu_trk_transmass_sel_scaled","Transverse mass of the track and the met scaled to per event", 90, 0., 180.);
    RegisterHisto(al_Wenu_mon,m_Wenu_trk_transmass_sel_scaled);
    m_Wenu_clus_pt = new TH1F("Wenu_cluster_pt","Transverse momentum of the leading em cluster",50,0.,100.);
    RegisterHisto(al_Wenu_mon,m_Wenu_clus_pt);

    // ***********************
    // Book cluster histograms
    // ***********************

    // first take care to clear all histogram vectors
    m_Wenu_Eop.clear();
    m_Wenu_Eopdiff.clear();
    m_Wenu_Eop_plus.clear();
    m_Wenu_Eop_minus.clear();
    m_Wenu_eta.clear();
    m_Wenu_phi.clear();
    m_Wenu_deta.clear();
    m_Wenu_deta_vs_eta.clear();
    m_Wenu_deta_vs_phi.clear();
    m_Wenu_absdeta_vs_eta.clear();
    m_Wenu_absdeta_vs_phi.clear();
    m_Wenu_dphi.clear();
    m_Wenu_dphi_vs_eta.clear();
    m_Wenu_dphi_vs_phi.clear();
    m_Wenu_absdphi_vs_eta.clear();
    m_Wenu_absdphi_vs_phi.clear();
    m_Wenu_trackmatched_eta.clear();
    m_Wenu_trackmatched_phi.clear();
    m_Wenu_trackmatched_Eopmatched_eta.clear();
    m_Wenu_trackmatched_Eopmatched_phi.clear();
    m_Wenu_trackmatched_tightEopmatched_eta.clear();
    m_Wenu_trackmatched_tightEopmatched_phi.clear();
    m_Wenu_trackmatch_eff_vs_eta.clear();
    m_Wenu_trackmatch_eff_vs_phi.clear();
    m_Wenu_Eopmatch_eff_vs_eta.clear();
    m_Wenu_Eopmatch_eff_vs_phi.clear();
    m_Wenu_tightEopmatch_eff_vs_eta.clear();
    m_Wenu_tightEopmatch_eff_vs_phi.clear();
    m_Wenu_meanEop_vs_p.clear();
    m_Wenu_meanEop_vs_invp.clear();
    m_Wenu_meanEop_vs_E.clear();
    m_Wenu_meanEop_vs_chargedp.clear();
    m_Wenu_meanEop_vs_chargedE.clear();
    m_Wenu_meanEop_vs_phi.clear();
    m_Wenu_meanEop_vs_eta.clear();
    m_Wenu_meanEop_vs_p_plus.clear();
    m_Wenu_meanEop_vs_invp_plus.clear();
    m_Wenu_meanEop_vs_E_plus.clear();
    m_Wenu_meanEop_vs_phi_plus.clear();
    m_Wenu_meanEop_vs_eta_plus.clear();
    m_Wenu_meanEop_vs_p_minus.clear();
    m_Wenu_meanEop_vs_invp_minus.clear();
    m_Wenu_meanEop_vs_E_minus.clear();
    m_Wenu_meanEop_vs_phi_minus.clear();
    m_Wenu_meanEop_vs_eta_minus.clear();
    m_Wenu_Eop_lt1_gt1.clear();
    m_Wenu_Eopdiff_vs_p.clear();
    m_Wenu_Eopdiff_vs_invp.clear();
    m_Wenu_Eopdiff_vs_E.clear();
    m_Wenu_Eopdiff_vs_phi.clear();
    m_Wenu_Eopdiff_vs_eta.clear();
    m_Wenu_Eop_lt1_vs_eta.clear();
    m_Wenu_Eop_lt1_vs_phi.clear();
    m_Wenu_Eop_gt1_vs_eta.clear();
    m_Wenu_Eop_gt1_vs_phi.clear();
    m_Wenu_frac_Eop_lt1_vs_eta.clear();
    m_Wenu_frac_Eop_lt1_vs_phi.clear();
    m_Wenu_Eop_05_25.clear();
    m_Wenu_Eop_15_25.clear();
    m_Wenu_frac_Eop_05_25_15_25.clear();

    for (int region=0; region!=Nregions; ++region) {
      std::string title;
      std::string name;

      name = "Wenu_Eop_" + m_region_strings[region];
      title = "E/p for Wenu EM-clusters in " + m_region_strings[region];
      m_Wenu_Eop.push_back(new TH1F(name.c_str(),title.c_str(), 60, 0., 10.));
     
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop[region]);
      name = "Wenu_Eopdiff_" + m_region_strings[region];
      title = "E/p difference (pos-neg) for Wenu EM-clusters in " + m_region_strings[region];
      m_Wenu_Eopdiff.push_back(new TH1F(name.c_str(),title.c_str(), 10, 0., 2.));
      
      RegisterHisto(al_Wenu_mon,m_Wenu_Eopdiff[region],true);
      name = "Wenu_Eop_plus_" + m_region_strings[region];
      title = "E/p for pos. charged Wenu EM-clusters in " + m_region_strings[region];
      m_Wenu_Eop_plus.push_back(new TH1F(name.c_str(),title.c_str(), 10, 0., 2.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop_plus[region],true);

      name = "Wenu_Eop_minus_" + m_region_strings[region];
      title = "E/p for neg. charged Wenu EM-clusters in " + m_region_strings[region];
      m_Wenu_Eop_minus.push_back(new TH1F(name.c_str(),title.c_str(), 10, 0., 2.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop_minus[region],true);

      if (region != incl) continue;

      m_Wenu_eta.push_back(new TH1F("Wenu_eta","Eta of Wenu EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_eta[region]);
      m_Wenu_phi.push_back(new TH1F("Wenu_phi","Phi of Wenu EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_phi[region]);

      m_Wenu_deta.push_back(new TH1F("Wenu_deta","deltaEta(EM-cluster,track) in Wenu events", 50, -0.5, 0.5));
      RegisterHisto(al_Wenu_mon,m_Wenu_deta[region]);
      m_Wenu_deta_vs_eta.push_back(new TProfile("Wenu_deta_vs_eta","deltaEta(EM-cluster,track) vs. eta in Wenu events", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_deta_vs_eta[region]);
      m_Wenu_deta_vs_phi.push_back(new TProfile("Wenu_deta_vs_phi","deltaEta(EM-cluster,track) vs. phi in Wenu events", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_deta_vs_phi[region]);
      m_Wenu_absdeta_vs_eta.push_back(new TProfile("Wenu_absdeta_vs_eta","Abs(deltaEta(EM-cluster,track)) vs. eta in Wenu events", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_absdeta_vs_eta[region]);
      m_Wenu_absdeta_vs_phi.push_back(new TProfile("Wenu_absdeta_vs_phi","Abs(deltaEta(EM-cluster,track)) vs. phi in Wenu events", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_absdeta_vs_phi[region]);
      m_Wenu_dphi.push_back(new TH1F("Wenu_dphi","deltaPhi(EM-cluster,track) in Wenu events", 50, -0.5, 0.5));
      RegisterHisto(al_Wenu_mon,m_Wenu_dphi[region]);
      m_Wenu_dphi_vs_eta.push_back(new TProfile("Wenu_dphi_vs_eta","deltaPhi(EM-cluster,track) vs. eta in Wenu events", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_dphi_vs_eta[region]);
      m_Wenu_dphi_vs_phi.push_back(new TProfile("Wenu_dphi_vs_phi","deltaPhi(EM-cluster,track) vs. phi in Wenu events", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_dphi_vs_phi[region]);
      m_Wenu_absdphi_vs_eta.push_back(new TProfile("Wenu_absdphi_vs_eta","Abs(deltaPhi(EM-cluster,track)) vs. eta in Wenu events", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_absdphi_vs_eta[region]);
      m_Wenu_absdphi_vs_phi.push_back(new TProfile("Wenu_absdphi_vs_phi","Abs(deltaPhi(EM-cluster,track)) vs. phi in Wenu events", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_absdphi_vs_phi[region]);

      m_Wenu_trackmatched_eta.push_back(new TH1F("Wenu_trackmatched_eta","Eta of track-matched Wenu EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_trackmatched_eta[region]);
      m_Wenu_trackmatched_phi.push_back(new TH1F("Wenu_trackmatched_phi","Phi of track-matched Wenu EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_trackmatched_phi[region]);
      m_Wenu_trackmatched_Eopmatched_eta.push_back(new TH1F("Wenu_trackmatched_Eopmatched_eta","Eta of track-matched and E/p-matched Wenu EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_trackmatched_Eopmatched_eta[region]);
      m_Wenu_trackmatched_Eopmatched_phi.push_back(new TH1F("Wenu_trackmatched_Eopmatched_phi","Phi of track-matched and E/p-matched Wenu EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_trackmatched_Eopmatched_phi[region]);
      m_Wenu_trackmatched_tightEopmatched_eta.push_back(new TH1F("Wenu_trackmatched_tightEopmatched_eta","Eta of track-matched and tight E/p-matched Wenu EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_trackmatched_tightEopmatched_eta[region]);
      m_Wenu_trackmatched_tightEopmatched_phi.push_back(new TH1F("Wenu_trackmatched_tightEopmatched_phi","Phi of track-matched and tight E/p-matched Wenu EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_trackmatched_tightEopmatched_phi[region]);

      m_Wenu_trackmatch_eff_vs_eta.push_back(new TH1F("Wenu_trackmatch_eff_vs_eta","Track-match efficiency for Wenu EM-clusters vs. eta", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_trackmatch_eff_vs_eta[region],1);
      m_Wenu_trackmatch_eff_vs_phi.push_back(new TH1F("Wenu_trackmatch_eff_vs_phi","Track-match efficiency for Wenu EM-clusters vs. phi", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_trackmatch_eff_vs_phi[region],1);
      m_Wenu_Eopmatch_eff_vs_eta.push_back(new TH1F("Wenu_Eopmatch_eff_vs_eta","E/p-match efficiency for Wenu EM-clusters vs. eta", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eopmatch_eff_vs_eta[region],1);
      m_Wenu_Eopmatch_eff_vs_phi.push_back(new TH1F("Wenu_Eopmatch_eff_vs_phi","E/p-match efficiency for Wenu EM-clusters vs. phi", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eopmatch_eff_vs_phi[region],1);
      m_Wenu_tightEopmatch_eff_vs_eta.push_back(new TH1F("Wenu_tightEopmatch_eff_vs_eta","Tight E/p-match efficiency for Wenu EM-clusters vs. eta", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_tightEopmatch_eff_vs_eta[region],1);
      m_Wenu_tightEopmatch_eff_vs_phi.push_back(new TH1F("Wenu_tightEopmatch_eff_vs_phi","Tight E/p-match efficiency for Wenu EM-clusters vs. phi", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_tightEopmatch_eff_vs_phi[region],1);

      m_Wenu_meanEop_vs_p.push_back(new TProfile("Wenu_meanEop_vs_p", "Mean E/p vs p for Wenu EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_p[region]);
      m_Wenu_meanEop_vs_invp.push_back(new TProfile("Wenu_meanEop_vs_invp", "Mean E/p vs 1/p for Wenu EM-clusters", 15, 0., 0.05));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_invp[region]);
      m_Wenu_meanEop_vs_E.push_back(new TProfile("Wenu_meanEop_vs_E", "Mean E/p vs E for Wenu EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_E[region]);
      m_Wenu_meanEop_vs_chargedp.push_back(new TProfile("Wenu_meanEop_vs_chargedp", "Mean E/p vs charge*p for Wenu EM-clusters", 30, -300., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_chargedp[region]);
      m_Wenu_meanEop_vs_chargedE.push_back(new TProfile("Wenu_meanEop_vs_chargedE", "Mean E/p vs charge*E for Wenu EM-clusters", 30, -300., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_chargedE[region]);
      m_Wenu_meanEop_vs_phi.push_back(new TProfile("Wenu_meanEop_vs_phi", "Mean E/p vs phi for Wenu EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_phi[region]);
      m_Wenu_meanEop_vs_eta.push_back(new TProfile("Wenu_meanEop_vs_eta", "Mean E/p vs eta for Wenu EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_eta[region]);

      m_Wenu_meanEop_vs_p_plus.push_back(new TProfile("Wenu_meanEop_vs_p_plus", "Mean E/p vs p for pos. charged Wenu EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_p_plus[region]);
      m_Wenu_meanEop_vs_invp_plus.push_back(new TProfile("Wenu_meanEop_vs_invp_plus", "Mean E/p vs 1/p for pos. charged Wenu EM-clusters", 15, 0., 0.05));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_invp_plus[region]);
      m_Wenu_meanEop_vs_E_plus.push_back(new TProfile("Wenu_meanEop_vs_E_plus", "Mean E/p vs E for pos. charged Wenu EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_E_plus[region]);
      m_Wenu_meanEop_vs_phi_plus.push_back(new TProfile("Wenu_meanEop_vs_phi_plus", "Mean E/p vs phi for pos. charged Wenu EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_phi_plus[region]);
      m_Wenu_meanEop_vs_eta_plus.push_back(new TProfile("Wenu_meanEop_vs_eta_plus", "Mean E/p vs eta for pos. charged Wenu EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_eta_plus[region]);

      m_Wenu_meanEop_vs_p_minus.push_back(new TProfile("Wenu_meanEop_vs_p_minus", "Mean E/p vs p for neg. charged Wenu EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_p_minus[region]);
      m_Wenu_meanEop_vs_invp_minus.push_back(new TProfile("Wenu_meanEop_vs_invp_minus", "Mean E/p vs 1/p for neg. charged Wenu EM-clusters", 15, 0., 0.05));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_invp_minus[region]);
      m_Wenu_meanEop_vs_E_minus.push_back(new TProfile("Wenu_meanEop_vs_E_minus", "Mean E/p vs E for neg. charged Wenu EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_E_minus[region]);
      m_Wenu_meanEop_vs_phi_minus.push_back(new TProfile("Wenu_meanEop_vs_phi_minus", "Mean E/p vs phi for neg. charged Wenu EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_phi_minus[region]);
      m_Wenu_meanEop_vs_eta_minus.push_back(new TProfile("Wenu_meanEop_vs_eta_minus", "Mean E/p vs eta for neg. charged Wenu EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_meanEop_vs_eta_minus[region]);

      m_Wenu_Eop_lt1_gt1.push_back(new TH1F("Wenu_Eop_lt1_gt1", "E/p below/above 1 for Wenu EM-clusters", 2, 0.5, 1.5));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop_lt1_gt1[region]);

      m_Wenu_Eopdiff_vs_p.push_back(new TProfile("Wenu_Eopdiff_vs_p", "E/p difference (pos-neg) vs p for Wenu EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eopdiff_vs_p[region]);
      m_Wenu_Eopdiff_vs_invp.push_back(new TProfile("Wenu_Eopdiff_vs_invp", "E/p difference (pos-neg) vs 1/p for Wenu EM-clusters", 15, 0., 0.05));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eopdiff_vs_invp[region]);
      m_Wenu_Eopdiff_vs_E.push_back(new TProfile("Wenu_Eopdiff_vs_E", "E/p difference (pos-neg) vs E for Wenu EM-clusters", 15, 0., 300.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eopdiff_vs_E[region]);
      m_Wenu_Eopdiff_vs_phi.push_back(new TProfile("Wenu_Eopdiff_vs_phi", "E/p difference (pos-neg) vs phi for Wenu EM-clusters", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eopdiff_vs_phi[region]);
      m_Wenu_Eopdiff_vs_eta.push_back(new TProfile("Wenu_Eopdiff_vs_eta", "E/p difference (pos-neg) vs eta for Wenu EM-clusters", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eopdiff_vs_eta[region]);

      m_Wenu_Eop_lt1_vs_eta.push_back(new TH1F("Wenu_Eop_lt1_vs_eta","Eta of Wenu EM-clusters with E/p < 1 (not incl. small E/p)", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop_lt1_vs_eta[region]);
      m_Wenu_Eop_lt1_vs_phi.push_back(new TH1F("Wenu_Eop_lt1_vs_phi","Phi of Wenu EM-clusters with E/p < 1 (not incl. small E/p)", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop_lt1_vs_phi[region]);
      m_Wenu_Eop_gt1_vs_eta.push_back(new TH1F("Wenu_Eop_gt1_vs_eta","Eta of Wenu EM-clusters with E/p < 1 (not incl. small E/p)", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop_gt1_vs_eta[region]);
      m_Wenu_Eop_gt1_vs_phi.push_back(new TH1F("Wenu_Eop_gt1_vs_phi","Phi of Wenu EM-clusters with E/p < 1 (not incl. small E/p)", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop_gt1_vs_phi[region]);
      m_Wenu_frac_Eop_lt1_vs_eta.push_back(new TH1F("Wenu_frac_Eop_lt1_vs_eta","Fraction of Wenu EM-clusters with E/p < 1 (not incl. small E/p) vs Eta", 15, -3., 3.));
      RegisterHisto(al_Wenu_mon,m_Wenu_frac_Eop_lt1_vs_eta[region],1);
      m_Wenu_frac_Eop_lt1_vs_phi.push_back(new TH1F("Wenu_frac_Eop_lt1_vs_phi","Fraction of Wenu EM-clusters with E/p < 1 (not incl. small E/p) vs Phi", 15, -3.15, 3.15));
      RegisterHisto(al_Wenu_mon,m_Wenu_frac_Eop_lt1_vs_phi[region],1);

      m_Wenu_Eop_05_25.push_back(new TH1F("Wenu_Eop_05_25","Number of Wenu events with 0.5 < E/p 2.5", 1, 0., 1.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop_05_25[region]);
      m_Wenu_Eop_15_25.push_back(new TH1F("Wenu_Eop_15_25","Number of Wenu events with 1.5 < E/p 2.5", 1, 0., 1.));
      RegisterHisto(al_Wenu_mon,m_Wenu_Eop_15_25[region]);
      m_Wenu_frac_Eop_05_25_15_25.push_back(new TH1F("Wenu_frac_Eop_05_25_15_25","Fraction of Wenu EM-clusters with 1.5 < E/p < 2.5 and 0.5 < E/p < 2.5", 1, 0., 1.));
      RegisterHisto(al_Wenu_mon,m_Wenu_frac_Eop_05_25_15_25[region],1);
    }

     m_histosBooked++;
  }
  return StatusCode::SUCCESS;
}

void IDPerfMonWenu::RegisterHisto(MonGroup& mon, TH1* histo, bool doSumw2) {
  if (doSumw2) histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    ATH_MSG_DEBUG( "Cannot book TH1 Histogram:" );
  }
}

void IDPerfMonWenu::RegisterHisto(MonGroup& mon, TProfile* histo) {
  if (mon.regHist(histo).isFailure() ) {
    ATH_MSG_DEBUG( "Cannot book TProfile Histogram:" );
  }
}

void IDPerfMonWenu::RegisterHisto(MonGroup& mon, TH2* histo, bool doSumw2) {
  if (doSumw2) histo->Sumw2();
  if (mon.regHist(histo).isFailure() ) {
    ATH_MSG_WARNING( "Cannot book TH2 Histogram:" );
  }
}

StatusCode IDPerfMonWenu::fillHistograms()
{
  int nevents = (int) m_Nevents->GetEntries();
  const bool firstEvent{nevents == 1};
  // get electron container from storegate
  auto formErrorMessage = [] (const std::string & contName)->std::string {
    return std::string(std::string("No Collection with name ") + contName + std::string(" found in StoreGate"));
  };
  const xAOD::ElectronContainer* electrons=getCollectionWithCheck<xAOD::ElectronContainer>(evtStore(),m_electronsName);
  if (not electrons){
    const std::string & errMsg=formErrorMessage(m_electronsName);
    if (firstEvent) ATH_MSG_WARNING( errMsg );
    else ATH_MSG_DEBUG(errMsg);
    return StatusCode::RECOVERABLE;
  }
  // get photon container from storegate
  const xAOD::PhotonContainer* photons = getCollectionWithCheck<xAOD::PhotonContainer>(evtStore(),m_photonsName);
  if (not photons){
    const std::string & errMsg =  formErrorMessage(m_photonsName);
    if (firstEvent) ATH_MSG_WARNING( errMsg );
    else ATH_MSG_DEBUG(errMsg);
    return StatusCode::RECOVERABLE;
  }
  // get emcluster container from storegate
  const xAOD::CaloClusterContainer* emclusters = getCollectionWithCheck<xAOD::CaloClusterContainer>(evtStore(),m_emclustersName);
  if (not emclusters){
    const std::string & errMsg =  formErrorMessage(m_emclustersName);
    if (firstEvent) ATH_MSG_WARNING( errMsg );
    else ATH_MSG_DEBUG(errMsg);
    return StatusCode::RECOVERABLE;
  }
  // get track container from storegate
  const xAOD::TrackParticleContainer* tracks = getCollectionWithCheck<xAOD::TrackParticleContainer>(evtStore(),m_tracksName);
  if (not tracks){
    const std::string & errMsg =  formErrorMessage(m_tracksName);
    if (firstEvent) ATH_MSG_WARNING( errMsg );
    else ATH_MSG_DEBUG(errMsg);
    return StatusCode::RECOVERABLE;
  }
  // get met container from storegate
  const xAOD::MissingETContainer* final_met = getCollectionWithCheck<xAOD::MissingETContainer>(evtStore(),m_metName);
  if (not final_met){
    const std::string & errMsg =  formErrorMessage(m_metName);
    if (firstEvent) ATH_MSG_WARNING( errMsg );
    else ATH_MSG_DEBUG(errMsg);
    return StatusCode::RECOVERABLE;
  }
  m_Nevents->Fill(0.);
  // ******************
  // Get the missing ET
  // ******************
  const xAOD::MissingET *MET;
  MET = (*final_met)[m_metRefFinalName];
  double met = MET->met();
  msg(MSG::DEBUG) << "MET = " << met << endmsg;
  // *******************
  // Look at EM clusters
  // *******************
  const xAOD::CaloCluster* LeadingEMcluster = getLeadingEMcluster(photons, electrons);
  const xAOD::CaloCluster* SecondLeadingEMcluster = getLeadingEMcluster(photons, electrons, LeadingEMcluster);
  if (LeadingEMcluster) {
    int leading_eta_region = etaRegion(LeadingEMcluster->eta());
    double leading_dPhi = electronTrackMatchEta(tracks,LeadingEMcluster);
    double leading_dEta = electronTrackMatchPhi(tracks,LeadingEMcluster);
    const xAOD::TrackParticle* track_leading_emcluster = electronTrackMatch(tracks,LeadingEMcluster);
    int selected = isWenu(LeadingEMcluster, SecondLeadingEMcluster, met);
    if (selected == 0) {
      // *********************
      // Fill event histograms
      // *********************
      m_Wenu_met_sel->Fill(met/Gaudi::Units::GeV);
      m_Wenu_clus_pt->Fill(LeadingEMcluster->pt()/Gaudi::Units::GeV);
      double cluster_met_transmass = TransMass(LeadingEMcluster,MET);
      if (cluster_met_transmass > 0.) m_Wenu_transmass_sel->Fill(cluster_met_transmass);
      double track_met_transmass = 0.;
      if (track_leading_emcluster ) {
	      track_met_transmass = TransMass(track_leading_emcluster,MET);
	      if (track_met_transmass > 0.) m_Wenu_trk_transmass_sel->Fill(track_met_transmass);
      }
      // ***********************
      // Fill cluster histograms
      // ***********************
      FillHistosPerCluster(LeadingEMcluster, track_leading_emcluster, leading_eta_region, leading_dEta, leading_dPhi);
      FillHistosPerCluster(LeadingEMcluster, track_leading_emcluster, incl, leading_dEta, leading_dPhi);
    }
  }
  return StatusCode::SUCCESS;
}

void IDPerfMonWenu::makeEffHisto(TH1F* h_num, TH1F* h_denom, TH1F* h_eff) {
  h_eff->Divide(h_num,h_denom,1.,1.,"B");
}

StatusCode IDPerfMonWenu::procHistograms()
{
  if( endOfRunFlag() ) {
    // PostProcess Wenu histograms
    for (int region=0; region!=1; ++region) {
      makeEffHisto(m_Wenu_trackmatched_eta[region],m_Wenu_eta[region],m_Wenu_trackmatch_eff_vs_eta[region]);
      makeEffHisto(m_Wenu_trackmatched_phi[region],m_Wenu_phi[region],m_Wenu_trackmatch_eff_vs_phi[region]);
      makeEffHisto(m_Wenu_trackmatched_Eopmatched_eta[region],m_Wenu_trackmatched_eta[region],m_Wenu_Eopmatch_eff_vs_eta[region]);
      makeEffHisto(m_Wenu_trackmatched_Eopmatched_phi[region],m_Wenu_trackmatched_phi[region],m_Wenu_Eopmatch_eff_vs_phi[region]);
      makeEffHisto(m_Wenu_trackmatched_tightEopmatched_eta[region],m_Wenu_trackmatched_eta[region],m_Wenu_tightEopmatch_eff_vs_eta[region]);
      makeEffHisto(m_Wenu_trackmatched_tightEopmatched_phi[region],m_Wenu_trackmatched_phi[region],m_Wenu_tightEopmatch_eff_vs_phi[region]);
      // commented out for some reason?
      m_Wenu_Eopdiff[region]->Add(m_Wenu_Eop_plus[region],m_Wenu_Eop_minus[region],1.,-1);
      m_Wenu_Eopdiff_vs_p[region]->Add(m_Wenu_meanEop_vs_p_plus[region],m_Wenu_meanEop_vs_p_minus[region],1.,-1);
      m_Wenu_Eopdiff_vs_invp[region]->Add(m_Wenu_meanEop_vs_invp_plus[region],m_Wenu_meanEop_vs_invp_minus[region],1.,-1);
      m_Wenu_Eopdiff_vs_E[region]->Add(m_Wenu_meanEop_vs_E_plus[region],m_Wenu_meanEop_vs_E_minus[region],1.,-1);
      m_Wenu_Eopdiff_vs_phi[region]->Add(m_Wenu_meanEop_vs_phi_plus[region],m_Wenu_meanEop_vs_phi_minus[region],1.,-1);
      m_Wenu_Eopdiff_vs_eta[region]->Add(m_Wenu_meanEop_vs_eta_plus[region],m_Wenu_meanEop_vs_eta_minus[region],1.,-1);
      //
      makeEffHisto(m_Wenu_Eop_lt1_vs_eta[region],m_Wenu_eta[region],m_Wenu_frac_Eop_lt1_vs_eta[region]);
      makeEffHisto(m_Wenu_Eop_lt1_vs_phi[region],m_Wenu_phi[region],m_Wenu_frac_Eop_lt1_vs_phi[region]);
      makeEffHisto(m_Wenu_Eop_15_25[region],m_Wenu_Eop_05_25[region],m_Wenu_frac_Eop_05_25_15_25[region]);
    }
  }
  return StatusCode::SUCCESS;
}


const xAOD::CaloCluster* IDPerfMonWenu::getLeadingEMcluster(const xAOD::CaloClusterContainer* clusters, const xAOD::CaloCluster* omitCluster) const {
  const xAOD::CaloCluster* leading_emcluster{nullptr};
  float max_pt = 0.;
  for (const auto & cl: *clusters) {
    if (cl == omitCluster) continue;
    double deltaR = std::sqrt(std::pow(std::fabs(cl->phi() - omitCluster->phi()),2) + std::pow(std::fabs(cl->eta() - omitCluster->eta()),2));
    if(deltaR < 0.005) continue;
    if (cl->pt()/Gaudi::Units::GeV < 10.) continue;
    if (cl->pt() > max_pt) {
      leading_emcluster = cl;
      max_pt = cl->pt();
    }
  }
  return leading_emcluster;
}

const xAOD::CaloCluster* IDPerfMonWenu::getLeadingEMcluster(const xAOD::PhotonContainer* /*photons*/, const xAOD::ElectronContainer* electrons, const xAOD::CaloCluster* omitCluster) const {
  const xAOD::CaloCluster* leading_emcluster = 0;
  bool LHSel;
  float max_pt = 0.;
  for (const auto & em: *electrons) {
    // check ID
    if(m_doIDCuts){
      LHSel = false;
      LHSel = (bool) m_LHTool2015->accept(em);
      if(!LHSel) continue;
      ATH_MSG_DEBUG("Electron passes " << m_electronIDLevel << " likelihood selection");
    }
    const xAOD::CaloCluster* cl = em->caloCluster();
    if (cl == omitCluster) continue;
    if (cl->pt()/Gaudi::Units::GeV < 10.) continue;
    if (cl->pt() > max_pt) {
      leading_emcluster = cl;
      max_pt = cl->pt();
    }
  }
  return leading_emcluster;

}

const xAOD::TrackParticle* IDPerfMonWenu::electronTrackMatch(const xAOD::TrackParticleContainer* tracks, const xAOD::CaloCluster* cluster, double dEta, double dPhi) const {
  const xAOD::TrackParticle* matched_track = 0;
  double min_dR = 1.0e+20;
  for (const auto & track: *tracks) {
    double deta = cluster->eta()-track->eta();
    double dphi = cluster->phi()-track->phi();
    double dr = std::sqrt(deta*deta + dphi*dphi);
    if (dr < min_dR && std::fabs(deta) < dEta && std::fabs(dphi) < dPhi) {
      min_dR = dr;
      matched_track = track;
    }
  }
  return matched_track;
}

double IDPerfMonWenu::electronTrackMatchEta(const xAOD::TrackParticleContainer* tracks, const xAOD::CaloCluster* cluster, double dEta) const {
  const xAOD::TrackParticle* matched_track = 0;
  double min_dEta = 1.0e+20;
  for (const auto & track: *tracks) {
    double deta = std::fabs(cluster->eta()-track->eta());
    if (deta < min_dEta && deta < dEta) {
      min_dEta = deta;
      matched_track = track;
    }
  }
  double dPhi = 1.0e+20;
  if (matched_track != 0) dPhi = signedDeltaPhi(cluster->phi(),matched_track->phi());
  return dPhi;
}

double IDPerfMonWenu::electronTrackMatchPhi(const xAOD::TrackParticleContainer* tracks, const xAOD::CaloCluster* cluster, double dPhi) const {
  const xAOD::TrackParticle* matched_track = 0;
  double min_dPhi = 1.0e+20;
  for (const auto & track : *tracks) {
    double dphi = std::abs(signedDeltaPhi(cluster->phi(),track->phi()));
    if (dphi < min_dPhi && dphi < dPhi) {
      min_dPhi = dphi;
      matched_track = track;
    }
  }
  double dEta = 1.0e+20;
  if (matched_track != 0) dEta = cluster->eta()-matched_track->eta();
  return dEta;

}

int IDPerfMonWenu::isWenu(const xAOD::CaloCluster* em, const xAOD::CaloCluster* em2, double met) const {
  int selected = 2;
  if(em->pt()/Gaudi::Units::GeV > 25.) --selected;
  if(met/Gaudi::Units::GeV > 20.) --selected; // was at 0 for some reason?
  if(!m_rejectSecondCluster) return selected;
  // else check 2nd EM cluster veto
  if(em2 != 0){
    if(em2->pt()/Gaudi::Units::GeV > 25.){
      ATH_MSG_DEBUG("Event rejected due to second EM cluster w/ pT > 25 GeV");
      selected++;
    }
  }
  return selected;
}

double IDPerfMonWenu::InvMass(const xAOD::CaloCluster* EM1, const xAOD::CaloCluster* EM2) const {
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

double IDPerfMonWenu::InvMass(const xAOD::TrackParticle* trk1, const xAOD::TrackParticle* trk2) const {
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

double IDPerfMonWenu::TransMass(const xAOD::CaloCluster* EM, const xAOD::MissingET* met) const {
  if (EM == 0 || met == 0) return -99.;
  double transmass = 0.;
  float dphi = signedDeltaPhi(EM->phi(),met->phi());
  transmass = std::sqrt(2.*EM->et()*met->met()*(1.-std::cos(dphi)));
  return transmass/Gaudi::Units::GeV;

}

double IDPerfMonWenu::TransMass(const xAOD::TrackParticle* track, const xAOD::MissingET* met) const {
  if (track == 0 || met == 0) return -99.;
  double transmass = 0.;
  float dphi = signedDeltaPhi(track->phi(),met->phi());
  transmass = std::sqrt(2.*(track->p4().Et()/Gaudi::Units::GeV)*(met->met()/Gaudi::Units::GeV)*(1.-std::cos(dphi)));
  return transmass;
}

double IDPerfMonWenu::deltaR(const xAOD::CaloCluster* cluster, const xAOD::TrackParticle* track) const {
  double dr =-999.;
  if (cluster == 0 || track == 0) return dr;
  double deta = cluster->eta()-track->eta();
  double dphi = cluster->phi()-track->phi();
  if(std::fabs(dphi) > M_PI) dphi = 2.*M_PI-std::fabs(dphi);
  dr = std::sqrt(deta*deta + dphi*dphi);
  return dr;

}

double IDPerfMonWenu::signedDeltaPhi(double phi1, double phi2) const {
  double phia=phi1;
  if(phi1>Gaudi::Units::pi) phia=phi1-2.*Gaudi::Units::pi;
  double phib=phi2;
  if(phi2>Gaudi::Units::pi) phib=phi2-2.*Gaudi::Units::pi;
  double dphi=phia-phib;
  if(dphi>Gaudi::Units::pi) dphi-=2.*Gaudi::Units::pi;
  if(dphi<-Gaudi::Units::pi) dphi+=2.*Gaudi::Units::pi;
  return dphi;
}

int IDPerfMonWenu::etaRegion(double eta) {
  int region = -99;
  if (std::fabs(eta) <= 1.) region = barrel;
  else if (eta > 1.) region = eca; // eca
  else if (eta < -1.) region = ecc; // ecc
  return region;

}

void IDPerfMonWenu::FillHistosPerCluster(const xAOD::CaloCluster* cluster, const xAOD::TrackParticle* track, int region, float dEta, float dPhi) {
  if (not cluster) return;
  if (region<0){
    throw std::out_of_range("Region is negative in IDPerfMonWenu::FillHistosPerCluster");
  }
  // THERE IS A CLUSTER
  if (region == incl) { // inclusive only
    m_Wenu_eta[region]->Fill(cluster->eta());
    m_Wenu_phi[region]->Fill(cluster->phi());
    // match in eta and phi separately and make dEta and dPhi plots
    if (dEta < 1.0e+20) {
      m_Wenu_deta[region]->Fill(dEta);
      if (std::fabs(dEta < 0.05)) { // calculate mean only for those in matching window
        m_Wenu_deta_vs_eta[region]->Fill(cluster->eta(),dEta);
        m_Wenu_deta_vs_phi[region]->Fill(cluster->phi(),dEta);
        m_Wenu_absdeta_vs_eta[region]->Fill(cluster->eta(),std::fabs(dEta));
        m_Wenu_absdeta_vs_phi[region]->Fill(cluster->phi(),std::fabs(dEta));
      }
    }
    if (dPhi < 1.0e+20) {
      m_Wenu_dphi[region]->Fill(dPhi);
      if (std::fabs(dPhi < 0.1)) { // calculate mean only for those in matching window
        m_Wenu_dphi_vs_eta[region]->Fill(cluster->eta(),dPhi);
        m_Wenu_dphi_vs_phi[region]->Fill(cluster->phi(),dPhi);
        m_Wenu_absdphi_vs_eta[region]->Fill(cluster->eta(),std::fabs(dPhi));
        m_Wenu_absdphi_vs_phi[region]->Fill(cluster->phi(),std::fabs(dPhi));
      }
    }

  } // end inclusive only

  if (track == 0) return;
  // TRACK-MATCHED
  float eoverp = 0.;
  float track_p = track->pt()*std::cosh(track->eta());
  if (track_p != 0.) eoverp = cluster->e()/track_p;
  m_Wenu_Eop[region]->Fill(eoverp);
  if (track->charge() == 1.) {
    m_Wenu_Eop_plus[region]->Fill(eoverp);
  }
  else if (track->charge() == -1.) {
    m_Wenu_Eop_minus[region]->Fill(eoverp);
  }
  if (region == incl) { // inclusive only
    m_Wenu_trackmatched_eta[region]->Fill(cluster->eta());
    m_Wenu_trackmatched_phi[region]->Fill(cluster->phi());
    if (eoverp > m_eoverp_tight_min && eoverp < m_eoverp_tight_max) {
      m_Wenu_meanEop_vs_p[region]->Fill(track_p/Gaudi::Units::GeV,eoverp);
      m_Wenu_meanEop_vs_invp[region]->Fill(1./(track_p/Gaudi::Units::GeV),eoverp);
      m_Wenu_meanEop_vs_E[region]->Fill(cluster->e()/Gaudi::Units::GeV,eoverp);
      m_Wenu_meanEop_vs_chargedp[region]->Fill(double(track->charge())*track_p/Gaudi::Units::GeV,eoverp);
      m_Wenu_meanEop_vs_chargedE[region]->Fill(double(track->charge())*cluster->e()/Gaudi::Units::GeV,eoverp);
      m_Wenu_meanEop_vs_phi[region]->Fill(track->phi(),eoverp);
      m_Wenu_meanEop_vs_eta[region]->Fill(track->eta(),eoverp);
    }
    if (track->charge() == 1.) {
      if (eoverp > m_eoverp_tight_min && eoverp < m_eoverp_tight_max) {
        m_Wenu_meanEop_vs_p_plus[region]->Fill(track_p/Gaudi::Units::GeV,eoverp);
        m_Wenu_meanEop_vs_invp_plus[region]->Fill(1./(track_p/Gaudi::Units::GeV),eoverp);
        m_Wenu_meanEop_vs_E_plus[region]->Fill(cluster->e()/Gaudi::Units::GeV,eoverp);
        m_Wenu_meanEop_vs_phi_plus[region]->Fill(track->phi(),eoverp);
        m_Wenu_meanEop_vs_eta_plus[region]->Fill(track->eta(),eoverp);
      }
    }
    else if (track->charge() == -1.) {
      if (eoverp > m_eoverp_tight_min && eoverp < m_eoverp_tight_max) {
        m_Wenu_meanEop_vs_p_minus[region]->Fill(track_p/Gaudi::Units::GeV,eoverp);
        m_Wenu_meanEop_vs_invp_minus[region]->Fill(1./(track_p/Gaudi::Units::GeV),eoverp);
        m_Wenu_meanEop_vs_E_minus[region]->Fill(cluster->e()/Gaudi::Units::GeV,eoverp);
        m_Wenu_meanEop_vs_phi_minus[region]->Fill(track->phi(),eoverp);
        m_Wenu_meanEop_vs_eta_minus[region]->Fill(track->eta(),eoverp);
      }
    }
    if (eoverp < m_eoverp_standard_max && eoverp > m_eoverp_standard_min) {
      m_Wenu_trackmatched_Eopmatched_eta[region]->Fill(cluster->eta());
      m_Wenu_trackmatched_Eopmatched_phi[region]->Fill(cluster->phi());
    }
    if (eoverp < m_eoverp_tight_max && eoverp > m_eoverp_tight_min) {
      m_Wenu_trackmatched_tightEopmatched_eta[region]->Fill(cluster->eta());
      m_Wenu_trackmatched_tightEopmatched_phi[region]->Fill(cluster->phi());
    }
    if (eoverp > m_eoverp_tight_min && eoverp < 1.) {
      m_Wenu_Eop_lt1_vs_eta[region]->Fill(cluster->eta());
      m_Wenu_Eop_lt1_vs_phi[region]->Fill(cluster->phi());
      m_Wenu_Eop_lt1_gt1[region]->Fill(0.75);
    }
    else if (eoverp > 1. && eoverp < m_eoverp_tight_max) {
      m_Wenu_Eop_gt1_vs_eta[region]->Fill(cluster->eta());
      m_Wenu_Eop_gt1_vs_phi[region]->Fill(cluster->phi());
      m_Wenu_Eop_lt1_gt1[region]->Fill(1.25);
    }
    if (eoverp > 0.5 && eoverp < 2.5) m_Wenu_Eop_05_25[region]->Fill(0.5);
    if (eoverp > 1.5 && eoverp < 2.5) m_Wenu_Eop_15_25[region]->Fill(0.5);

  } // end inclusive only

}
