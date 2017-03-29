// Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#if 0

#include "TrigIDtrkMonitoring/HLTIDZeeTagProbe.h"
#include "GaudiKernel/MsgStream.h"
#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "StoreGate/StoreGateSvc.h"
#include "Particle/TrackParticleContainer.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "TLorentzVector.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticle/TrigPhotonContainer.h"
//#include "TrigMissingEtEvent/TrigMissingETContainer.h"
//#include "CaloUtils/CaloClusterVertexCorrection.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

using namespace Analysis;
using namespace HLTIDTagProbe;
using namespace CLHEP;

HLTIDZeeTagProbe::HLTIDZeeTagProbe(const std::string & type, const std::string & name, const IInterface* parent)
  :IHLTMonTool(type, name, parent),
   TrigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
   tracktocalo("ExtrapolateToCaloTool/ExtrapolateToCaloTool"),
   m_trackExtrapolator("TrigInDetTrackExtrapolator")
{

  double PI = TMath::Pi();
  declareProperty("TriggerChains", m_TrigChains);
  declareProperty("Algorithm_Names", m_AlgNames);
  declareProperty("Parameters_to_use", m_Parameters);

  declareProperty("Output_File_Name", m_FileName);

  /// Histogram properties
  declareProperty("Eta_bin_numbers",m_etabins = 50 );
  declareProperty("Eta_bin_max", m_etabinmax = 2.5);
  declareProperty("Eta_bin_min", m_etabinmin = -2.5 );
  declareProperty("Phi_bin_numbers", m_phibins = 25 );
  declareProperty("Phi_bin_max", m_phibinmax = PI );
  declareProperty("Phi_bin_min", m_phibinmin = -PI );
  declareProperty("Z0_bin_numbers", m_z0bins = 25 );
  declareProperty("Z0_bin_max", m_z0binmax = 60. );
  declareProperty("Z0_bin_min", m_z0binmin = -60. );
  declareProperty("D0_bin_numbers", m_d0bins = 25 );
  declareProperty("D0_bin_max", m_d0binmax = 3. );
  declareProperty("D0_bin_min", m_d0binmin = -3. );
  declareProperty("Nvtx_bin_numbers", m_nvtxbins = 30 );
  declareProperty("Nvtx_bin_max", m_nvtxbinmax = 30.5 );
  declareProperty("Nvtx_bin_min", m_nvtxbinmin = 0.5 );
  declareProperty("MZ_bin_numbers", m_mZbins = 20 );
  declareProperty("MZ_bin_max", m_mZbinmax = 120.0 );
  declareProperty("MZ_bin_min", m_mZbinmin = 60.0 );
  declareProperty("Pt_bin_numbers", m_ptbins = 50 );
  declareProperty("Pt_bin_max" , m_ptbinmax = 100000. );
  declareProperty("Pt_bin_min" , m_ptbinmin = 0. );
  declareProperty( "RCalBarrelFace",       m_RCAL = 1470.0*mm );
  declareProperty( "ZCalEndcapFace",       m_ZCAL = 3800.0*mm );

  m_TrigChainName.clear();
  m_TrigChainName.push_back("e24vh_tight1_e15_NoCut_Zee");
  declareProperty("TrigChainName",  m_TrigChainName[0] );


  //  declareProperty("tight_electron_collection_name", m_tightElectronCollectionName = "ElectronAODCollection");

  declareProperty("Debug", do_debugging = true);

  declareProperty("ExcludeICElectrons", do_excludeICelec=1);

  declareProperty("ExtrapolateToCaloTool", tracktocalo);
  declareProperty("TrackExtrapolatorTool", m_trackExtrapolator);


}

StatusCode HLTIDZeeTagProbe::init()
{
    //    ATH_MSG_INFO("yvonne: we initialize the tp code");

  ATH_MSG_DEBUG("initialize()");

  /// Locate the StoreGateSvc and initialize our local ptr
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (!sc.isSuccess() || 0 == m_storeGate)
    ATH_MSG_ERROR("Could not find StoreGateSvc");

  /// Retrieve the TrigDecisionTool
  sc =  TrigDec.retrieve();
  if (sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve TrigDecisionTool");
    return sc;
  }
  ATH_MSG_DEBUG("Trigger initialization successful");


  if(tracktocalo.retrieve().isFailure()){
    ATH_MSG_ERROR("Could not retrieve track to calo tool ");
  }
  else {
    ATH_MSG_VERBOSE("Extrapolator tool recovered ");
  }


  if(m_trackExtrapolator.retrieve().isFailure()) {
 	 ATH_MSG_FATAL("Unable to locate TrackExtrapolator tool ");
    return StatusCode::FAILURE;
  }

  //    ATH_MSG_INFO("yvonne: we are done to initialize the tp code");

  return StatusCode::SUCCESS;
}


#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTIDZeeTagProbe::book()
#else
StatusCode HLTIDZeeTagProbe::book(bool /*newEventsBlock*/, bool /*newLumiBlock*/, bool newRun)
#endif
{ //yvonne: change back - but for testing on files just check if histos are not there yet
  //   if(HistoNames.size()==0) {

  //   ATH_MSG_INFO("yvonne: we make histos in  the tp code");
  if (newRun){
    m_TrigChains.push_back(m_TrigChainName[0]); //yvonne: if we want to loop it over more, it still needs to be implemented!
    m_AlgNames.push_back("_IDS_");
    m_AlgNames.push_back("_SIT_");
    m_AlgNames.push_back("_L2StarA_");
    m_AlgNames.push_back("_L2StarB_");
    m_AlgNames.push_back("_L2StarC_");
    m_AlgNames.push_back("_IDS_L2only_");
    m_AlgNames.push_back("_SIT_L2only_");
    m_AlgNames.push_back("_L2StarA_L2only_");
    m_AlgNames.push_back("_L2StarB_L2only_");
    m_AlgNames.push_back("_L2StarC_L2only_");
    m_AlgNames.push_back("_EF_");
    m_Parameters.push_back("pT");
    m_Parameters.push_back("eta");
    m_Parameters.push_back("phi");
    m_Parameters.push_back("phi_poscharge");
    m_Parameters.push_back("phi_negcharge");
    m_Parameters.push_back("z0");
    m_Parameters.push_back("d0");
    m_Parameters.push_back("nvtx");
    m_Parameters.push_back("ntrk");
    m_Parameters.push_back("ntrktag");
    m_Parameters.push_back("emenergyfrac");
    m_Parameters.push_back("mZ");


    /// Book histograms
    MonGroup* monGroup = new MonGroup(this, "HLT/ZeeTagProbe", run);
    MonGroup* monGroup_ROI = new MonGroup(this, "HLT/ZeeTagProbe/ROI_Variables", run);
    MonGroup* monGroup_Off = new MonGroup(this, "HLT/ZeeTagProbe/Offline_Selections", run);
    MonGroup* monGroup_On = new MonGroup(this, "HLT/ZeeTagProbe/Online_Selections", run);
    MonGroup* monGroup_DeltaR = new MonGroup(this, "HLT/ZeeTagProbe/DeltaR_Plots", run);
    MonGroup* monGroup_ZMass = new MonGroup(this, "HLT/ZeeTagProbe/ZMass_Plots", run);
    MonGroup* monGroup_Eff = new MonGroup(this, "HLT/ZeeTagProbe/Efficiency_Plots", run);
    MonGroup* monGroup_Track = new MonGroup(this, "HLT/ZeeTagProbe/Tracks", run);
    MonGroup* monGroup_Debug = new MonGroup(this, "HLT/ZeeTagProbe/Debug", run);
    MonGroup* monGroup_InEff = new MonGroup(this, "HLT/ZeeTagProbe/Inefficient", run);
    addMonGroup(monGroup);
    addMonGroup(monGroup_ROI);
    addMonGroup(monGroup_Off);
    addMonGroup(monGroup_On);
    addMonGroup(monGroup_DeltaR);
    addMonGroup(monGroup_ZMass);
    addMonGroup(monGroup_Eff);
    addMonGroup(monGroup_Track);
    addMonGroup(monGroup_Debug);
    addMonGroup(monGroup_InEff);

    HistoNames.clear();
    HistoPointers.clear();

    std::string histName="";
    for (std::vector<std::string>::iterator it = m_TrigChains.begin(); it!= m_TrigChains.end(); ++it){
      for (std::vector<std::string>::iterator it2 = m_AlgNames.begin(); it2!= m_AlgNames.end(); ++it2){
	for (std::vector<std::string>::iterator it3 = m_Parameters.begin(); it3!= m_Parameters.end(); ++it3){
	  histName = (((*it) + (*it2) + (*it3)).c_str());
	  if (histName != "") HistoNames.push_back(histName);
	  histName = "";
	}
      }
    }

    //    ATH_MSG_INFO("yvonne: we are in the middle to make histos in  the tp code");

    this->setCurrentMonGroup("HLT/ZeeTagProbe/Efficiency_Plots");
    for (std::vector<std::string>::iterator it4 = HistoNames.begin(); it4!= HistoNames.end(); ++it4){
      std::string name = (std::string)(*it4);
      if (name.find("pT") != std::string::npos){
	addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), m_ptbins, m_ptbinmin, m_ptbinmax, 0, 110));
	addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), m_ptbins, m_ptbinmin, m_ptbinmax, 0, 110));
      }
      if (name.find("eta") != std::string::npos){
	addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), m_etabins, m_etabinmin, m_etabinmax, 0, 110));
	addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), m_etabins, m_etabinmin, m_etabinmax, 0, 110));
	//      addProfile( new TProfile((name + "_eff_OS_deltaR").c_str(), (name + "_eff_OS_deltaR").c_str(), m_etabins, m_etabinmin, m_etabinmax, 0, 110)); //yvonne: try only opposite sign tracks
	//      addProfile( new TProfile((name + "_eff_SS_deltaR").c_str(), (name + "_eff_SS_deltaR").c_str(), m_etabins, m_etabinmin, m_etabinmax, 0, 110)); // yvonne: try only same sign tracks
      }
      if (name.find("phi") != std::string::npos){
	addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), m_phibins, m_phibinmin, m_phibinmax, 0, 110));
	addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), m_phibins, m_phibinmin, m_phibinmax, 0, 110));
      }
      if (name.find("z0") != std::string::npos){
	addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), m_z0bins, m_z0binmin, m_z0binmax, 0, 110));
	addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), m_z0bins, m_z0binmin, m_z0binmax, 0, 110));
      }
      if (name.find("d0") != std::string::npos){
	addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), m_d0bins, m_d0binmin, m_d0binmax, 0, 110));
	addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), m_d0bins, m_d0binmin, m_d0binmax, 0, 110));
      }
      if (name.find("nvtx") != std::string::npos){
	addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), m_nvtxbins, m_nvtxbinmin, m_nvtxbinmax, 0, 110));
	addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), m_nvtxbins, m_nvtxbinmin, m_nvtxbinmax, 0, 110));
      }
      if (name.find("ntrk") != std::string::npos){
	addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), 20, 0.0, 20.0));
	addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), 20, 0.0, 20.0));
      }
      if (name.find("mZ") != std::string::npos){
	addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), m_mZbins, m_mZbinmin, m_mZbinmax, 0, 110));
	addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), m_mZbins, m_mZbinmin, m_mZbinmax, 0, 110));
      }
      if (name.find("ntrktag") != std::string::npos){
        addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), 20, 0.0, 20.0));
        addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), 20, 0.0, 20.0));
      }
      if (name.find("emenergyfrac") != std::string::npos){
        addProfile( new TProfile((name + "_eff_offline").c_str(), (name + "_eff_offline").c_str(), 21, 0.0, 1.05));
        addProfile( new TProfile((name + "_eff_ptl20_offline").c_str(), (name + "_eff_ptl20_offline").c_str(), 21, 0.0, 1.05));
        addProfile( new TProfile((name + "_eff_ptge20_offline").c_str(), (name + "_eff_ptge20_offline").c_str(), 21, 0.0, 1.05));

        addProfile( new TProfile((name + "_eff_deltaR").c_str(), (name + "_eff_deltaR").c_str(), 21, 0.0, 1.05));
	addProfile( new TProfile((name + "_eff_ptl20_deltaR").c_str(), (name + "_eff_ptl20_deltaR").c_str(), 21, 0.0, 1.05));
	addProfile( new TProfile((name + "_eff_ptge20_deltaR").c_str(), (name + "_eff_ptge20_deltaR").c_str(), 21, 0.0, 1.05));


      }



    }

    this->setCurrentMonGroup("HLT/ZeeTagProbe/ROI_Variables");
    addHistogram(new TH1F("ROIs_per_event", "Number of ROIs per event", 10, 0 ,10), "HLT/ZeeTagProbe/ROI_Variables");
    addHistogram(new TH1F("ROIs_per_event_(Tight)", "Number of Tight ROIs per event", 10, 0 ,10), "HLT/ZeeTagProbe/ROI_Variables");
    addHistogram(new TH1F("ROIs_per_event_(Loose)", "Number of Loose ROIs per event", 10, 0 ,10), "HLT/ZeeTagProbe/ROI_Variables");
    addHistogram(new TH1F("h_roi_eta", "Roi eta", 50,-2.5,2.5));
    addHistogram(new TH1F("h_roi_eta_(tight)", "Roi eta", 50,-2.5,2.5));
    addHistogram(new TH1F("h_roi_eta_(loose)", "Roi eta", 50,-2.5,2.5));
    addHistogram(new TH1F("h_roi_phi", "Roi phi", 50,-4.,4.));
    addHistogram(new TH1F("h_roi_phi_(tight)", "Roi phi", 50,-4.,4.));
    addHistogram(new TH1F("h_roi_phi_(loose)", "Roi phi", 50,-4.,4.));
    //  addHistogram(new TH1F("h_roi_track", "Tracks per ROI", 20, 0, 20));
    addHistogram(new TH1F("h_features_EF", "Number of features per event" ,10, 0, 10));
    //  addHistogram(new TH1F("h_rois_EF", "Number of rois per feature" ,10, 0, 10));
    addHistogram(new TH1F("h_tracks_roi", "Number of tracks per ROI", 20, 0, 20));
    addHistogram(new TH1F("L2_ROIs_per_event", "Number of ROIs per event", 10, 0 ,10), "HLT/ZeeTagProbe/ROI_Variables");
    addHistogram(new TH1F("L2_ROIs_per_event_(Tight)", "Number of Tight ROIs per event", 10, 0 ,10), "HLT/ZeeTagProbe/ROI_Variables");
    addHistogram(new TH1F("L2_ROIs_per_event_(Loose)", "Number of Loose ROIs per event", 10, 0 ,10), "HLT/ZeeTagProbe/ROI_Variables");
    addHistogram(new TH1F("L2_h_roi_eta", "Roi eta", 50,-2.5,2.5));
    addHistogram(new TH1F("L2_h_roi_eta_(tight)", "Roi eta", 50,-2.5,2.5));
    addHistogram(new TH1F("L2_h_roi_eta_(loose)", "Roi eta", 50,-2.5,2.5));
    addHistogram(new TH1F("L2_h_roi_phi", "Roi phi", 50,-4.,4.));
    addHistogram(new TH1F("L2_h_roi_phi_(tight)", "Roi phi", 50,-4.,4.));
    addHistogram(new TH1F("L2_h_roi_phi_(loose)", "Roi phi", 50,-4.,4.));
    //  addHistogram(new TH1F("L2_h_roi_track", "Tracks per ROI", 20, 0, 20));
    addHistogram(new TH1F("h_features_L2", "Number of features per event" ,10, 0, 10));
    //  addHistogram(new TH1F("h_rois_L2", "Number of rois per feature" ,10, 0, 10));
    addHistogram(new TH1F("L2_h_tracks_roi", "Number of tracks per ROI", 20, 0, 20));

    this->setCurrentMonGroup("HLT/ZeeTagProbe/Offline_Selections");
    addHistogram(new TH1F("h_offline_electron_pT", "Offline Electron pT" ,50, 0, 100000), "HLT/ZeeTagProbe/Offline_Selections");
    addHistogram(new TH1F("h_offline_electron_Eta", "Offline Electron Eta" ,50, -2.5, 2.5), "HLT/ZeeTagProbe/Offline_Selections");
    addHistogram(new TH1F("h_offline_electron_Phi", "Offline Electron Phi" ,50, -4, 4), "HLT/ZeeTagProbe/Offline_Selections");
    addHistogram(new TH1F("h_offline_electron_Et", "Offline Electron Et", 100, 0, 100000.), "HLT/ZeeTagProbe/Offline_Selections");
    addHistogram(new TH1F("h_offline_electron_charge", "Offline Electron charge", 5, -2, 2), "HLT/ZeeTagProbe/Offline_Selections");

    this->setCurrentMonGroup("HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_tag_pT", "Tag Electron pT" ,50, 0, 100000),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_tag_Eta", "Tag Electron Eta" ,50, -2.5, 2.5),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_tag_Phi", "Tag Electron Phi" ,50, -4, 4),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_tag_Et", "Track Et", 100, 0, 100000.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_tag_charge", "Tag Electron charge", 5, -2, 2),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_electron_tight_phi", "Electrons from tight Roi phi" ,50, -4., 4.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_electron_tight_pt", "Electron from tight Roi pt" ,50, 0, 100000.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_electron_tight_eta", "Electrons from tight Roi eta" ,50, -2.5, 2.5),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_electron_tight_et", "Electrons from tight Roi et" ,50, 0, 100000.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_electron_noCut_phi", "Electrons from noCut Roi phi" ,50, -4., 4.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_electron_noCut_pt", "Electron from noCut Roi pt" ,50, 0, 100000.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_electron_noCut_eta", "Electrons from noCut Roi eta" ,50, -2.5, 2.5),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_electron_noCut_et", "Electrons from noCut Roi et" ,50, 0, 100000),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_probe_pT_cluster", "Probe Cluster pT" ,50, 0, 100000),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_probe_Eta_cluster", "Probe Cluster Eta" ,50, -2.5, 2.5),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_probe_Phi_cluster", "Probe Cluster Phi" ,50, -4, 4),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_probe_Et_cluster", "Track Et", 100, 0, 100000.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_delPhi_cluster_electron", "DeltaPhi of cluster and electron before ZMass hypothesis", 50,0.0, TMath::Pi()),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("h_delPhi_tag_probe", "DeltaPhi of cluster and electron after ZMass hypothesis", 50, 0.0, TMath::Pi()),"HLT/ZeeTagProbe/Online_Selections");


    addHistogram(new TH1F("L2_h_tag_pT", "Tag Electron pT" ,50, 0, 100000),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_tag_Eta", "Tag Electron Eta" ,50, -2.5, 2.5),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_tag_Phi", "Tag Electron Phi" ,50, -4, 4),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_tag_Et", "Track Et", 100, 0, 100000.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_tag_charge", "Tag Electron charge", 5, -2, 2),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_electron_tight_phi", "Electrons from tight Roi phi" ,50, -4., 4.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_electron_tight_pt", "Electron from tight Roi pt" ,50, 0, 100000.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_electron_tight_eta", "Electrons from tight Roi eta" ,50, -2.5, 2.5),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_electron_tight_et", "Electrons from tight Roi et" ,50, 0, 100000),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_electron_noCut_phi", "Electrons from noCut Roi phi" ,50, -4., 4.),"HLT/ZeeTagProbe/Online_Selections");
    //  addHistogram(new TH1F("L2_h_electron_noCut_pt", "Electron from noCut Roi pt" ,50, 0, 100000.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_electron_noCut_eta", "Electrons from noCut Roi eta" ,50, -2.5, 2.5),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_electron_noCut_et", "Electrons from noCut Roi et" ,50, 0, 100000),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_electron_noCut_e", "Electrons from noCut Roi e" ,50, 0, 100000),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_probe_pT_cluster", "Probe Cluster pT" ,50, 0, 100000),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_probe_Eta_cluster", "Probe Cluster Eta" ,50, -2.5, 2.5),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_probe_Phi_cluster", "Probe Cluster Phi" ,50, -4, 4),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_probe_Et_cluster", "Track Et", 100, 0, 100000.),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_delPhi_cluster_electron", "DeltaPhi of cluster and electron before ZMass hypothesis", 50, 0.0, TMath::Pi()),"HLT/ZeeTagProbe/Online_Selections");
    addHistogram(new TH1F("L2_h_delPhi_tag_probe", "DeltaPhi of cluster and electron after ZMass hypothesis", 50, 0.0, TMath::Pi()),"HLT/ZeeTagProbe/Online_Selections");


    this->setCurrentMonGroup("HLT/ZeeTagProbe/DeltaR_Plots");
    addHistogram(new TH1F("h_deltaPhi_electron_track", "", 50, 0.0, TMath::Pi()));
    addHistogram(new TH1F("h_deltaPhi_electron_cluster", "", 50, 0.0, TMath::Pi()));
    addHistogram(new TH1F("h_deltaPhi_roi_track", "", 50, 0.0, 0.4));
    addHistogram(new TH1F("h_deltaPhi_roi_electron", "", 50, 0.0, TMath::Pi()));
    addHistogram(new TH1F("h_deltaEta_electron_track", "", 50, -1.0, 1.0));
    addHistogram(new TH1F("h_deltaEta_roi_track", "", 50, -0.4, 0.4));
    addHistogram(new TH1F("h_deltaEta_roi_electron", "", 50, -1.0, 1.0));
    addHistogram(new TH1F("h_Eta_cluster", "", 200, -2.5, 2.5));
    addHistogram(new TH1F("h_Eta_roi", "", 200, -2.5, 2.5));
    addHistogram(new TH1F("h_deltaR_electron_track", "", 50, 0, TMath::Pi()));
    addHistogram(new TH1F("h_deltaR_roi_track", "", 50, 0., 1.0));
    addHistogram(new TH1F("h_deltaR_roi_electron", "", 50, 0., TMath::Pi()));
    addHistogram(new TH1F("h_deltaPhi_roi_track", "", 50, 0.0, 0.4));
    addHistogram(new TH1F("h_deltaPhi_roi_cluster", "", 50, 0.0, 0.4));
    addHistogram(new TH1F("h_deltaEta_roi_track", "", 50, -0.4, 0.4));
    addHistogram(new TH1F("h_deltaEta_roi_cluster", "", 50, -0.4, 0.4));
    addHistogram(new TH1F("h_deltaR_roi_cluster", "", 50, 0., 1.));
    addHistogram(new TH1F("h_deltaPhi_track_cluster", "",50,0.0,0.4));
    addHistogram(new TH1F("h_deltaEta_track_cluster", "",50,-0.4,0.4));
    addHistogram(new TH1F("h_deltaR_track_cluster", "",50,0.0,1.0));


    addHistogram(new TH1F("L2_h_deltaPhi_electron_track", "", 50, 0.0, TMath::Pi()));
    addHistogram(new TH1F("L2_h_deltaPhi_electron_cluster", "", 50, 0.0, TMath::Pi()));
    addHistogram(new TH1F("L2_h_deltaPhi_roi_track", "", 50, 0.0, 0.4));
    addHistogram(new TH1F("L2_h_deltaPhi_roi_electron", "", 50, 0.0, TMath::Pi()));
    addHistogram(new TH1F("L2_h_deltaEta_electron_track", "", 50, -1.0, 1.0));
    addHistogram(new TH1F("L2_h_deltaEta_roi_track", "", 50, -0.4, 0.4));
    addHistogram(new TH1F("L2_h_deltaEta_roi_electron", "", 50, -1.0, 1.0));
    addHistogram(new TH1F("L2_h_deltaR_electron_track", "", 50, 0., TMath::Pi()));
    addHistogram(new TH1F("L2_h_deltaR_roi_track", "", 50, 0., 1.0));
    addHistogram(new TH1F("L2_h_deltaR_roi_electron", "", 50, 0., TMath::Pi()));
    addHistogram(new TH1F("L2_h_deltaPhi_roi_track", "", 50, 0.0, 0.4));
    addHistogram(new TH1F("L2_h_deltaPhi_roi_cluster", "", 50, 0.0, 0.4));
    addHistogram(new TH1F("L2_h_deltaEta_roi_track", "", 50, -0.4, 0.4));
    addHistogram(new TH1F("L2_h_Eta_cluster", "", 200, -2.5, 2.5));
    addHistogram(new TH1F("L2_h_Eta_roi", "", 200, -2.5, 2.5));
    addHistogram(new TH1F("L2_h_deltaEta_roi_cluster", "", 50, -0.4, 0.4));
    addHistogram(new TH1F("L2_h_deltaR_roi_cluster", "", 50, 0., 1.));
    addHistogram(new TH1F("L2_h_deltaPhi_track_cluster", "",50,0.0,0.4));
    addHistogram(new TH1F("L2_h_deltaEta_track_cluster", "",50,-0.4,0.4));
    addHistogram(new TH1F("L2_h_deltaR_track_cluster", "",50,0.0,1.0));

    this->setCurrentMonGroup("HLT/ZeeTagProbe/ZMass_Plots");
    addHistogram(new TH1F("h_Z_mass_electrons_clusters", "Reconstructed Z mass distribution", 50, 0, 150000));
    addHistogram(new TH1F("h_Z_mass_electrons_clusters_cut", "Reconstructed Z mass distribution with cut", 50, 0, 150000));
    addHistogram(new TH1F("h_Z_mass_electrons_clusters_allcut", "Reconstructed Z mass distribution with all cuts before fillprobe", 50, 0, 150000));
    addHistogram(new TH1F("h_Z_mass_tracks", "Z mass using track and tag electron", 100, 0., 150000.));
    //  addHistogram(new TH1F("h_Z_mass_tracks_found", "Z mass using track and tag electron", 100, 0., 150000.));
    //  addHistogram(new TH1F("h_Z_mass_tracks_notfound", "Z mass using track and tag electron", 100, 0., 150000.));
    addHistogram(new TH1F("h_Z_mass_electron_highpTtrack", "Z mass using the highest pT track and tag electron", 100, 0., 150000.));

    addHistogram(new TH1F("h_Z_mass_offline", "Z mass using the offline Electrons", 100, 0., 150000.));
    //  addHistogram(new TH1F("h_Z_mass_offline_track", "Z mass using the offline Electrons", 100, 0., 150000.));

    addHistogram(new TH1F("L2_h_Z_mass_electrons_clusters", "Reconstructed Z mass distribution", 50, 0., 150000));
    addHistogram(new TH1F("L2_h_Z_mass_electrons_clusters_cut", "Reconstructed Z mass distribution", 50, 0, 150000));
    addHistogram(new TH1F("L2_h_Z_mass_tracks", "Z mass using track and tag electron", 100, 0., 150000.));
    // addHistogram(new TH1F("L2_h_Z_mass_tracks_found", "Z mass using track and tag electron", 100, 0., 150000.));
    //  addHistogram(new TH1F("L2_h_Z_mass_tracks_notfound", "Z mass using track and tag electron", 100, 0., 150000.));
    //  addHistogram(new TH1F("L2_h_Z_mass_electron_highpTtrack", "Z mass using the highest pT track and tag electron", 100, 0., 150000.));

    this->setCurrentMonGroup("HLT/ZeeTagProbe/Debug");
    addHistogram( new TH1F("Execution_fail","Stage an event was rejected in the alg",7,0,7));
    addHistogram( new TH1F("h_match_fail","diagnostic histograms", 5,0,5));
    addHistogram( new TH1F("TriggerPassed","Check number of times triggers passed", 4,0,4)); // 0/1 EF Trigger Pass/Fail, 2/3 L2 Trigger Pass/Fail
    addHistogram( new TH1F("TruncatedResult","Check if Ef/L2 result was truncated", 4,0,4)); // 0/1 EF Truncated/Not, 2/3 L2 Truncated/Not

    //  addHistogram( new TH1F("L2_Execution_fail","Stage an event was rejected in the alg",7,0,7));
    //  addHistogram( new TH1F("L2_h_match_fail","diagnostic histograms", 5,0,5));
    addHistogram( new TH1F("L2_emenergyfrac_cluster", "EM fraction of energy of the probe cluster at L2", 41, 0.0, 1.025));
    addHistogram( new TH1F("EF_emenergyfrac_cluster", "EM fraction of energy of the probe cluster at EF", 41, 0.0, 1.025));
    addHistogram( new TH1F("EF_emenergyfrac_cluster_tag", "EM fraction of energy of the tag electron at EF", 41, 0.0, 1.025));

    // check the offline electron on its variables to distinguish bremsstrahlung

    this->setCurrentMonGroup("HLT/ZeeTagProbe/Tracks");
    addHistogram( new TH1F("h_track_EF_pT", "Track pT", 50, 0, 100000));
    addHistogram( new TH1F("h_track_EF_Phi", "Track Phi", 50, -4, 4));
    addHistogram( new TH1F("h_track_EF_Eta", "Track Eta", 50, -2.5, 2.5));
    addHistogram( new TH1F("h_track_EF_Et", "Track Et", 100, 0, 100000.));
    addHistogram( new TH1F("h_track_EF_z0", "Track Et", 100, -50, 50));
    //  addHistogram( new TH1F("h_tag_z0", "Tag Electron track z0", 50, -50, 50));
    addHistogram( new TH1F("h_track_EF_deltaz0", "Track Et", 50, -50, 50));

    addHistogram( new TH1F("h_track_L2_pT", "Track pT", 50, 0, 100000));
    addHistogram( new TH1F("h_track_L2_Phi", "Track Phi", 50, -4, 4));
    addHistogram( new TH1F("h_track_L2_Eta", "Track Eta", 50, -2.5, 2.5));
    addHistogram( new TH1F("h_track_L2_Et", "Track Et", 100, 0, 100000.));
    addHistogram( new TH1F("h_track_L2_z0", "Track Et", 100, -50, 50));
    //  addHistogram( new TH1F("L2_h_tag_z0", "Tag Electron track z0", 50, -50, 50));
    //  addHistogram( new TH1F("h_track_L2_deltaz0", "Track Et", 50, -50, 50));

    this->setCurrentMonGroup("HLT/ZeeTagProbe/Inefficient");
    addHistogram(new TH1F("h_tag_Pt_InEff", "Tag Electron pT in inefficient events" ,50, 0, 100000));
    addHistogram(new TH1F("h_tag_Eta_InEff", "Tag Electron Eta in inefficient events" ,50, -2.5, 2.5));
    addHistogram(new TH1F("h_tag_Phi_InEff", "Tag Electron Phi in inefficient events" ,50, -4, 4));
    addHistogram(new TH1F("h_tag_Et_InEff", "Tag Et in inefficient events", 100, 0, 100000.));
    addHistogram(new TH1F("h_probe_Pt_cluster_InEff", "Probe Cluster pT in inefficient events" ,50, 0, 100000));
    addHistogram(new TH1F("h_probe_Eta_cluster_InEff", "Probe Cluster Eta in inefficient events" ,50, -2.5, 2.5));
    addHistogram(new TH1F("h_probe_Phi_cluster_InEff", "Probe Cluster Phi in inefficient events" ,50, -4, 4));
    addHistogram(new TH1F("h_probe_Et_cluster_InEff", "Probe Cluster Et in inefficient events", 100, 0, 100000.));
    addHistogram(new TH1F("h_Z_mass_electrons_clusters_InEff", "Reconstructed Z mass distribution in inefficient events", 50, 0, 150000));

    addHistogram(new TH1F("L2_h_tag_Pt_InEff", "Tag Electron pT in inefficient events" ,50, 0, 100000));
    addHistogram(new TH1F("L2_h_tag_Eta_InEff", "Tag Electron Eta in inefficient events" ,50, -2.5, 2.5));
    addHistogram(new TH1F("L2_h_tag_Phi_InEff", "Tag Electron Phi in inefficient events" ,50, -4, 4));
    addHistogram(new TH1F("L2_h_tag_Et_InEff", "Tag Et in inefficient events", 100, 0, 100000.));
    addHistogram(new TH1F("L2_h_probe_Pt_cluster_InEff", "Probe Cluster pT in inefficient events" ,50, 0, 100000));
    addHistogram(new TH1F("L2_h_probe_Eta_cluster_InEff", "Probe Cluster Eta in inefficient events" ,50, -2.5, 2.5));
    addHistogram(new TH1F("L2_h_probe_Phi_cluster_InEff", "Probe Cluster Phi in inefficient events" ,50, -4, 4));
    addHistogram(new TH1F("L2_h_probe_Et_cluster_InEff", "Probe Cluster Et in inefficient events", 100, 0, 100000.));
    addHistogram(new TH1F("L2_h_Z_mass_electrons_clusters_InEff", "Reconstructed Z mass distribution in inefficient events", 50, 0, 150000));

  }

  //    ATH_MCG_INFO("yvonne: we are done to make histos in  the tp code");
  return StatusCode::SUCCESS;
}

StatusCode HLTIDZeeTagProbe::fill()

{
    //      ATH_MSG_INFO("yvonne: we start to fill stuff in the tp code");
  /// Loop over trigger signitures (not necessary really as there is only one now)
  for (std::vector<std::string>::const_iterator trigit = m_TrigChains.begin(); trigit != m_TrigChains.end() ; ++trigit){

    std::string EFChainName = "EF_" + (*trigit);
    std::string L2ChainName = "L2_" + (*trigit);

    //Check that event passed the trigger
    if (!(TrigDec->isPassed(L2ChainName))) hist("TriggerPassed", "HLT/ZeeTagProbe/Debug")->Fill(3.);
    else hist("TriggerPassed", "HLT/ZeeTagProbe/Debug")->Fill(2.);

    if (!(TrigDec->isPassed(EFChainName))) hist("TriggerPassed", "HLT/ZeeTagProbe/Debug")->Fill(1.);
    else hist("TriggerPassed", "HLT/ZeeTagProbe/Debug")->Fill(0);

    // Check if the event was truncated at L2 or EF. If truncated at L2 or EF skip entirely!

    const DataHandle<TrigDec::TrigDecision> td;
    StatusCode sc2 = StatusCode::SUCCESS;
    sc2 = evtStore()->retrieve(td);
    if (sc2.isFailure()) {
    }

    //      ATH_MSG_INFO("yvonne: test 1");

    bool EventTruncated_EF = false;
    bool EventTruncated_L2 = false;
    const HLT::HLTResult* r = (&(td->getL2Result()));
    if (r->isHLTResultTruncated()){
      hist("TruncatedResult", "HLT/ZeeTagProbe/Debug")->Fill(2);
      EventTruncated_L2 = true;
    }
    else hist("TruncatedResult", "HLT/ZeeTagProbe/Debug")->Fill(3);

    r = (&(td->getEFResult()));
    if (r->isHLTResultTruncated()){
      hist("TruncatedResult", "HLT/ZeeTagProbe/Debug")->Fill(0);
      EventTruncated_EF = true;
    }
    else hist("TruncatedResult", "HLT/ZeeTagProbe/Debug")->Fill(1);

    if (EventTruncated_L2 || EventTruncated_EF) return StatusCode::SUCCESS;

    /// Get the features from the Trig Decision tool
    Trig::FeatureContainer featContEF = TrigDec->features(EFChainName);
    Trig::FeatureContainer featContL2 = TrigDec->features(L2ChainName);
    const std::vector< Trig::Feature<egammaContainer> > EF_Electrons = featContEF.get<egammaContainer>("");
    const std::vector< Trig::Feature<TrigElectronContainer> > L2_Electrons = featContL2.get<TrigElectronContainer>("L2ElectronFex");

    /// Get the combined feature containers
    const std::vector< Trig::Combination > comb_container_EF = featContEF.getCombinations();
    const std::vector< Trig::Combination > comb_container_L2 = featContL2.getCombinations();
    hist("h_features_EF", "HLT/ZeeTagProbe/ROI_Variables")->Fill(comb_container_EF.size());
    hist("h_features_L2", "HLT/ZeeTagProbe/ROI_Variables")->Fill(comb_container_L2.size());

    /// Get offline tracks for use in debugging
    StatusCode sc_offlinetracks = StatusCode::SUCCESS;
    std::vector< simpleTrack > OFFTracks;
    std::vector< simpleTrack > OFF_EGAMMA_Tracks;
    const DataHandle<Rec::TrackParticleContainer> trackCollectionOFF;
    std::vector< const Analysis::Electron* > m_OfflineTightElectrons_filter;
    std::string m_offlineCollection = "TrackParticleCandidate";




    //bool istightelectron = false;

     if (do_debugging){
      sc_offlinetracks = m_storeGate->retrieve(trackCollectionOFF,m_offlineCollection);
      if (sc_offlinetracks.isFailure()) {
	      ATH_MSG_DEBUG(" Failed to retrieve offline tracks!");
      }
      ATH_MSG_DEBUG("Before electron offline selections");
      /// Get offline electrons for use in debugging
      //      ElectronContainer* ElectronCollection;
      const DataHandle<ElectronContainer> ElectronCollection;
      std::string m_ElectronCollection = "ElectronAODCollection";

      StatusCode sc_electrons=m_storeGate->retrieve( ElectronCollection, m_ElectronCollection);
      if (sc_electrons.isFailure()){
	      ATH_MSG_DEBUG("No Electrons of type " << m_ElectronCollection << " found in storeGate");
	//	ElectronCollection = 0;
      }
      else{
	      ATH_MSG_DEBUG("Number of electrons = " << ElectronCollection->size());
      }


      if (ElectronCollection && ElectronCollection->size() != 0){
	int counter_offline_electrons = 0;
	for (ElectronContainer::const_iterator elecItr  = ElectronCollection->begin(); elecItr != ElectronCollection->end(); ++elecItr) {
	  const Analysis::Electron* temp = *elecItr;
	  hist("h_offline_electron_pT","HLT/ZeeTagProbe/Offline_Selections")->Fill(temp->pt());
	  hist("h_offline_electron_Eta","HLT/ZeeTagProbe/Offline_Selections")->Fill(temp->eta());
	  hist("h_offline_electron_Phi","HLT/ZeeTagProbe/Offline_Selections")->Fill(temp->phi());
	  hist("h_offline_electron_Et","HLT/ZeeTagProbe/Offline_Selections")->Fill(temp->et());
	  hist("h_offline_electron_charge","HLT/ZeeTagProbe/Offline_Selections")->Fill(temp->charge());
	  if( (*elecItr)->isem(egammaPID::ElectronMediumPP)==0 || 1== (*elecItr)->passID(egammaPID::ElectronIDMediumPP)  ){
	      //istightelectron=false;
	      if(!do_excludeICelec || (do_excludeICelec && (TMath::Abs(temp->eta())<1.37 || TMath::Abs(temp->eta())>1.52) ))
		  {
		      m_OfflineTightElectrons_filter.push_back(temp); // tight
		      //istightelectron=true;
		      ATH_MSG_DEBUG("JWH    Offline Electron: " << counter_offline_electrons
                                                    << " - pT = " << (*elecItr)->pt()
                                                    << ", eta = " << (*elecItr)->eta()
                                                    << ", phi = " << (*elecItr)->phi()
                                                    << ", charge = " << (*elecItr)->charge());
		  }



	  }


	  ++counter_offline_electrons;
	}
      }

     /// Event filters for debugging
      if (m_OfflineTightElectrons_filter.size() < 2){
	      ATH_MSG_DEBUG("Killing event as there are only " << m_OfflineTightElectrons_filter.size()
                      << " electrons in the offline collection");
	  hist("Execution_fail", "HLT/ZeeTagProbe/Debug")->Fill(0);
	// return StatusCode::SUCCESS; //yvonne: put in for clean events
      }


      bool trueZ = false;
      for( std::vector< const Analysis::Electron* >::const_iterator it = m_OfflineTightElectrons_filter.begin(); it != m_OfflineTightElectrons_filter.end(); ++it){
	for( std::vector< const Analysis::Electron* >::const_iterator it2 = m_OfflineTightElectrons_filter.begin(); it2 != m_OfflineTightElectrons_filter.end(); ++it2){
	  if (it == it2) continue;
	  double opposite_sign = (((*it)->charge() + (*it2)->charge()) == 0);
	  TLorentzVector Tag(0.,0.,0.,0.);
	  Tag.SetPtEtaPhiE((*it)->pt(),(*it)->eta(),(*it)->phi(),(*it)->e());
	  TLorentzVector Probe(0.,0.,0.,0.);
	  Probe.SetPtEtaPhiE((*it2)->pt(),(*it2)->eta(),(*it2)->phi(),(*it2)->e());
	  TLorentzVector ZBoson = Tag + Probe;
	  hist("h_Z_mass_offline", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(fabs(ZBoson.M()));
	  if ((fabs(fabs(ZBoson.M())-91000)<3000) && opposite_sign){
	    trueZ = true;
	  }
	}
      }
      if (!trueZ){
	  hist("Execution_fail", "HLT/ZeeTagProbe/Debug")->Fill(0);
	  // return StatusCode::SUCCESS; //yvonne: put in for clean events
      }

      ATH_MSG_DEBUG("Get Electron tracks");

      /// Get tracks from offline electrons for debugging
      for( std::vector< const Analysis::Electron* >::const_iterator it = m_OfflineTightElectrons_filter.begin(); it != m_OfflineTightElectrons_filter.end(); ++it){
	if ((*it)->trackParticle() /*&& !(*it)->conversion()*/) {
	  simpleTrack temp;
	  const Trk::TrackSummary* summary = (*it)->trackParticle()->trackSummary();
	  //	  HepVector perigeeParams = (*it)->trackParticle()->measuredPerigee()->parameters();
	  float pt  = (*it)->trackParticle()->measuredPerigee()->pT();
	  float eta = (*it)->trackParticle()->measuredPerigee()->eta();
	  float charge = (*it)->trackParticle()->charge();
	  float phi = (*it)->trackParticle()->measuredPerigee()->parameters()[Trk::phi0];
	  float z0  = (*it)->trackParticle()->measuredPerigee()->parameters()[Trk::z0];
	  float d0  = (*it)->trackParticle()->measuredPerigee()->parameters()[Trk::d0];
	  float nPixHits = summary->get(Trk::numberOfPixelHits);
	  float nSctHits = summary->get(Trk::numberOfSCTHits);
	  float nTrtHits = summary->get(Trk::numberOfTRTHits);
	  float etaR = -999;
	  float phiR = -999;
	  temp.SetPtEtaPhiM(pt,eta,phi,0.);
	  temp.z0 = z0;
	  temp.d0 = d0;
	  temp.nPixHits = nPixHits;
	  temp.nSctHits = nSctHits;
	  temp.nTrtHits = nTrtHits;
	  temp.etaR = etaR;
	  temp.phiR = phiR;
	  temp.charge = charge;

	  OFF_EGAMMA_Tracks.push_back(temp);
	}
      }
    } // end of if debug



     ///////////////////// Do EF Level /////////////////////////////////////


     if (comb_container_EF.size() > 0 && (TrigDec->isPassed(EFChainName)))  {

	    ATH_MSG_DEBUG("Get Rois EF");
      int counter_egamma = -1;
      int counter_track = -1;
      int counter_cluster = -1;

      for( Trig::FeatureContainer::combination_const_iterator c = comb_container_EF.begin(); c != comb_container_EF.end(); ++c){
	try{
	std::vector< const egamma* > m_TightElectrons_EF;
	//	std::vector< const egamma* > m_MediumElectrons_EF;
	//	std::vector< const egamma* > m_LooseElectrons_EF;
	std::vector< const egamma* > m_TightElectrons_EF_noCut;
	//	std::vector< const egamma* > m_MediumElectrons_EF_noCut;
	//	std::vector< const egamma* > m_LooseElectrons_EF_noCut;
	std::vector< const CaloCluster*> ClusterContainer_EF;

	std::vector< Trig::Feature<TrigRoiDescriptor> > vecRoi = c->get<TrigRoiDescriptor>("");
	std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiItr = vecRoi.begin();
	std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiLast = vecRoi.end();

	std::vector< const TrigRoiDescriptor* > tight_rois;
	std::vector< const TrigRoiDescriptor* > loose_rois;

	const TrigRoiDescriptor* roi_noCut = 0;
	const TrigRoiDescriptor* roi_tight = 0;

	//////////////// Find ROIs for Loose and Tight Electrons from the Trigger ///////////////////

	hist("ROIs_per_event","HLT/ZeeTagProbe/ROI_Variables")->Fill(vecRoi.size());
	for ( ; (roiItr != roiLast); ++roiItr) {

	  hist("h_roi_eta", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->eta());
	  hist("h_roi_phi", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->phi());

	  const HLT::TriggerElement* a = (*roiItr).te();
	  a = const_cast<HLT::TriggerElement*>(a);
	  std::string TEName = "";
	  TEName = (Trig::getTEName((*a))).c_str();


	   ATH_MSG_DEBUG("TEN Name = "<< TEName);
	  // Note that the Trigger Element name always appears to be L2_, there is no EF equivalent (which I suppose is due to the ROI's not being updated between L2 and EF)

	  if (TEName == "L2_e20_tightcl" || (TEName.find ("tight") != std::string::npos)) {  //TEName == "L2_e24vh_tight1cl"){ ){
	  // if (TEName == "EF_e20_tightcl" || TEName == "EF_e24vh_tight1cl" ||  TEName == "EF_e24vh_tight1id"){
	  hist("h_roi_eta_(tight)", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->eta());
	    hist("h_roi_phi_(tight)", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->phi());
	    roi_tight = (*roiItr).cptr();
	    tight_rois.push_back(roi_tight);
	  }
	  	  if (TEName == "L2_e15_NoCutcl"){
	  //  if (TEName == "EF_e15_NoCutcl" || TEName == "EF_e15_NoCut" || TEName == "EF_e15_NoCutid" ){
	    hist("h_roi_eta_(loose)", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->eta());
	    hist("h_roi_phi_(loose)", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->phi());
	    roi_noCut = (*roiItr).cptr();
	    loose_rois.push_back(roi_noCut);
	  }
	}

	hist("ROIs_per_event_(Tight)","HLT/ZeeTagProbe/ROI_Variables")->Fill(tight_rois.size());
	hist("ROIs_per_event_(Loose)","HLT/ZeeTagProbe/ROI_Variables")->Fill(loose_rois.size());

	//////////////// Find the tight and loose electrons ///////////////////

	counter_egamma = -1;
	counter_cluster = -1;

	const std::vector< Trig::Feature<CaloClusterContainer> > cluster_cont = c->get<CaloClusterContainer>("");
	if (cluster_cont.size() != 0){

	  for (std::vector< Trig::Feature<CaloClusterContainer> >::const_iterator contit = cluster_cont.begin(); contit != cluster_cont.end(); ++contit){
	    ++counter_cluster;
	    if( !(counter_cluster%2 == 0)){
		    ATH_MSG_DEBUG("There are " << ((*contit).cptr())->size() << " loose clusters in this event");
	      for ( DataVector<CaloCluster>::const_iterator clusItr = ((*contit).cptr())->begin(); clusItr!= ((*contit).cptr())->end(); ++clusItr){
		const CaloCluster* tempCalo = *clusItr;
		if(!do_excludeICelec || (do_excludeICelec && (TMath::Abs(tempCalo->eta())<1.37 || TMath::Abs(tempCalo->eta())>1.52) ))
		    {
			hist("h_electron_noCut_phi","HLT/ZeeTagProbe/Online_Selections")->Fill((*clusItr)->phi());
			hist("h_electron_noCut_pt","HLT/ZeeTagProbe/Online_Selections")->Fill((*clusItr)->pt());
			hist("h_electron_noCut_eta","HLT/ZeeTagProbe/Online_Selections")->Fill((*clusItr)->eta());
			hist("h_electron_noCut_et","HLT/ZeeTagProbe/Online_Selections")->Fill((*clusItr)->et());
			ClusterContainer_EF.push_back(tempCalo);
		    }
	      }
	    }
	  }
	}
	const std::vector< Trig::Feature<egammaContainer> > temp = c->get<egammaContainer>(""); // Loose electrons kept incase needed for debugging
	if (temp.size() != 0){
	  for (std::vector< Trig::Feature<egammaContainer> >::const_iterator contit = temp.begin(); contit != temp.end(); ++contit){
	    ++counter_egamma;
	    if (!(counter_egamma%2 == 0)){
		for (  DataVector<egamma>::const_iterator elecItr = ((*contit).cptr())->begin(); elecItr!= ((*contit).cptr())->end(); ++elecItr){
		    const egamma* tempEl = *elecItr;
		    if(!do_excludeICelec || (do_excludeICelec && (TMath::Abs(tempEl->eta())<1.37 || TMath::Abs(tempEl->eta())>1.52) ))
		    {
			if( (*elecItr)->isem(egammaPID::ElectronTight)==0  || (*elecItr)->isem(egammaPID::ElectronTightPP)==0)  //yvonne: now only use tight electrons!
			    m_TightElectrons_EF_noCut.push_back(tempEl);
		    }
		    //  if( (*elecItr)->isem(egammaPID::ElectronMediumPP)==0  || (*elecItr)->passID(egammaPID::ElectronIDMediumPP)==1){
		    //	m_MediumElectrons_EF_noCut.push_back(tempEl);
		    //  }
		    // if( (*elecItr)->isem(egammaPID::ElectronLoose)==0){
		    //  m_LooseElectrons_EF_noCut.push_back(tempEl);
		    // }
	      }
	    }
	    //if (counter_egamma == 0 || counter_egamma == 2 || counter_egamma == 4){
	    if (counter_egamma%2 == 0){
	      for (  DataVector<egamma>::const_iterator elecItr = ((*contit).cptr())->begin(); elecItr!= ((*contit).cptr())->end(); ++elecItr){
		const egamma* tempEl = *elecItr;
		if(!do_excludeICelec || (do_excludeICelec && (TMath::Abs(tempEl->eta())<1.37 || TMath::Abs(tempEl->eta())>1.52) ))	// if( (*elecItr)->isem(egammaPID::ElectronTight)==0)
		{
		    //		    if( (*elecItr)->isem(egammaPID::ElectronTight)==0  || (*elecItr)->isem(egammaPID::ElectronTightPP)==0 )  //yvonne: now only use tight electrons!
		  m_TightElectrons_EF.push_back(tempEl);
		}
		//	if( (*elecItr)->isem(egammaPID::ElectronMedium)==0){
		//	  m_MediumElectrons_EF.push_back(tempEl);
		//	}
		hist("h_electron_tight_phi", "HLT/ZeeTagProbe/Online_Selections")->Fill((*elecItr)->phi());
		hist("h_electron_tight_pt", "HLT/ZeeTagProbe/Online_Selections")->Fill((*elecItr)->pt());
		hist("h_electron_tight_eta", "HLT/ZeeTagProbe/Online_Selections")->Fill((*elecItr)->eta());
		hist("h_electron_tight_et", "HLT/ZeeTagProbe/Online_Selections")->Fill((*elecItr)->et());
	      }
	    }
	  }
	}

	/// Get tag electron
	const egamma* m_tagElectron = 0;
	const CaloCluster* probeCluster_EF = 0;

	//////////////// Get the Tag and Probes ///////////////////

	if (m_TightElectrons_EF.size() != 0) {
	  for( std::vector< const egamma* >::const_iterator it3 =   m_TightElectrons_EF.begin(); it3 !=  m_TightElectrons_EF.end(); ++it3){
	    for( std::vector< const CaloCluster* >::const_iterator it4 = ClusterContainer_EF.begin(); it4 != ClusterContainer_EF.end(); ++it4) {
	      TLorentzVector Tag(0.,0.,0.,0.);
	      Tag.SetPtEtaPhiE((*it3)->pt(),(*it3)->eta(),(*it3)->phi(),(*it3)->e());
	      TLorentzVector Probe(0.,0.,0.,0.);
	      Probe.SetPtEtaPhiE((*it4)->et(),(*it4)->eta(),(*it4)->phi(),(*it4)->e());
	      TLorentzVector ZBoson = Tag + Probe;
	      hist("h_Z_mass_electrons_clusters", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(fabs(ZBoson.M()));

	      double delPhi = dPhiHelper((*it3)->phi(), (*it4)->phi());
	      hist("h_delPhi_cluster_electron", "HLT/ZeeTagProbe/Online_Selections")->Fill(delPhi);


	      if ((fabs(fabs(ZBoson.M())-91000) < 3000) && dPhiHelper((*it4)->phi(), (*it3)->phi()) > (TMath::Pi() - 0.3)  ) { // Since the pointers aren't pointing to the same object, have to do some deltaPhi check to make sure that the tag and probe are not the same. After tests this turns out to almost never be true.
		  m_tagElectron = (*it3);
		  probeCluster_EF = (*it4);
		  hist("h_Z_mass_electrons_clusters_cut", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(fabs(ZBoson.M()));
		  hist("h_tag_pT", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->pt());
		  hist("h_tag_Eta", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->eta());
		  hist("h_tag_Phi", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->phi());
		  hist("h_tag_charge", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->charge());
		  hist("h_tag_Et", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->et());
		  hist("h_delPhi_tag_probe", "HLT/ZeeTagProbe/Online_Selections")->Fill(delPhi);

		  hist("h_probe_pT_cluster", "HLT/ZeeTagProbe/Online_Selections")->Fill(probeCluster_EF->pt());
		  hist("h_probe_Eta_cluster", "HLT/ZeeTagProbe/Online_Selections")->Fill(probeCluster_EF->eta());
		  hist("h_probe_Phi_cluster", "HLT/ZeeTagProbe/Online_Selections")->Fill(probeCluster_EF->phi());
		  hist("h_probe_Et_cluster", "HLT/ZeeTagProbe/Online_Selections")->Fill(probeCluster_EF->et());

		  //		std::cout << "JWH    TAG ELECTRON" << std::endl;
		  //		std::cout << "JWH       (Tight) - pT = " << m_tagElectron->pt() << ", eta = " << m_tagElectron->eta() << ", phi = " << m_tagElectron->phi() << ", charge = " << m_tagElectron->charge() << std::endl; // JWH
		  //		std::cout << "JWH    PROBE CLUSTER" << std::endl;
		  //		std::cout << "JWH       (Loose) - eT = " << probeCluster_EF->et() << ", eta = " << probeCluster_EF->eta() << ", phi = " << probeCluster_EF->phi() << std::endl; // JWH
	      }
	    }
	  }
	} // end of probe electron selection



	ATH_MSG_DEBUG("Check tags and probes have offline match");

	//////////////// Offline comparions (DEBUGGING ONLY!) ///////////////////

	if (m_tagElectron == 0){
	    ATH_MSG_DEBUG("Tag is empty, killing event");
	  throw 1;
	  hist("Execution_fail", "HLT/ZeeTagProbe/Debug")->Fill(1);
	}
	if (probeCluster_EF == 0 ){
	    ATH_MSG_DEBUG("Electron and Cluster probes = 0, killing event");
	  throw 2;
	  hist("Execution_fail", "HLT/ZeeTagProbe/Debug")->Fill(2);
	}

	TLorentzVector tag(0.,0.,0.,0.);
	tag.SetPtEtaPhiM(m_tagElectron->pt(),m_tagElectron->eta(),m_tagElectron->phi(),m_tagElectron->e());
	TLorentzVector probe(0.,0.,0.,0.);
	probe.SetPtEtaPhiM(probeCluster_EF->et(),probeCluster_EF->eta(),probeCluster_EF->phi(),probeCluster_EF->e());

	bool tag_match = false;
	bool probe_match = false;
	const Analysis::Electron* offline_probe = 0;


	if (m_OfflineTightElectrons_filter.size() >0){
	  for( std::vector< const Analysis::Electron* >::const_iterator it = m_OfflineTightElectrons_filter.begin(); it != m_OfflineTightElectrons_filter.end(); ++it){
	    TLorentzVector offline(0.,0.,0.,0.);
	    offline.SetPtEtaPhiE((*it)->pt(),(*it)->eta(),(*it)->phi(),(*it)->e());
	    if (offline.DeltaR(tag) < 0.05){
	      tag_match=true;
	    }
	    if (offline.DeltaR(probe) < 0.05) {
	      probe_match=true;
	      offline_probe=(*it);
	    }
	  }
	  if (!tag_match){
	      //  throw 1; //yvonne" turn on for clean events
	  }
	  if (!probe_match){
	      //   throw 2; // yvonne: turn on for clean events
	  }
	}


	/// Get the correct track for the probe
	simpleTrack probeTrack;
	if (probe_match){
	    if(offline_probe)
                {
                    if(offline_probe->trackParticle())
                        {
                            if( offline_probe->trackParticle()->measuredPerigee())
                                {
				  //				    HepVector perigeeParams = offline_probe->trackParticle()->measuredPerigee()->parameters();
				    float pt  = offline_probe->trackParticle()->measuredPerigee()->pT();
				    float eta = offline_probe->trackParticle()->measuredPerigee()->eta();
				    float phi = offline_probe->trackParticle()->measuredPerigee()->parameters()[Trk::phi0];
				    probeTrack.SetPtEtaPhiM(pt, eta, phi, 0);
				}
			    else ATH_MSG_WARNING("Offine probe match track has no perigee");
                        }
                    else  ATH_MSG_WARNING("Offine probe match track has no trackparticle");
		}
	}
	//	else probeTrack.SetPtEtaPhiM(0, 100.0, 0.0, 0);

	//////////////// Event Level Selections for EF ///////////////////


	//////////////// Get the online tracks and do efficiency plots ///////////////////

	/// Get online track collections

	std::string EFCollection = "InDetTrigParticleCreation_Electron_EFID";

	std::vector< simpleTrack > alltracks_tag;
	std::vector< simpleTrack > allTracks;
	std::vector< Trig::Feature<Rec::TrackParticleContainer> > feature_EF;
	feature_EF = c->get<Rec::TrackParticleContainer>("InDetTrigParticleCreation_Electron_EFID");

	counter_track=-1;
	if (feature_EF.size() != 0){
	  for (std::vector< Trig::Feature<Rec::TrackParticleContainer> >::const_iterator contit = feature_EF.begin(); contit != feature_EF.end(); ++contit){
	    ++counter_track;
	    if(roi_tight)    alltracks_tag =  GetTrigInDetTrackVectors((*contit), false, alltracks_tag, roi_tight);
	      allTracks = GetTrigInDetTrackVectors((*contit), false, allTracks, roi_noCut);
	  }
	}



	double deltaz_tag_min=1000.0;
	if(alltracks_tag.size()>1 && do_debugging)
	    {

		for(unsigned int tt=1; tt<alltracks_tag.size();++tt)
		    {
			double delta_z0 = alltracks_tag[0].z0 - alltracks_tag[tt].z0;
			if(TMath::Abs(delta_z0)<TMath::Abs(deltaz_tag_min)) deltaz_tag_min = delta_z0;
		    }

		if(TMath::Abs(deltaz_tag_min)<2.0)
		    ATH_MSG_DEBUG("More than one track in the tag - throw this event away ");
		throw 1;
	    }

	TLorentzVector Tagh(0.,0.,0.,0.);
	TLorentzVector Probeh(0.,0.,0.,0.);
	Tagh.SetPtEtaPhiE(m_tagElectron->pt(),m_tagElectron->eta(),m_tagElectron->phi(),m_tagElectron->e());
	Probeh.SetPtEtaPhiE(probeCluster_EF->et(),probeCluster_EF->eta(),probeCluster_EF->phi(),probeCluster_EF->e());

	hist("h_Z_mass_electrons_clusters_allcut", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(fabs((Tagh+Probeh).M()));

	bool event_efficient = false;
      	if (probeCluster_EF != 0 && m_tagElectron !=0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_EF_", m_tagElectron, probeCluster_EF, roi_noCut, allTracks, probeTrack, alltracks_tag.size());

	if (!event_efficient){

	  TLorentzVector Tag(0.,0.,0.,0.);
	  TLorentzVector Probe(0.,0.,0.,0.);
	  Tag.SetPtEtaPhiE(m_tagElectron->pt(),m_tagElectron->eta(),m_tagElectron->phi(),m_tagElectron->e());
	  Probe.SetPtEtaPhiE(probeCluster_EF->et(),probeCluster_EF->eta(),probeCluster_EF->phi(),probeCluster_EF->e());

	  hist("h_tag_Pt_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Tag.Pt());
	  hist("h_tag_Eta_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Tag.Eta());
	  hist("h_tag_Phi_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Tag.Phi());
	  hist("h_tag_Et_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Tag.Et());

	  hist("h_probe_Pt_cluster_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Probe.Pt());
	  hist("h_probe_Eta_cluster_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Probe.Eta());
	  hist("h_probe_Phi_cluster_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Probe.Phi());
	  hist("h_probe_Et_cluster_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Probe.Et());

	  TLorentzVector ZBoson = Tag + Probe;

	  hist("h_Z_mass_electrons_clusters_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(fabs(ZBoson.M()));

	  /*
	  //write out some info about the inefficienct events
	  ATH_MSG_INFO(" This was an inefficient EF event - now we write out some info about it");
	  ATH_MSG_INFO(" number of tracks associated to ROI is " << allTracks.size());
	  ATH_MSG_INFO(" the tag electron pt eta phi et is " << Tag.Pt() << " " << Tag.Eta()
                 << " " << Tag.Phi() << " " << Tag.Et());
	  ATH_MSG_INFO(" the probe cluster pt eta phi et is " << Probe.Pt() << " " << Probe.Eta()
                 << " " << Probe.Phi() << " " << Probe.Et());


	  const DataHandle<EventInfo> eventInfo;
	  StatusCode sc3 = evtStore()->retrieve(eventInfo);
	  if(sc3.isFailure()) std::cout << "Failed to get eventInfo" << std::endl;
	  unsigned int eventNumber = (eventInfo->event_ID())->event_number();
	  const EventInfo* pEventInfo;
	  int run_number        = 0;
	  unsigned event_number      = 0;

	  if (evtStore()->retrieve(pEventInfo).isFailure() ) {
	  } else {
	      run_number        = pEventInfo->event_ID()->run_number();
	      event_number      = pEventInfo->event_ID()->event_number();
	  }

	  ATH_MSG_INFO(" run and event number of this event: " << run_number << " " << event_number);


	   //yvonne: test to check the inefficient events
	  sc_offlinetracks = m_storeGate->retrieve(trackCollectionOFF,m_offlineCollection);
	  if (sc_offlinetracks.isFailure()) {
	      ATH_MSG_INFO(" Failed to retrieve offline tracks!");
	  }

	  int counter = 0;

	  Rec::TrackParticleContainer::const_iterator trackParticle = trackCollectionOFF->begin();
	  Rec::TrackParticleContainer::const_iterator lastTrkPart   = trackCollectionOFF->end();


	   for (; trackParticle != lastTrkPart; ++trackParticle) {

	      HepVector perigeeParams = (*trackParticle)->measuredPerigee()->parameters();
	      ATH_MSG_INFO(" offline track " << counter << " pt eta phi: "
                     << (*trackParticle)->measuredPerigee()->pT() << " "
                     << (*trackParticle)->measuredPerigee()->eta() << " "
                     << perigeeParams[Trk::phi0]);
	      counter++;
	   }

	   counter=0;
	  for (std::vector< Trig::Feature<Rec::TrackParticleContainer> >::const_iterator contit = feature_EF.begin(); contit != feature_EF.end(); ++contit){
	      const Rec::TrackParticleContainer* trigInDetTrkCont = (*contit).cptr();



	      Rec::TrackParticleContainer::const_iterator track     = trigInDetTrkCont->begin();
	      Rec::TrackParticleContainer::const_iterator lastTrack = trigInDetTrkCont->end();



	      for (; track != lastTrack; ++track) {

	      const Trk::TrackSummary* summary = (*track)->trackSummary();
	      const Trk::MeasuredPerigee* measPer = (*track)->measuredPerigee();
	      const Trk::ParametersBase* perTrkPar = 0;
	      perTrkPar = dynamic_cast<const Trk::MeasuredPerigee*>(&((*track)->definingParameters()));
	      if (summary == 0 || measPer==0) {
	      ATH_MSG_INFO("No measured perigee or track summary parameters assigned to the track");
	      continue;
	      }
	      const Trk::ParametersBase *result=0;
	      result  = tracktocalo->extrapolate(perTrkPar, CaloCell_ID::EMB2, 0.0, Trk::photon,   Trk::alongMomentum);

	      HepVector perigeeParams = (*track)->measuredPerigee()->parameters();

	      float Eta_extrapol =  (*track)->measuredPerigee()->eta();
	      float Phi_extrapol =  perigeeParams[Trk::phi0];

	      if(result)
	      {
              Eta_extrapol = result->position().eta() ;
              Phi_extrapol = result->position().phi() ;
	      }


		    ATH_MSG_INFO(" trigger track " << counter << " pt eta phi eta_extrapol phi_extrapol: "
                     << (*track)->measuredPerigee()->pT() << " "
                     << (*track)->measuredPerigee()->eta() << " "
                     << perigeeParams[Trk::phi0] << " " << Eta_extrapol
                     << " " << Phi_extrapol);
		  counter++;
	      }
	  }

	  */
	}

	/**************************************************************************************/
	/************        L2 track for the EF clusters and features             ************/
	/**************************************************************************************/

	bool doIDS = 1;

	std::string IDSCollection = "TrigIDSCAN_eGamma";
	allTracks.clear();
	alltracks_tag.clear();
	std::vector< simpleTrack > allTracks_A; // Deals with L2Star
	std::vector< simpleTrack > allTracks_B;
	std::vector< simpleTrack > allTracks_C;

	std::vector< simpleTrack > alltracks_tag_A; // Deals with L2Star
        std::vector< simpleTrack > alltracks_tag_B;
        std::vector< simpleTrack > alltracks_tag_C;


	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature_L2_IDS;

	feature_L2_IDS = c->get<TrigInDetTrackCollection>("TrigIDSCAN_eGamma");

	counter_track=-1;
	if (feature_L2_IDS.size() != 0){
	  for (std::vector< Trig::Feature<TrigInDetTrackCollection> >::const_iterator contit = feature_L2_IDS.begin(); contit != feature_L2_IDS.end(); ++contit){
	    ++counter_track;

	    if(roi_tight)	    alltracks_tag =  GetTrigInDetTrackVectors((*contit), false, alltracks_tag, roi_tight);
	    allTracks = GetTrigInDetTrackVectors((*contit), false, allTracks, roi_noCut);
	      //}
	  }
	}
	event_efficient = false;
	if (doIDS && probeCluster_EF != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_IDS_", m_tagElectron, probeCluster_EF, roi_noCut, allTracks, probeTrack, alltracks_tag.size());


	/// Get online track collections

	bool doSIT = 1;

	std::string SITCollection = "TrigSiTrack_eGamma";
	allTracks.clear();
	alltracks_tag.clear();
	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature_L2_SIT;

	feature_L2_SIT = c->get<TrigInDetTrackCollection>("TrigSiTrack_eGamma");

	counter_track=-1;
	if (feature_L2_SIT.size() != 0){
	  for (std::vector< Trig::Feature<TrigInDetTrackCollection> >::const_iterator contit = feature_L2_SIT.begin(); contit != feature_L2_SIT.end(); ++contit){
	    ++counter_track;
	    //if (counter_track == 0 || counter_track == 2 || counter_track == 4){
	    if(roi_tight)  alltracks_tag =  GetTrigInDetTrackVectors((*contit), false, alltracks_tag, roi_tight);
	    allTracks = GetTrigInDetTrackVectors((*contit), false, allTracks, roi_noCut);
	      //}
	  }
	}
	event_efficient = false;
	if (doSIT && probeCluster_EF != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_SIT_", m_tagElectron, probeCluster_EF, roi_noCut, allTracks, probeTrack, alltracks_tag.size());






    //////////dol2star - here need three histos - for the different a b c tracks! ///////
	/// Get online track collections

	bool doL2Star = 1;

	std::string L2StarCollection = "TrigSiTrackFinder";
	allTracks.clear();
	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature_L2_L2Star;

	feature_L2_L2Star = c->get<TrigInDetTrackCollection>("TrigL2SiTrackFinder_eGamma", TrigDefs::alsoDeactivateTEs);

	counter_track=-1;
	if (feature_L2_L2Star.size() != 0){
	  for (std::vector< Trig::Feature<TrigInDetTrackCollection> >::const_iterator contit = feature_L2_L2Star.begin(); contit != feature_L2_L2Star.end(); ++contit){
	    ++counter_track;
	    //if (counter_track == 0 || counter_track == 2 || counter_track == 4){

	    if(roi_tight)  alltracks_tag_A = GetTrigInDetTrackVectors((*contit), false, roi_tight, 0);
            if(roi_tight) alltracks_tag_B = GetTrigInDetTrackVectors((*contit), false, roi_tight, 1);
            if(roi_tight) alltracks_tag_C = GetTrigInDetTrackVectors((*contit), false, roi_tight, 2);

	    allTracks_A = GetTrigInDetTrackVectors((*contit), false, roi_noCut, 0);
	    allTracks_B = GetTrigInDetTrackVectors((*contit), false, roi_noCut, 1);
	    allTracks_C = GetTrigInDetTrackVectors((*contit), false, roi_noCut, 2);
	      //}
	  }
	}
	event_efficient = false;
	if (doL2Star && probeCluster_EF != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_L2StarA_", m_tagElectron, probeCluster_EF, roi_noCut, allTracks_A, probeTrack, alltracks_tag_A.size());
	if (doL2Star && probeCluster_EF != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_L2StarB_", m_tagElectron, probeCluster_EF, roi_noCut, allTracks_B, probeTrack, alltracks_tag_B.size());
	if (doL2Star && probeCluster_EF != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_L2StarC_", m_tagElectron, probeCluster_EF, roi_noCut, allTracks_C, probeTrack, alltracks_tag_C.size());




      }
	catch (int e) {
	    if (e == 1) ATH_MSG_DEBUG("JWH     Exception occured, EF tag is empty! ");
	    if (e == 2) ATH_MSG_DEBUG("JWH     Exception occured, EF probe is empty! ");
	    if (e == 3) ATH_MSG_DEBUG("JWH     Exception occured, EF Probe bremmed");
	}
      }
  }


    ////////////////////// Now do L2 ///////////////////////////////////

     //          ATH_MSG_INFO("yvonne: test 5");

     if (comb_container_L2.size() > 0 && (TrigDec->isPassed(EFChainName))){ // (TrigDec->isPassed(L2ChainName))){ // Changed to EF, was L2

      ATH_MSG_DEBUG("Get Rois L2");
      int counter_egamma = -1;
      int counter_track = -1;
      int counter_cluster = -1;

      for( Trig::FeatureContainer::combination_const_iterator c = comb_container_L2.begin(); c != comb_container_L2.end(); ++c){
	try {
	std::vector< const TrigElectron* > m_TightElectrons_L2;
	//	std::vector< const TrigElectron* > m_MediumElectrons_L2;
	//	std::vector< const TrigElectron* > m_LooseElectrons_L2;
	std::vector< const TrigElectron* > m_TightElectrons_L2_noCut;
	//	std::vector< const TrigElectron* > m_MediumElectrons_L2_noCut;
	//	std::vector< const TrigElectron* > m_LooseElectrons_L2_noCut;
	std::vector< const TrigEMCluster*> ClusterContainer_L2;

	std::vector< Trig::Feature<TrigRoiDescriptor> > vecRoi = c->get<TrigRoiDescriptor>("");
	std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiItr = vecRoi.begin();
	std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiLast = vecRoi.end();

	std::vector< const TrigRoiDescriptor* > tight_rois;
	std::vector< const TrigRoiDescriptor* > loose_rois;

	const TrigRoiDescriptor* roi_noCut = 0;
	const TrigRoiDescriptor* roi_tight = 0;

	//////////////// Find ROIs for Loose and Tight Electrons from the Trigger ///////////////////

	hist("L2_ROIs_per_event","HLT/ZeeTagProbe/ROI_Variables")->Fill(vecRoi.size());

	for ( ; (roiItr != roiLast); ++roiItr) {

	  hist("L2_h_roi_eta", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->eta());
	  hist("L2_h_roi_phi", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->phi());

	  const HLT::TriggerElement* a = (*roiItr).te();
	  a = const_cast<HLT::TriggerElement*>(a);
	  std::string TEName = "";
	  TEName =  (Trig::getTEName((*a))).c_str();


 	  ATH_MSG_DEBUG("TEN Name = "<< TEName);

	  if (TEName == "L2_e20_tightcl" || (TEName.find ("tight") != std::string::npos)) {  //TEName == "L2_e24vh_tight1cl"){
	    hist("L2_h_roi_eta_(tight)", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->eta());
	    hist("L2_h_roi_phi_(tight)", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->phi());
	    roi_tight = (*roiItr).cptr();
	    tight_rois.push_back(roi_tight);
	  }
	  else
	      {
		  // ATH_MSG_DEBUG("tight roi not found - check hard coded name " << TEName.c_str());
		  //		  std::cout << "yvonne: tename is " << TEName.c_str()<< std::endl;
	      }

	  if (TEName == "L2_e15_NoCutcl"){ // Will take the first probe it finds that isnt the tag. Wont work for multiple tags yet.
	    hist("L2_h_roi_eta_(loose)", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->eta());
	    hist("L2_h_roi_phi_(loose)", "HLT/ZeeTagProbe/ROI_Variables")->Fill(((*roiItr).cptr())->phi());
	    roi_noCut = (*roiItr).cptr();
	    loose_rois.push_back(roi_noCut);
	  }
	}
	hist("L2_ROIs_per_event_(Tight)","HLT/ZeeTagProbe/ROI_Variables")->Fill(tight_rois.size());
	hist("L2_ROIs_per_event_(Loose)","HLT/ZeeTagProbe/ROI_Variables")->Fill(loose_rois.size());


	//////////////// Find the tight and loose electrons ///////////////////


	counter_egamma = -1;
	counter_cluster = -1;

  nvtx = 0;
	const VxContainer* vxcoll;
	StatusCode scVtx = evtStore()->retrieve(vxcoll, "VxPrimaryCandidate");
  if(scVtx.isFailure())  std::cout << "Cannot find primary vertex container!!!" << std::endl;
  else  nvtx = vxcoll->size();

	const std::vector< Trig::Feature<TrigEMCluster> > cluster_cont = c->get<TrigEMCluster>("");
	if (cluster_cont.size() != 0){
	  for (std::vector< Trig::Feature<TrigEMCluster> >::const_iterator contit = cluster_cont.begin(); contit != cluster_cont.end(); ++contit){
	    ++counter_cluster;

	      const TrigEMCluster* tempCalo = ((*contit).cptr());
	      if(!do_excludeICelec || (do_excludeICelec && (TMath::Abs(tempCalo->eta())<1.37 || TMath::Abs(tempCalo->eta())>1.52) ))
		  {
		      hist("L2_h_electron_noCut_phi","HLT/ZeeTagProbe/Online_Selections")->Fill(tempCalo->phi());
		      hist("L2_h_electron_noCut_eta","HLT/ZeeTagProbe/Online_Selections")->Fill(tempCalo->eta());
		      hist("L2_h_electron_noCut_et","HLT/ZeeTagProbe/Online_Selections")->Fill(tempCalo->et());
		      hist("L2_h_electron_noCut_e","HLT/ZeeTagProbe/Online_Selections")->Fill(tempCalo->e());
		      ClusterContainer_L2.push_back(tempCalo);
		  }
	  }
	}
	const std::vector< Trig::Feature<TrigElectronContainer> > temp = c->get<TrigElectronContainer>("L2ElectronFex");
	if (temp.size() != 0){
	  for (std::vector< Trig::Feature<TrigElectronContainer> >::const_iterator contit = temp.begin(); contit != temp.end(); ++contit){
	    ++counter_egamma;
	    if (!(counter_egamma%2 == 0)){
	      for (  DataVector<TrigElectron>::const_iterator elecItr = ((*contit).cptr())->begin(); elecItr!= ((*contit).cptr())->end(); ++elecItr){
		const TrigElectron* tempEl = *elecItr;
		if(!do_excludeICelec || (do_excludeICelec && (TMath::Abs(tempEl->eta())<1.37 || TMath::Abs(tempEl->eta())>1.52) ))
		    {
			m_TightElectrons_L2_noCut.push_back(tempEl);
			//			m_MediumElectrons_L2_noCut.push_back(tempEl);
			//			m_LooseElectrons_L2_noCut.push_back(tempEl);
		    }
	      }
	    }
	    if(counter_egamma%2==0){
	      for (  DataVector<TrigElectron>::const_iterator elecItr = ((*contit).cptr())->begin(); elecItr!= ((*contit).cptr())->end(); ++elecItr){
		const TrigElectron* tempEl = *elecItr;
		if(!do_excludeICelec || (do_excludeICelec && (TMath::Abs(tempEl->eta())<1.37 || TMath::Abs(tempEl->eta())>1.52) ))
		    {
			m_TightElectrons_L2.push_back(tempEl);
			//			m_MediumElectrons_L2.push_back(tempEl);
			//			m_LooseElectrons_L2.push_back(tempEl);
			hist("L2_h_electron_tight_phi", "HLT/ZeeTagProbe/Online_Selections")->Fill((*elecItr)->phi());
			hist("L2_h_electron_tight_pt", "HLT/ZeeTagProbe/Online_Selections")->Fill((*elecItr)->pt());
			hist("L2_h_electron_tight_eta", "HLT/ZeeTagProbe/Online_Selections")->Fill((*elecItr)->eta());
			hist("L2_h_electron_tight_et", "HLT/ZeeTagProbe/Online_Selections")->Fill((*elecItr)->et());
		    }
	      }
	    }
	  }
	}

	/// Get tag electron
	const TrigElectron* m_tagElectron = 0;
	const TrigEMCluster* probeCluster_L2 = 0;

	//////////////// Get the Tag and Probes ///////////////////
	//     ATH_MSG_INFO("yvonne: test 6");

	if (m_TightElectrons_L2.size() != 0) {
	  for( std::vector< const TrigElectron* >::const_iterator it3 =   m_TightElectrons_L2.begin(); it3 !=  m_TightElectrons_L2.end(); ++it3){
	    for( std::vector< const TrigEMCluster* >::const_iterator it4 = ClusterContainer_L2.begin(); it4 != ClusterContainer_L2.end(); ++it4) {
	      TLorentzVector Tag(0.,0.,0.,0.);
	      //Tag.SetPtEtaPhiE((*it3)->pt(),(*it3)->eta(),(*it3)->phi(),(*it3)->e());
	      Tag.SetPtEtaPhiE((*it3)->pt(),(*it3)->eta(),(*it3)->phi(),(*it3)->e());
	      TLorentzVector Probe(0.,0.,0.,0.);
	      hist("L2_h_probe_pT_cluster","HLT/ZeeTagProbe/Online_Selections")->Fill(((*it4)->e()/TMath::CosH((*it4)->eta())));
	      //Probe.SetPtEtaPhiE(((*it4)->e()/TMath::CosH((*it4)->eta())),(*it4)->eta(),(*it4)->phi(),(*it4)->e());
	      Probe.SetPtEtaPhiE((*it4)->et(),(*it4)->eta(),(*it4)->phi(),(*it4)->e());
	      TLorentzVector ZBoson = Tag + Probe;
	      double delPhi = dPhiHelper((*it3)->phi(), (*it4)->phi());
	      hist("L2_h_Z_mass_electrons_clusters", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(fabs(ZBoson.M()));
	      hist("L2_h_delPhi_cluster_electron", "HLT/ZeeTagProbe/Online_Selections")->Fill(delPhi);



	      if ((fabs(fabs(ZBoson.M())-91000) < 3000) && dPhiHelper((*it4)->phi(), (*it3)->phi()) > (TMath::Pi() -0.3) /*&& !(fabs((*it4)->phi()-(*it3)->phi())<0.01)*/) {
		m_tagElectron = (*it3);
		probeCluster_L2 = (*it4);
		hist("L2_h_Z_mass_electrons_clusters_cut", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(fabs(ZBoson.M()));
		hist("L2_h_tag_pT", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->pt());
		hist("L2_h_tag_Eta", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->eta());
		hist("L2_h_tag_Phi", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->phi());
		hist("L2_h_tag_charge", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->charge());
		hist("L2_h_tag_Et", "HLT/ZeeTagProbe/Online_Selections")->Fill(m_tagElectron->et());
		hist("L2_h_delPhi_tag_probe", "HLT/ZeeTagProbe/Online_Selections")->Fill(delPhi);


		hist("L2_h_probe_Eta_cluster", "HLT/ZeeTagProbe/Online_Selections")->Fill(probeCluster_L2->eta());
		hist("L2_h_probe_Phi_cluster", "HLT/ZeeTagProbe/Online_Selections")->Fill(probeCluster_L2->phi());
		hist("L2_h_probe_Et_cluster", "HLT/ZeeTagProbe/Online_Selections")->Fill(probeCluster_L2->et());

		//		std::cout << "JWH    TAG ELECTRON" << std::endl;
		//		std::cout << "JWH       (Tight) - pT = " << m_tagElectron->pt() << ", eta = " << m_tagElectron->eta() << ", phi = " << m_tagElectron->phi() << ", charge = " << m_tagElectron->charge() << std::endl; // JWH
		//		std::cout << "JWH    PROBE CLUSTER" << std::endl;
		//		std::cout << "JWH       (Loose) - eT = " << probeCluster_L2->et() << ", eta = " << probeCluster_L2->eta() << ", phi = " << probeCluster_L2->phi() << std::endl; // JWH
	      }
	    }
	  }
	} // end of probe electron selection


	//	      ATH_MSG_INFO("yvonne: test 7");

	ATH_MSG_DEBUG("Check tags and probes have offline match");

	//////////////// Offline comparions (DEBUGGING ONLY!) ///////////////////

	if (m_tagElectron == 0){
	  hist("Execution_fail", "HLT/ZeeTagProbe/Debug")->Fill(1);
	    throw 1;
	  //return StatusCode::SUCCESS;
	}
	if (probeCluster_L2 == 0 ){
	  hist("Execution_fail", "HLT/ZeeTagProbe/Debug")->Fill(2);
	    throw 2;
	  //return StatusCode::SUCCESS;
	}

	TLorentzVector tag(0.,0.,0.,0.);
	tag.SetPtEtaPhiM(m_tagElectron->pt(),m_tagElectron->eta(),m_tagElectron->phi(),m_tagElectron->e());
	TLorentzVector probe(0.,0.,0.,0.);
	probe.SetPtEtaPhiM(probeCluster_L2->et(),probeCluster_L2->eta(),probeCluster_L2->phi(),probeCluster_L2->e()); // Should this be e?

	bool tag_match = false;
	bool probe_match = false;
	const Analysis::Electron* offline_probe = 0;

	//      ATH_MSG_INFO("yvonne: test 8");

	if (m_OfflineTightElectrons_filter.size() >0){
	  for( std::vector< const Analysis::Electron* >::const_iterator it = m_OfflineTightElectrons_filter.begin(); it != m_OfflineTightElectrons_filter.end(); ++it){
	    TLorentzVector offline(0.,0.,0.,0.);
	    offline.SetPtEtaPhiE((*it)->pt(),(*it)->eta(),(*it)->phi(),(*it)->e());

	    if (offline.DeltaR(tag) < 0.05){
	      tag_match=true;
	    }
	    if (offline.DeltaR(probe) < 0.05) {
	      probe_match=true;
	      offline_probe=(*it);
	      }
	  }

	  if (!tag_match){
	      ATH_MSG_DEBUG("Failed to match Tag!");
	    //   throw 1; // yvonne: turn on for clean events
	  }
	  if (!probe_match){
	      ATH_MSG_DEBUG("Failed to match Probe!");
	    //    throw 2; // yvonne: turn on for clean events
	  }


	}

	/// Get the correct track for the probe
	simpleTrack probeTrack;
	if (probe_match){
	    if(offline_probe)
		{
		    if(offline_probe->trackParticle())
			{
			    if( offline_probe->trackParticle()->measuredPerigee())
				{
				  // HepVector perigeeParams = offline_probe->trackParticle()->measuredPerigee()->parameters();
				    float pt  = offline_probe->trackParticle()->measuredPerigee()->pT();
				    float eta = offline_probe->trackParticle()->measuredPerigee()->eta();
				    float phi = offline_probe->trackParticle()->measuredPerigee()->parameters()[Trk::phi0];
				    probeTrack.SetPtEtaPhiM(pt, eta, phi, 0);
				    probeTrack.charge = offline_probe->trackParticle()->charge();
				}
			    else ATH_MSG_WARNING("Offine probe match track has no perigee");
			}
		    else ATH_MSG_WARNING("Offine probe match track has no trackparticle");
		}
	}
	//	else probeTrack.SetPtEtaPhiM(0.0, 100.0,0.0, 0);


	//////////////// Get the online tracks and do efficiency plots ///////////////////

	//      ATH_MSG_INFO("yvonne: test 9");
	/// Get online track collections

	bool doIDS = 1;

	std::string IDSCollection = "TrigIDSCAN_eGamma";
	std::vector< simpleTrack > allTracks;
	std::vector< simpleTrack > alltracks_tag;
	std::vector< simpleTrack > allTracks_A; // Deals with L2Star
	std::vector< simpleTrack > allTracks_B;
	std::vector< simpleTrack > allTracks_C;

	std::vector< simpleTrack > alltracks_tag_A; // Deals with L2Star
        std::vector< simpleTrack > alltracks_tag_B;
        std::vector< simpleTrack > alltracks_tag_C;


	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature_L2_IDS;

	feature_L2_IDS = c->get<TrigInDetTrackCollection>("TrigIDSCAN_eGamma");

	counter_track=-1;
	if (feature_L2_IDS.size() != 0){
	  for (std::vector< Trig::Feature<TrigInDetTrackCollection> >::const_iterator contit = feature_L2_IDS.begin(); contit != feature_L2_IDS.end(); ++contit){
	    ++counter_track;

	    if(roi_tight)	    alltracks_tag =  GetTrigInDetTrackVectors((*contit), false, alltracks_tag, roi_tight);
	    allTracks = GetTrigInDetTrackVectors((*contit), false, allTracks, roi_noCut);
	      //}
	  }
	}
	bool event_efficient = false;
	if (doIDS && probeCluster_L2 != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_IDS_L2only_", m_tagElectron, probeCluster_L2, roi_noCut, allTracks, probeTrack, alltracks_tag.size());

	if (!event_efficient){

	  TLorentzVector Tag(0.,0.,0.,0.);
	  TLorentzVector Probe(0.,0.,0.,0.);
	  Tag.SetPtEtaPhiE(m_tagElectron->pt(),m_tagElectron->eta(),m_tagElectron->phi(),m_tagElectron->e());
	  Probe.SetPtEtaPhiE(probeCluster_L2->et(),probeCluster_L2->eta(),probeCluster_L2->phi(),probeCluster_L2->e());

	  hist("L2_h_tag_Pt_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Tag.Pt());
	  hist("L2_h_tag_Eta_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Tag.Eta());
	  hist("L2_h_tag_Phi_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Tag.Phi());
	  hist("L2_h_tag_Et_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Tag.Et());

	  hist("L2_h_probe_Pt_cluster_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Probe.Pt());
	  hist("L2_h_probe_Eta_cluster_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Probe.Eta());
	  hist("L2_h_probe_Phi_cluster_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Probe.Phi());
	  hist("L2_h_probe_Et_cluster_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(Probe.Et());

	  TLorentzVector ZBoson = Tag + Probe;

	  hist("L2_h_Z_mass_electrons_clusters_InEff","HLT/ZeeTagProbe/Inefficient")->Fill(fabs(ZBoson.M()));
	}



    //////////dositrack! ///////
	/// Get online track collections

	bool doSIT = 1;

	std::string SITCollection = "TrigSiTrack_eGamma";
	allTracks.clear();
	alltracks_tag.clear();
	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature_L2_SIT;

	feature_L2_SIT = c->get<TrigInDetTrackCollection>("TrigSiTrack_eGamma");

	counter_track=-1;
	if (feature_L2_SIT.size() != 0){
	  for (std::vector< Trig::Feature<TrigInDetTrackCollection> >::const_iterator contit = feature_L2_SIT.begin(); contit != feature_L2_SIT.end(); ++contit){
	    ++counter_track;
	    //if (counter_track == 0 || counter_track == 2 || counter_track == 4){
	    if(roi_tight)  alltracks_tag =  GetTrigInDetTrackVectors((*contit), false, alltracks_tag, roi_tight);
	    allTracks = GetTrigInDetTrackVectors((*contit), false, allTracks, roi_noCut);
	      //}
	  }
	}
	event_efficient = false;
	if (doSIT && probeCluster_L2 != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_SIT_L2only_", m_tagElectron, probeCluster_L2, roi_noCut, allTracks, probeTrack, alltracks_tag.size());




    //////////dol2star - here need three histos - for the different a b c tracks! ///////
	/// Get online track collections

	bool doL2Star = 1;

	std::string L2StarCollection = "TrigSiTrackFinder";
	allTracks.clear();
	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature_L2_L2Star;

	feature_L2_L2Star = c->get<TrigInDetTrackCollection>("TrigL2SiTrackFinder_eGamma", TrigDefs::alsoDeactivateTEs);

	counter_track=-1;
	if (feature_L2_L2Star.size() != 0){
	  for (std::vector< Trig::Feature<TrigInDetTrackCollection> >::const_iterator contit = feature_L2_L2Star.begin(); contit != feature_L2_L2Star.end(); ++contit){
	    ++counter_track;
	    //if (counter_track == 0 || counter_track == 2 || counter_track == 4){

	    if(roi_tight)  alltracks_tag_A = GetTrigInDetTrackVectors((*contit), false, roi_tight, 0);
            if(roi_tight) alltracks_tag_B = GetTrigInDetTrackVectors((*contit), false, roi_tight, 1);
            if(roi_tight) alltracks_tag_C = GetTrigInDetTrackVectors((*contit), false, roi_tight, 2);

	    allTracks_A = GetTrigInDetTrackVectors((*contit), false, roi_noCut, 0);
	    allTracks_B = GetTrigInDetTrackVectors((*contit), false, roi_noCut, 1);
	    allTracks_C = GetTrigInDetTrackVectors((*contit), false, roi_noCut, 2);
	      //}
	  }
	}
	event_efficient = false;
	if (doL2Star && probeCluster_L2 != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_L2StarA_L2only_", m_tagElectron, probeCluster_L2, roi_noCut, allTracks_A, probeTrack, alltracks_tag_A.size());
	if (doL2Star && probeCluster_L2 != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_L2StarB_L2only_", m_tagElectron, probeCluster_L2, roi_noCut, allTracks_B, probeTrack, alltracks_tag_B.size());
	if (doL2Star && probeCluster_L2 != 0 ) event_efficient = fillProbeHistograms(((*trigit).c_str()), "_L2StarC_L2only_", m_tagElectron, probeCluster_L2, roi_noCut, allTracks_C, probeTrack, alltracks_tag_C.size());

	}
	catch (int e) {
	  if (e == 1) ATH_MSG_DEBUG("Exception occured, L2 tag is empty! ");
	  if (e == 2) ATH_MSG_DEBUG("Exception occured, L2 probe is empty! ");
	  if (e == 3) ATH_MSG_DEBUG("Exception occured, L2 probe bremmed");
	}
      }
    }



  }

  //  ATH_MSG_INFO("yvonne: we are done with fill method");

  return StatusCode::SUCCESS;
}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTIDZeeTagProbe::proc()
#else
StatusCode HLTIDZeeTagProbe::proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool endOfRun )
#endif
{
  if (endOfRun){

  }
  return StatusCode::SUCCESS;
}



bool HLTIDZeeTagProbe::fillProbeHistograms( std::string chain, std::string alg, const egamma* m_tagElectron, const CaloCluster* m_probeElectron, const TrigRoiDescriptor* roi, std::vector< simpleTrack > allTracks, simpleTrack offlineTrack,  int ntracks_tag){

  if(offlineTrack.Pt() == 0.) offlineTrack.SetPtEtaPhiM(0.1,10.1,0.1,0.1); // TJN: This is an absolutely absurd way of doing something and should be changed!

    //  ATH_MSG_INFO("yvonne: we start to fill efficiency stuff in the tp code");

    if (m_tagElectron ==0 ) ATH_MSG_DEBUG("Tag Electron = 0");

    if (allTracks.size() == 0){
	hist("Execution_fail", "HLT/ZeeTagProbe/Debug")->Fill(5);
    }
    else hist("h_tracks_roi","HLT/ZeeTagProbe/ROI_Variables")->Fill(allTracks.size());

  float tag_z0 = 0;


  std::vector< simpleTrack >::const_iterator trkitr = allTracks.begin();
  std::vector< simpleTrack >::const_iterator trkitrlast = allTracks.end();

   bool track_found_offline = false;
  bool track_found_delR = false;

  int track_counter = 0;

  if (m_probeElectron == 0)
    {
      ATH_MSG_DEBUG("Probe Electron = 0");
      return false;
    }

  double electron_eta = m_tagElectron->eta();
  double electron_phi = m_tagElectron->phi();
  double cluster_eta = m_probeElectron->eta();
  double cluster_phi = m_probeElectron->phi();
  double cluster_et = m_probeElectron->et();

  double electron_z0=-1000.0;
  double electron_d0=-1000.0;
  if( m_tagElectron->trackParticle())
    {
      // HepVector perigeeParams = m_tagElectron->trackParticle()->measuredPerigee()->parameters();
	  electron_z0= m_tagElectron->trackParticle()->measuredPerigee()->parameters()[Trk::z0];
	  electron_d0= m_tagElectron->trackParticle()->measuredPerigee()->parameters()[Trk::d0];
 }

  double roi_eta = -99;
  double roi_phi = -99;
  if (roi != 0){
    roi_eta = roi->eta();
    roi_phi = roi->phi();
  }




  // yvonne: try to plot efficiency versus calo cluster energyBE/e -> some hint for jet background?
  double EMHadfrac_tag = (m_tagElectron->cluster()->energyBE(0)+m_tagElectron->cluster()->energyBE(1)+m_tagElectron->cluster()->energyBE(2)+m_tagElectron->cluster()->energyBE(3))/m_tagElectron->cluster()->energy();
  double EMHadfrac_probe = (m_probeElectron->energyBE(0)+m_probeElectron->energyBE(1)+m_probeElectron->energyBE(2)+m_probeElectron->energyBE(3))/m_probeElectron->energy();

  hist("EF_emenergyfrac_cluster_tag", "HLT/ZeeTagProbe/Debug")->Fill(EMHadfrac_tag);
  hist("EF_emenergyfrac_cluster", "HLT/ZeeTagProbe/Debug")->Fill(EMHadfrac_probe);


  //

   TLorentzVector e1(0.,0.,0.,0.);
    e1.SetPtEtaPhiE(m_tagElectron->pt(), m_tagElectron->eta(), m_tagElectron->phi(), m_tagElectron->e());

  TLorentzVector e3(0.,0.,0.,0.);
    e3.SetPtEtaPhiE(m_probeElectron->et(), m_probeElectron->eta(), m_probeElectron->phi(), m_probeElectron->e());

  TLorentzVector Z_boson_cluster = e1 + e3;
  double mass_cluster = fabs(Z_boson_cluster.M())/1000.0;



  simpleTrack high_pT_track = simpleTrack();
  for (; trkitr != trkitrlast; ++trkitr) {


    double track_eta = (*trkitr).Eta();
    double track_phi = (*trkitr).Phi();

    double track_eta_extrapol = (*trkitr).eta_extrapol;
    double track_phi_extrapol = (*trkitr).phi_extrapol;

    //yvonne: what to compare to roi? the extrapolated track or not?
    double deleta_electron_track = (electron_eta - track_eta);
    double deleta_roi_track = (roi_eta - track_eta);
    double deleta_roi_electron = (roi_eta - electron_eta);
    double deleta_roi_cluster = (roi_eta - cluster_eta);
    double deleta_track_cluster = (track_eta_extrapol - cluster_eta);

    double delphi_electron_track = dPhiHelper(electron_phi , track_phi);
    double delphi_roi_track = dPhiHelper(roi_phi , track_phi);
    double delphi_roi_electron = dPhiHelper(roi_phi , electron_phi);
    double delphi_roi_cluster = dPhiHelper(roi_phi , cluster_phi);
    double delphi_track_cluster = dPhiHelper(track_phi_extrapol , cluster_phi);




    double delR_electron_track = sqrt((deleta_electron_track*deleta_electron_track) + (delphi_electron_track*delphi_electron_track));
    double delR_roi_track = sqrt((deleta_roi_track*deleta_roi_track) + (delphi_roi_track*delphi_roi_track));
    double delR_roi_electron = sqrt((deleta_roi_electron*deleta_roi_electron) + (delphi_roi_electron*delphi_roi_electron));
    double delR_roi_cluster = sqrt((deleta_roi_cluster*deleta_roi_cluster) + (delphi_roi_cluster*delphi_roi_cluster));
    double delR_track_cluster = sqrt((deleta_track_cluster*deleta_track_cluster) + (delphi_track_cluster*delphi_track_cluster));

    double deleta_track_offline_v2 = (*trkitr).Eta()-offlineTrack.Eta();
    double delphi_track_offline_v2 = dPhiHelper((*trkitr).Phi(),offlineTrack.Phi());
    double delR_track_offline_v2 = sqrt((deleta_track_offline_v2*deleta_track_offline_v2) + (delphi_track_offline_v2*delphi_track_offline_v2));



    if (high_pT_track.Pt() == 0) high_pT_track = (*trkitr);
    if (trkitr->Pt() > high_pT_track.Pt()) high_pT_track = (*trkitr);
    hist("h_track_EF_pT", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).Pt());
    hist("h_track_EF_Eta", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).Eta());
    hist("h_track_EF_Phi", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).Phi());
    hist("h_track_EF_Et", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).Et());
    hist("h_track_EF_z0", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).z0);
    hist("h_track_EF_deltaz0", "HLT/ZeeTagProbe/Tracks")->Fill(fabs((*trkitr).z0-tag_z0));

    ++track_counter;

    hist("h_deltaPhi_electron_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_electron_track);
    hist("h_deltaPhi_electron_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(dPhiHelper(electron_phi, cluster_phi));
    hist("h_deltaPhi_roi_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_roi_track);
    hist("h_deltaPhi_roi_electron", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_roi_electron);
    hist("h_deltaPhi_roi_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_roi_cluster);
    hist("h_deltaPhi_track_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_track_cluster);

    hist("h_deltaEta_electron_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_electron_track);
    hist("h_deltaEta_roi_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_roi_track);
    hist("h_deltaEta_roi_electron", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_roi_electron);
    hist("h_deltaEta_roi_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_roi_cluster);
    hist("h_deltaEta_track_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_track_cluster);

    hist("h_deltaR_electron_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_electron_track);
    hist("h_deltaR_roi_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_roi_track);
    hist("h_deltaR_roi_electron", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_roi_electron);
    hist("h_deltaR_roi_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_roi_cluster);
    hist("h_deltaR_track_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_track_cluster);
    hist("h_Eta_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(cluster_eta);
    hist("h_Eta_roi", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(roi_eta);




    //TLorentzVector e2 = ((*trkitr).Pt(), (*trkitr).Eta(), (*trkitr).Phi(), (((*trkitr).Pt())*TMath::CosH((*trkitr).Eta())));
    TLorentzVector e2(0.,0.,0.,0.);
    e2.SetPtEtaPhiM((*trkitr).Pt(), (*trkitr).Eta(), (*trkitr).Phi(), 0);

   TLorentzVector Z_boson = e1 + e2;
    double mass = fabs(Z_boson.M());


   hist("h_Z_mass_tracks", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(mass);

   if (!track_found_offline && delR_track_offline_v2 < 0.05 && TMath::Abs(offlineTrack.Eta())<10.0){
	track_found_offline = true;

	    std::string histname1 = (chain + alg + "phi" + "_eff_offline").c_str();
	    std::string histname1_1 = (chain + alg + "phi_poscharge" + "_eff_offline").c_str();
	    std::string histname1_2 = (chain + alg + "phi_negcharge" + "_eff_offline").c_str();
	    std::string histname2 = (chain + alg + "pT" + "_eff_offline").c_str();
	    std::string histname3 = (chain + alg + "eta" + "_eff_offline").c_str();
	    std::string histname4 = (chain + alg + "z0" + "_eff_offline").c_str();
	    std::string histname5 = (chain + alg + "d0" + "_eff_offline").c_str();
	    std::string histname6 = (chain + alg + "nvtx" + "_eff_offline").c_str();
	    std::string histname7 = (chain + alg + "mZ" + "_eff_offline").c_str();
	    std::string histname8 = (chain + alg + "ntrk" + "_eff_offline").c_str();
	    std::string histname9 = (chain + alg + "ntrktag" + "_eff_offline").c_str();
	    std::string histname10 = (chain + alg + "emenergyfrac" + "_eff_offline").c_str();
	    std::string histname10_1 = (chain + alg + "emenergyfrac" + "_eff_ptl20_offline").c_str();
	    std::string histname10_2 = (chain + alg + "emenergyfrac" + "_eff_ptge20_offline").c_str();
	    profile(histname1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 100.,1.);
	    profile(histname2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Pt(),  100. ,1.);
	    profile(histname3, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Eta(), 100.,1.);
	    profile(histname4, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.z0, 100.,1.);
	    profile(histname5, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.d0, 100.,1.);
	    profile(histname6, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(nvtx, 100.,1.);
	    profile(histname7, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(mass_cluster, 100.,1.);
	    profile(histname8, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(allTracks.size(), 100.,1.);
	    profile(histname9, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(ntracks_tag, 100.,1.);
	    profile(histname10, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 100.,1.);
	    if(offlineTrack.Pt()/1000.0<20.0)
		profile(histname10_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 100.,1.);
	    else
		profile(histname10_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 100.,1.);

	    if(offlineTrack.charge>0)
	      {
		profile(histname1_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 100.,1.);
	      }
	    else
	      {
		profile(histname1_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 100.,1.);
	      }


    }

    if (!track_found_delR && deleta_track_cluster < 0.05 && delphi_track_cluster < 0.05 ){
      track_found_delR = true;

      std::string histname1 = (chain + alg + "phi" + "_eff_deltaR").c_str();
      std::string histname1_1 = (chain + alg + "phi_poscharge" + "_eff_deltaR").c_str();
      std::string histname1_2 = (chain + alg + "phi_negcharge" + "_eff_deltaR").c_str();
      std::string histname2 = (chain + alg + "pT" + "_eff_deltaR").c_str();
      std::string histname3 = (chain + alg + "eta" + "_eff_deltaR").c_str();
      //      std::string histname3_1 = (chain + alg + "eta" + "_eff_OS_deltaR").c_str();
      //      std::string histname3_2 = (chain + alg + "eta" + "_eff_SS_deltaR").c_str();
      std::string histname4 = (chain + alg + "z0" + "_eff_deltaR").c_str();
      std::string histname5 = (chain + alg + "d0" + "_eff_deltaR").c_str();
      std::string histname6 = (chain + alg + "nvtx" + "_eff_deltaR").c_str();
      std::string histname7 = (chain + alg + "mZ" + "_eff_deltaR").c_str();
      std::string histname8 = (chain + alg + "ntrk" + "_eff_deltaR").c_str();
      std::string histname9 = (chain + alg + "ntrktag" + "_eff_deltaR").c_str();
      std::string histname10 = (chain + alg + "emenergyfrac" + "_eff_deltaR").c_str();
      std::string histname10_1 = (chain + alg + "emenergyfrac" + "_eff_ptl20_deltaR").c_str();
      std::string histname10_2 = (chain + alg + "emenergyfrac" + "_eff_ptge20_deltaR").c_str();

     profile(histname1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 100.,1.);
      profile(histname2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_et, 100.,1.);
      profile(histname3, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 100.,1.);
      profile(histname6, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(nvtx, 100.,1.);
      profile(histname7, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(mass_cluster, 100.,1.);
      profile(histname8, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(allTracks.size(), 100.,1.);
      profile(histname9, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(ntracks_tag, 100.,1.);
      profile(histname10, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 100.,1.);
      profile(histname4, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill( electron_z0, 100.,1.);
      profile(histname5, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(  electron_d0, 100.,1.);


      if(cluster_et/1000.0<20.0)
	  profile(histname10_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 100.,1.);
      else
	  profile(histname10_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 100.,1.);

      if(m_tagElectron->charge()>0)
	{
	  profile(histname1_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 100.,1.);
	}
      else
	{
	  profile(histname1_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 100.,1.);
	}

      //      if(m_tagElectron->charge() + (*trkitr).charge ==0 )
      //	  profile(histname3_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 100.,1.);
      //      else
      //	  profile(histname3_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 100.,1.);


	}


  }


  if( track_counter>0)
      {
	  //TLorentzVector e2_high_pT = (high_pT_track.Pt(),  high_pT_track.Eta(), high_pT_track.Phi(), (high_pT_track.Pt()*TMath::CosH(high_pT_track.Eta())));
	  TLorentzVector e2_high_pT(0.,0.,0.,0.);
	  e2_high_pT.SetPtEtaPhiM(high_pT_track.Pt(),  high_pT_track.Eta(), high_pT_track.Phi(), 0);
	  TLorentzVector Z_boson_high_pT = e1 + e2_high_pT;

	  hist("h_Z_mass_electron_highpTtrack", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(fabs(Z_boson_high_pT.M()));
      }


  if (!track_found_offline  && TMath::Abs(offlineTrack.Eta())<10.0){
    std::string histname1 = (chain + alg + "phi" + "_eff_offline").c_str();
    std::string histname1_1 = (chain + alg + "phi_poscharge" + "_eff_offline").c_str();
    std::string histname1_2 = (chain + alg + "phi_negcharge" + "_eff_offline").c_str();
    std::string histname2 = (chain + alg + "eta" + "_eff_offline").c_str();
    std::string histname3 = (chain + alg + "pT" + "_eff_offline").c_str();
    std::string histname4 = (chain + alg + "z0" + "_eff_offline").c_str();
    std::string histname5 = (chain + alg + "d0" + "_eff_offline").c_str();
    std::string histname6 = (chain + alg + "nvtx" + "_eff_offline").c_str();
    std::string histname7 = (chain + alg + "mZ" + "_eff_offline").c_str();
    std::string histname8 = (chain + alg + "ntrk" + "_eff_offline").c_str();
    std::string histname9 = (chain + alg + "ntrktag" + "_eff_offline").c_str();
    std::string histname10 = (chain + alg + "emenergyfrac" + "_eff_offline").c_str();
    std::string histname10_1 = (chain + alg + "emenergyfrac" + "_eff_ptl20_offline").c_str();
    std::string histname10_2 = (chain + alg + "emenergyfrac" + "_eff_ptge20_offline").c_str();

    profile(histname1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 0.,1.);
    profile(histname2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Eta(), 0.,1.);
    profile(histname3, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Pt(), 0.,1.);
    profile(histname4, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.z0, 0.,1.);
    profile(histname5, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.d0, 0.,1.);
    profile(histname6, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(nvtx, 0.,1.);
    profile(histname7, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(mass_cluster, 0.,1.);
    profile(histname8, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(allTracks.size(), 0.,1.);
    profile(histname9, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(ntracks_tag, 0.,1.);
    profile(histname10, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 0.,1.);

    if(offlineTrack.Pt()/1000.0<20.0)
	    profile(histname10_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 0.,1.);
    else
	    profile(histname10_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 0.,1.);

    if(offlineTrack.charge>0)
      {
	profile(histname1_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 0.,1.);
      }
    else
      {
	profile(histname1_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 0.,1.);
      }


  }
  if (!track_found_delR){
    std::string histname1 = (chain + alg + "phi" + "_eff_deltaR").c_str();
     std::string histname1_1 = (chain + alg + "phi_poscharge" + "_eff_deltaR").c_str();
      std::string histname1_2 = (chain + alg + "phi_negcharge" + "_eff_deltaR").c_str();
     std::string histname2 = (chain + alg + "eta" + "_eff_deltaR").c_str();
    std::string histname3 = (chain + alg + "pT" + "_eff_deltaR").c_str();
    //  std::string histname2_1 = (chain + alg + "eta" + "_eff_OS_deltaR").c_str();
    //   std::string histname2_2 = (chain + alg + "eta" + "_eff_SS_deltaR").c_str();
    std::string histname4 = (chain + alg + "z0" + "_eff_deltaR").c_str();
    std::string histname5 = (chain + alg + "d0" + "_eff_deltaR").c_str();
     std::string histname6 = (chain + alg + "nvtx" + "_eff_deltaR").c_str();
    std::string histname7 = (chain + alg + "mZ" + "_eff_deltaR").c_str();
    std::string histname8 = (chain + alg + "ntrk" + "_eff_deltaR").c_str();
    std::string histname9 = (chain + alg + "ntrktag" + "_eff_deltaR").c_str();
    std::string histname10 = (chain + alg + "emenergyfrac" + "_eff_deltaR").c_str();
    std::string histname10_1 = (chain + alg + "emenergyfrac" + "_eff_ptl20_deltaR").c_str();
    std::string histname10_2 = (chain + alg + "emenergyfrac" + "_eff_ptge20_deltaR").c_str();
    profile(histname1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 0.,1.);
    profile(histname2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 0.,1.);
    profile(histname3, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_et, 0.,1.);
    profile(histname6, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(nvtx, 0.,1.);
    profile(histname7, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(mass_cluster, 0.,1.);
    profile(histname8, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(allTracks.size(), 0.,1.);
    profile(histname9, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(ntracks_tag, 0.,1.);
    profile(histname10, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 0.,1.);
    profile(histname4, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(  electron_z0, 0.,1.);
    profile(histname5, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(  electron_d0 , 0.,1.);

    if(cluster_et/1000.0<20.0)
	profile(histname10_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 0.,1.);
    else
	profile(histname10_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(EMHadfrac_probe, 0.,1.);

    if(m_tagElectron->charge()>0)
      {
	profile(histname1_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 0.,1.);
      }
    else
      {
	profile(histname1_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 0.,1.);
      }


    //    profile(histname2_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 0.,1.);
    //    profile(histname2_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 0.,1.);

  }

  //      ATH_MSG_INFO("yvonne: we are done  to fill efficiency stuff in the tp code");
  if (!track_found_delR) return false;
  else return true;
}


bool HLTIDZeeTagProbe::fillProbeHistograms( std::string chain, std::string alg, const TrigElectron* m_tagElectron, const TrigEMCluster* m_probeElectron, const TrigRoiDescriptor* roi, std::vector< simpleTrack > allTracks, simpleTrack offlineTrack,  int ntracks_tag){

  if(offlineTrack.Pt() == 0.) offlineTrack.SetPtEtaPhiM(0.1,10.1,0.1,0.1); // TJN: This is an absolutely absurd way of doing something and should be changed!

    //  ATH_MSG_INFO("yvonne: we start to fill more efficiency stuff in the tp code");

  if (allTracks.size() == 0){
      ATH_MSG_DEBUG("No L2 Tracks in the loose ROI ");
    hist("Execution_fail", "HLT/ZeeTagProbe/Debug")->Fill(5);
  }
  else hist("L2_h_tracks_roi","HLT/ZeeTagProbe/ROI_Variables")->Fill(allTracks.size());



  std::vector< simpleTrack >::const_iterator trkitr = allTracks.begin();
  std::vector< simpleTrack >::const_iterator trkitrlast = allTracks.end();


  bool track_found_offline = false;
  bool track_found_delR = false;

  int track_counter = 0;

  if (m_probeElectron == 0) return false;
  double electron_eta = m_tagElectron->eta();
  double electron_phi = m_tagElectron->phi();
  double cluster_eta = m_probeElectron->eta();
  double cluster_phi = m_probeElectron->phi();
  double cluster_et = m_probeElectron->et();
  double roi_eta = -99;
  double roi_phi = -99;
  if (roi != 0){
    roi_eta = roi->eta();
    roi_phi = roi->phi();
  }

  //try something about hadronic energy - use the one from first hadronic layer and divide e/(e+ehad)
  double emhadfraction = (m_probeElectron->energy() - m_probeElectron->ehad1()) / m_probeElectron->energy();

  hist("L2_emenergyfrac_cluster", "HLT/ZeeTagProbe/Debug")->Fill(emhadfraction);

   TLorentzVector e1(0.,0.,0.,0.);
    e1.SetPtEtaPhiE(m_tagElectron->pt(), m_tagElectron->eta(), m_tagElectron->phi(), m_tagElectron->e());

  TLorentzVector e3(0.,0.,0.,0.);
    e3.SetPtEtaPhiE(m_probeElectron->et(), m_probeElectron->eta(), m_probeElectron->phi(), m_probeElectron->e());

 TLorentzVector Z_boson_cluster = e1 + e3;
    double mass_cluster = fabs(Z_boson_cluster.M())/1000.0;




  simpleTrack high_pT_track = simpleTrack();
  for (; trkitr != trkitrlast; ++trkitr) {

      if(alg == "_L2StarA_")
	  if((*trkitr).author != 5 ) continue;
      if(alg == "_L2StarB_")
	  if((*trkitr).author != 6 ) continue;
      if(alg == "_L2StarC_")
	  if((*trkitr).author != 7 ) continue;




    double track_eta = (*trkitr).Eta();
    double track_phi = (*trkitr).Phi();
    double track_eta_extrapol = (*trkitr).eta_extrapol;
    double track_phi_extrapol = (*trkitr).phi_extrapol;





    double deleta_electron_track = (electron_eta - track_eta);
    double deleta_roi_track = (roi_eta - track_eta);
    double deleta_roi_electron = (roi_eta - electron_eta);
    double deleta_roi_cluster = (roi_eta - cluster_eta);
    double deleta_track_cluster = (track_eta_extrapol - cluster_eta);

    double delphi_electron_track = dPhiHelper(electron_phi , track_phi);
    double delphi_roi_track = dPhiHelper(roi_phi , track_phi);
    double delphi_roi_electron = dPhiHelper(roi_phi , electron_phi);
    double delphi_roi_cluster = dPhiHelper(roi_phi , cluster_phi);
    double delphi_track_cluster = dPhiHelper(track_phi_extrapol , cluster_phi);

    double delR_electron_track = sqrt((deleta_electron_track*deleta_electron_track) + (delphi_electron_track*delphi_electron_track));
    double delR_roi_track = sqrt((deleta_roi_track*deleta_roi_track) + (delphi_roi_track*delphi_roi_track));
    double delR_roi_electron = sqrt((deleta_roi_electron*deleta_roi_electron) + (delphi_roi_electron*delphi_roi_electron));
    double delR_roi_cluster = sqrt((deleta_roi_cluster*deleta_roi_cluster) + (delphi_roi_cluster*delphi_roi_cluster));
    double delR_track_cluster = sqrt((deleta_track_cluster*deleta_track_cluster) + (delphi_track_cluster*delphi_track_cluster));

    double deleta_track_offline_v2 = (*trkitr).Eta()-offlineTrack.Eta();
    double delphi_track_offline_v2 = dPhiHelper((*trkitr).Phi(),offlineTrack.Phi());
    double delR_track_offline_v2 = sqrt((deleta_track_offline_v2*deleta_track_offline_v2) + (delphi_track_offline_v2*delphi_track_offline_v2));


    if (high_pT_track.Pt() == 0) high_pT_track = (*trkitr);
    if (trkitr->Pt() > high_pT_track.Pt()) high_pT_track = (*trkitr);
    hist("h_track_L2_pT", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).Pt());
    hist("h_track_L2_Eta", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).Eta());
    hist("h_track_L2_Phi", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).Phi());
    hist("h_track_L2_Et", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).Et());
    hist("h_track_L2_z0", "HLT/ZeeTagProbe/Tracks")->Fill((*trkitr).z0);

     ++track_counter;

    hist("L2_h_deltaPhi_electron_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_electron_track);
    hist("L2_h_deltaPhi_electron_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(dPhiHelper(electron_phi, cluster_phi));
    hist("L2_h_deltaPhi_roi_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_roi_track);
    hist("L2_h_deltaPhi_roi_electron", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_roi_electron);
    hist("L2_h_deltaPhi_roi_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_roi_cluster);
    hist("L2_h_deltaPhi_track_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delphi_track_cluster);

    hist("L2_h_deltaEta_electron_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_electron_track);
    hist("L2_h_deltaEta_roi_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_roi_track);
    hist("L2_h_deltaEta_roi_electron", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_roi_electron);
    hist("L2_h_deltaEta_roi_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_roi_cluster);
    hist("L2_h_deltaEta_track_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(deleta_track_cluster);

    hist("L2_h_deltaR_electron_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_electron_track);
    hist("L2_h_deltaR_roi_track", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_roi_track);
    hist("L2_h_deltaR_roi_electron", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_roi_electron);
    hist("L2_h_deltaR_roi_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_roi_cluster);
    hist("L2_h_deltaR_track_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(delR_track_cluster);
    hist("L2_h_Eta_cluster", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(cluster_eta);
    hist("L2_h_Eta_roi", "HLT/ZeeTagProbe/DeltaR_Plots")->Fill(roi_eta);

    // Just do the efficiency as a function of the offline track

    TLorentzVector e2(0.,0.,0.,0.);
    e2.SetPtEtaPhiM((*trkitr).Pt(), (*trkitr).Eta(), (*trkitr).Phi(), 0);


    TLorentzVector Z_boson = e1 + e2;
    double mass = fabs(Z_boson.M());


    hist("L2_h_Z_mass_tracks", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(mass);


    if (!track_found_offline && delR_track_offline_v2 < 0.05  && TMath::Abs(offlineTrack.Eta())<10.0){
	track_found_offline = true;
	std::string histname1 = (chain + alg + "phi" + "_eff_offline").c_str();
	std::string histname1_1 = (chain + alg + "phi_poscharge" + "_eff_offline").c_str();
	std::string histname1_2 = (chain + alg + "phi_negcharge" + "_eff_offline").c_str();
  	    std::string histname2 = (chain + alg + "pT" + "_eff_offline").c_str();
	    std::string histname3 = (chain + alg + "eta" + "_eff_offline").c_str();
	    std::string histname4 = (chain + alg + "z0" + "_eff_offline").c_str();
	    std::string histname5 = (chain + alg + "d0" + "_eff_offline").c_str();
	    std::string histname6 = (chain + alg + "nvtx" + "_eff_offline").c_str();
	    std::string histname7 = (chain + alg + "mZ" + "_eff_offline").c_str();
	    std::string histname8 = (chain + alg + "ntrk" + "_eff_offline").c_str();
	    std::string histname9 = (chain + alg + "ntrktag" + "_eff_offline").c_str();
	    std::string histname10 = (chain + alg + "emenergyfrac" + "_eff_offline").c_str();
	    std::string histname10_1 = (chain + alg + "emenergyfrac" + "_eff_ptl20_offline").c_str();
	    std::string histname10_2 = (chain + alg + "emenergyfrac" + "_eff_ptge20_offline").c_str();
	    profile(histname1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 100.,1.);
	    profile(histname2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Pt(),  100. ,1.);
	    profile(histname3, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Eta(), 100.,1.);
	    profile(histname4, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.z0, 100.,1.);
	    profile(histname5, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.d0, 100.,1.);
	    profile(histname6, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(nvtx, 100.,1.);
	    profile(histname7, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(mass_cluster, 100.,1.);
	    profile(histname8, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(allTracks.size(), 100.,1.);
	    profile(histname9, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(ntracks_tag, 100.,1.);
	    profile(histname10, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 100.,1.);

	    if(offlineTrack.Pt()/1000.0<20.0)
		profile(histname10_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 100.,1.);
	    else
		profile(histname10_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 100.,1.);


	    if(offlineTrack.charge>0)
	      {
		profile(histname1_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 100.,1.);
	      }
	    else
	      {
		profile(histname1_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 100.,1.);
	      }
    }


    if (!track_found_delR && deleta_track_cluster < 0.05 && delphi_track_cluster < 0.05 ){
      track_found_delR = true;
      std::string histname1 = (chain + alg + "phi" + "_eff_deltaR").c_str();
      std::string histname1_1 = (chain + alg + "phi_poscharge" + "_eff_deltaR").c_str();
      std::string histname1_2 = (chain + alg + "phi_negcharge" + "_eff_deltaR").c_str();
      std::string histname2 = (chain + alg + "pT" + "_eff_deltaR").c_str();
      std::string histname3 = (chain + alg + "eta" + "_eff_deltaR").c_str();
      //      std::string histname3_1 = (chain + alg + "eta" + "_eff_OS_deltaR").c_str();
      //      std::string histname3_2 = (chain + alg + "eta" + "_eff_SS_deltaR").c_str();
      std::string histname4 = (chain + alg + "z0" + "_eff_deltaR").c_str();
      std::string histname6 = (chain + alg + "nvtx" + "_eff_deltaR").c_str();
      std::string histname7 = (chain + alg + "mZ" + "_eff_deltaR").c_str();
      std::string histname8 = (chain + alg + "ntrk" + "_eff_deltaR").c_str();
      std::string histname9 = (chain + alg + "ntrktag" + "_eff_deltaR").c_str();
      std::string histname10 = (chain + alg + "emenergyfrac" + "_eff_deltaR").c_str();
      std::string histname10_1 = (chain + alg + "emenergyfrac" + "_eff_ptl20_deltaR").c_str();
      std::string histname10_2 = (chain + alg + "emenergyfrac" + "_eff_ptge20_deltaR").c_str();
      profile(histname1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 100.,1.);
      profile(histname2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_et, 100.,1.);
      profile(histname3, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 100.,1.);
      profile(histname6, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(nvtx, 100.,1.);
      profile(histname7, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(mass_cluster, 100.,1.);
      profile(histname8, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(allTracks.size(), 100.,1.);
      profile(histname9, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(ntracks_tag, 100.,1.);
      profile(histname10, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 100.,1.);
      profile(histname4, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill( m_tagElectron->Zvtx(), 100.,1.);


      if(cluster_et/1000.0<20.0)
	  profile(histname10_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 100.,1.);
      else
	  profile(histname10_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 100.,1.);

      if(m_tagElectron->charge()>0)
	{
	  profile(histname1_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 100.,1.);
	}
      else
	{
	  profile(histname1_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 100.,1.);
	}


      //      if(m_tagElectron->charge() + (*trkitr).charge ==0 )
      //	  profile(histname3_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 100.,1.);
      //      else
      //	  profile(histname3_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 100.,1.);


	}

  }

  if(track_counter>0)
      {
	  //TLorentzVector e2_high_pT = (high_pT_track.Pt(),  high_pT_track.Eta(), high_pT_track.Phi(), (high_pT_track.Pt()*TMath::CosH(high_pT_track.Eta())));
	  TLorentzVector e2_high_pT(0.,0.,0.,0.);
	  e2_high_pT.SetPtEtaPhiM(high_pT_track.Pt(),  high_pT_track.Eta(), high_pT_track.Phi(), 0);
	  TLorentzVector Z_boson_high_pT = e1 + e2_high_pT;


	  hist("h_Z_mass_electron_highpTtrack", "HLT/ZeeTagProbe/ZMass_Plots")->Fill(fabs(Z_boson_high_pT.M()));
      }

  if (!track_found_offline  && TMath::Abs(offlineTrack.Eta())<10.0){
    std::string histname1 = (chain + alg + "phi" + "_eff_offline").c_str();
    std::string histname1_1 = (chain + alg + "phi_poscharge" + "_eff_offline").c_str();
    std::string histname1_2 = (chain + alg + "phi_negcharge" + "_eff_offline").c_str();
    std::string histname2 = (chain + alg + "eta" + "_eff_offline").c_str();
    std::string histname3 = (chain + alg + "pT" + "_eff_offline").c_str();
    std::string histname4 = (chain + alg + "z0" + "_eff_offline").c_str();
    std::string histname5 = (chain + alg + "d0" + "_eff_offline").c_str();
    std::string histname6 = (chain + alg + "nvtx" + "_eff_offline").c_str();
    std::string histname7 = (chain + alg + "mZ" + "_eff_offline").c_str();
    std::string histname8 = (chain + alg + "ntrk" + "_eff_offline").c_str();
    std::string histname9 = (chain + alg + "ntrktag" + "_eff_offline").c_str();
    std::string histname10 = (chain + alg + "emenergyfrac" + "_eff_offline").c_str();
    std::string histname10_1 = (chain + alg + "emenergyfrac" + "_eff_ptl20_offline").c_str();
    std::string histname10_2 = (chain + alg + "emenergyfrac" + "_eff_ptge20_offline").c_str();
    profile(histname1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 0.,1.);
    profile(histname2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Eta(), 0.,1.);
    profile(histname3, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Pt(), 0.,1.);
    profile(histname4, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.z0, 0.,1.);
    profile(histname5, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.d0, 0.,1.);
    profile(histname6, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(nvtx, 0.,1.);
    profile(histname7, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(mass_cluster, 0.,1.);
    profile(histname8, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(allTracks.size(), 0.,1.);
    profile(histname9, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(ntracks_tag, 0.,1.);
    profile(histname10, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 0.,1.);

    if(offlineTrack.Pt()/1000.0<20.0)
	profile(histname10_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 0.,1.);
    else
	profile(histname10_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 0.,1.);


    if(offlineTrack.charge>0)
      {
	profile(histname1_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 0.,1.);
      }
    else
      {
	profile(histname1_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(offlineTrack.Phi(), 0.,1.);
      }


  }
  if (!track_found_delR){
    std::string histname1 = (chain + alg + "phi" + "_eff_deltaR").c_str();
    std::string histname2 = (chain + alg + "eta" + "_eff_deltaR").c_str();
    std::string histname1_1 = (chain + alg + "phi_poscharge" + "_eff_deltaR").c_str();
    std::string histname1_2 = (chain + alg + "phi_negcharge" + "_eff_deltaR").c_str();
    //    std::string histname2_1 = (chain + alg + "eta" + "_eff_OS_deltaR").c_str();
    //    std::string histname2_2 = (chain + alg + "eta" + "_eff_SS_deltaR").c_str();
    std::string histname3 = (chain + alg + "pT" + "_eff_deltaR").c_str();
    std::string histname4 = (chain + alg + "z0" + "_eff_deltaR").c_str();
    std::string histname6 = (chain + alg + "nvtx" + "_eff_deltaR").c_str();
    std::string histname7 = (chain + alg + "mZ" + "_eff_deltaR").c_str();
    std::string histname8 = (chain + alg + "ntrk" + "_eff_deltaR").c_str();
    std::string histname9 = (chain + alg + "ntrktag" + "_eff_deltaR").c_str();
    std::string histname10 = (chain + alg + "emenergyfrac" + "_eff_deltaR").c_str();
    std::string histname10_1 = (chain + alg + "emenergyfrac" + "_eff_ptl20_deltaR").c_str();
    std::string histname10_2 = (chain + alg + "emenergyfrac" + "_eff_ptge20_deltaR").c_str();
    profile(histname1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 0.,1.);
    profile(histname2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 0.,1.);
    profile(histname3, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_et, 0.,1.);
    profile(histname6, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(nvtx, 0.,1.);
    profile(histname7, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(mass_cluster, 0.,1.);
    profile(histname8, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(allTracks.size(), 0.,1.);
    profile(histname9, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(ntracks_tag, 0.,1.);
    profile(histname10, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 0.,1.);
    profile(histname4, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill( m_tagElectron->Zvtx(), 0.,1.);


    if(cluster_et/1000.0<20.0)
	profile(histname10_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 0.,1.);
    else
	profile(histname10_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(emhadfraction , 0.,1.);

    if(m_tagElectron->charge()>0)
      {
	profile(histname1_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 0.,1.);
      }
    else
      {
	profile(histname1_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_phi, 0.,1.);
      }

 //    profile(histname2_1, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 0.,1.);
    //    profile(histname2_2, "HLT/ZeeTagProbe/Efficiency_Plots")->Fill(cluster_eta, 0.,1.);

  }


  //      ATH_MSG_INFO("yvonne: we are done to fill more efficiency stuff in the tp code");

  if (!track_found_delR) return false;
  else return true;
}

std::vector< simpleTrack > HLTIDZeeTagProbe::GetTrigInDetTrackVectors( const Trig::Feature<Rec::TrackParticleContainer>&  feature, bool /*selection*/, std::vector< simpleTrack > outputVector, const TrigRoiDescriptor* roi) {

  const Rec::TrackParticleContainer* trigInDetTrkCont = feature.cptr();

    Rec::TrackParticleContainer::const_iterator track     = trigInDetTrkCont->begin();
    Rec::TrackParticleContainer::const_iterator lastTrack = trigInDetTrkCont->end();



    for (; track != lastTrack; ++track) {

      const Trk::TrackSummary* summary = (*track)->trackSummary();
      const Trk::Perigee* measPer = (*track)->measuredPerigee();
      //const Trk::ParametersBase* perTrkPar = 0;
      //perTrkPar = dynamic_cast<const Trk::MeasuredPerigee*>(&((*track)->definingParameters()));
      if (summary == 0 || measPer==0) {
        ATH_MSG_DEBUG("No measured perigee or track summary parameters assigned to the track");
        continue;
      }

      //      const Trk::ParametersBase<5, Trk::Charged>* result = 0;
      const Trk::TrackParameters* result = 0;
      //      result  = tracktocalo->extrapolate(perTrkPar, CaloCell_ID::EMB2, 0.0, Trk::undefined,   Trk::alongMomentum);

      if (fabs( (*track)->eta()) < 1.5)
	{
	  //result  = tracktocalo->extrapolate(perTrkPar, CaloCell_ID::EMB2, 0.0, Trk::undefined,   Trk::alongMomentum);
	  result = tracktocalo->extrapolate((const Trk::TrackParticleBase) *(*track),
					    CaloCell_ID::EMB2,
					    0.0,
					    Trk::alongMomentum,
					    Trk::undefined
					    );

	}
      else
	{
	  //	    result  = tracktocalo->extrapolate(perTrkPar, CaloCell_ID::EME2, 0.0, Trk::undefined,   Trk::alongMomentum);

	    result = tracktocalo->extrapolate(( Trk::TrackParticleBase) *(*track),
					      CaloCell_ID::EME2,
					      0.0,
					      Trk::alongMomentum,
					      Trk::undefined);
	}


      //      HepVector perigeeParams = measPer->parameters();
      float pt  = measPer->pT();
      float eta = measPer->eta();
      float phi=  measPer->parameters()[Trk::phi0];
      float z0  = measPer->parameters()[Trk::z0];
      float d0  = measPer->parameters()[Trk::d0];
      float nPixHits = summary->get(Trk::numberOfPixelHits);
      float nSctHits = summary->get(Trk::numberOfSCTHits);
      float nTrtHits = summary->get(Trk::numberOfTRTHits);
      float etaR = -999;
      float phiR = -999;
      float charge = (*track)->charge();

      float delEta = fabs(eta - roi->eta());
      float delPhi = dPhiHelper(phi , roi->phi());


      float delEta_extrapol = delEta;
      float delPhi_extrapol = delPhi;

      if(result)
	  {
	      delEta_extrapol = fabs(result->position().eta() - roi->eta());
	      delPhi_extrapol = dPhiHelper(result->position().phi() , roi->phi());
	  }


      if (delEta_extrapol > 0.1 || delPhi_extrapol >0.1){
	  continue;
      }
      //+++ Return tracks in generic format
      simpleTrack temp;
      temp.SetPtEtaPhiM(pt,eta,phi,0.);
      temp.z0 = z0;
      temp.d0 = d0;
      temp.nPixHits = nPixHits;
      temp.nSctHits = nSctHits;
      temp.nTrtHits = nTrtHits;
      temp.etaR = etaR;
      temp.phiR = phiR;
      temp.charge = charge;
      temp.author = -1; //not needed for EF
      if(result)
	{
	  temp.eta_extrapol=result->position().eta();
	  temp.phi_extrapol=result->position().phi();
	}
      else
	{
	  temp.eta_extrapol=-999.;
	  temp.phi_extrapol=-999.;
	}
      outputVector.push_back(temp);

      if(result) delete result;

    }



  return outputVector;
}

std::vector< simpleTrack > HLTIDZeeTagProbe::GetTrigInDetTrackVectors( const Trig::Feature<TrigInDetTrackCollection>&  feature, bool /*selection*/, std::vector< simpleTrack > outputVector, const TrigRoiDescriptor* roi) {

  const TrigInDetTrackCollection* trigInDetTrkCont = feature.cptr();

    TrigInDetTrackCollection::const_iterator track     = trigInDetTrkCont->begin();
    TrigInDetTrackCollection::const_iterator lastTrack = trigInDetTrkCont->end();

    for (; track != lastTrack; ++track) {

      float pt  = (*track)->param()->pT();
      float eta = (*track)->param()->eta();
      float phi = (*track)->param()->phi0();
      float z0  = (*track)->param()->z0();
      float d0  = (*track)->param()->a0();
      float nPixHits = 0;
      float nSctHits = 0;
      float nTrtHits = 0;
      float etaR = -999;
      float phiR = -999;
      float charge = (pt>0)? 1:-1 ;

	  double phiAtCalo = 0, etaAtCalo = 0;
      StatusCode sc = m_trackExtrapolator->extrapolateToCalo((*track), m_RCAL, m_ZCAL, phiAtCalo, etaAtCalo);
      if(sc.isFailure())
	  {
	     ATH_MSG_DEBUG("L2 extrapolator failed ");
	  }


      float delEta = fabs(eta - roi->eta());
      float delPhi = dPhiHelper(phi , roi->phi());

      float delEta_extrapol = delEta;
      float delPhi_extrapol = delPhi;

      if(!sc.isFailure())
          {
              delEta_extrapol = fabs(etaAtCalo - roi->eta());
              delPhi_extrapol = dPhiHelper(phiAtCalo, roi->phi());
          }

      if (delEta_extrapol > 0.1 || delPhi_extrapol >0.1){
          continue;
      }


      //+++ Return tracks in generic format
      simpleTrack temp;
      temp.SetPtEtaPhiM(pt,eta,phi,0.);
      temp.z0 = z0;
      temp.d0 = d0;
      temp.nPixHits = nPixHits;
      temp.nSctHits = nSctHits;
      temp.nTrtHits = nTrtHits;
      temp.etaR = etaR;
      temp.phiR = phiR;
      temp.charge = charge;
      temp.author = (*track)->algorithmId();

      if(!sc.isFailure())
	{
	    temp.eta_extrapol=etaAtCalo;
	    temp.phi_extrapol=phiAtCalo;
	}
      else
	  {
	      temp.eta_extrapol=-999.;
	      temp.phi_extrapol=-999.;
	  }
      outputVector.push_back(temp);
    }
    // }

  return outputVector;
}

std::vector< simpleTrack > HLTIDZeeTagProbe::GetTrigInDetTrackVectors( const Trig::Feature<TrigInDetTrackCollection>&  feature, bool /*selection*/, const TrigRoiDescriptor* roi, int alg) {

  // Overloaded fucntion to deal with L2Star track collections
  std::vector< simpleTrack > outputVector_A;
  std::vector< simpleTrack > outputVector_B;
  std::vector< simpleTrack > outputVector_C;

  // Check that the user has specified some useful alg value
  if (alg < 0 || alg > 3) {
    ATH_MSG_WARNING("Incorrect argument passed for 'alg' = " << alg << " value must be 0, 1 or 2");
    return outputVector_A;
  }

  // Define Expert methods so that you can navigate to the collection that you need
  Trig::ExpertMethods* em = TrigDec->ExperimentalAndExpertMethods();
  em->enable();
  const HLT::NavigationCore* nc = em->getNavigation();

  // Get the trigger element that you're trying to look at from the feature
  const HLT::TriggerElement* te = feature.te();

  // Retrieve a vector of all possible collections with the key from that feature (the key in this case is 'TrigL2SiTrackFinder'). The vector will be of the form collectionVector{ Strategy_A_Tracks, Strategy_B_Tracks, Strategy_C_Tracks }

  std::vector< const TrigInDetTrackCollection*> collectionVector;
  if ( !const_cast<HLT::NavigationCore *>(nc)->getFeatures( te, collectionVector, "TrigL2SiTrackFinder_eGamma") ){
    ATH_MSG_WARNING("Found no collections with the key TrigL2SiTrackFinder key in the feature");
  }

  double size = collectionVector.size();
  // Sanity check, there should only be 3 collections with this key in the feature
  if (size > 3) ATH_MSG_WARNING("More than 3 collections found in L2Star");
  unsigned int vec = 0;


  for(; vec < size; ++ vec){

    //    const TrigInDetTrackCollection* trigInDetTrkCont = feature.cptr();
    //TrigInDetTrackCollection::const_iterator track     = trigInDetTrkCont->begin();
    //TrigInDetTrackCollection::const_iterator lastTrack = trigInDetTrkCont->end();
    TrigInDetTrackCollection::const_iterator track     = collectionVector[vec]->begin();
    TrigInDetTrackCollection::const_iterator lastTrack = collectionVector[vec]->end();

    for (; track != lastTrack; ++track) {

      float pt  = (*track)->param()->pT();
      float eta = (*track)->param()->eta();
      float phi = (*track)->param()->phi0();
      float z0  = (*track)->param()->z0();
      float d0  = (*track)->param()->a0();
      float nPixHits = 0;
      float nSctHits = 0;
      float nTrtHits = 0;
      float etaR = -999;
      float phiR = -999;
      float charge = (pt>0)? 1:-1 ;

      double phiAtCalo = 0, etaAtCalo = 0;
      StatusCode sc = m_trackExtrapolator->extrapolateToCalo((*track), m_RCAL, m_ZCAL, phiAtCalo, etaAtCalo);
      if(sc.isFailure()) ATH_MSG_DEBUG("L2 extrapolator failed ");

      float delEta = fabs(eta - roi->eta());
      float delPhi = dPhiHelper(phi , roi->phi());

      float delEta_extrapol = delEta;
      float delPhi_extrapol = delPhi;

      if(!sc.isFailure())
          {
              delEta_extrapol = fabs(etaAtCalo - roi->eta());
              delPhi_extrapol = dPhiHelper(phiAtCalo, roi->phi());
          }

      //yvonne: maybe this is not the right cut? => jiri: don't cut on here
      if (delEta_extrapol > 0.1 || delPhi_extrapol >0.1){
          continue;
      }


      //+++ Return tracks in generic format
      simpleTrack temp;
      temp.SetPtEtaPhiM(pt,eta,phi,0.);
      temp.z0 = z0;
      temp.d0 = d0;
      temp.nPixHits = nPixHits;
      temp.nSctHits = nSctHits;
      temp.nTrtHits = nTrtHits;
      temp.etaR = etaR;
      temp.phiR = phiR;
      temp.charge = charge;
      temp.author = (*track)->algorithmId();
      //      std::cout << " yvonne test: " << temp.author << std::endl;

      if(!sc.isFailure())
	{
	    temp.eta_extrapol=etaAtCalo;
	    temp.phi_extrapol=phiAtCalo;
	}
      else
	  {
	      temp.eta_extrapol=-999.;
	      temp.phi_extrapol=-999.;
	  }

      if (vec == 0) outputVector_A.push_back(temp);
      if (vec == 1) outputVector_B.push_back(temp);
      if (vec == 2) outputVector_C.push_back(temp);
    }
  }

  if (alg == 0) return outputVector_A;
  else if (alg == 1) return outputVector_B;
  else if (alg == 2) return outputVector_C;
  return outputVector_A;
  // Should never get to this point;
}


double HLTIDZeeTagProbe::dPhiHelper(double phi1, double phi2)
{

 Double_t dphi=fabs(phi1-phi2);
  if(dphi>TMath::Pi() ) dphi = 2*TMath::Pi() - dphi;

  return dphi;
}

#endif

