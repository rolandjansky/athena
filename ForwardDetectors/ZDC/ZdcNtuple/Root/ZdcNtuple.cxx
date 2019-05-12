/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <TSystem.h>
#include <TFile.h>
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODCore/ShallowCopy.h"
//#include <EventLoopAlgs/NTupleSvc.h>
//#include <EventLoopAlgs/AlgSelect.h>
//#include <HIEventUtils/ZdcUtils.h>
#include <xAODTruth/TruthVertex.h>
#include <xAODCaloEvent/CaloCluster.h>
#include <xAODTracking/TrackParticleContainer.h>
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include <ZdcNtuple/ZdcNtuple.h>

// this is needed to distribute the algorithm to the workers
//ClassImp(ZdcNtuple)

ZdcNtuple :: ZdcNtuple (const std::string& name,ISvcLocator *pSvcLocator) 
  : EL::AnaAlgorithm(name,pSvcLocator),  
    m_trigConfigTool("TrigConf::xAODConfigTool/xAODConfigTool",this),
    m_trigDecisionTool ("Trig::TrigDecisionTool/TrigDecisionTool",this),
    m_trigMatchingTool("Trig::MatchingTool/TrigMatchingTool",this),
    m_grl ("GoodRunsListSelectionTool/grl", this),
    m_jetCleaning ("JetCleaningTool/JetCleaning", this),
    m_muonSelection ("CP::MuonSelectionTool", this),
    m_muonCalibrationAndSmearingTool ("CP::MuonCalibrationAndSmearingTool/MuonCorrectionTool",this),
    m_electronLooseEMSelector ("AsgElectronIsEMSelector/ElectronLooseEMSelector",this),
    m_electronMediumEMSelector ("AsgElectronIsEMSelector/ElectronMediumEMSelector",this),
    m_electronLooseLHSelector ("AsgElectronLikelihoodTool/ElectronLooseLHSelector",this),
    m_electronMediumLHSelector ("AsgElectronLikelihoodTool/ElectronMediumLHSelector",this),
    m_zdcAnalysisTool("ZDC::ZdcAnalysisTool/ZdcAnalysisTool",this)
    //m_photonLooseIsEMSelector ("AsgPhotonIsEMSelector/PhotonLooseEMSelector",this),
    //m_photonMediumIsEMSelector ("AsgPhotonIsEMSelector/PhotonMediumEMSelector",this)
    //m_isoTool("CP::IsolationSelectionTool/IsolationSelector",this);
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
  m_setupTrigHist = false;
  m_eventCounter = 0;

  declareProperty("isMC",  m_isMC = false, "MC mode");
  declareProperty("enableOutputTree",  enableOutputTree = false, "Enable output tree");
  declareProperty("enableOutputSamples",  enableOutputSamples = false, "comment");
  declareProperty("enableTrigger",  enableTrigger = false, "comment");
  declareProperty("writeOnlyTriggers",  writeOnlyTriggers = false, "comment");
  declareProperty("useGRL",  useGRL = true, "comment");
  declareProperty("grlFilename",  grlFilename = "$ROOTCOREBIN/data/ZdcNtuple/data16_hip8TeV.periodAllYear_DetStatus-v86-pro20-19_DQDefects-00-02-04_PHYS_HeavyIonP_All_Good.xml", "comment");
  declareProperty("slimmed",  slimmed = false, "comment");
  declareProperty("zdcCalib",  zdcCalib = false, "comment");
  declareProperty("zdcLaser",  zdcLaser = false, "comment");
  declareProperty("zdcLowGainOnly",  zdcLowGainOnly = false, "comment");
  declareProperty("enableTracks",  enableTracks = false, "comment");
  declareProperty("enableClusters",  enableClusters = false, "comment");
  declareProperty("enableJets",  enableJets = false, "comment");
  declareProperty("enableTriggerJets",  enableTriggerJets = false, "comment");
  declareProperty("enableMuons",  enableMuons = false, "comment");
  declareProperty("enableElectrons",  enableElectrons = false, "comment");
  declareProperty("enablePhotons",  enablePhotons = false, "comment");
  declareProperty("enableTT",  enableTT = false, "comment");
  declareProperty("trackLimit",  trackLimit = 1e6, "comment");
  declareProperty("trackLimitReject",  trackLimitReject = false, "comment");
  declareProperty("enableTruth",  enableTruth = false, "comment");
  declareProperty("outputTreeScaledown",  outputTreeScaledown = 1, "comment");
  declareProperty("flipDelay",  flipDelay = 0, "comment");
  declareProperty("reprocZdc",  reprocZdc = 0, "comment");
  declareProperty("auxSuffix",  auxSuffix = "", "comment");
  declareProperty("nsamplesZdc",  nsamplesZdc = 7, "comment");
  declareProperty("hion4",  hion4 = false, "comment");
  declareProperty("upc2015",  upc2015 = false, "comment");
  declareProperty("upcL2015",  upcL2015 = false, "comment");
  declareProperty("mb2015",  mb2015 = false, "comment");
  declareProperty("main2016A",  main2016A = false, "comment");
  declareProperty("express2016A",  express2016A = false, "comment");
  declareProperty("upc2016A",  upc2016A = false, "comment");
  declareProperty("main2016B",  main2016B = false, "comment");
  declareProperty("express2016B",  express2016B = false, "comment");
  declareProperty("upc2016B",  upc2016B = false, "comment");
  declareProperty("upc2016C",  upc2016C = false, "comment");
  declareProperty("upc2018",  upc2018 = false, "comment");
  declareProperty("mboverlay2016",  mboverlay2016 = false, "comment");
  declareProperty("zdcConfig", zdcConfig = "PbPb2018", "argument to configure ZdcAnalysisTool");
  declareProperty("doZdcCalib", doZdcCalib = false, "perform ZDC energy calibration");

  m_zdcAnalysisTool.declarePropertyFor (this, "zdcAnalysisTool");
  
  m_jetContainerNames.clear();
  // m_jetContainerNames.push_back("AntiKt4HIJets");
  //m_jetContainerNames.push_back("AntiKt4PV0TrackJets");
  //m_jetContainerNames.push_back("HLT_xAOD__JetContainer_a4tcemsubjesFS");
  //m_jetContainerNames.push_back("AntiKt4TruthJets");
  //m_jetContainerNames.push_back("HLT_xAOD__JetContainer_a4ionemsubjesFS");

  //m_jetContainerNames.push_back("AntiKt4LCTopoJets");
  //m_jetContainerNames.push_back("AntiKt4EMTopoJets");

  /*
  */

  m_jetContainers.clear();
  m_jetContainers.resize(m_jetContainerNames.size());

  m_jetCleaningFlag.clear();
  
  for (size_t ijet = 0;ijet<m_jetContainerNames.size();ijet++)
    {
      
      if( (m_jetContainerNames.at(ijet).find("Track") != std::string::npos) || (m_jetContainerNames.at(ijet).find("Truth") != std::string::npos) )
	{
	  //ANA_MSG_INFO("ZdcNtuple()","Not cleaning %s",m_jetContainerNames.at(ijet).c_str());
	  m_jetCleaningFlag.push_back(false); 
	}
      else
	{ 
	  m_jetCleaningFlag.push_back(true);  
	}
    }

}

/*
StatusCode ZdcNtuple :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD ();
  ANA_CHECK( "setupJob()", xAOD::Init() ); // call before opening first file

  return StatusCode::SUCCESS;
}
*/


StatusCode ZdcNtuple :: initialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  ANA_MSG_INFO("Howdy from histInitialize!");

  //char name[50];

  if (enableOutputTree)
    {
      //EL::NTupleSvc *m_ntupleSvc = EL::getNTupleSvc (wk(), "output");
      //TTree* m_outputTree = m_ntupleSvc->tree();

      //TFile* outputFile = wk()->getOutputFile(outputName);
      ANA_CHECK( book(TTree("zdcTree","ZDC Tree")));
      m_outputTree = tree( "zdcTree" );
      //m_outputTree->SetDirectory (outputFile);

      m_outputTree->Branch("runNumber",&t_runNumber,"runNumber/i");
      m_outputTree->Branch("eventNumber",&t_eventNumber,"eventNumber/i");
      m_outputTree->Branch("lumiBlock",&t_lumiBlock,"lumiBlock/i");
      m_outputTree->Branch("bcid",&t_bcid,"bcid/i");
      m_outputTree->Branch("avgIntPerCrossing",&t_avgIntPerCrossing,"avgIntPerCrossing/F");
      m_outputTree->Branch("actIntPerCrossing",&t_actIntPerCrossing,"actIntPerCrossing/F");
      m_outputTree->Branch("trigger",&t_trigger,"trigger/l");
      m_outputTree->Branch("trigger_TBP",&t_trigger_TBP,"trigger_TBP/i");
      m_outputTree->Branch("tbp",&t_tbp,"tbp[16]/i");
      m_outputTree->Branch("tav",&t_tav,"tav[16]/i");
      m_outputTree->Branch("passBits",&t_passBits,"passBits/i");

      //m_outputTree->Branch("decisions","vector<unsigned short>",&t_decisions);
      //m_outputTree->Branch("prescales","vector<float>",&t_prescales);

      if (enableOutputSamples)
	{
	  if (nsamplesZdc==7)
	    {
	      ANA_MSG_INFO("Setting up for 7 samples");
	      m_outputTree->Branch("zdc_raw",&t_raw7,"zdc_raw[2][4][2][2][7]/s"); // 7 samples
	    }
	  
	  if (nsamplesZdc==15) 
	    {
	      ANA_MSG_INFO("Setting up for 15 samples");
	      m_outputTree->Branch("zdc_raw",&t_raw15,"zdc_raw[2][4][2][2][15]/s"); // 7 samples
	    }
	}
      /*
      m_outputTree->Branch("zdc_ampHG",&t_ampHG,"zdc_ampHG[2][4]/F");
      m_outputTree->Branch("zdc_ampLG",&t_ampLG,"zdc_ampLG[2][4]/F");
      m_outputTree->Branch("zdc_sumHG",&t_sumHG,"zdc_sumHG[2]/F");
      m_outputTree->Branch("zdc_sumLG",&t_sumLG,"zdc_sumLG[2]/F");
      m_outputTree->Branch("zdc_amp",&t_amp,"zdc_amp[2][4]/F");
      */
      m_outputTree->Branch("zdc_amp_rp",&t_amp_rp,"zdc_amp_rp[2][4]/F");
      m_outputTree->Branch("zdc_time",&t_time,"zdc_time[2][4]/F");
      m_outputTree->Branch("zdc_ampHG_rp",&t_ampHG_rp,"zdc_ampHG_rp[2][4]/F");
      m_outputTree->Branch("zdc_ampLG_rp",&t_ampLG_rp,"zdc_ampLG_rp[2][4]/F");
      m_outputTree->Branch("zdc_sumHG_rp",&t_sumHG_rp,"zdc_sumHG_rp[2]/F");
      m_outputTree->Branch("zdc_sumLG_rp",&t_sumLG_rp,"zdc_sumLG_rp[2]/F");

      m_outputTree->Branch("zdc_ZdcAmp",&t_ZdcAmp,"zdc_ZdcAmp[2]/F");
      m_outputTree->Branch("zdc_ZdcAmpErr",&t_ZdcAmpErr,"zdc_ZdcAmpErr[2]/F");
      m_outputTree->Branch("zdc_ZdcEnergy",&t_ZdcEnergy,"zdc_ZdcEnergy[2]/F");
      m_outputTree->Branch("zdc_ZdcEnergyErr",&t_ZdcEnergyErr,"zdc_ZdcEnergyErr[2]/F");
      m_outputTree->Branch("zdc_ZdcTime",&t_ZdcTime,"zdc_ZdcTime[2]/F");
      m_outputTree->Branch("zdc_ZdcStatus",&t_ZdcStatus,"zdc_ZdcStatus[2]/S");
      m_outputTree->Branch("zdc_ZdcTrigEff",&t_ZdcTrigEff,"zdc_ZdcTrigEff[2]/F");
      m_outputTree->Branch("zdc_ZdcModuleMask",&t_ZdcModuleMask,"zdc_ZdcModuleMask/i");

      m_outputTree->Branch("zdc_ZdcModuleAmp",&t_ZdcModuleAmp,"zdc_ZdcModuleAmp[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModuleTime",&t_ZdcModuleTime,"zdc_ZdcModuleTime[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModuleFitAmp",&t_ZdcModuleFitAmp,"zdc_ZdcModuleFitAmp[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModuleFitT0",&t_ZdcModuleFitT0,"zdc_ZdcModuleFitT0[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModuleStatus",&t_ZdcModuleStatus,"zdc_ZdcModuleStatus[2][4]/i");
      m_outputTree->Branch("zdc_ZdcModuleChisq",&t_ZdcModuleChisq,"zdc_ZdcModuleChisq[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModuleCalibAmp",&t_ZdcModuleCalibAmp,"zdc_ZdcModuleCalibAmp[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModuleCalibTime",&t_ZdcModuleCalibTime,"zdc_ZdcModuleCalibTime[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModuleBkgdMaxFraction",&t_ZdcModuleBkgdMaxFraction,"zdc_ZdcModuleBkgdMaxFraction[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModuleAmpError",&t_ZdcModuleAmpError,"zdc_ZdcModuleAmpError[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModuleMinDeriv2nd",&t_ZdcModuleMinDeriv2nd,"zdc_ZdcModuleMinDeriv2nd[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModulePresample",&t_ZdcModulePresample,"zdc_ZdcModulePresample[2][4]/F");
      m_outputTree->Branch("zdc_ZdcModulePreSampleAmp",&t_ZdcModulePreSampleAmp,"zdc_ZdcModulePreSamplemp[2][4]/F");
      
      if (!(zdcCalib||zdcLaser))
	{
	  m_outputTree->Branch("mbts_in_e",&t_mbts_in_e,"mbts_in_e[2][8]/F");
	  m_outputTree->Branch("mbts_in_t",&t_mbts_in_t,"mbts_in_t[2][8]/F");
	  m_outputTree->Branch("mbts_out_e",&t_mbts_out_e,"mbts_out_e[2][4]/F");
	  m_outputTree->Branch("mbts_out_t",&t_mbts_out_t,"mbts_out_t[2][4]/F");
	  
	  m_outputTree->Branch("T2mbts_in_e",&t_T2mbts_in_e,"T2mbts_in_e[2][8]/F");
	  m_outputTree->Branch("T2mbts_in_t",&t_T2mbts_in_t,"T2mbts_in_t[2][8]/F");
	  m_outputTree->Branch("T2mbts_out_e",&t_T2mbts_out_e,"T2mbts_out_e[2][4]/F");
	  m_outputTree->Branch("T2mbts_out_t",&t_T2mbts_out_t,"T2mbts_out_t[2][4]/F");
	  
	  m_outputTree->Branch("L1ET",&t_L1ET,"L1ET/F");
	  m_outputTree->Branch("L1ET24",&t_L1ET24,"L1ET24/F");
	  
	  m_outputTree->Branch("totalEt",&t_totalEt,"totalEt/F");
	  m_outputTree->Branch("totalEt_TTsum",&t_totalEt_TTsum,"totalEt_TTsum/F");

	  m_outputTree->Branch("totalEt24",&t_totalEt24,"totalEt24/F");
	  m_outputTree->Branch("totalEt24_TTsum",&t_totalEt24_TTsum,"totalEt24_TTsum/F");

	  m_outputTree->Branch("fcalEt",&t_fcalEt,"fcalEt/F");
	  m_outputTree->Branch("fcalEtA",&t_fcalEtA,"fcalEtA/F");
	  m_outputTree->Branch("fcalEtC",&t_fcalEtC,"fcalEtC/F");
	  m_outputTree->Branch("fcalEtA_TT",&t_fcalEtA_TT,"fcalEtA_TT/F");
	  m_outputTree->Branch("fcalEtC_TT",&t_fcalEtC_TT,"fcalEtC_TT/F");
	  
	  m_outputTree->Branch("nvx",&t_nvx,"nvx/I");
	  m_outputTree->Branch("vx",&t_vx,"vx[3]/F");
	  m_outputTree->Branch("pvindex",&t_pvindex,"pvindex/I");
	  m_outputTree->Branch("vxntrk",&t_vxntrk,"vxntrk/I");
	  m_outputTree->Branch("vx_trk_index","vector<int>",&t_vx_trk_index);
	  m_outputTree->Branch("vxtype",&t_vxtype,"vxtype/I");
	  m_outputTree->Branch("vxcov",&t_vxcov,"vxcov[6]/F");
	  m_outputTree->Branch("vxsumpt2",&t_vxsumpt2,"vxsumpt2/F");
	  m_outputTree->Branch("nstrong",&t_nstrong,"nstrong/I");
	  m_outputTree->Branch("puvxntrk",&t_puvxntrk,"puvxntrk/I");
	  m_outputTree->Branch("puvxsumpt",&t_puvxsumpt,"puvxsumpt/F");
	  m_outputTree->Branch("puvxz",&t_puvxz,"puvxz/F");
	  m_outputTree->Branch("vxnlooseprimary",&t_vxnlooseprimary,"vxnlooseprimary/I");
	  m_outputTree->Branch("vxnminbias",&t_vxnminbias,"vxnminbias/I");
	  m_outputTree->Branch("vxngoodmuon",&t_vxngoodmuon,"vxngoodmuon/I");
	  
	  m_outputTree->Branch("t_nvtx",&t_nvtx,"nvtx/I");
	  m_outputTree->Branch("vtx_type","vector<int8_t>",&t_vtx_type);
	  m_outputTree->Branch("vtx_x","vector<float>",&t_vtx_x);
	  m_outputTree->Branch("vtx_y","vector<float>",&t_vtx_y);
	  m_outputTree->Branch("vtx_z","vector<float>",&t_vtx_z);
	  m_outputTree->Branch("vtx_ntrk_all","vector<int16_t>",&t_vtx_ntrk_all);
	  m_outputTree->Branch("vtx_sumpt2_all","vector<float>",&t_vtx_sumpt2_all);
	  m_outputTree->Branch("vtx_ntrk","vector<int16_t>",&t_vtx_ntrk);
	  m_outputTree->Branch("vtx_sumpt2","vector<float>",&t_vtx_sumpt2);
	  m_outputTree->Branch("vtx_trk_index","vector< vector<int16_t> >",&t_vtx_trk_index);

	  m_outputTree->Branch("mbts_countA",&t_mbts_countA,"mbts_countA/s");
	  m_outputTree->Branch("mbts_countC",&t_mbts_countC,"mbts_countC/s");
	  m_outputTree->Branch("T2mbts_countAin",&t_T2mbts_countAin,"T2mbts_countAin/s");
	  m_outputTree->Branch("T2mbts_countCin",&t_T2mbts_countCin,"T2mbts_countCin/s");
	  m_outputTree->Branch("mbts_timeA",&t_mbts_timeA,"mbts_timeA/F");
	  m_outputTree->Branch("mbts_timeC",&t_mbts_timeC,"mbts_timeC/F");
	  m_outputTree->Branch("mbts_timeDiff",&t_mbts_timeDiff,"mbts_timeDiff/F");
	  
	  t_nclus = 0;
	  m_outputTree->Branch("nclus",&t_nclus,"nclus/i");
	  m_outputTree->Branch("clusEt",&t_clusEt,"clusEt/F");
	  m_outputTree->Branch("clusEtMax",&t_clusEtMax,"clusEtMax/F");
	  m_outputTree->Branch("clusetaMax",&t_clusetaMax,"clusetaMax/F");
	  m_outputTree->Branch("clusphiMax",&t_clusphiMax,"clusphiMax/F");
	  
	  m_outputTree->Branch("cc_pt","vector<float>",&t_cc_pt);
	  m_outputTree->Branch("cc_eta","vector<float>",&t_cc_eta);
	  m_outputTree->Branch("cc_phi","vector<float>",&t_cc_phi);
	  m_outputTree->Branch("cc_e","vector<float>",&t_cc_e);
	  m_outputTree->Branch("cc_raw_m","vector<float>",&t_cc_raw_m);
	  m_outputTree->Branch("cc_raw_eta","vector<float>",&t_cc_raw_eta);
	  m_outputTree->Branch("cc_raw_phi","vector<float>",&t_cc_raw_phi);
	  m_outputTree->Branch("cc_raw_e","vector<float>",&t_cc_raw_e);
	  m_outputTree->Branch("cc_raw_samp","vector<vector<float>>",&t_cc_raw_samp);
	  m_outputTree->Branch("cc_sig","vector<float>",&t_cc_sig);
	  m_outputTree->Branch("cc_layer","vector<int>",&t_cc_layer);

	  if (enableTT)
	    {	      
	      m_outputTree->Branch("fcalEtA_TTsum",&t_fcalEtA_TTsum,"fcalEtA_TTsum/F");
	      m_outputTree->Branch("fcalEtC_TTsum",&t_fcalEtC_TTsum,"fcalEtC_TTsum/F");
	      m_outputTree->Branch("tt_pt","vector<float>",&t_tt_pt);
	      m_outputTree->Branch("tt_eta","vector<float>",&t_tt_eta);
	      m_outputTree->Branch("tt_phi","vector<float>",&t_tt_phi);
	      m_outputTree->Branch("tt_e","vector<float>",&t_tt_e);
	      m_outputTree->Branch("tt_samp","vector<uint8_t>",&t_tt_samp);
	    }

	  m_outputTree->Branch("edgeGapA",&t_edgeGapA,"edgeGapA/F");
	  m_outputTree->Branch("edgeGapC",&t_edgeGapC,"edgeGapC/F");

	  int njetCollections = m_jetContainerNames.size();
	  t_njet.resize(njetCollections);
	  t_jet_pt.resize(njetCollections);
	  t_jet_eta.resize(njetCollections);
	  t_jet_phi.resize(njetCollections);
	  t_jet_m.resize(njetCollections);
	  t_jet_acc.resize(njetCollections);

	  for (size_t ijet = 0;ijet<m_jetContainerNames.size();ijet++)
	    {      
	      TString jetName(m_jetContainerNames.at(ijet).c_str());
	      m_outputTree->Branch(jetName+"_n",&t_njet[ijet],jetName+"_n/I");
	      if (enableJets)
		{
		  m_outputTree->Branch(jetName+"_pt","vector<float>",&t_jet_pt[ijet]);
		  m_outputTree->Branch(jetName+"_eta","vector<float>",&t_jet_eta[ijet]);
		  m_outputTree->Branch(jetName+"_phi","vector<float>",&t_jet_phi[ijet]);
		  m_outputTree->Branch(jetName+"_m","vector<float>",&t_jet_m[ijet]);
		  m_outputTree->Branch(jetName+"_acc","vector<uint8_t>",&t_jet_acc[ijet]);
		}
	      
	    }
	  
	  
	  t_nmuon = 0;
	  m_outputTree->Branch("nmuon",&t_nmuon,"nmuon/I");
	  if (enableMuons)
	    {
	      m_outputTree->Branch("muon_pt","vector<float>",&t_muon_pt);
	      m_outputTree->Branch("muon_eta","vector<float>",&t_muon_eta);
	      m_outputTree->Branch("muon_phi","vector<float>",&t_muon_phi);
	      m_outputTree->Branch("muon_m","vector<float>",&t_muon_m);
	      m_outputTree->Branch("muon_charge","vector<int8_t>",&t_muon_charge);	  
	      m_outputTree->Branch("muon_author","vector<int8_t>",&t_muon_author);	  
	      m_outputTree->Branch("muon_acc","vector<uint8_t>",&t_muon_acc);
	      m_outputTree->Branch("muon_qual","vector<uint8_t>",&t_muon_qual);
	      m_outputTree->Branch("muon_l1match","vector<int8_t>",&t_muon_l1match);
	      m_outputTree->Branch("muon_l1dR","vector<float>",&t_muon_l1dR);
	      m_outputTree->Branch("muon_iso","vector<bool>",&t_muon_iso);
	      m_outputTree->Branch("muon_cluster_E","vector<float>",&t_muon_cluster_E);
	      m_outputTree->Branch("muon_cluster_t","vector<float>",&t_muon_cluster_t);
	      m_outputTree->Branch("muon_MS_pt","vector<float>",&t_muon_MS_pt);
	      m_outputTree->Branch("muon_MS_pt_orig","vector<float>",&t_muon_MS_pt_orig);
	      m_outputTree->Branch("muon_MS_eta","vector<float>",&t_muon_MS_eta);
	      m_outputTree->Branch("muon_MS_phi","vector<float>",&t_muon_MS_phi);
	      m_outputTree->Branch("muon_ID_pt","vector<float>",&t_muon_ID_pt);
	      m_outputTree->Branch("muon_ID_index","vector<int>",&t_muon_ID_index);
	      m_outputTree->Branch("muon_MSEx_pt","vector<float>",&t_muon_MSEx_pt);	  
	      m_outputTree->Branch("muon_MSEx_eta","vector<float>",&t_muon_MSEx_eta);	  
	      m_outputTree->Branch("muon_MSEx_phi","vector<float>",&t_muon_MSEx_phi);	  
	      m_outputTree->Branch("muon_nPrecision","vector<uint8_t>",&t_muon_nPrecision);
	      m_outputTree->Branch("muon_nPrecisionHoles","vector<uint8_t>",&t_muon_nPrecisionHoles);
	      m_outputTree->Branch("muon_hasTrack","vector<uint8_t>",&t_muon_hasTrack);
	      m_outputTree->Branch("muon_nPixHits","vector<uint8_t>",&t_muon_nPixHits);
	      m_outputTree->Branch("muon_nPixDead","vector<uint8_t>",&t_muon_nPixDead);
	      m_outputTree->Branch("muon_nPixHoles","vector<uint8_t>",&t_muon_nPixHoles);
	      m_outputTree->Branch("muon_nSctHits","vector<uint8_t>",&t_muon_nSctHits);
	      m_outputTree->Branch("muon_nSctDead","vector<uint8_t>",&t_muon_nSctDead);
	      m_outputTree->Branch("muon_nSctHoles","vector<uint8_t>",&t_muon_nSctHoles);
	      m_outputTree->Branch("muon_nTrtHits","vector<uint8_t>",&t_muon_nTrtHits);
	      m_outputTree->Branch("muon_nTrtOutliers","vector<uint8_t>",&t_muon_nTrtOutliers);
	      m_outputTree->Branch("muon_d0","vector<float>",&t_muon_d0);
	      m_outputTree->Branch("muon_z0","vector<float>",&t_muon_z0);
	      m_outputTree->Branch("muon_vtxz","vector<float>",&t_muon_vtxz);
	      m_outputTree->Branch("muon_theta","vector<float>",&t_muon_theta);
	      m_outputTree->Branch("muon_rho","vector<float>",&t_muon_rho);
	      m_outputTree->Branch("muon_qOverPsigma","vector<float>",&t_muon_qOverPsigma);
	      m_outputTree->Branch("muon_qOverPsignif","vector<float>",&t_muon_qOverPsignif);
	      m_outputTree->Branch("muon_reducedChi2","vector<float>",&t_muon_reducedChi2);
	      m_outputTree->Branch("muon_type","vector<int8_t>",&t_muon_type);
	      
	      m_outputTree->Branch("muon_eff","vector<float>",&t_muon_eff);
	      m_outputTree->Branch("muon_sf","vector<float>",&t_muon_sf);
	      m_outputTree->Branch("muon_sfvar","vector< vector<float> >",&t_muon_sfvar);
	      m_outputTree->Branch("muon_sfvarLowpT","vector< vector<float> >",&t_muon_sfvarLowpT);
	      if (m_isMC)
		{
		  m_outputTree->Branch("muon_smear_pt","vector< vector<float> >",&t_muon_smear_pt);
		  m_outputTree->Branch("muon_smear_charge","vector< vector<int8_t> >",&t_muon_smear_charge);
		  m_outputTree->Branch("muon_smear_ID","vector< vector<float> >",&t_muon_smear_ID);
		  m_outputTree->Branch("muon_smear_MS","vector< vector<float> >",&t_muon_smear_MS);
		}
	      
	      
	      m_outputTree->Branch("ndimuon",&t_ndimuon,"ndimuon/I");
	      m_outputTree->Branch("dimuon_mu0","vector<int8_t>",&t_dimuon_mu0);
	      m_outputTree->Branch("dimuon_mu1","vector<int8_t>",&t_dimuon_mu1);
	      m_outputTree->Branch("dimuon_m","vector<float>",&t_dimuon_m);
	      m_outputTree->Branch("dimuon_rap","vector<float>",&t_dimuon_rap);
	      m_outputTree->Branch("dimuon_phi","vector<float>",&t_dimuon_phi);
	      m_outputTree->Branch("dimuon_pt","vector<float>",&t_dimuon_pt);

	      t_nmuroi = 0;
	      m_outputTree->Branch("nmuroi",&t_nmuroi,"nmuroi/I");

	      m_outputTree->Branch("muroi_eta","vector<float>",&t_muroi_eta);
	      m_outputTree->Branch("muroi_phi","vector<float>",&t_muroi_phi);
	      m_outputTree->Branch("muroi_thr","vector<float>",&t_muroi_thr);

	      m_outputTree->Branch("nmsex",&t_nmsex,"nmsex/I");
	      m_outputTree->Branch("msex_eta","vector<float>",&t_msex_eta);
	      m_outputTree->Branch("msex_phi","vector<float>",&t_msex_phi);
	      m_outputTree->Branch("msex_pt","vector<float>",&t_msex_pt);
	      m_outputTree->Branch("msex_m","vector<float>",&t_msex_m);
	      m_outputTree->Branch("msex_charge","vector<int8_t>",&t_msex_charge);
	      m_outputTree->Branch("msex_nPrecision","vector<uint8_t>",&t_msex_nPrecision);
	      m_outputTree->Branch("msex_nPrecisionHoles","vector<uint8_t>",&t_msex_nPrecisionHoles);

	      if (enableMuons && enableTruth)
		{
		  t_ntrmu = 0;
		  m_outputTree->Branch("trmu_pt","vector<float>",&t_trmu_pt);
		  m_outputTree->Branch("trmu_eta","vector<float>",&t_trmu_eta);
		  m_outputTree->Branch("trmu_phi","vector<float>",&t_trmu_phi);
		  m_outputTree->Branch("trmu_m","vector<float>",&t_trmu_m);
		  m_outputTree->Branch("trmu_vx","vector<float>",&t_trmu_vx);
		  m_outputTree->Branch("trmu_vy","vector<float>",&t_trmu_vy);
		  m_outputTree->Branch("trmu_vz","vector<float>",&t_trmu_vz);
		  m_outputTree->Branch("trmu_pdgId","vector<int>",&t_trmu_pdgId);
		  m_outputTree->Branch("trmu_barcode","vector<int>",&t_trmu_barcode);
		  m_outputTree->Branch("trmu_status","vector<int>",&t_trmu_status);
		  m_outputTree->Branch("trmu_muindex","vector<int>",&t_trmu_muindex);
		  m_outputTree->Branch("trmu_L1MUindex","vector<int>",&t_trmu_L1MUindex);
		  m_outputTree->Branch("trmu_muDr","vector<float>",&t_trmu_muDr);
		  m_outputTree->Branch("trmu_L1MUDr","vector<float>",&t_trmu_L1MUDr);	  
		  m_outputTree->Branch("ntrmu",&t_ntrmu,"ntrmu/I");
		}
	    }
	  t_ntrk = 0;
	  m_outputTree->Branch("ntrk",&t_ntrk,"ntrk/i");
	  if (enableTracks)
	    {
	      m_outputTree->Branch("trk_pt","vector<float>",&t_trk_pt);
	      m_outputTree->Branch("trk_eta","vector<float>",&t_trk_eta);
	      m_outputTree->Branch("trk_theta","vector<float>",&t_trk_theta);
	      m_outputTree->Branch("trk_phi","vector<float>",&t_trk_phi);
	      m_outputTree->Branch("trk_e","vector<float>",&t_trk_e);
	      m_outputTree->Branch("trk_index","vector<int>",&t_trk_index);
	      m_outputTree->Branch("trk_d0","vector<float>",&t_trk_d0);
	      m_outputTree->Branch("trk_z0","vector<float>",&t_trk_z0);
	      m_outputTree->Branch("trk_vz","vector<float>",&t_trk_vz);
	      m_outputTree->Branch("trk_vtxz","vector<float>",&t_trk_vtxz);
	      m_outputTree->Branch("trk_pixeldEdx","vector<float>",&t_trk_pixeldEdx);
	      m_outputTree->Branch("trk_charge","vector<int8_t>",&t_trk_charge);
	      m_outputTree->Branch("trk_quality","vector<int16_t>",&t_trk_quality);
	      m_outputTree->Branch("trk_nPixHits","vector<uint8_t>",&t_trk_nPixHits);
	      m_outputTree->Branch("trk_nSctHits","vector<uint8_t>",&t_trk_nSctHits);
	      m_outputTree->Branch("trk_nPixDead","vector<uint8_t>",&t_trk_nPixDead);
	      m_outputTree->Branch("trk_nSctDead","vector<uint8_t>",&t_trk_nSctDead);
	      m_outputTree->Branch("trk_nPixHoles","vector<uint8_t>",&t_trk_nPixHoles);
	      m_outputTree->Branch("trk_nSctHoles","vector<uint8_t>",&t_trk_nSctHoles);
	      m_outputTree->Branch("trk_nTrtHits","vector<uint8_t>",&t_trk_nTrtHits);
	      m_outputTree->Branch("trk_nTrtOutliers","vector<uint8_t>",&t_trk_nTrtOutliers);
	      m_outputTree->Branch("trk_inPixHits","vector<uint8_t>",&t_trk_inPixHits);
	      m_outputTree->Branch("trk_exPixHits","vector<uint8_t>",&t_trk_exPixHits);
	      m_outputTree->Branch("trk_ninPixHits","vector<uint8_t>",&t_trk_ninPixHits);
	      m_outputTree->Branch("trk_nexPixHits","vector<uint8_t>",&t_trk_nexPixHits);	  
	    }
	 

	  m_outputTree->Branch("nphoton",&t_nphoton,"nphoton/I");
	  if (enablePhotons)
	    {
	      m_outputTree->Branch("ph_author","vector<uint8_t>",&t_ph_author);
	      m_outputTree->Branch("ph_conv","vector<uint8_t>",&t_ph_conv);
	      m_outputTree->Branch("ph_oq","vector<uint32_t>",&t_ph_oq);
	      m_outputTree->Branch("ph_pt","vector<float>",&t_ph_pt);
	      m_outputTree->Branch("ph_eta","vector<float>",&t_ph_eta);
	      m_outputTree->Branch("ph_eta2","vector<float>",&t_ph_eta2);
	      m_outputTree->Branch("ph_phi","vector<float>",&t_ph_phi);
	      m_outputTree->Branch("ph_iso","vector<bool>",&t_ph_iso);
	      m_outputTree->Branch("ph_e","vector<float>",&t_ph_e);
	      m_outputTree->Branch("ph_isEMTight","vector<uint32_t>",&t_ph_isEMTight);
	      m_outputTree->Branch("ph_isEMLoose","vector<uint32_t>",&t_ph_isEMLoose);
	      m_outputTree->Branch("ph_tight","vector<uint8_t>",&t_ph_tight);
	      m_outputTree->Branch("ph_loose","vector<uint8_t>",&t_ph_loose);
	      m_outputTree->Branch("ph_topoetcone40","vector<float>",&t_ph_topoetcone40);
	      m_outputTree->Branch("ph_etcone40","vector<float>",&t_ph_etcone40);
	      //m_outputTree->Branch("ph_HLT_match","vector<bool>",&t_ph_HLT_match);
	      //m_outputTree->Branch("ph_HLT_dR","vector<float>",&t_ph_HLT_dR);
	    }

	  m_outputTree->Branch("nelectron",&t_nelectron,"nelectron/I");
	  if (enableElectrons)
	    {
	      m_outputTree->Branch("el_author","vector<uint8_t>",&t_el_author);
	      m_outputTree->Branch("el_oq","vector<uint32_t>",&t_el_oq);
	      m_outputTree->Branch("el_pt","vector<float>",&t_el_pt);
	      m_outputTree->Branch("el_charge","vector<int8_t>",&t_el_charge);
	      m_outputTree->Branch("el_eta","vector<float>",&t_el_eta);
	      m_outputTree->Branch("el_eta2","vector<float>",&t_el_eta2);
	      m_outputTree->Branch("el_phi","vector<float>",&t_el_phi);
	      m_outputTree->Branch("el_e","vector<float>",&t_el_e);
	      m_outputTree->Branch("el_trackpt","vector<float>",&t_el_trackpt);
	      m_outputTree->Branch("el_trk_index","vector<int>",&t_el_trk_index);
	      m_outputTree->Branch("el_d0","vector<float>",&t_el_d0);
	      m_outputTree->Branch("el_sigmad0","vector<float>",&t_el_sigmad0);
	      m_outputTree->Branch("el_z0","vector<float>",&t_el_z0);
	      m_outputTree->Branch("el_vz","vector<float>",&t_el_vz);
	      m_outputTree->Branch("el_vtxz","vector<float>",&t_el_vtxz);
	      m_outputTree->Branch("el_theta","vector<float>",&t_el_theta);
	      m_outputTree->Branch("el_isEMMediumt","vector<uint32_t>",&t_el_isEMMedium);
	      m_outputTree->Branch("el_isEMLoose","vector<uint32_t>",&t_el_isEMLoose);
	      m_outputTree->Branch("el_medium","vector<uint8_t>",&t_el_medium);
	      m_outputTree->Branch("el_loose","vector<uint8_t>",&t_el_loose);
	      m_outputTree->Branch("el_lhmedium","vector<uint8_t>",&t_el_lhmedium);
	      m_outputTree->Branch("el_lhloose","vector<uint8_t>",&t_el_lhloose);
	      m_outputTree->Branch("el_topoetcone20","vector<float>",&t_el_topoetcone20);
	      m_outputTree->Branch("el_etcone20","vector<float>",&t_el_etcone20);
	      //m_outputTree->Branch("el_HLT_match","vector<bool>",&t_el_HLT_match);
	      //m_outputTree->Branch("el_HLT_dR","vector<float>",&t_el_HLT_dR);
	      m_outputTree->Branch("el_iso","vector<bool>",&t_el_iso);
	    }	  
	}

      /*
      m_outputTree->Branch("nTjet",&t_nTjet,"nTjet/I");
      if (enableTriggerJets)
	{
	  m_outputTree->Branch("Tjet_pt","vector<float>",&t_Tjet_pt);
	  m_outputTree->Branch("Tjet_eta","vector<float>",&t_Tjet_eta);
	  m_outputTree->Branch("Tjet_phi","vector<float>",&t_Tjet_phi);
	  m_outputTree->Branch("Tjet_m","vector<float>",&t_Tjet_m);
	  m_outputTree->Branch("Tjet_acc","vector<uint8_t>",&t_Tjet_acc);
	}
      */
      
      //wk()->addOutput(m_outputTree);
    }

  ANA_MSG_INFO("Anti-howdy from histInitialize!");


  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  ANA_MSG_INFO("enableOutputTree = " << enableOutputTree);
  ANA_MSG_INFO("enableOutputSamples = " << enableOutputSamples);
  ANA_MSG_INFO("enableTrigger = " << enableTrigger);
  ANA_MSG_INFO("zdcCalib = " << zdcCalib);
  ANA_MSG_INFO("zdcLaser = " << zdcLaser);
  ANA_MSG_INFO("reprocZdc = " << reprocZdc);
  ANA_MSG_INFO("zdcLowGainOnly = " << zdcLowGainOnly);
  ANA_MSG_INFO("enableTracks = " << enableTracks);
  ANA_MSG_INFO("enableClusters = " << enableClusters);
  ANA_MSG_INFO("enableJets = "<< enableJets);
  ANA_MSG_INFO("enableTriggerJets = " << enableTriggerJets);
  ANA_MSG_INFO("enableMuons = " << enableMuons);
  ANA_MSG_INFO("trackLimit = " << trackLimit);
  ANA_MSG_INFO("trackLimitReject = " << trackLimitReject);
  ANA_MSG_INFO("auxSuffix = " << auxSuffix );
  
  if (auxSuffix != "") auxSuffix = "_" + auxSuffix; // prepend "_"

  //xAOD::TEvent* event = wk()->xaodEvent();
  ANA_MSG_DEBUG("initialize: Initialize!");

  // Event
  //ANA_CHECK(evtStore()->retrieve( m_eventInfo, "EventInfo"));   // Now 

  if (enableTrigger)
    {
      // Trigger
      ANA_MSG_INFO("Initializing trigConfigTool");
      ANA_CHECK (m_trigConfigTool.initialize());
      ANA_MSG_INFO("Initializing trigDecisionTool");
      ANA_CHECK (m_trigDecisionTool.setProperty ("ConfigTool", m_trigConfigTool.getHandle())); // connect the TrigDecisionTool to the ConfigTool
      ANA_CHECK (m_trigDecisionTool.setProperty ("TrigDecisionKey", "xTrigDecision"));
      ANA_CHECK (m_trigDecisionTool.initialize());
    }

  // GRL

  if (useGRL)
    {
      //const char* GRLFilePath = "$ROOTCOREBIN/data/HIMBAnalysis/LB_collection_PbPb2015.xml";
      //const char* GRLFilePath = "$ROOTCOREBIN/data/ZdcNtuple/data15_hi.periodAllYear_DetStatus-v75-pro19-09_DQDefects-00-02-02_PHYS_HeavyIonP_All_Good.UPC.xml";
      //const char* fullGRLFilePath = gSystem->ExpandPathName (GRLFilePath);
      const char* fullGRLFilePath = gSystem->ExpandPathName (grlFilename.c_str());
      ANA_MSG_INFO("GRL: " << fullGRLFilePath);
      std::vector<std::string> vecStringGRL;
      vecStringGRL.push_back(fullGRLFilePath);
      ANA_CHECK(m_grl.setProperty( "GoodRunsListVec", vecStringGRL));
      ANA_CHECK(m_grl.setProperty("PassThrough", false)); // if true (default) will ignore result of GRL and will just pass all events
      ANA_CHECK(m_grl.initialize());
      /*
	m_grl_mb = new GoodRunsListSelectionTool("GoodRunsListSelectionTool_MB");
	//const char* GRLFilePath = "$ROOTCOREBIN/data/HIMBAnalysis/LB_collection_PbPb2015.xml";
	const char* GRLFilePath_mb = "$ROOTCOREBIN/data/ZdcNtuple/data15_hi.periodAllYear_DetStatus-v75-repro20-01_DQDefects-00-02-02_PHYS_HeavyIonP_All_Good.xml";
	const char* fullGRLFilePath_mb = gSystem->ExpandPathName (GRLFilePath_mb);
	std::vector<std::string> vecStringGRL_mb;
	vecStringGRL_mb.push_back(fullGRLFilePath_mb);
	ANA_CHECK(m_grl_mb->setProperty( "GoodRunsListVec", vecStringGRL_mb));
	ANA_CHECK(m_grl_mb->setProperty("PassThrough", false)); // if true (default) will ignore result of GRL and will just pass all events
	ANA_CHECK(m_grl_mb->initialize());
      */
    }

  // Jet cleaning tool
  //m_jetCleaning = new JetCleaningTool("JetCleaning");
  //m_jetCleaning.msg().setLevel( MSG::DEBUG ); 
  ANA_CHECK(m_jetCleaning.setProperty( "CutLevel", "LooseBad"));
  ANA_CHECK(m_jetCleaning.setProperty("DoUgly", false));
  ANA_CHECK(m_jetCleaning.initialize());

  // B tagger
  /*
  m_btagtool = new BTaggingSelectionTool("BTaggingSelectionTool");
  ANA_CHECK(m_btagtool->setProperty("MaxEta", 2.5));
  ANA_CHECK(m_btagtool->setProperty("MinPt", 20000.));
  ANA_CHECK(m_btagtool->setProperty("JetAuthor", "AntiKt4EMTopoJets"));
  ANA_CHECK(m_btagtool->setProperty("TaggerName", "MV2c10"));
  //2014-Winter-8TeV-MC12-CDI.root
  //2016-20_7-13TeV-MC15-CDI-2016-11-25_v1.root
  ANA_CHECK(m_btagtool->setProperty("FlvTagCutDefinitionsFileName", "xAODBTaggingEfficiency/13TeV/2016-20_7-13TeV-MC15-CDI-2016-11-25_v1.root" ));
  ANA_CHECK(m_btagtool->setProperty("OperatingPoint", "FixedCutBEff_70"));
  ANA_CHECK(m_btagtool->initialize());

  m_btagtool_tj = new BTaggingSelectionTool("BTaggingSelectionTool_TJ"); // for track jets
  ANA_CHECK(m_btagtool_tj->setProperty("MaxEta", 2.5));
  ANA_CHECK(m_btagtool_tj->setProperty("MinPt", 7000.));
  ANA_CHECK(m_btagtool_tj->setProperty("JetAuthor", "AntiKt4PV0TrackJets"));
  ANA_CHECK(m_btagtool_tj->setProperty("TaggerName", "MV2c10"));
  ANA_CHECK(m_btagtool_tj->setProperty("FlvTagCutDefinitionsFileName", "xAODBTaggingEfficiency/13TeV/2016-20_7-13TeV-MC15-CDI-2016-11-25_v1.root" ));
  ANA_CHECK(m_btagtool_tj->setProperty("OperatingPoint", "FixedCutBEff_70"));
  ANA_CHECK(m_btagtool_tj->initialize());
  */

  // Muon selection tool
  //m_muonSelectionTool = new CP::MuonSelectionTool("MuonSelection");
  ANA_CHECK(m_muonSelection.setProperty("MaxEta",2.7));
  ANA_CHECK(m_muonSelection.setProperty("MuQuality",3));
  //m_muonSelectionTool->msg().setLevel(MSG::VERBOSE);
  ANA_CHECK(m_muonSelection.initialize());

  //m_muonTrigMatchTool = 0;
  //m_trigEgammaMatchingTool = 0;
  if (enableTrigger)
    {

      if (!upc2015 && !mb2015 && !upcL2015 && !upc2018)
	{
	  m_muon_triggers.push_back("HLT_mu15_L1MU10");
	  m_muon_triggers.push_back("HLT_mu20_L1MU15");
	  
	  m_dimuon_triggers.push_back("HLT_2mu4");
	  m_dimuon_triggers.push_back("HLT_2mu4_nomucomb");

	  m_el_triggers.push_back("HLT_e17_lhloose");
	  m_el_triggers.push_back("HLT_e17_lhloose_nod0");
	  
	  m_ph_triggers.push_back("HLT_g35_loose");
	}

      if (upc2018)
	{
	  m_muon_triggers.push_back("HLT_mu4_L1MU4_VTE50");
	  m_muon_triggers.push_back("HLT_mu4_L1MU4_VZDC_AORC_VTE200");
	  m_muon_triggers.push_back("HLT_mu6_L1MU4_VTE50");
	  m_muon_triggers.push_back("HLT_mu8_L1MU6_VTE50");
	  m_muon_triggers.push_back("HLT_mu4_hi_upc_FgapAC3_L1MU4_VTE50");
	}

      ANA_MSG_INFO("Initializing trig matching tool");
      ANA_CHECK(m_trigMatchingTool.setProperty("TrigDecisionTool", m_trigDecisionTool.getHandle())); 
      ANA_CHECK(m_trigMatchingTool.initialize());           
      /*
      m_muonTrigMatchTool = new Trig::TrigMuonMatching("MuonTrigMatchTool");
      ANA_CHECK(m_muonTrigMatchTool->setProperty("TriggerTool",trigDecisionHandle));
      ANA_CHECK(m_muonTrigMatchTool->initialize());
      */
      /*
      m_trigEgammaMatchingTool = new Trig::TrigEgammaMatchingTool("EgammaTrigMatchTool");
      ANA_CHECK(m_trigEgammaMatchingTool->setProperty("TriggerTool",trigDecisionHandle));
      ANA_CHECK(m_trigEgammaMatchingTool->initialize());
      */

    }


  //m_muonCalibrationAndSmearingTool = new CP::MuonCalibrationAndSmearingTool("MuonCalibration");
  //m_muonCalibrationAndSmearingTool->setProperty( "Release", "Recs_2016_01_19" );
  ANA_CHECK(m_muonCalibrationAndSmearingTool.initialize());
  std::string muVars[3] = {"MUONS_ID","MUONS_MS","MUONS_SCALE"};
  for (int ivar=0;ivar<3;ivar++)
    {
      muonSysList.push_back( CP::SystematicSet() );
      muonSysList.back().insert( CP::SystematicVariation(muVars[ivar],1) );
      muonSysList.push_back( CP::SystematicSet() );
      muonSysList.back().insert( CP::SystematicVariation(muVars[ivar],-1) );
    }

  /*
  const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
  muonSysList.push_back( CP::SystematicSet() );
  for( CP::SystematicSet::const_iterator sysItr = recommendedSystematics.begin(); sysItr != recommendedSystematics.end(); ++sysItr ) {
    muonSysList.push_back( CP::SystematicSet() );
    muonSysList.back().insert( *sysItr );
    ANA_MSG_INFO("syst: %s", sysItr->name().c_str());
  }
  */

  /*
  m_muonEfficiencyScaleFactors = new CP::MuonEfficiencyScaleFactors("MuonScaleFactors");
  //m_muonEfficiencyScaleFactors->setProperty("WorkingPoint","Tight");
  m_sysup.insert(CP::SystematicVariation ("MUON_EFF_SYS", 1));
  m_sysdown.insert(CP::SystematicVariation ("MUON_EFF_SYS", -1));
  m_statup.insert(CP::SystematicVariation ("MUON_EFF_STAT", 1));
  m_statdown.insert(CP::SystematicVariation ("MUON_EFF_STAT", -1));
  m_sysupLowpT.insert(CP::SystematicVariation ("MUON_EFF_SYS_LOWPT", 1));
  m_sysdownLowpT.insert(CP::SystematicVariation ("MUON_EFF_SYS_LOWPT", -1));
  m_statupLowpT.insert(CP::SystematicVariation ("MUON_EFF_STAT_LOWPT", 1));
  m_statdownLowpT.insert(CP::SystematicVariation ("MUON_EFF_STAT_LOWPT", -1));
  */

  // serious problem with PileupReweighting here
  //ANA_CHECK(m_muonEfficiencyScaleFactors->initialize());

  std::string confDir = "ElectronPhotonSelectorTools/offline/mc15_20150712/";

  // photon initialize
  //m_photonTightIsEMSelector = new AsgPhotonIsEMSelector ( "PhotonTightIsEMSelector" );

  /*
  m_photonTightIsEMSelector.setProperty("isEMMask",egammaPID::PhotonTight);
  m_photonTightIsEMSelector.setProperty("ConfigFile",confDir+"PhotonIsEMTightSelectorCutDefs.conf");
  ANA_CHECK(m_photonTightIsEMSelector.initialize());

  m_photonLooseIsEMSelector = new AsgPhotonIsEMSelector ( "PhotonLooseIsEMSelector" );
  m_photonLooseIsEMSelector.setProperty("isEMMask",egammaPID::PhotonLoose);
  m_photonLooseIsEMSelector.setProperty("ConfigFile",confDir+"PhotonIsEMLooseSelectorCutDefs.conf");
  ANA_CHECK(m_photonLooseIsEMSelector.initialize());
  */

  // electron initialize
  //m_electronLooseLHSelector = new AsgElectronLikelihoodTool ("LooseLH");
  //ANA_CHECK(m_electronLooseLHSelector.setProperty("WorkingPoint", "LooseLHElectron"));
  ANA_CHECK(m_electronLooseLHSelector.setProperty("ConfigFile",confDir+"ElectronLikelihoodLooseOfflineConfig2015.conf"));
  ANA_CHECK(m_electronLooseLHSelector.initialize());

  //m_electronMediumLHSelector = new AsgElectronLikelihoodTool ("MediumLH");
  //ANA_CHECK(m_electronMediumLHSelector.setProperty("WorkingPoint", "LooseLHElectron"));
  ANA_CHECK(m_electronMediumLHSelector.setProperty("ConfigFile",confDir+"ElectronLikelihoodMediumOfflineConfig2015.conf"));
  ANA_CHECK(m_electronMediumLHSelector.initialize());

  //m_electronLooseEMSelector = new AsgElectronIsEMSelector("electronLooseIsEMSelector");
  ANA_CHECK(m_electronLooseEMSelector.setProperty("ConfigFile",confDir+"ElectronIsEMLooseSelectorCutDefs.conf"));
  ANA_CHECK(m_electronLooseEMSelector.initialize());

  //m_electronMediumEMSelector = new AsgElectronIsEMSelector("electronMediumIsEMSelector");
  ANA_CHECK(m_electronMediumEMSelector.setProperty("ConfigFile",confDir+"ElectronIsEMMediumSelectorCutDefs.conf"));
  ANA_CHECK(m_electronMediumEMSelector.initialize());

  //m_isoTool = new CP::IsolationSelectionTool( "isoTool" );
  /*
  ANA_CHECK(m_isoTool.setProperty("PhotonWP","FixedCutTight"));
  ANA_CHECK(m_isoTool.setProperty("MuonWP","Gradient"));
  ANA_CHECK(m_isoTool.setProperty("ElectronWP","Gradient"));
  ANA_CHECK(m_isoTool.initialize());
  */
  // tracks

  // Initialize array of track selector tools
  /*
  if (enableTracks)
    {
      std::vector<std::string> trackselections;
      trackselections.push_back("LoosePrimary");
      trackselections.push_back("TightPrimary");
      trackselections.push_back("LooseMuon");
      trackselections.push_back("MinBias");
      //trackselections.push_back("HILoose");
      //trackselections.push_back("HITight");

      for(const auto selection : trackselections)
	{
	  m_trackSelectionTools.push_back(new InDet::InDetTrackSelectionTool(selection));
	  ANA_CHECK(m_trackSelectionTools.back()->setProperty("CutLevel", selection));
	  ANA_CHECK(m_trackSelectionTools.back()->initialize());
	}
    }
  */

  // ZDC re-reco tool
  //m_zdcRecTool = new ZDC::ZdcRecTool("ZdcRecTool");
  if (reprocZdc)
    {

      m_zdcAnalysisTool.setProperty("FlipEMDelay",flipDelay);
      m_zdcAnalysisTool.setProperty("LowGainOnly",zdcLowGainOnly);
      m_zdcAnalysisTool.setProperty("DoCalib",doZdcCalib);
      m_zdcAnalysisTool.setProperty("Configuration",zdcConfig);
      m_zdcAnalysisTool.setProperty("AuxSuffix",auxSuffix);
      m_zdcAnalysisTool.setProperty("ForceCalibRun",-1);

      if (zdcConfig=="PbPb2018")
	{
	  m_zdcAnalysisTool.setProperty("T0",68); 
	  m_zdcAnalysisTool.setProperty("DoTrigEff",false); // for now
	  m_zdcAnalysisTool.setProperty("DoTimeCalib",false); // for now
	}
      else if (zdcConfig=="pPb2016")
	{
	  m_zdcAnalysisTool.setProperty("Configuration","pPb2016");	  
	}
      else if (zdcConfig=="PbPb2015")
	{
	  m_zdcAnalysisTool.setProperty("Configuration","PbPb2015");
	}

      if (flipDelay) 
	ANA_MSG_INFO("FLIP ZDC DELAY IN EM MODULES");
      else
	ANA_MSG_INFO("NO FLIP ZDC DELAY IN EM MODULES");

      ANA_CHECK(m_zdcAnalysisTool.initialize());

      /*
      m_zdcAnalysisTool.setProperty("Configuration","PbPb2018");
      m_zdcAnalysisTool.setProperty("FlipEMDelay",flipDelay);
      m_zdcAnalysisTool.setProperty("LowGainOnly",zdcLowGainOnly);
      m_zdcAnalysisTool.setProperty("ForceCalibRun",-1);
      m_zdcAnalysisTool.setProperty("AuxSuffix",auxSuffix);
      m_zdcAnalysisTool.setProperty("DoCalib",false);
      if (flipDelay) 
	ANA_MSG_INFO("FLIP ZDC DELAY IN EM MODULES");
      else
	ANA_MSG_INFO("NO FLIP ZDC DELAY IN EM MODULES");
      
      ANA_CHECK(m_zdcAnalysisTool->initialize());
      ANA_MSG_INFO( "ZDC analysis tool initialized?");
      */

    }


  // initialize gaps
  h_TCSigCut=new TH1F("h_TCSigCut","TopoCluster significance cuts",98,-4.9,4.9);
  for(int xbin=0; xbin<h_TCSigCut->GetNbinsX(); xbin++)
    {
      h_TCSigCut->SetBinContent(xbin+1,m_gapThresholds[xbin]);
    }
  m_gapPtMin = 200;

  // as a check, let's see the number of events in our xAOD
  //ANA_MSG_INFO( "Number of events = "<< evtStore()->getEntries() ); // print long long int

  return StatusCode::SUCCESS;
}


/*
StatusCode ZdcNtuple :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return StatusCode::SUCCESS;
}
*/


/*
StatusCode ZdcNtuple :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  if (firstFile)
    {
      ANA_MSG_INFO("changeInput()","firstFile=true!");
    }
  return StatusCode::SUCCESS;
}
*/


StatusCode ZdcNtuple :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  //xAOD::TEvent* event = wk()->xaodEvent();

  // prefilter with a track limit

  if (!evtStore())
    {
      ANA_MSG_INFO("*** No event found! ***");
      return StatusCode::SUCCESS;
    }

  ANA_CHECK(evtStore()->retrieve( m_eventInfo, "EventInfo"));  
  processEventInfo();
 
  if (!m_setupTrigHist) setupTriggerHistos();

  //tracks used to go here

  m_trackParticles=0;

  if (!(zdcCalib||zdcLaser))
    {
      ANA_MSG_DEBUG("Trying to extract InDetTrackParticles from evtStore()=" << evtStore());
      ANA_CHECK(evtStore()->retrieve( m_trackParticles, "InDetTrackParticles") );
      size_t n = m_trackParticles->size();
      ANA_MSG_DEBUG("Done w/ extracting InDetTrackParticles with size = " << n);
      
      if (n>trackLimit && trackLimitReject)  return StatusCode::SUCCESS;
    }


  bool passTrigger = true;
  m_trigDecision=0;
  if (enableTrigger)
    {
      ANA_CHECK(evtStore()->retrieve( m_trigDecision, "xTrigDecision"));  
      passTrigger = processTriggerDecision();
    }

  if (!m_isMC)
    {
      if (reprocZdc) 
	{
	  m_zdcAnalysisTool->reprocessZdc();
	}
      else
	ANA_MSG_DEBUG ("No reprocessing");

      m_zdcSums=0;
      ANA_CHECK( evtStore()->retrieve( m_zdcSums, "ZdcSums"+auxSuffix) );      

      m_zdcModules=0;
      ANA_CHECK(evtStore()->retrieve( m_zdcModules, "ZdcModules" ) ); // ZDC modules keep same name, but the aux data get different suffix during reprocessing

      processModules();
      processZdcNtupleFromModules(); // same model in both cases -- processZdcNtuple() goes straight to the anlaysis tool, which is good for debugging

      /*
      if (reprocZdc) 
	{
	  processZdcNtuple();
	}
      else
	processZdcNtupleFromModules();
      */

    }

  if (!(zdcCalib||zdcLaser))
    {

      // disable this for 7660
      ANA_CHECK(evtStore()->retrieve( m_caloSums, "CaloSums") );
      ANA_CHECK(evtStore()->retrieve( m_eventShapes, "HIEventShape") );
      m_TTeventShapes = 0;
      /*
      if (t_runNumber>312000)
	{
	  ANA_CHECK(evtStore()->retrieve( m_TTeventShapes, "HLT_xAOD__HIEventShapeContainer_HIFCAL") );
	}
      */

      m_lvl1EnergySumRoI = 0;
      ANA_CHECK(evtStore()->retrieve( m_lvl1EnergySumRoI,"LVL1EnergySumRoI") );
      processFCal();

      ANA_CHECK(evtStore()->retrieve( m_mbtsInfo, "MBTSForwardEventInfo") );
      ANA_CHECK(evtStore()->retrieve( m_mbtsModules, "MBTSModules") );

      m_trigT2MbtsBits=0;
      ANA_CHECK(evtStore()->retrieve( m_trigT2MbtsBits, "HLT_xAOD__TrigT2MbtsBitsContainer_T2Mbts") );
      processMBTS();
      
      ANA_CHECK(evtStore()->retrieve( m_primaryVertices, "PrimaryVertices") );
      processInDet();


      for (size_t ijet=0;ijet<m_jetContainerNames.size();ijet++)
	{
	  ANA_CHECK(evtStore()->retrieve( m_jetContainers[ijet],m_jetContainerNames[ijet] ));
	}
      //ANA_CHECK(evtStore()->retrieve( m_AntiKt4HIJets,"AntiKt4HIJets"));
      //ANA_CHECK(evtStore()->retrieve( m_TrigAntiKt4HIJets,"HLT_xAOD__JetContainer_a4ionemsubjesFS"));
      processJets();

      ANA_CHECK(evtStore()->retrieve( m_muons,"Muons"));
      ANA_CHECK(evtStore()->retrieve( m_muonRoIContainer,"LVL1MuonRoIs"));
      ANA_CHECK(evtStore()->retrieve( m_extrapolatedMuonTrackParticles,"ExtrapolatedMuonTrackParticles"));
      processMuons();

      ANA_CHECK(evtStore()->retrieve( m_photons,"Photons"));
      processPhotons();

      ANA_CHECK(evtStore()->retrieve( m_electrons,"Electrons"));
      processElectrons();

      ANA_CHECK(evtStore()->retrieve( m_caloClusters,"CaloCalTopoClusters"));
      processClusters();
      processGaps();

      if (enableTT)
	{
	  m_TTcontainer = 0;
	  ANA_CHECK(evtStore()->retrieve( m_TTcontainer,"xAODTriggerTowers"));
	  processTriggerTowers();
	}
    }

  if (m_isMC)
    {
      ANA_CHECK(evtStore()->retrieve( m_truthParticleContainer,"TruthParticles"));
      //ANA_CHECK(evtStore()->retrieve( m_truthParticleContainer,"MuonTruthParticles"));
      processTruthMuons();
    }

  // if trigger enabled, only write out events which pass one of them, unless using MC

  if (enableTrigger && !passTrigger && !m_isMC && writeOnlyTriggers) return StatusCode::SUCCESS;

  // if tree is enabled, write it out
  if (t_ZdcTrigEff[0]<0 || t_ZdcTrigEff[1]<0)
  {
    ANA_MSG_VERBOSE ("Negative trigger efficiencies for Run/LB/Event=" << t_runNumber << "/" << t_lumiBlock << "/" << t_eventNumber);
  }

  if (enableOutputTree)
    {
      // m_outputTree->Fill();
      tree( "zdcTree" )->Fill();
    }

  return StatusCode::SUCCESS;
}

void ZdcNtuple::processZdcNtupleFromModules()
{

  ANA_MSG_DEBUG ("copying already processed info!");
  if (m_zdcSums)
    {
      
      ANA_MSG_DEBUG ("Sum 0 = " << m_zdcSums->at(0)->auxdataConst<float>("CalibEnergy") 
                     << ", Sum 1 = " << m_zdcSums->at(1)->auxdataConst<float>("CalibEnergy"));
      ANA_MSG_DEBUG ("Final 0 = " << m_zdcSums->at(0)->auxdataConst<float>("FinalEnergy") 
                     << ", Final 1 = " << m_zdcSums->at(1)->auxdataConst<float>("FinalEnergy"));
      
    }
  else
    {
      ANA_MSG_INFO( "No ZDC sums produced!");
    }
 
  for (size_t iside=0;iside<2;iside++)
    {
      t_ZdcAmp[iside] = 0;t_ZdcEnergy[iside]=0;t_ZdcTime[iside]=0;t_ZdcStatus[iside]=0;
      for (int imod=0;imod<4;imod++)
	{
	  t_ZdcModuleAmp[iside][imod]=0;t_ZdcModuleTime[iside][imod]=0;t_ZdcModuleStatus[iside][imod]=0;t_ZdcTrigEff[iside]=0;
	  t_ZdcModuleCalibAmp[iside][imod]=0;t_ZdcModuleCalibTime[iside][imod]=0;t_ZdcModuleChisq[iside][imod]=0;t_ZdcModuleFitAmp[iside][imod]=0;
	  t_ZdcModuleFitT0[iside][imod] =0;t_ZdcModuleBkgdMaxFraction[iside][imod]=0;t_ZdcModuleAmpError[iside][imod]=0;
	  t_ZdcModuleMinDeriv2nd[iside][imod]=0;t_ZdcModulePresample[iside][imod]=0;t_ZdcModulePreSampleAmp[iside][imod]=0;
	}    
    }

  t_ZdcModuleMask=0;

  if (m_zdcSums)
    {
      ANA_MSG_DEBUG( "accessing ZdcSums" );
      for (const auto zdcSum : *m_zdcSums)
	{
	  int iside = 0;
	  if (zdcSum->side()>0) iside=1;
	  
	  //static SG::AuxElement::ConstAccessor< float > acc( "CalibEnergy" );
	  //t_ZdcEnergy[iside] = acc(*zdcSum);
	  
	  t_ZdcEnergy[iside] = zdcSum->auxdataConst<float>("CalibEnergy");
	  t_ZdcEnergyErr[iside] = zdcSum->auxdataConst<float>("CalibEnergyErr");

	  t_ZdcAmp[iside] = zdcSum->auxdataConst<float>("UncalibSum");
	  t_ZdcAmpErr[iside] = zdcSum->auxdataConst<float>("UncalibSumErr");
	  
	  ANA_MSG_VERBOSE("processZdcNtupleFromModules: ZdcSum energy = " << t_ZdcEnergy[iside]);
	  
	  t_ZdcTime[iside] = zdcSum->auxdataConst<float>("AverageTime");
	  t_ZdcStatus[iside] = zdcSum->auxdataConst<unsigned int>("Status");
	  t_ZdcModuleMask += (zdcSum->auxdataConst<unsigned int>("ModuleMask") << 4*iside);
	}
    }

  ANA_MSG_DEBUG(  "accessing ZdcModules" );
  if (m_zdcModules)
    {
      for (const auto zdcMod : *m_zdcModules)
	{
	  int iside = 0;
	  if (zdcMod->side()>0) iside=1;
	  int imod = zdcMod->zdcModule();
	  
	  ANA_MSG_VERBOSE ("Module " << zdcMod->side() << " " << zdcMod->zdcModule() << " amp:" << zdcMod->auxdataConst<float>("Amplitude"));
	  
	  if (zdcMod->type()!=0) continue;
	  
	  t_ZdcModuleCalibAmp[iside][imod] = zdcMod->auxdataConst<float>("CalibEnergy"+auxSuffix);
	  t_ZdcModuleCalibTime[iside][imod] = zdcMod->auxdataConst<float>("CalibTime"+auxSuffix); 
	  t_ZdcModuleStatus[iside][imod] = zdcMod->auxdataConst<unsigned int>("Status"+auxSuffix);
	  if (t_ZdcModuleAmp[iside][imod]!=0.) 
	    Warning("processZdcNtupleFromModules","overwriting side %d module %d!",iside,imod);
	  t_ZdcModuleAmp[iside][imod] = zdcMod->auxdataConst<float>("Amplitude"+auxSuffix);
	  t_ZdcModuleTime[iside][imod] = zdcMod->auxdataConst<float>("Time"+auxSuffix);
	  
	  t_ZdcModuleChisq[iside][imod] = zdcMod->auxdataConst<float>("Chisq"+auxSuffix);
	  t_ZdcModuleFitAmp[iside][imod] =zdcMod->auxdataConst<float>("FitAmp"+auxSuffix);
	  t_ZdcModuleAmpError[iside][imod] = zdcMod->auxdataConst<float>("FitAmpError"+auxSuffix);
	  t_ZdcModuleFitT0[iside][imod] = zdcMod->auxdataConst<float>("FitT0"+auxSuffix);
	  t_ZdcModuleBkgdMaxFraction[iside][imod] = zdcMod->auxdataConst<float>("BkgdMaxFraction"+auxSuffix);
	  t_ZdcModuleMinDeriv2nd[iside][imod] = zdcMod->auxdataConst<float>("MinDeriv2nd"+auxSuffix);
	  t_ZdcModulePresample[iside][imod] = zdcMod->auxdataConst<float>("Presample"+auxSuffix);	  
	  t_ZdcModulePreSampleAmp[iside][imod] = zdcMod->auxdataConst<float>("PreSampleAmp"+auxSuffix);	  
	}
    }
  else
    {
      ANA_MSG_INFO("No ZdcModules" << auxSuffix << " when expected!");
    }

  if (msgLvl (MSG::VERBOSE))
  {
    std::ostringstream message;
    message << "Dump zdc_ZdcModuleAmp: ";
    for (int iside=0;iside<2;iside++)
    {
      for (int imod=0;imod<4;imod++)
      {
        message << t_ZdcModuleAmp[iside][imod] << " ";
      }
    }      
  }
}

/*
void ZdcNtuple::processZdcNtuple()
{
  // this processes from the newly produced ZDC info
  // need to access ZDC sums if they have just been made
  
  if (m_zdcSums)
    {
      ANA_MSG_DEBUG ("Sum 0 = " << m_zdcSums->at(0)->auxdataConst<float>("CalibEnergy") 
		<< ", Sum 1 = " << m_zdcSums->at(1)->auxdataConst<float>("CalibEnergy"));
      ANA_MSG_DEBUG ("Final 0 = " << m_zdcSums->at(0)->auxdataConst<float>("FinalEnergy") 
		<< ", Final 1 = " << m_zdcSums->at(1)->auxdataConst<float>("FinalEnergy"));
    }
  else
    {
      ANA_MSG_INFO( "No ZDC sums produced!");
    }
 
  for (size_t iside=0;iside<2;iside++)
    {
      t_ZdcAmp[iside] = 0;t_ZdcEnergy[iside]=0;t_ZdcTime[iside]=0;t_ZdcStatus[iside]=0;
      for (int imod=0;imod<4;imod++)
	{
	  t_ZdcModuleAmp[iside][imod]=0;t_ZdcModuleTime[iside][imod]=0;t_ZdcModuleStatus[iside][imod]=0;t_ZdcTrigEff[iside]=0;
	  t_ZdcModuleCalibAmp[iside][imod]=0;t_ZdcModuleCalibTime[iside][imod]=0;t_ZdcModuleChisq[iside][imod]=0;t_ZdcModuleFitAmp[iside][imod]=0;
	  t_ZdcModuleFitT0[iside][imod] =0;t_ZdcModuleBkgdMaxFraction[iside][imod]=0;t_ZdcModuleAmpError[iside][imod]=0;
	}
    }

  t_ZdcModuleMask=0;

  const ZDCDataAnalyzer* zdcAnalyzer = m_zdcAnalysisTool->getDataAnalyzer();
  
  if (zdcAnalyzer)
    {
      t_ZdcModuleMask = zdcAnalyzer->GetModuleMask();
      for (size_t iside=0;iside<2;iside++)
	{
	  t_ZdcAmp[iside] = zdcAnalyzer->GetModuleSum(iside);
	  t_ZdcEnergy[iside] = zdcAnalyzer->GetCalibModuleSum(iside);
	  t_ZdcTime[iside] = zdcAnalyzer->GetAverageTime(iside);
	  t_ZdcStatus[iside] = !(zdcAnalyzer->SideFailed(iside));
	  for (size_t imod=0;imod<4;imod++)
	    {
	      if (t_ZdcModuleAmp[iside][imod]!=0.) 
		Warning("processZdcNtupleFromModules","overwriting side %zd module %zd!",iside,imod);
	      t_ZdcModuleAmp[iside][imod] = zdcAnalyzer->GetModuleAmplitude(iside,imod);
	      t_ZdcModuleTime[iside][imod] = zdcAnalyzer->GetModuleTime(iside,imod);
	      t_ZdcModuleStatus[iside][imod] = zdcAnalyzer->GetModuleStatus(iside,imod);
	      
	      t_ZdcModuleCalibAmp[iside][imod] = zdcAnalyzer->GetModuleCalibAmplitude(iside,imod);
	      t_ZdcModuleCalibTime[iside][imod] = zdcAnalyzer->GetModuleCalibTime(iside,imod);	 
	      
	      t_ZdcModuleChisq[iside][imod] = zdcAnalyzer->GetPulseAnalyzer(iside,imod)->GetChisq();
	      t_ZdcModuleFitAmp[iside][imod] = zdcAnalyzer->GetPulseAnalyzer(iside,imod)->GetFitAmplitude();
	      t_ZdcModuleFitT0[iside][imod] = zdcAnalyzer->GetPulseAnalyzer(iside,imod)->GetFitT0();
	      t_ZdcModuleBkgdMaxFraction[iside][imod] = zdcAnalyzer->GetPulseAnalyzer(iside,imod)->GetBkgdMaxFraction();
	      t_ZdcModuleAmpError[iside][imod] = zdcAnalyzer->GetPulseAnalyzer(iside,imod)->GetAmpError();
	      
	    }
	  t_ZdcTrigEff[iside] = m_zdcAnalysisTool->getTriggerEfficiency(iside);
	  if (t_ZdcTrigEff[iside]<0) 
	    {
	      ANA_MSG_INFO ("LB/event=" << t_lumiBlock << "/" << t_eventNumber << "  Trigger efficiency  " << iside << " == " << t_ZdcTrigEff[iside]);
	    }
	}
    }

  ANA_MSG_DEBUG ("Tool Sum 0 = " << t_ZdcEnergy[0] << " Sum 1 = " << t_ZdcEnergy[1]);
}
*/

bool ZdcNtuple::processTriggerDecision()
{
  ANA_MSG_DEBUG ("Processing trigger");

  bool passTrigger = false;

  t_trigger = 0;
  t_trigger_TBP = 0;
  for (int i = 0;i<16;i++)
    {
      t_tav[i] = 0;
      t_tbp[i] = 0;
    }

  if (enableTrigger)
    {
      //ANA_MSG_INFO("filling trigger histos!");
      h_zdcTriggers->Fill(0+.5,1.); // first bin = all

      int ic = 0;
      for (auto cg : m_chainGroups)
	{
	  //ANA_MSG_INFO("execute()","checking cg = %s",cg->getListOfTriggers().at(0).c_str());
	  if (cg->isPassed())
	    {
	      h_zdcTriggers->Fill(ic+.5+1,1.);	  
	      //ANA_MSG_INFO("Chain group passed %s",cg->getListOfTriggers().at(0).c_str());
	      t_trigger += (1<<ic);
	      //t_decisions.at(ic)=1;
	      //t_prescales.at(ic)=cg->getPrescale();
	      t_decisions[ic]=true;
	      t_prescales[ic]=cg->getPrescale();
	      passTrigger = true;
	    }
	  else
	    {
	      //t_decisions.at(ic)=0;
	      //t_prescales.at(ic)=0;
	      t_decisions[ic]=0;
	      t_prescales[ic]=0;
	    }

	  
	  if (cg->isPassedBits()&TrigDefs::EF_passedRaw)
	    {
	      h_zdcTriggersTBP->Fill(ic+.5+1,1.);	  
	      t_trigger_TBP += (1<<ic);
	    }
	  

	  ic++;
	}

      int irc = 0;
      for (auto cg : m_rerunChainGroups)
	{
	  t_rerunDecisions[irc] = false;
	  if (cg->isPassedBits()&TrigDefs::EF_passedRaw)
	    {
	      t_rerunDecisions[irc] = true;
	    }
	  irc++;
	}

      for (int i=0;i<16;i++)
	{
	  t_tbp[i] = m_trigDecision->tbp().at(i);
	  t_tav[i] = m_trigDecision->tav().at(i);
	}
    }

  return passTrigger;
}

void ZdcNtuple::processEventInfo()
{
  ANA_MSG_DEBUG( "processing event info");

  t_bcid = m_eventInfo->bcid();
  t_runNumber = m_eventInfo->runNumber();
  t_eventNumber = m_eventInfo->eventNumber();
  t_lumiBlock = m_eventInfo->lumiBlock();
  t_passBits = acceptEvent();
  t_avgIntPerCrossing = m_eventInfo->averageInteractionsPerCrossing();
  t_actIntPerCrossing = m_eventInfo->actualInteractionsPerCrossing();

  // determine if event is MC
  m_isMC = false;
  if(m_eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) )
    {
      m_isMC = true; // can do something with this later
    }   


  if ( !(m_eventCounter++%1000) || msgLvl(MSG::DEBUG))
    {
      ANA_MSG_INFO("Event# " << m_eventCounter << "Run " << m_eventInfo->runNumber() << " Event " << m_eventInfo->eventNumber() << " LB " << m_eventInfo->lumiBlock() );
    }

}

void ZdcNtuple::processFCal()
{
  ANA_MSG_DEBUG("processFCal: processing FCal");

  t_fcalEt = 0.;
  t_fcalEtA = 0.;
  t_fcalEtC = 0.;
  t_fcalEtA_TT = 0.;
  t_fcalEtC_TT = 0.;
  for (const auto calosum : *m_caloSums) 
    {
      const std::string name = calosum->auxdataConst<std::string>("Summary");
      if (name=="FCal")
	{
	  t_fcalEt = calosum->et();
	  ANA_MSG_DEBUG("processFCal: fcalEt = " << t_fcalEt);
	}

      if (name=="All")
	{
	  t_totalEt = calosum->et();
	  ANA_MSG_DEBUG("processFCal: totalEt = " << t_totalEt);
	}
    }

  for (const auto eventShape : *m_eventShapes) 
    {
      int layer = eventShape->layer();
      float eta = eventShape->etaMin();
      float et = eventShape->et();
      if (layer==21 || layer==22 || layer==23)
	{
	  if (eta>0) t_fcalEtA += et;
	  if (eta<0) t_fcalEtC += et;
	}

      if (TMath::Abs(eta)<2.4)
	{
	  t_totalEt24 += et;
	}
    }

  t_L1ET = 0;
  t_L1ET24 = 0;

  if (m_lvl1EnergySumRoI)
    {
      t_L1ET = m_lvl1EnergySumRoI->energyT();
      //t_L1ET24 = m_lvl1EnergySumRoI->energyTRestricted();
    }  

  /*
  if (m_TTeventShapes)
    {
      for (const auto eventShape : *m_TTeventShapes) 
	{
	  //int layer = eventShape->layer();
	  float eta = eventShape->etaMin();
	  float et = eventShape->et();
	  //if (layer==21 || layer==22 || layer==23)
	    {
	      if (eta>0) t_fcalEtA_TT += et;
	      if (eta<0) t_fcalEtC_TT += et;
	    }
	}      
    }
  */

  return;
}

void ZdcNtuple::processJets()
{
  ANA_MSG_DEBUG("processJets: processing Jets");

  for (size_t ijet =0;ijet<m_jetContainerNames.size();ijet++)
    {
      t_jet_pt[ijet].clear();
      t_jet_eta[ijet].clear();
      t_jet_phi[ijet].clear();
      t_jet_m[ijet].clear();
      t_jet_acc[ijet].clear();
      t_njet[ijet] = 0;

      for (const auto jet : *m_jetContainers[ijet])
	{
	  if (enableJets)
	    {
	      t_jet_pt[ijet].push_back(jet->pt());
	      t_jet_eta[ijet].push_back(jet->eta());
	      t_jet_phi[ijet].push_back(jet->phi());
	      t_jet_m[ijet].push_back(jet->m());
	      /*
	      if (m_jetCleaningFlag[ijet]) 
		t_jet_acc[ijet].push_back(m_jetCleaning->accept( *jet ));
	      else
	      */

	      uint8_t jet_acc = 1; // bit 0 -- jet cleaning
	      if (m_jetCleaningFlag[ijet] && !m_jetCleaning->keep( *jet )) jet_acc = 0; 

	      /*
	      if (m_jetContainerNames[ijet] == "AntiKt4EMTopoJets")
		if (m_btagtool->accept(*jet)) jet_acc += 2; // bit 1 -- b tagging

	      if (m_jetContainerNames[ijet] == "AntiKt4PV0TrackJets")
		if (m_btagtool_tj->accept(*jet)) jet_acc += 2; // bit 1 -- b tagging
	      */

	      t_jet_acc[ijet].push_back(jet_acc);
	    }
	  t_njet[ijet]++; 
	}
    }
  /*
  t_Tjet_pt.clear();
  t_Tjet_eta.clear();
  t_Tjet_phi.clear();
  t_Tjet_m.clear();
  t_Tjet_acc.clear();
  t_nTjet = 0;

  for (const auto Tjet : *m_TrigAntiKt4HIJets)
    {
      if (enableTriggerJets)
	{
	  t_Tjet_pt.push_back(Tjet->pt());
	  t_Tjet_eta.push_back(Tjet->eta());
	  t_Tjet_phi.push_back(Tjet->phi());
	  t_Tjet_m.push_back(Tjet->m());
	  t_Tjet_acc.push_back(0);
	}
      t_nTjet++; 
    }
  */

  return;
}

int ZdcNtuple::processMuons()
{
  ANA_MSG_DEBUG("processMuons: processing Muons");

  t_muroi_eta.clear();
  t_muroi_phi.clear();
  t_muroi_thr.clear();
  t_nmuroi = 0;

  if (m_muonRoIContainer)
    {
      for (const auto muroi : *m_muonRoIContainer)
	{
	  if (enableMuons)
	    {
	      t_muroi_phi.push_back(muroi->phi());
	      t_muroi_eta.push_back(muroi->eta());
	      t_muroi_thr.push_back(muroi->thrValue());
	    }
	  t_nmuroi++;
	}
    }

  t_muon_pt.clear();
  t_muon_eta.clear();
  t_muon_phi.clear();
  t_muon_m.clear();
  t_muon_charge.clear();
  t_muon_author.clear();
  t_muon_acc.clear();
  t_muon_qual.clear();
  t_muon_l1match.clear();
  t_muon_iso.clear();
  t_muon_l1dR.clear();
  t_muon_cluster_E.clear();
  t_muon_cluster_t.clear();
  t_muon_MS_pt.clear();
  t_muon_MS_pt_orig.clear();
  t_muon_MS_eta.clear();
  t_muon_MS_phi.clear();
  t_muon_ID_pt.clear();
  t_muon_ID_index.clear();
  t_muon_MSEx_pt.clear();
  t_muon_MSEx_eta.clear();
  t_muon_MSEx_phi.clear();
  t_muon_nPrecision.clear();
  t_muon_nPrecisionHoles.clear();
  t_muon_hasTrack.clear();
  t_muon_nPixHits.clear();
  t_muon_nPixDead.clear();
  t_muon_nPixHoles.clear();
  t_muon_nSctHits.clear();
  t_muon_nSctDead.clear();
  t_muon_nSctHoles.clear();
  t_muon_nTrtHits.clear();
  t_muon_nTrtOutliers.clear();
  t_muon_d0.clear();
  t_muon_z0.clear();
  t_muon_vtxz.clear();
  t_muon_theta.clear();

  t_muon_rho.clear();
  t_muon_qOverPsigma.clear();
  t_muon_qOverPsignif.clear();
  t_muon_reducedChi2.clear();
  t_muon_type.clear();

  t_muon_eff.clear();
  t_muon_sf.clear();
  t_muon_sfvar.clear();
  t_muon_sfvarLowpT.clear();

  t_muon_smear_pt.clear();
  t_muon_smear_charge.clear();
  t_muon_smear_MS.clear();
  t_muon_smear_ID.clear();

  for (size_t imutr = 0;imutr<m_muon_triggers.size();imutr++)
    {
      t_muon_HLT_match.at(imutr).clear();
      t_muon_HLT_dR.at(imutr).clear();
    }

  t_nmuon = 0;
  t_dimuon_mu0.clear();
  t_dimuon_mu1.clear();
  t_dimuon_m.clear();
  t_dimuon_rap.clear();
  t_dimuon_phi.clear();
  t_dimuon_pt.clear();

  t_nmsex = 0;
  t_msex_eta.clear();
  t_msex_phi.clear();
  t_msex_pt.clear();
  t_msex_m.clear();
  t_msex_charge.clear();
  t_msex_nPrecision.clear();
  t_msex_nPrecisionHoles.clear();

  for (size_t idimutr = 0;idimutr<m_dimuon_triggers.size();idimutr++)
    {
      t_dimuon_HLT_match.at(idimutr).clear();
    }
  t_ndimuon = 0;

  if (m_muons)
    {
      std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > muons_shallowCopy = xAOD::shallowCopyContainer( *m_muons );
      xAOD::MuonContainer* muonsCorr = muons_shallowCopy.first;

      for (const auto muon : *muonsCorr) // get each muon from the copy, copy it (original) and then correct it
	//for (auto original_muon : *muonsCorr)
	{

	  xAOD::Muon* original_muon = new xAOD::Muon();
	  original_muon->makePrivateStore( *muon );

	  if (!m_muonCalibrationAndSmearingTool->applyCorrection(*muon))
	    {
	      ANA_MSG_INFO("processMuons(): Can't apply calibration!");
	    }

	  if (enableMuons)
	    {

	      t_muon_pt.push_back(muon->pt());
	      t_muon_eta.push_back(muon->eta());
	      t_muon_phi.push_back(muon->phi());
	      t_muon_m.push_back(muon->m());
	      t_muon_charge.push_back(muon->charge());
	      t_muon_author.push_back(muon->author());

	      bool isIso = false;
	      //if (m_isoTool->accept(*muon)) isIso = true;
	      t_muon_iso.push_back(isIso);

	      uint8_t nprecisionLayers,nprecisionHoleLayers;
	      if (!muon->summaryValue(nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers) ||
		  !muon->summaryValue(nprecisionHoleLayers, xAOD::SummaryType::numberOfPrecisionHoleLayers))
		{
		  ATH_MSG_VERBOSE("getQuality - #precision layers missing in combined muon! Aborting.");
		  nprecisionLayers = 0;
		  nprecisionHoleLayers = 0;
		}
	      t_muon_nPrecision.push_back(nprecisionLayers);
	      t_muon_nPrecisionHoles.push_back(nprecisionHoleLayers);
	      t_muon_qual.push_back(bool(m_muonSelection->accept(*muon)));
	      t_muon_acc.push_back(m_muonSelection->getQuality(*muon));
	      int8_t l1match=-1;
	      float l1dR=999.;
	      if (m_muonRoIContainer)
		{
		  for (int imuroi=0;imuroi<t_nmuroi;imuroi++)
		    {
		      double dr = dR(muon->eta(),muon->phi(),t_muroi_eta[imuroi],t_muroi_phi[imuroi]);
		      if (dr<m_L1MuonMatchdR && dr<l1dR) // no check on threshold since MU0
			{
			  l1match = imuroi;
			  l1dR = dr;
			}
		    }
		}
	      t_muon_l1match.push_back(l1match);
	      t_muon_l1dR.push_back(l1dR);

	      for (size_t imutr=0;imutr<m_muon_triggers.size();imutr++)
		{
		  uint8_t muon_HLT_match = 0;

		  muon_HLT_match = m_trigMatchingTool->match(*muon,m_muon_triggers.at(imutr));

		  /*
		  if (m_muonTrigMatchTool)
		    {
		      muon_HLT_match = m_muonTrigMatchTool->match(muon,m_muon_triggers.at(imutr),0.1);
		    }
		  */

		  t_muon_HLT_match.at(imutr).push_back(muon_HLT_match);
		  t_muon_HLT_dR.at(imutr).push_back(0);
		}

	      const xAOD::CaloCluster* cc = muon->cluster();
	      float cc_t = -1;
	      float cc_E = -1;
	      if (cc)
		{
		  cc_t = cc->time();
		  cc_E = cc->e();
		}
	      t_muon_cluster_E.push_back(cc_E);
	      t_muon_cluster_t.push_back(cc_t);

	      float ms_pt = -1;
	      ms_pt = muon->auxdata< float >( "MuonSpectrometerPt" );
	      t_muon_MS_pt.push_back(ms_pt);
	      //ANA_MSG_INFO("processMuons(): processMuons(): final MS pT = %f, eta = %f",ms_pt,muon->eta());

	      float msex_pt = -1;
	      float msex_eta = -999;
	      float msex_phi = -999;
	      if (muon->extrapolatedMuonSpectrometerTrackParticleLink().isValid())
		{
		  msex_pt = (*muon->extrapolatedMuonSpectrometerTrackParticleLink())->pt();
		  msex_eta = (*muon->extrapolatedMuonSpectrometerTrackParticleLink())->eta();
		  msex_phi = (*muon->extrapolatedMuonSpectrometerTrackParticleLink())->phi();
		}
	      t_muon_MSEx_pt.push_back(msex_pt);
	      t_muon_MSEx_eta.push_back(msex_eta);
	      t_muon_MSEx_phi.push_back(msex_phi);
	      
	      float id_pt = -1;
	      id_pt = muon->auxdata< float >( "InnerDetectorPt" );	      
	      t_muon_ID_pt.push_back(id_pt);

	      if (muon->inDetTrackParticleLink().isValid())
		{
		  t_muon_ID_index.push_back(muon->inDetTrackParticleLink().index());
		}
	      else
		{
		  t_muon_ID_index.push_back(-1);
		}

	      //
	      float ms_pt_orig = -1;
	      float ms_eta = -999;
	      float ms_phi = -999;
	      if (muon->muonSpectrometerTrackParticleLink().isValid())
		{
		  ms_pt_orig = (*muon->muonSpectrometerTrackParticleLink())->pt();
		  ms_eta = (*muon->muonSpectrometerTrackParticleLink())->eta();
		  ms_phi = (*muon->muonSpectrometerTrackParticleLink())->phi();
		  //ANA_MSG_INFO("processMuons(): processMuons(): original MS pT = %f",ms_pt_orig);
		}
	      t_muon_MS_pt_orig.push_back(ms_pt_orig);
	      t_muon_MS_eta.push_back(ms_eta);
	      t_muon_MS_phi.push_back(ms_phi);
	      
	      const xAOD::TrackParticle* track = muon->primaryTrackParticle();
	      uint8_t hasTrack=0;
	      uint8_t nPixHits=0;
	      uint8_t nPixHoles=0;
	      uint8_t nPixDead=0;
	      uint8_t nSctHits=0;
	      uint8_t nSctHoles=0;
	      uint8_t nSctDead=0;
	      uint8_t nTrtHits=0;
	      uint8_t nTrtOutliers=0;
	      float d0 = 0;
	      float z0 = 0;
	      float theta = 0;
	      float vtxz = -999;
	      if (track)
		{
		  hasTrack=1;
		  uint8_t value=0;
		  if (track->summaryValue(value, xAOD::SummaryType::numberOfPixelHits)) nPixHits=value;
		  if (track->summaryValue(value, xAOD::SummaryType::numberOfPixelDeadSensors)) nPixDead=value;
		  if (track->summaryValue(value, xAOD::SummaryType::numberOfPixelHoles)) nPixHoles=value;
		  if (track->summaryValue(value, xAOD::SummaryType::numberOfSCTHits)) nSctHits=value;
		  if (track->summaryValue(value, xAOD::SummaryType::numberOfSCTDeadSensors)) nSctDead=value;
		  if (track->summaryValue(value, xAOD::SummaryType::numberOfSCTHoles)) nSctHoles=value;
		  if (track->summaryValue(value, xAOD::SummaryType::numberOfTRTHits)) nTrtHits=value;
		  if (track->summaryValue(value, xAOD::SummaryType::numberOfTRTOutliers)) nTrtOutliers=value;
		  d0 = track->d0();
		  z0 = track->z0();
		  theta = track->theta();
		  if (track->vertex()) vtxz = track->vertex()->z();
		}

	      t_muon_hasTrack.push_back(hasTrack);
	      t_muon_nPixHits.push_back(nPixHits);
	      t_muon_nPixHoles.push_back(nPixHoles);
	      t_muon_nPixDead.push_back(nPixDead);
	      t_muon_nSctHits.push_back(nSctHits);
	      t_muon_nSctHoles.push_back(nSctHoles);
	      t_muon_nSctDead.push_back(nSctDead);
	      t_muon_nTrtHits.push_back(nTrtHits);
	      t_muon_nTrtOutliers.push_back(nTrtOutliers);
	      t_muon_d0.push_back(d0);
	      t_muon_z0.push_back(z0);
	      t_muon_vtxz.push_back(vtxz);
	      t_muon_theta.push_back(theta);

	      t_muon_type.push_back(muon->muonType());

	      // reproducing selection tight cuts
	      const xAOD::TrackParticle* idtrack = muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
	      const xAOD::TrackParticle* metrack = muon->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
	      float rho = 999;
	      float qOverPsigma = 999;
	      float qOverPsignif = 999;
	      float reducedChi2 = 999;
	      if( idtrack && metrack ) {
		float mePt = -999999., idPt = -999999.;
		
		//::: Retrieve the event information:                                                                                              
		if( m_isMC ) { // now using corrected ID/MS momenta                                  
		  static SG::AuxElement::Accessor<float> mePt_acc("MuonSpectrometerPt");//A bit of optimization 
		  static SG::AuxElement::Accessor<float> idPt_acc("InnerDetectorPt");
		  if (!mePt_acc.isAvailable(*muon) || !idPt_acc.isAvailable(*muon)) {
		    ATH_MSG_FATAL( "No momentum decoration available in MC! MuonSelectorTool can not work!" );
		    exit(-1);
		  }
		  mePt = mePt_acc(*muon);
		  idPt = idPt_acc(*muon);
		} else {
		  mePt = metrack->pt();
		  idPt = idtrack->pt();
		}

		float cbPt = muon->pt();
		float meP  = 1.0 / ( sin(metrack->theta()) / mePt);
		float idP  = 1.0 / ( sin(idtrack->theta()) / idPt);

		rho           = fabs( idPt - mePt ) / cbPt;
		qOverPsigma  = sqrt( idtrack->definingParametersCovMatrix()(4,4) + metrack->definingParametersCovMatrix()(4,4) );
		qOverPsignif  = fabs( (metrack->charge() / meP) - (idtrack->charge() / idP) ) / qOverPsigma;
		reducedChi2 = muon->primaryTrackParticle()->chiSquared()/muon->primaryTrackParticle()->numberDoF();
	      }
	      
	      t_muon_rho.push_back(rho);
	      t_muon_qOverPsigma.push_back(qOverPsigma);
	      t_muon_qOverPsignif.push_back(qOverPsignif);
	      t_muon_reducedChi2.push_back(reducedChi2);

	      // end selection tight cuts

	      // disable scale factors for now
	      if (0) // do scale factors on MC mean anything? 
	      //if (!m_isMC) 
	      //if (1) 
		{
		  float eff=0;
		  float sf = 0;
		  std::vector<float> sfvar(4);
		  std::vector<float> sfvarLowpT(4);
		  
		  /*
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getRecoEfficiency(*muon,eff));
		  //CHECK_CPCorr( m_muonEfficiencyScaleFactors->getMCEfficiency(*muon,eff));
		  
		  CHECK_CPSys(  m_muonEfficiencyScaleFactors->applySystematicVariation(CP::SystematicSet()));
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getEfficiencyScaleFactor( *muon, sf ) );		  
		  CHECK_CPSys(  m_muonEfficiencyScaleFactors->applySystematicVariation(m_statup));
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getEfficiencyScaleFactor( *muon, sfvar[0] ) );
		  CHECK_CPSys(  m_muonEfficiencyScaleFactors->applySystematicVariation(m_statdown));
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getEfficiencyScaleFactor( *muon, sfvar[1] ) );
		  CHECK_CPSys(  m_muonEfficiencyScaleFactors->applySystematicVariation(m_sysup));
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getEfficiencyScaleFactor( *muon, sfvar[2] ) );
		  CHECK_CPSys(  m_muonEfficiencyScaleFactors->applySystematicVariation(m_sysdown));
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getEfficiencyScaleFactor( *muon, sfvar[3] ) );
		  //ANA_MSG_INFO("processMuons(): pT,eta = %f,%f -> muon eff = %f, sf = %f stat(up) = %f, stat(down) = %f, sys(up) = %f, sys(down) = %f",
		  //     muon->pt(),muon->eta(),eff,sf,sfvar[0],sfvar[1],sfvar[2],sfvar[3]);
		  
		  CHECK_CPSys(  m_muonEfficiencyScaleFactors->applySystematicVariation(m_statupLowpT));
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getEfficiencyScaleFactor( *muon, sfvarLowpT[0] ) );
		  CHECK_CPSys(  m_muonEfficiencyScaleFactors->applySystematicVariation(m_statdownLowpT));
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getEfficiencyScaleFactor( *muon, sfvarLowpT[1] ) );
		  CHECK_CPSys(  m_muonEfficiencyScaleFactors->applySystematicVariation(m_sysupLowpT));
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getEfficiencyScaleFactor( *muon, sfvarLowpT[2] ) );
		  CHECK_CPSys(  m_muonEfficiencyScaleFactors->applySystematicVariation(m_sysdownLowpT));
		  CHECK_CPCorr( m_muonEfficiencyScaleFactors->getEfficiencyScaleFactor( *muon, sfvarLowpT[3] ) );
		  */
		  //ANA_MSG_INFO("processMuons(): LowpT pT,eta = %f,%f -> muon eff = %f, sf = %f stat(up) = %f, stat(down) = %f, sys(up) = %f, sys(down) = %f",
		  //     muon->pt(),muon->eta(),eff,sf,sfvarLowpT[0],sfvarLowpT[1],sfvarLowpT[2],sfvarLowpT[3]);

		  t_muon_eff.push_back(eff);
		  t_muon_sf.push_back(sf);
		  t_muon_sfvar.push_back(sfvar);
		  t_muon_sfvarLowpT.push_back(sfvarLowpT);
		}

	      if (m_isMC) // process MC smearing systematics
		{
		  std::vector<float> smvec_pt(6); // smearing vector
		  std::vector<int8_t> smvec_charge(6); // smearing vector
		  std::vector<float> smvec_MS(6); // smearing vector
		  std::vector<float> smvec_ID(6); // smearing vector
		  //ANA_MSG_INFO("processMuons(): eta: muon = %f, original = %f",muon->eta(),original_muon->eta());

		  for (size_t ivar=0;ivar<muonSysList.size();ivar++)
		    {
		      xAOD::Muon* var_muon = 0;
		      m_muonCalibrationAndSmearingTool->applySystematicVariation(muonSysList.at(ivar));
		      //if (m_muonCalibrationAndSmearingTool->applyCorrection(*muon))
		      if (m_muonCalibrationAndSmearingTool->correctedCopy(*original_muon,var_muon) )
			{		      
			  //ANA_MSG_INFO("processMuons(): eta: var = %f",var_muon->eta());
			  smvec_pt[ivar] = var_muon->pt();
			  smvec_charge[ivar] = var_muon->charge();
			  smvec_ID[ivar] = var_muon->auxdata< float >( "InnerDetectorPt" );
			  smvec_MS[ivar] = var_muon->auxdata< float >( "MuonSpectrometerPt" );
			  delete var_muon;
			}
		      else
			{
			  smvec_pt[ivar] = 0;
			  smvec_charge[ivar] = 0;
			  smvec_ID[ivar] = 0;
			  smvec_MS[ivar] = 0;
			}
		    }
		  t_muon_smear_pt.push_back(smvec_pt);
		  t_muon_smear_charge.push_back(smvec_charge);
		  t_muon_smear_MS.push_back(smvec_MS);
		  t_muon_smear_ID.push_back(smvec_ID);
		}
	    }
	  t_nmuon++; 
	  delete original_muon;
	}

      t_nmsex = m_extrapolatedMuonTrackParticles->size();
      for (const auto msex : *m_extrapolatedMuonTrackParticles)
	{
	  t_msex_pt.push_back(msex->pt());
	  t_msex_eta.push_back(msex->eta());
	  t_msex_phi.push_back(msex->phi());
	  t_msex_m.push_back(msex->m());
	  t_msex_charge.push_back(msex->charge());
	  uint8_t nprecisionLayers,nprecisionHoleLayers;
	  if (!msex->summaryValue(nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers) ||
	      !msex->summaryValue(nprecisionHoleLayers, xAOD::SummaryType::numberOfPrecisionHoleLayers))
	    {
	      ATH_MSG_INFO("getQuality - #precision layers missing in combined muon! Aborting.");
	      nprecisionLayers = 0;
	      nprecisionHoleLayers = 0;
	    }
	  t_msex_nPrecision.push_back(nprecisionLayers);
	  t_msex_nPrecisionHoles.push_back(nprecisionHoleLayers);
	}

      processMuonVertex(muonsCorr); // to this we need a full container of corrected muons

      
      TLorentzVector mu0,mu1,dimu;
      if (t_nmuon>1 && enableMuons)
	{

	  ANA_MSG_DEBUG("processMuons(): processing dimuons from " << t_nmuon << " muons, size =" << m_muons->size());
      
	  for (int imu0=0;imu0<t_nmuon-1;imu0++)
	    {
	      for (int imu1=imu0+1;imu1<t_nmuon;imu1++)
		{
		  mu0.SetPtEtaPhiM(t_muon_pt.at(imu0),t_muon_eta.at(imu0),t_muon_phi.at(imu0),t_muon_m.at(imu0));
		  mu1.SetPtEtaPhiM(t_muon_pt.at(imu1),t_muon_eta.at(imu1),t_muon_phi.at(imu1),t_muon_m.at(imu1));
		  dimu = mu0+mu1;
		  t_dimuon_mu0.push_back(imu0);
		  t_dimuon_mu1.push_back(imu1);
		  t_dimuon_m.push_back(dimu.M());
		  t_dimuon_rap.push_back(dimu.Rapidity());
		  t_dimuon_phi.push_back(dimu.Phi());
		  t_dimuon_pt.push_back(dimu.Pt());
		  std::pair<Bool_t,Bool_t> result1, result2; 
		  ANA_MSG_DEBUG("processMuons(): processing dimuon pair trigger match for " << imu0 << " " << imu1);
		  for (size_t idimutr=0;idimutr<m_dimuon_triggers.size();idimutr++)
		    {
		      /*
		      if (m_muonTrigMatchTool)
			{
			  m_muonTrigMatchTool->matchDimuon(m_muons->at(imu0),m_muons->at(imu1),m_dimuon_triggers.at(idimutr),result1,result2);
			}	      
		      */
		      bool dimu_match = false;
		      if (result1.first && result2.first)
			{
			  dimu_match = true;
			}
		      t_dimuon_HLT_match.at(idimutr).push_back(dimu_match);
		    }
		}
	    }
	  ANA_MSG_DEBUG("processMuons(): finished processing dimuons");
	}


      delete muons_shallowCopy.first;
      delete muons_shallowCopy.second;

    }

  return 0;
}

void ZdcNtuple::processMuonVertex(xAOD::MuonContainer* muons)
{
  t_vxngoodmuon = -1;

  if (t_vxtype != xAOD::VxType::PriVtx) 
    {
      return; // if no primary vertex don't bother trying to match muons to it
    }

  t_vxngoodmuon = 0;

  const xAOD::Vertex* pv = m_primaryVertices->at(t_pvindex);
  
  for (const auto muon : *muons)
    {
      if (!m_muonSelection->accept(*muon)) continue; // skip bad muons

      bool has_track = false;
      for (auto vertexTrack : pv->trackParticleLinks() )
	{
	  if (vertexTrack.index() == muon->inDetTrackParticleLink().index())
	    {
	      has_track = true;
	    }
	}
      if (has_track) t_vxngoodmuon++;
    }
  
  return;

}

void ZdcNtuple::processTruthMuons()
{
  t_ntrmu = 0;
  t_trmu_pt.clear();
  t_trmu_eta.clear();
  t_trmu_phi.clear();
  t_trmu_m.clear();
  t_trmu_vx.clear();
  t_trmu_vy.clear();
  t_trmu_vz.clear();
  t_trmu_pdgId.clear();
  t_trmu_barcode.clear();
  t_trmu_status.clear();
  t_trmu_muindex.clear();
  t_trmu_muDr.clear();
  t_trmu_L1MUindex.clear();
  t_trmu_L1MUDr.clear();

  for(const auto tp : *m_truthParticleContainer)
    {
      int abspid = TMath::Abs(tp->pdgId());

      if (tp->status()!=1) continue;
      if (abspid != 13 && abspid!=22) continue;

      t_trmu_pt.push_back(tp->pt());
      t_trmu_eta.push_back(tp->eta());
      t_trmu_phi.push_back(tp->phi());
      t_trmu_m.push_back(tp->m());
      t_trmu_pdgId.push_back(tp->pdgId());
      t_trmu_barcode.push_back(tp->barcode());
      t_trmu_status.push_back(tp->status());
      if (tp->hasProdVtx())
	{
	  const xAOD::TruthVertex* vtx = tp->prodVtx();
	  t_trmu_vx.push_back(vtx->x());
	  t_trmu_vy.push_back(vtx->y());
	  t_trmu_vz.push_back(vtx->z());
	}
      else
	{
	  t_trmu_vx.push_back(-999);
	  t_trmu_vy.push_back(-999);
	  t_trmu_vz.push_back(-999);
	}

      float Dr = 999.;
      int index = -1;

      const xAOD::Muon* matched_reco_muon=0;
      if(tp->isAvailable<ElementLink<xAOD::MuonContainer> >("recoMuonLink")) 
	{
	  ElementLink<xAOD::MuonContainer> link = tp->auxdata<ElementLink<xAOD::MuonContainer> >("recoMuonLink");
	  if(link.isValid())
	    {
	      matched_reco_muon = *link;
	      index = link.index();
	      Dr = dR(t_trmu_eta.back(),t_trmu_phi.back(),
				(matched_reco_muon)->eta(),(matched_reco_muon)->phi());
	    }
	}
      // find closest muon
      /*
      for (int i=0;i<t_nmuon;i++)
	{
	  float thisDr = dR(t_trmu_eta.back(),t_trmu_phi.back(),t_muon_eta.at(i),t_muon_phi.at(i));
	  if (thisDr<Dr)
	    {
	      Dr = thisDr;
	      index = i;
	    }
	}
      */
      t_trmu_muindex.push_back(index);
      t_trmu_muDr.push_back(Dr);

      // find closest L1MU
      Dr = 999.;
      index = -1;
      for (int i=0;i<t_nmuroi;i++)
	{
	  float thisDr = dR(t_trmu_eta.back(),t_trmu_phi.back(),t_muroi_eta.at(i),t_muroi_phi.at(i));
	  if (thisDr<Dr)
	    {
	      Dr = thisDr;
	      index = i;
	    }
	}
      t_trmu_L1MUindex.push_back(index);
      t_trmu_L1MUDr.push_back(Dr);
      
      t_ntrmu++;
    }

  return;
}

void ZdcNtuple::processPhotons()
{
  ANA_MSG_DEBUG("processPhotons(): Processing photons!");

  t_ph_author.clear();
  t_ph_conv.clear();
  t_ph_oq.clear();
  t_ph_pt.clear();
  t_ph_eta.clear();
  t_ph_eta2.clear();
  t_ph_phi.clear();
  t_ph_iso.clear();
  t_ph_e.clear();
  t_ph_iso.clear();
  t_ph_isEMLoose.clear();
  t_ph_isEMTight.clear();
  t_ph_tight.clear();
  t_ph_loose.clear();
  t_ph_topoetcone40.clear();
  t_ph_etcone40.clear();
  for (size_t iphtr=0;iphtr<m_ph_triggers.size();iphtr++)
    {
      t_ph_HLT_match.at(iphtr).clear();
      t_ph_HLT_dR.at(iphtr).clear();
    }

  t_nphoton = 0;
  for (const auto ph : *m_photons)
    {
      //if (ph->pt()<5e3 && !upcL2015) continue;
      t_nphoton++;
      t_ph_author.push_back(ph->author());
      t_ph_conv.push_back(ph->conversionType());
      t_ph_oq.push_back(ph->OQ());
      t_ph_pt.push_back(ph->pt());
      t_ph_eta2.push_back(ph->caloCluster()->etaBE(2));
      t_ph_eta.push_back(ph->eta());
      t_ph_phi.push_back(ph->phi());
      t_ph_e.push_back(ph->e());
      /*
      t_ph_tight.push_back( m_photonTightIsEMSelector->accept(ph) );
      t_ph_isEMTight.push_back(m_photonTightIsEMSelector->IsemValue());
      t_ph_loose.push_back( m_photonLooseIsEMSelector->accept(ph) );
      t_ph_isEMLoose.push_back(m_photonLooseIsEMSelector->IsemValue());
      */
      t_ph_tight.push_back( 0 );
      t_ph_isEMTight.push_back( 0 );
      t_ph_loose.push_back( 0 );
      t_ph_isEMLoose.push_back( 0 );
      t_ph_topoetcone40.push_back(ph->isolation(xAOD::Iso::topoetcone40));
      if (!upcL2015 && !upc2015)
	{
	  t_ph_etcone40.push_back(ph->isolation(xAOD::Iso::etcone40));
	}
      else
	{
	  t_ph_etcone40.push_back(0.);
	}

      bool isIso = false;
      //if (m_isoTool->accept(*ph)) isIso = true;
      t_ph_iso.push_back(isIso);
      
      // crude way
      /*
      t_ph_isEMTight.push_back(ph->selectionisEM(isEMLoose));
      t_ph_isEMLoose.push_back(ph->selectionisEM(isEMTight)); 

      bool tight = false;
      if (!ph->passSelection(tight, "Tight")) {ANA_MSG_INFO("processPhotons(): No tight decision available");}
      t_ph_tight.push_back(tight);

      bool loose = false;
      if (!ph->passSelection(loose, "Loose")) {ANA_MSG_INFO("processPhotons(): No loose decision available");}
      t_ph_loose.push_back(loose);
      */

    }

}

void ZdcNtuple::processElectrons()
{
  ANA_MSG_DEBUG("processElectrons(): Processing electrons!");

  t_el_author.clear();
  t_el_oq.clear();
  t_el_pt.clear();
  t_el_charge.clear();
  t_el_eta.clear();
  t_el_eta2.clear();
  t_el_phi.clear();
  t_el_e.clear();
  t_el_trackpt.clear();
  t_el_trk_index.clear();
  t_el_d0.clear();
  t_el_sigmad0.clear();
  t_el_z0.clear();
  t_el_vz.clear();
  t_el_vtxz.clear();
  t_el_theta.clear();
  t_el_isEMMedium.clear();
  t_el_isEMLoose.clear();
  t_el_medium.clear();
  t_el_loose.clear();
  t_el_lhmedium.clear();
  t_el_lhloose.clear();
  t_el_topoetcone20.clear();
  t_el_etcone20.clear();
  t_el_iso.clear();

  for (size_t ieltr=0;ieltr<m_el_triggers.size();ieltr++)
    {
      t_el_HLT_match.at(ieltr).clear();
      t_el_HLT_dR.at(ieltr).clear();
    }

  t_nelectron=0;

  for (const auto el : *m_electrons)
    {
      //if (el->pt()<5e3 && !upcL2015) continue;
      t_nelectron++;
      t_el_author.push_back(el->author());
      t_el_oq.push_back(el->OQ());
      t_el_pt.push_back(el->pt());
      t_el_charge.push_back(el->charge());
      t_el_eta2.push_back(el->caloCluster()->etaBE(2));
      t_el_eta.push_back(el->eta());
      t_el_phi.push_back(el->phi());
      t_el_e.push_back(el->e());

      bool isIso = false;
      //if (m_isoTool->accept(*el)) isIso = true;
      t_el_iso.push_back(isIso);

      const xAOD::TrackParticle *tp = el->trackParticle();
      t_el_trackpt.push_back(tp->pt());
      float vtxz = -999;
      if (tp)
	{
	  if (tp->vertex())
	    {
	      vtxz = tp->vertex()->z();
	    }
	}
      t_el_vtxz.push_back(vtxz);

      int index = -1;
      if (*el->trackParticleLink())
	{
	  index = el->trackParticleLink().index();
	}
      t_el_trk_index.push_back(index);
      t_el_d0.push_back(tp->d0());
      t_el_z0.push_back(tp->z0());
      t_el_vz.push_back(tp->vz());
      

      t_el_theta.push_back(tp->theta());
      t_el_sigmad0.push_back(0);
      //t_el_sigmad0.push_back(xAOD::TrackingHelpers::d0significance(tp,m_eventInfo->beamPosSigmaX(), m_eventInfo->beamPosSigmaY(), m_eventInfo->beamPosSigmaZ()));

      t_el_medium.push_back(m_electronMediumEMSelector->accept(el));
      t_el_isEMMedium.push_back(m_electronMediumEMSelector->IsemValue());

      t_el_loose.push_back(m_electronLooseEMSelector->accept(el));
      t_el_isEMLoose.push_back(m_electronLooseEMSelector->IsemValue());

      t_el_lhmedium.push_back(m_electronMediumLHSelector->accept(el));
      t_el_lhloose.push_back(m_electronLooseLHSelector->accept(el));

      t_el_topoetcone20.push_back(el->isolation(xAOD::Iso::topoetcone20));
      t_el_etcone20.push_back(el->isolation(xAOD::Iso::etcone20));

      for (size_t ieltr=0;ieltr<m_el_triggers.size();ieltr++)
	{
	  bool trig_match = 0;
	  //trig_match = m_trigEgammaMatchingTool->matchHLT(el,m_el_triggers.at(ieltr));
	  t_el_HLT_match.at(ieltr).push_back(trig_match);
	  const xAOD::Egamma* el_trig = 0;
	  //el_trig = m_trigEgammaMatchingTool->closestHLTObject(el,m_el_triggers.at(ieltr));
	  if (el_trig)
	    t_el_HLT_dR.at(ieltr).push_back(el->p4().DeltaR(el_trig->p4()));
	  else
	    t_el_HLT_dR.at(ieltr).push_back(999.);
	}
    }
}

void ZdcNtuple::processTriggerTowers()
{
  t_tt_pt.clear();
  t_tt_eta.clear();
  t_tt_phi.clear();
  t_tt_e.clear();
  t_tt_samp.clear();

  t_fcalEtA_TTsum = 0;
  t_fcalEtC_TTsum = 0;

  t_totalEt_TTsum = 0;
  t_totalEt24_TTsum = 0;

  if (m_TTcontainer)
    {
      ANA_MSG_DEBUG("processTriggerTowers(): size = %zd, " << m_TTcontainer->size() << " TTsums before: %f %f" << t_fcalEtA_TTsum << " " << t_fcalEtC_TTsum);
      
      for (const auto tt: *m_TTcontainer)
	{
	  t_totalEt_TTsum += tt->jepET()*1000;
	  if (TMath::Abs(tt->eta())<2.4)  t_totalEt24_TTsum += tt->jepET()*1000;
	  
	  if (tt->eta()>3.) t_fcalEtA_TTsum += tt->jepET()*1000;
	  if (tt->eta()<-3.) t_fcalEtC_TTsum += tt->jepET()*1000;
	  // "eta=" << tt->p4().Eta() << " phi=" << tt->p4().Phi() << " pt=" << tt->p4().Pt() << "...   TTsums during: " << t_fcalEtA_TTsum << " " << t_fcalEtC_TTsum << std::endl;
	  
	  if (tt->jepET()>0)
	    {
	      t_tt_pt.push_back(tt->jepET()*1000);
	      t_tt_eta.push_back(tt->eta());
	      t_tt_phi.push_back(tt->phi());
	      t_tt_e.push_back(1000*tt->jepET()*TMath::CosH(tt->eta()));
	      t_tt_samp.push_back(tt->sampling());
	    }
	}
    }
  
  ANA_MSG_DEBUG("processTriggerTowers(): TTsums after: " <<  t_fcalEtA_TTsum << " " <<  t_fcalEtC_TTsum);
  
}

void ZdcNtuple::processGaps()
{

  float eta_min = 5;
  float eta_max = -5;
  
  for (const auto cl: *m_caloClusters)
    {

      if (cl->pt()<m_gapPtMin) continue;
      
      int etabin = h_TCSigCut->GetXaxis()->FindBin(cl->eta());
      if (etabin<1 || etabin>h_TCSigCut->GetNbinsX()) continue;
      float sig_cut=h_TCSigCut->GetBinContent(etabin);
      float sig = cl->getMomentValue(xAOD::CaloCluster::CELL_SIGNIFICANCE);
      int cl_cell_sig_samp=static_cast<int>(cl->getMomentValue(xAOD::CaloCluster::CELL_SIG_SAMPLING));

      ANA_MSG_VERBOSE ("gapclus: etabin " << etabin << " sig_cut=" << sig_cut << " sig=" << sig << " samp=" << cl_cell_sig_samp);

      if(sig < sig_cut) continue;

      if(cl_cell_sig_samp>=CaloSampling::TileBar0 && cl_cell_sig_samp<=CaloSampling::TileExt2) continue;

      if(cl->eta() < eta_min) eta_min = cl->eta();
      if(cl->eta() > eta_max) eta_max = cl->eta();

    }

  t_edgeGapA = 4.9 - eta_max;
  t_edgeGapC = eta_min + 4.9;
  ANA_MSG_DEBUG("processGaps(): egA " << t_edgeGapA << " , egC " << t_edgeGapC);

}

void ZdcNtuple::processMBTS()
{
  ANA_MSG_DEBUG("processMBTS: trying to process!");
  t_mbts_countA = 0;
  t_mbts_countC = 0;
  t_T2mbts_countAin = 0;
  t_T2mbts_countCin = 0;
  t_mbts_timeA = 0.;
  t_mbts_timeC = 0.;
  t_mbts_timeDiff = 0.;
  
  if (m_mbtsInfo->size()>0)
    {
      t_mbts_countA = m_mbtsInfo->at(0)->countA();
      t_mbts_countC = m_mbtsInfo->at(0)->countC();
      t_mbts_timeA = m_mbtsInfo->at(0)->timeA();
      t_mbts_timeC = m_mbtsInfo->at(0)->timeC();
      t_mbts_timeDiff = m_mbtsInfo->at(0)->timeDiff();
    }
  else
    {
      ANA_MSG_INFO("processMBTS: Warning: MBTS info empty!");
    }

  for (int iside=0;iside<2;iside++)
    {
      for (int iin=0;iin<8;iin++)
	{
	  t_mbts_in_e[iside][iin]=0.;
	  t_mbts_in_t[iside][iin]=0.;
	  t_T2mbts_in_e[iside][iin]=0.;
	  t_T2mbts_in_t[iside][iin]=0.;
	}
      for (int iout=0;iout<4;iout++)
	{
	  t_mbts_out_e[iside][iout]=0.;
	  t_mbts_out_t[iside][iout]=0.;
	  t_T2mbts_out_e[iside][iout]=0.;
	  t_T2mbts_out_t[iside][iout]=0.;
	}
    }

  ANA_MSG_DEBUG ("filling MBTS");

  if (m_mbtsModules==0)
    {
      ANA_MSG_INFO("processMBTS: no MBTS container?");
      return;
    }

  for (const auto mbtsMod : *m_mbtsModules)
    {
      int iside = 1;
      if (mbtsMod->type()<0) iside=0.;
      float phibin = 0.;
      int iphibin = -1;
      if (mbtsMod->eta()>3)
	{
	  phibin = mbtsMod->phi()/(2*TMath::Pi()/8.)-0.4;
	  iphibin = int(phibin);
	  if (iphibin<0||iphibin>7) 
	    {
	      ANA_MSG_INFO("processMBTS: MBTS has bad phi bin");
	      continue;
	    }
	  t_mbts_in_e[iside][iphibin]=mbtsMod->e();
	  t_mbts_in_t[iside][iphibin]=mbtsMod->time();
	}
      else
	{
	  phibin = mbtsMod->phi()/(2*TMath::Pi()/4.)-0.24;
	  iphibin = int(phibin);	  
	  if (iphibin<0||iphibin>3) 
	    {
	      ANA_MSG_INFO("processMBTS: MBTS has bad phi bin");
	      continue;
	    }
	  t_mbts_out_e[iside][iphibin]=mbtsMod->e();
	  t_mbts_out_t[iside][iphibin]=mbtsMod->time();
	}
    }

  if (!m_trigT2MbtsBits) return;

  for (const auto mbtsBits : *m_trigT2MbtsBits)
    {
      const std::vector<float>& energies = mbtsBits->triggerEnergies();
      const std::vector<float>& times = mbtsBits->triggerTimes();
      for (int imbts=0;imbts<32;imbts++)
	{
	  int side=imbts/16;
	  int ring = (imbts - 16*side)/8;
	  bool isInner = (ring==0);
	  int index = (imbts - 16*side - ring*8);
	  if (!isInner) 
	    {
	      if ((index%2)!=0) continue; // skip odd out ring 
	      index /= 2;
	    }
	  int iside = (side==0) ? 1 : 0; // code maps side 1 into first 16 bits and side -1 into second set

          ANA_MSG_VERBOSE ("imbts=" << imbts << " isInner=" << isInner << " iside=" << iside << " index=" << index << " e=" << energies.at(imbts) << " t=" << times.at(imbts));
	  if (isInner)
	    {
	      t_T2mbts_in_e[iside][index] = energies.at(imbts);
	      t_T2mbts_in_t[iside][index] = times.at(imbts);
	      if (TMath::Abs(times.at(imbts))<12.0 && energies.at(imbts)>40/222.)
		{
		  if (iside==0) t_T2mbts_countCin++;
		  if (iside==1) t_T2mbts_countAin++;
		}
	    }
	  else
	    {
	      t_T2mbts_out_e[iside][index] = energies.at(imbts);
	      t_T2mbts_out_t[iside][index] = times.at(imbts);
	    }
	}
    }
  //t_mbtsToolDecision = m_hiMBTSTimeSelectionTool->accept();
  return;
}

void ZdcNtuple::processInDet()
{
  ANA_MSG_DEBUG("processInDet(): processing tracks & vertices!");
  t_ntrk = 0;
  t_nvx = 0;
  t_vxntrk = 0;
  t_vx_trk_index.clear();
  t_vxsumpt2 = 0;
  t_vxtype = 0;
  t_pvindex = -1;
  t_puvxntrk = 0;
  t_puvxsumpt = 0;
  t_vxnlooseprimary = 0;
  t_vxnminbias = 0;

  int i;
  for (i=0;i<3;i++) t_vx[i] = 0;
  for (i=0;i<6;i++) t_vxcov[i] = 0;

  const xAOD::Vertex* primary_vertex = nullptr;
  size_t pv_index = -1;
  size_t vx_index = 0;
  float max_pileup_sumpT=0.;
  int max_pileup_nTrack=0;
  float max_pileup_z=0;
  int nStrongPileup = 0;

  t_nvtx = 0;
  t_vtx_type.clear();
  t_vtx_x.clear();
  t_vtx_y.clear();
  t_vtx_z.clear();
  t_vtx_ntrk_all.clear();
  t_vtx_sumpt2_all.clear();
  t_vtx_ntrk.clear();
  t_vtx_sumpt2.clear();
  t_vtx_trk_index.clear();

  if (m_primaryVertices)
    {
      ANA_MSG_DEBUG("processInDet: processing vertices");

      t_nvx = m_primaryVertices->size();

      // start of new vertex representation
      t_nvtx = m_primaryVertices->size();
      for (const auto vertex: *m_primaryVertices)
	{
	  float vtx_sumpt2 = 0;
	  int vtx_ntrk = 0;

	  t_vtx_type.push_back(vertex->vertexType());
	  //t_vtx_x.push_back(vertex->x());
	  //t_vtx_y.push_back(vertex->y());
	  t_vtx_x.push_back(0);
	  t_vtx_y.push_back(0);
	  t_vtx_z.push_back(vertex->z());
	  for (size_t itr=0;itr<vertex->nTrackParticles();itr++)
	    {
	      /*
	      if (m_trackSelectionTools.at(0)->accept(* vertex->trackParticle(itr),vertex)) // LoosePrimary
		{
		  vtx_sumpt2 += TMath::Power(vertex->trackParticle(itr)->pt(),2.);
		  vtx_ntrk++;
		}
	      */
	    }
	  
	  t_vtx_ntrk.push_back(vtx_ntrk);
	  t_vtx_sumpt2.push_back(vtx_sumpt2/1e6);
	  t_vtx_ntrk_all.push_back(vertex->nTrackParticles());
	  
	  if (vertex->isAvailable<float>("sumPt2"))
	    t_vtx_sumpt2_all.push_back(vertex->auxdataConst<float>("sumPt2"));
	  else
	    t_vtx_sumpt2_all.push_back(-1);
	    
	  std::vector<int16_t> trk_index;
	  if ( m_trackParticles && vertex->nTrackParticles()<=trackLimit )
	    {
	      const std::vector< ElementLink< xAOD::TrackParticleContainer > >& vxTrackParticles = vertex->trackParticleLinks();	  
	      for (size_t itrk = 0;itrk<vxTrackParticles.size();itrk++)
		{
		  ElementLink< xAOD::TrackParticleContainer > trkLink = vxTrackParticles.at(itrk);
		  trk_index.push_back(trkLink.index());
		}
	    }
	  t_vtx_trk_index.push_back(trk_index);	  

	  // end of new vertex representation

	  if (vertex->vertexType() == xAOD::VxType::PriVtx)
	    {
	      primary_vertex = vertex;
	      pv_index = vx_index;
	    }
	  if (vertex->vertexType() == xAOD::VxType::PileUp)
	    {
	      float pileup_sumpT=0;
	      int pileup_nTrack=0;
	      for (size_t itr=0;itr<vertex->nTrackParticles();itr++)
		{
		  int track_quality = trackQuality(vertex->trackParticle(itr),vertex);
		  if (track_quality != -1 && (track_quality&128)!=0)
		    {
		      pileup_nTrack++;
		      pileup_sumpT += vertex->trackParticle(itr)->pt();
		    }
		}
	      if (pileup_sumpT>max_pileup_sumpT)
		{
		  max_pileup_sumpT = pileup_sumpT;
		  max_pileup_nTrack = pileup_nTrack;
		  max_pileup_z = vertex->z();
		}
	      if (pileup_sumpT>5e3 || pileup_nTrack>5) nStrongPileup++;
	    }
	  vx_index++;
	}
    }
  
  t_nstrong = nStrongPileup;

  if (primary_vertex != nullptr)
    {
      t_vx[0] = primary_vertex->x();
      t_vx[1] = primary_vertex->y();
      t_vx[2] = primary_vertex->z();
      /*
      const std::vector<float>& cov = primary_vertex->covariance();
      */
      //for (size_t i=0;i<cov.size();i++)
      for (size_t i=0;i<6;i++)
	{
	  //t_vxcov[i] = cov.at(i);
	  t_vxcov[i] = 0;
	}
      t_vxntrk = primary_vertex->nTrackParticles();
      if (primary_vertex->isAvailable<float>("sumPt2"))
	t_vxsumpt2 = primary_vertex->auxdata<float>("sumPt2");
      else
	t_vxsumpt2 = 0;
      t_vxtype = primary_vertex->vertexType();
      t_pvindex = pv_index;
      t_puvxz = max_pileup_z;
      t_puvxsumpt = max_pileup_sumpT;
      t_puvxntrk = max_pileup_nTrack;

      const std::vector< ElementLink< xAOD::TrackParticleContainer > >& vxTrackParticles = primary_vertex->trackParticleLinks();
      
      for (size_t itrk = 0;itrk<vxTrackParticles.size();itrk++)
	{
	  //const xAOD::TrackParticle* track = primary_vertex->trackParticle(itrk);
	  ElementLink< xAOD::TrackParticleContainer > trkLink = vxTrackParticles.at(itrk);
	  if (!trkLink.isValid()) continue;
	  //if (m_trackSelectionTools.at(0)->accept(*(*(trkLink)),primary_vertex) ) t_vxnlooseprimary++;
	  //if (m_trackSelectionTools.at(3)->accept(*(*(trkLink)),primary_vertex) ) t_vxnminbias++;
	  if (m_trackParticles)
	    {
	      //if (m_trackParticles->size()<=trackLimit || 
	      //  (t_decisions[28]&&m_trackParticles->size()<200)) t_vx_trk_index.push_back(trkLink.index());
	      if (m_trackParticles->size()<=trackLimit)
		t_vx_trk_index.push_back(trkLink.index());
	    }
	}

    }

  
  if (m_trackParticles)
    {
      ANA_MSG_DEBUG("processInDet: processing trackss");

      t_trk_pt.clear();
      t_trk_eta.clear();
      t_trk_phi.clear();
      t_trk_e.clear();
      t_trk_index.clear();
      t_trk_theta.clear();
      t_trk_charge.clear();
      t_trk_d0.clear();
      t_trk_z0.clear();
      t_trk_vz.clear();
      t_trk_vtxz.clear();
      t_trk_quality.clear();
      t_trk_nPixHits.clear();
      t_trk_nSctHits.clear();
      t_trk_nPixDead.clear();
      t_trk_nSctDead.clear();
      t_trk_nPixHoles.clear();
      t_trk_nSctHoles.clear();
      t_trk_nTrtHits.clear();
      t_trk_nTrtOutliers.clear();
      t_trk_inPixHits.clear();
      t_trk_exPixHits.clear();
      t_trk_ninPixHits.clear();
      t_trk_nexPixHits.clear();
      t_trk_pixeldEdx.clear();

      t_ntrk = m_trackParticles->size();
      //int index = 0;
      //if ( enableTracks && (t_ntrk<=trackLimit || (t_decisions[28]&&m_trackParticles->size()<200)))

      if ( !enableTracks ) return;

      if ( t_ntrk<=trackLimit ) // dump all tracks || (t_decisions[28]&&m_trackParticles->size()<200)))
	{
	  int trk_index = 0;
	  for (const auto track : *m_trackParticles)
	    {
	      writeTrack(track,primary_vertex,trk_index++);
	    }
	}
      else // write small vertices
	{
	  for (const auto vertex: *m_primaryVertices)
	    {
	      if (vertex->nTrackParticles() <= trackLimit )
		{
		  const std::vector< ElementLink< xAOD::TrackParticleContainer > >& vxTrackParticles = vertex->trackParticleLinks();	  
		  for (size_t itrk = 0;itrk<vxTrackParticles.size();itrk++)
		    {
		      ElementLink< xAOD::TrackParticleContainer > trkLink = vxTrackParticles.at(itrk);
		      writeTrack(*trkLink,vertex,trkLink.index());
		    }
		}
	    }	  
	}
    }

  //t_vertexToolDecision = m_hiVertexSelectionTool->accept();

  return;
}

void ZdcNtuple::writeTrack(const xAOD::TrackParticle* track,const xAOD::Vertex* vertex, int trk_index)
{
  t_trk_pt.push_back(track->pt());
  t_trk_eta.push_back(track->eta());
  t_trk_phi.push_back(track->phi());
  t_trk_e.push_back(track->e());
  t_trk_index.push_back(trk_index);
  t_trk_theta.push_back(track->theta());
  t_trk_charge.push_back(track->charge());
  t_trk_d0.push_back(track->d0());
  t_trk_z0.push_back(track->z0());
  t_trk_vz.push_back(track->vz());

  float vtxz = -999.;
  if (track->vertex())
    {
      vtxz = track->vertex()->z();
    }
  t_trk_vtxz.push_back(vtxz);

  t_trk_quality.push_back(trackQuality(track,vertex));
  t_trk_inPixHits.push_back(track->auxdata<uint8_t>("numberOfInnermostPixelLayerHits"));
  t_trk_exPixHits.push_back(track->auxdata<uint8_t>("expectInnermostPixelLayerHit"));
  t_trk_ninPixHits.push_back(track->auxdata<uint8_t>("numberOfNextToInnermostPixelLayerHits"));
  t_trk_nexPixHits.push_back(track->auxdata<uint8_t>("expectNextToInnermostPixelLayerHit"));
  t_trk_nSctHits.push_back(track->auxdata<uint8_t>("numberOfSCTHits"));
  t_trk_nPixHits.push_back(track->auxdata<uint8_t>("numberOfPixelHits"));
  t_trk_nSctDead.push_back(track->auxdata<uint8_t>("numberOfSCTDeadSensors"));
  t_trk_nPixDead.push_back(track->auxdata<uint8_t>("numberOfPixelDeadSensors"));
  t_trk_nSctHoles.push_back(track->auxdata<uint8_t>("numberOfSCTHoles"));
  t_trk_nPixHoles.push_back(track->auxdata<uint8_t>("numberOfPixelHoles"));
  t_trk_nTrtHits.push_back(track->auxdata<uint8_t>("numberOfTRTHits"));
  t_trk_nTrtOutliers.push_back(track->auxdata<uint8_t>("numberOfTRTOutliers"));
  float pixeldEdx=0;
  track->summaryValue(pixeldEdx,xAOD::SummaryType::pixeldEdx);
  t_trk_pixeldEdx.push_back(pixeldEdx);
}

int ZdcNtuple::trackQuality(const xAOD::TrackParticle* track, const xAOD::Vertex* vertex)
{
  // Code from Soumya

  // dummy
  if (vertex){}

  if (!track) return -1;

  float pt      = track->pt();
  float eta     = track->eta();
  float chi2    = track->chiSquared();
  float ndof    = track->numberDoF();
  float d0      = track->d0();
  float z0_wrtPV= track->z0();
  float theta   = track->theta();

  uint8_t   n_Ipix_hits     =track->auxdata<uint8_t>("numberOfInnermostPixelLayerHits");
  uint8_t   n_Ipix_expected =track->auxdata<uint8_t>("expectInnermostPixelLayerHit");
  uint8_t   n_NIpix_hits    =track->auxdata<uint8_t>("numberOfNextToInnermostPixelLayerHits");
  uint8_t   n_NIpix_expected=track->auxdata<uint8_t>("expectNextToInnermostPixelLayerHit");
  uint8_t   n_sct_hits      =track->auxdata<uint8_t>("numberOfSCTHits");
  uint8_t   n_pix_hits      =track->auxdata<uint8_t>("numberOfPixelHits");
  uint8_t   n_sct_holes     =track->auxdata<uint8_t>("numberOfSCTHoles");
  //uint8_t   n_pix_holes     =track->auxdata<uint8_t>("numberOfPixelHoles");
  uint8_t   n_sct_dead      =track->auxdata<uint8_t>("numberOfSCTDeadSensors");
  uint8_t   n_pix_dead      =track->auxdata<uint8_t>("numberOfPixelDeadSensors");

  if(fabs(eta)>2.5) return -1;

  bool pass_min_bias=true;
  {
    if(n_Ipix_expected>0){
      if (n_Ipix_hits==0) pass_min_bias=false;
    }
    else{
      if(n_NIpix_expected>0 && n_NIpix_hits==0) pass_min_bias=false;
    }

    int n_sct=n_sct_hits+n_sct_dead;
    if     (pt<=300) {if (n_sct <2)  pass_min_bias=false;}
    else if(pt<=400) {if (n_sct <4)  pass_min_bias=false;}
    else if(pt> 400) {if (n_sct <6)  pass_min_bias=false;}

    int n_pix=n_pix_hits+n_pix_dead;
    if(n_pix<=0) pass_min_bias=false;
    if(fabs(d0)>1.5) pass_min_bias=false;
    if(fabs(z0_wrtPV*sin(theta))>1.5) pass_min_bias=false;

    if(pt>10000 && TMath::Prob(chi2,ndof)<=0.01) pass_min_bias=false;
    //if(n_sct_holes>1 || n_pix_holes>0) continue;                                                                          //if(n_pix_hits<3 || n_sct_hits<8) continue;                                                                        
  }

  bool pass_hi_loose=true;
  {
    if(n_Ipix_expected>0){
      if (n_Ipix_hits==0) pass_hi_loose=false;
    }
    else{
      if(n_NIpix_expected>0 && n_NIpix_hits==0) pass_hi_loose=false;
    }

    if(n_pix_hits==0) pass_hi_loose=false;
    if(n_sct_hits< 6) pass_hi_loose=false;
    if(pt>10000 && TMath::Prob(chi2,ndof)<=0.01) pass_hi_loose=false;
    if(fabs(d0) >1.5) pass_hi_loose=false;
    if(fabs(z0_wrtPV*sin(theta))>1.5) pass_hi_loose=false;
  }

  bool pass_hi_tight=true;
  if(!pass_hi_loose) pass_hi_tight=false;
  else{
    if(n_pix_hits <2  ) pass_hi_tight=false;
    if(n_sct_hits <8  ) pass_hi_tight=false;
    if(n_sct_holes>1  ) pass_hi_tight=false;
    if(fabs(d0)   >1.0) pass_hi_tight=false;
    if(fabs(z0_wrtPV*sin(theta))>1.0) pass_hi_tight=false;
    if(ndof==0) pass_hi_tight=false;
    else if(chi2/ndof>6) pass_hi_tight=false;
  }

  int quality=0;
  if(pass_min_bias) quality+=2;
  if(pass_hi_loose) quality+=4;
  if(pass_hi_tight) quality+=8;

  /*
  for (size_t i = 0;i<m_trackSelectionTools.size();i++)
    {
      bool acc = m_trackSelectionTools.at(i)->accept(*track,vertex);
      if ( acc ) quality += 16<<i;      
    }
  */

  return quality;
}

void ZdcNtuple::processClusters()
{
  //t_nclus = 0;

  t_cc_pt.clear();
  t_cc_eta.clear();
  t_cc_phi.clear();
  t_cc_e.clear();
  t_cc_raw_m.clear();
  t_cc_raw_eta.clear();
  t_cc_raw_phi.clear();
  t_cc_raw_e.clear();
  t_cc_raw_samp.clear();
  t_cc_layer.clear();
  t_cc_sig.clear();

  t_nclus = m_caloClusters->size();

  t_clusEt = 0;
  t_clusEtMax = -999;
  t_clusetaMax = 0;
  t_clusphiMax = 0;

  for (const auto cluster : *m_caloClusters)
    {
      t_cc_pt.push_back(cluster->pt());
      t_cc_eta.push_back(cluster->eta());
      t_cc_phi.push_back(cluster->phi());
      t_cc_e.push_back(cluster->e());
      t_cc_raw_m.push_back(cluster->rawM());
      t_cc_raw_eta.push_back(cluster->rawEta());
      t_cc_raw_phi.push_back(cluster->rawPhi());
      t_cc_raw_e.push_back(cluster->rawE());

      std::vector<float> energies;

      for (size_t s = CaloSampling::PreSamplerB; s< CaloSampling::Unknown; s++ )
	{
	  bool hasSample=cluster->hasSampling( (xAOD::CaloCluster::CaloSample) s );
	  float e = 0;
	  if (hasSample)
	    {
	      e = cluster->eSample( (xAOD::CaloCluster::CaloSample) s);
	    }
	  energies.push_back(e);
	}
      t_cc_raw_samp.push_back(energies);

      float et = cluster->e()/TMath::CosH(cluster->eta());
      t_clusEt += et;
      if (et>t_clusEtMax)
	{
	  t_clusEtMax = et;
	  t_clusetaMax =cluster->eta();
	  t_clusphiMax =cluster->phi();
	}

      double cell_sig = 0;
      if (!cluster->retrieveMoment(xAOD::CaloCluster::CELL_SIGNIFICANCE,cell_sig)){ANA_MSG_DEBUG("processClusters() : No CELL_SIGNIFICANCE!");}
      t_cc_sig.push_back(cell_sig);
      double cell_layer = 0;
      if (!cluster->retrieveMoment(xAOD::CaloCluster::CELL_SIG_SAMPLING,cell_layer)){ANA_MSG_DEBUG("processClusters() : No CELL_SIG_SAMPLING!");}
      t_cc_layer.push_back(int(cell_layer));
      //t_nclus++;
    }

  //if (t_decisions[28]) return; // HACK! always keep clusters if my primary trigger.

  //ANA_MSG_INFO("processClusters()", "t_ntrk = %d, enable clusters = %d", t_ntrk, enableClusters);

  if ( (!enableClusters) || (t_ntrk >= trackLimit) ) // if disabled or if too many tracks
    {
      t_cc_pt.clear();
      t_cc_eta.clear();
      t_cc_phi.clear();
      t_cc_e.clear();
      t_cc_layer.clear();
      t_cc_sig.clear();
    }
  else
    {
      ANA_MSG_DEBUG("processClusters(): keeping clusters");
    }
  return;
}

void ZdcNtuple::processModules()
{
  ANA_MSG_DEBUG("processModules: processing ZDC modules");

  // make shallow copy

  //std::pair< xAOD::ZdcModuleContainer*, xAOD::ShallowAuxContainer* > zdcModules_shallowCopy =
  //  xAOD::shallowCopyContainer( *m_zdcModules );

  // Loop over modules and histogram individual entries

  float sum_A_HG = 0;
  float sum_A_LG = 0;
  float sum_C_HG = 0;
  float sum_C_LG = 0;
  float sum_A_HG_reproc = 0;
  float sum_A_LG_reproc= 0;
  float sum_C_HG_reproc = 0;
  float sum_C_LG_reproc= 0;

  //for (const auto zdcmod : *zdcModules_shallowCopy.first)
  for (const auto zdcmod : *m_zdcModules)
    {
      //ANA_MSG_INFO("processModules(): side %d mod %d type %d channel %d",zdcmod->side(),zdcmod->zdcModule(),zdcmod->type(),zdcmod->channel() );
      int side_index = 0;
      int mod_index = zdcmod->zdcModule();
      if (zdcmod->side()>0) side_index = 1;
      if (zdcmod->type()==0)
	{
	  if (zdcmod->channel()!=0)
	    {
	      ANA_MSG_INFO("execute: Integrity check failed, bigtube channel!=0");
	      continue;
	    }
	  //int h_index = side_index*4 + zdcmod->zdcModule();

	  t_amp[side_index][mod_index] = zdcmod->amplitude();
	  t_time[side_index][mod_index] = zdcmod->time();
	  t_ampLG[side_index][mod_index] = zdcmod->amplitudeG0();
	  t_ampHG[side_index][mod_index] = zdcmod->amplitudeG1();

	  if (zdcmod->zdcModule()==0 && flipDelay)
	    reprocessZdcModule(zdcmod,1); // reprocess delayed amplitudes for EM only
	  else
	    reprocessZdcModule(zdcmod,0); // don't do it for others
	    
	  float amp0_reproc = zdcmod->auxdecor<float>("amplitudeG0_reproc");
	  float amp1_reproc = zdcmod->auxdecor<float>("amplitudeG1_reproc");

	  if (enableOutputTree)
	    {
	      t_ampLG_rp[side_index][mod_index] = amp0_reproc;
	      t_ampHG_rp[side_index][mod_index] = amp1_reproc;
	      
	      if (side_index==0) // side C
		{
		  sum_C_LG += zdcmod->amplitudeG0();
		  sum_C_HG += zdcmod->amplitudeG1();
		  sum_C_LG_reproc += amp0_reproc;
		  sum_C_HG_reproc += amp1_reproc;
		}
	      else // side A
		{
		  sum_A_LG += zdcmod->amplitudeG0();
		  sum_A_HG += zdcmod->amplitudeG1();
		  sum_A_LG_reproc += amp0_reproc;
		  sum_A_HG_reproc += amp1_reproc;
		}
	      
	      if (enableOutputSamples)
		{
		  for (unsigned int isamp=0;isamp<nsamplesZdc;isamp++) // 7 samples
		    {
		      if (nsamplesZdc==7)
			{
			  t_raw7[side_index][mod_index][0][0][isamp] = (*(zdcmod->TTg0d0Link()))->adc().at(isamp);
			  t_raw7[side_index][mod_index][0][1][isamp] = (*(zdcmod->TTg0d1Link()))->adc().at(isamp);
			  t_raw7[side_index][mod_index][1][0][isamp] = (*(zdcmod->TTg1d0Link()))->adc().at(isamp);
			  t_raw7[side_index][mod_index][1][1][isamp] = (*(zdcmod->TTg1d1Link()))->adc().at(isamp);
			  ANA_MSG_DEBUG (side_index << " " << mod_index << " " 
					       << t_raw7[side_index][mod_index][0][0][isamp] << " "
					       << t_raw7[side_index][mod_index][0][1][isamp] << " "
					       << t_raw7[side_index][mod_index][1][0][isamp] << " "
                                         << t_raw7[side_index][mod_index][1][1][isamp] << " ");
			}

		      if (nsamplesZdc==15)
			{
			  t_raw15[side_index][mod_index][0][0][isamp] = (*(zdcmod->TTg0d0Link()))->adc().at(isamp);
			  t_raw15[side_index][mod_index][0][1][isamp] = (*(zdcmod->TTg0d1Link()))->adc().at(isamp);
			  t_raw15[side_index][mod_index][1][0][isamp] = (*(zdcmod->TTg1d0Link()))->adc().at(isamp);
			  t_raw15[side_index][mod_index][1][1][isamp] = (*(zdcmod->TTg1d1Link()))->adc().at(isamp);
			}
		    }
		}
	    }
	}
    }

  t_sumHG[0] = sum_C_HG;
  t_sumHG[1] = sum_A_HG;
  t_sumLG[0] = sum_C_LG;
  t_sumLG[1] = sum_A_LG;
  t_sumHG_rp[0] = sum_C_HG_reproc;
  t_sumHG_rp[1] = sum_A_HG_reproc;
  t_sumLG_rp[0] = sum_C_LG_reproc;
  t_sumLG_rp[1] = sum_A_LG_reproc;

  // if (msgLvl (MSG::VERBOSE))
  // {
  //   // test code for reprocessing tool
  //   float checkSumLG_A_mf = ZDC::getAmplitudeSumG0(m_zdcModules,ZDC::sideA,"_mf");
  //   float checkSumLG_C_mf = ZDC::getAmplitudeSumG0(m_zdcModules,ZDC::sideC,"_mf");
  //   float checkSumLG_A_pf = ZDC::getAmplitudeSumG0(m_zdcModules,ZDC::sideA,"_pf");
  //   float checkSumLG_C_pf = ZDC::getAmplitudeSumG0(m_zdcModules,ZDC::sideC,"_pf");
  //   float checkSumLG_A_si = ZDC::getAmplitudeSumG0(m_zdcModules,ZDC::sideA,"_si");
  //   float checkSumLG_C_si = ZDC::getAmplitudeSumG0(m_zdcModules,ZDC::sideC,"_si");

  //   ANA_MSG_VERBOSE ("sum_A_LG_reproc=" << sum_A_LG_reproc 
  //                    << " checkSumLG_A_mf=" << checkSumLG_A_mf 
  //                    << " checkSumLG_A_pf=" << checkSumLG_A_pf 
  //                    << " checkSumLG_A_si=" << checkSumLG_A_si);
  //   ANA_MSG_VERBOSE ("sum_C_LG_reproc=" << sum_C_LG_reproc 
  //                    << " checkSumLG_C_mf=" << checkSumLG_C_mf 
  //                    << " checkSumLG_C_pf=" << checkSumLG_C_pf 
  //                    << " checkSumLG_C_si=" << checkSumLG_C_si);
  // }

  /*
  xAOD::TStore* store = wk()->xaodStore();
  ANA_CHECK("execute()",store->record( zdcModules_shallowCopy.first, "ShallowCopiedZdcModules" ));
  ANA_CHECK("execute()",store->record( zdcModules_shallowCopy.second, "ShallowCopiedZdcModulesAux." ));
  */

}

void ZdcNtuple::reprocessZdcModule(const xAOD::ZdcModule* zdcmod, bool flipdelay)
{

  float newAmpG0 =  zdcmod->amplitudeG0();
  float newAmpG1 = zdcmod->amplitudeG1();
  ANA_MSG_VERBOSE ("Old  G0=" << newAmpG0 << " G1=" << newAmpG1);

  zdcmod->auxdecor<float>("amplitudeG0_reproc") = newAmpG0;
  zdcmod->auxdecor<float>("amplitudeG1_reproc") = newAmpG1;

  if (!flipdelay) return;

  if ( !(zdcmod->TTg0d1Link().isValid() && zdcmod->TTg1d1Link().isValid()) ) return;

  const std::vector<uint16_t>& v0 = (*(zdcmod->TTg0d1Link()))->adc();
  const std::vector<uint16_t>& v1 = (*(zdcmod->TTg1d1Link()))->adc();
  uint16_t maxv0 = 0;
  uint16_t maxv1 = 0;
  for (size_t isamp=0;isamp<v0.size();isamp++)
    {
      if (v0.at(isamp)>maxv0) maxv0 = v0.at(isamp);
      if (v1.at(isamp)>maxv1) maxv1 = v1.at(isamp);				
    }
  
  newAmpG0 = maxv0 - v0.at(0);
  newAmpG1 = maxv1 - v1.at(0);
  ANA_MSG_VERBOSE ("Flip G0=" << newAmpG0 << " G1=" << newAmpG1);

  zdcmod->auxdecor<float>("amplitudeG0_reproc") = newAmpG0;
  zdcmod->auxdecor<float>("amplitudeG1_reproc") = newAmpG1;

  return;
}

uint32_t ZdcNtuple::acceptEvent()
{
  uint32_t passbits = 0;

  if(!m_isMC)
    { 
      if (useGRL)
	{
	  if(!m_grl->passRunLB(*m_eventInfo)){
	    passbits += 1; // UPC GRL
	  }
	}
      /*
      if(!m_grl_mb->passRunLB(*m_eventInfo)){
	passbits += 4; // MB GRL
      }
      */

      if(   (m_eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error ) 
	    || (m_eventInfo->errorState(xAOD::EventInfo::Tile)==xAOD::EventInfo::Error ) 
	    || (m_eventInfo->errorState(xAOD::EventInfo::SCT)==xAOD::EventInfo::Error ) 
	    || (m_eventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18) ) )
	{
	  passbits += 2;
	} // end if event flags check
    } // end if the event is data

  return passbits;
}

void ZdcNtuple::setupTriggerHistos()
{
  if (!enableTrigger) return;

  m_outputTree = tree( "zdcTree" );
  ANA_MSG_INFO("setupTriggerHistos(): Setting up trigger histos and ntuple = " << m_outputTree);

  //auto chainGroup = m_trigDecisionTool->getChainGroup("L1_ZDC.*");
  std::vector<std::string> triggers;
  std::vector<std::string> rerunTriggers;
  bool zdc_triggers = zdcCalib;
  bool mb_triggers = mb2015 &&!(zdcCalib||zdcLaser);
  bool upc_triggers = upc2015 &&!(zdcCalib||zdcLaser);
  bool limitedupc_triggers = upcL2015 &&!(zdcCalib||zdcLaser);
  bool express2016A_triggers = express2016A &&!(zdcCalib||zdcLaser);
  bool upc2016A_triggers = upc2016A &&!(zdcCalib||zdcLaser);
  bool upc2018_triggers = upc2018 &&!(zdcCalib||zdcLaser);
  //bool main2016A_triggers = main2016A &&!(zdcCalib||zdcLaser);
  bool express2016B_triggers = express2016B &&!(zdcCalib||zdcLaser);
  bool upc2016B_triggers = upc2016B &&!(zdcCalib||zdcLaser);
  bool upc2016C_triggers = upc2016C &&!(zdcCalib||zdcLaser);
  bool main2016B_triggers = main2016B &&!(zdcCalib||zdcLaser);
  bool mboverlay2016_triggers = mboverlay2016 && !(zdcCalib||zdcLaser);

  // ZDC triggers
  if (zdc_triggers)
    {
      triggers.push_back("L1_ZDC_A");
      triggers.push_back("L1_ZDC_C");
      triggers.push_back("L1_ZDC_AND");
      triggers.push_back("L1_ZDC_A_C");
      triggers.push_back("L1_MBTS_2");
    }


  if (limitedupc_triggers)
    {
      triggers.push_back("HLT_noalg_L1MU0_VTE50");
      triggers.push_back("HLT_mb_sptrk_vetombts2in_L1MU0_VTE50");
      triggers.push_back("HLT_mb_perf_L1RD1_EMPTY");
      triggers.push_back("HLT_noalg_cosmiccalo_L1RD1_EMPTY");
    }

  if (mboverlay2016_triggers)
    {
      triggers.push_back("HLT_mb_sptrk_L1MBTS_1_OVERLAY");
      triggers.push_back("HLT_noalg_L1TE10_OVERLAY");
      triggers.push_back("HLT_noalg_L1TE50_OVERLAY");
    }

  if (upc_triggers)
    {
      // UPC triggers
      triggers.push_back("HLT_noalg_L1ZDC_XOR");
      triggers.push_back("HLT_noalg_L1ZDC_XOR_TE5_VTE200");
      triggers.push_back("HLT_mb_sptrk_vetombts2in_L1ZDC_XOR_TE5_VTE200");
      triggers.push_back("HLT_noalg_L1ZDC_A_C_TE5_VTE200");
      triggers.push_back("HLT_noalg_L1ZDC_A_C_VTE50");
      //
      triggers.push_back("HLT_mb_sptrk_vetombts2in_L1ZDC_A_C_VTE50");
      triggers.push_back("HLT_hi_loose_upc_L1ZDC_A_C_VTE50");
      triggers.push_back("HLT_mb_sptrk_ion_L1ZDC_VTE200");
      triggers.push_back("HLT_hi_gg_upc_L1TE5_VTE200");
      //
      triggers.push_back("HLT_mb_sptrk_ion_L1ZDC_A_C_TE5_VTE200");
      triggers.push_back("HLT_mb_sptrk_ion_L1ZDC_XOR_TE5_VTE200");
      //
      triggers.push_back("HLT_j10_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200");
      triggers.push_back("HLT_j10_320eta490_ion_L1TE5_VTE200");
      triggers.push_back("HLT_j10_320eta490_ion_L1ZDC_XOR_TE5_VTE200");
      triggers.push_back("HLT_j10_320eta490_ion_L1ZDC_A_C_TE5_VTE200");
      //
      triggers.push_back("HLT_j5_ion_L1TE5_VTE200");
      triggers.push_back("HLT_j10_ion_L1TE5_VTE200");
      triggers.push_back("HLT_j15_ion_L1TE5_VTE200");
      triggers.push_back("HLT_j20_ion_L1TE5_VTE200");
      //
      triggers.push_back("HLT_j5_ion_L1ZDC_XOR_TE5_VTE200");
      triggers.push_back("HLT_j10_ion_L1ZDC_XOR_TE5_VTE200");
      triggers.push_back("HLT_j15_ion_L1ZDC_XOR_TE5_VTE200");
      triggers.push_back("HLT_j20_ion_L1ZDC_XOR_TE5_VTE200");
      //
      triggers.push_back("HLT_j5_ion_L1ZDC_A_C_TE5_VTE200");
      triggers.push_back("HLT_j10_ion_L1ZDC_A_C_TE5_VTE200");
      triggers.push_back("HLT_j15_ion_L1ZDC_A_C_TE5_VTE200");
      triggers.push_back("HLT_j20_ion_L1ZDC_A_C_TE5_VTE200");
      //
      triggers.push_back("HLT_noalg_L1MU0_VTE50");
      triggers.push_back("HLT_mb_sptrk_vetombts2in_L1MU0_VTE50");
      //
    }
  

  if (mb_triggers)
    {
      triggers.push_back("HLT_mb_sp");
      triggers.push_back("HLT_noalg_mb_L1TE50");
      triggers.push_back("HLT_mb_sptrk_ion_L1ZDC_A_C_VTE50");
      triggers.push_back("HLT_mb_perf_L1RD1_FILLED");
      triggers.push_back("HLT_mb_perf_L1RD1_EMPTY");
      triggers.push_back("HLT_hi_perfzdc_ucc_L1ZDC_A_C");
      triggers.push_back("HLT_noalg_mb_L1ZDC_A_C");
      triggers.push_back("HLT_hi_v2_th16_L1TE50");
      triggers.push_back("HLT_hi_v3_th16_L1TE50");
      triggers.push_back("HLT_hi_th2_ucc_L1TE10000");
      triggers.push_back("HLT_hi_th3_ucc_L1TE10000");
      triggers.push_back("HLT_hi_th2_ucc_L1TE12000");
      triggers.push_back("HLT_hi_th3_ucc_L1TE12000");
      triggers.push_back("HLT_hi_th2_ucc_L1TE14000");
      triggers.push_back("HLT_hi_th3_ucc_L1TE14000");
      triggers.push_back("HLT_mb_mbts_L1MBTS_1_1");

      // triggers.push_back("HLT_noalg_mb_L1MBTS_1_1");
      // triggers.push_back("HLT_noalg_mb_L1ZDC_AND");
      // triggers.push_back("HLT_noalg_mb_L1ZDC_A_C");
    }

  if (express2016A_triggers)
    {
      triggers.push_back("HLT_mu4_L1MU4_VZDC_A");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10");
      triggers.push_back("HLT_2mu4_L12MU4_VZDC_A");
      triggers.push_back("HLT_mu4_L1MU4_VTE10_VZDC_A");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10_VZDC_A");
      triggers.push_back("HLT_mu6");
      triggers.push_back("HLT_mb_mbts_L1MBTS_1");
      triggers.push_back("HLT_mb_sptrk");
      triggers.push_back("HLT_mb_sptrk_L1MBTS_1");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1MU4_VTE10");
      triggers.push_back("HLT_mb_perf_L1MBTS_2");
      triggers.push_back("HLT_mb_sp5000_trk260_hmt_L1TE160");
      triggers.push_back("HLT_noalg_mb_L1MBTS_1");
      triggers.push_back("HLT_noalg_mb_L1MBTS_2");
      triggers.push_back("HLT_noalg_mb_L1LHCF");
      triggers.push_back("HLT_noalg_L1MU4");
      triggers.push_back("HLT_noalg_L1MU6");
      triggers.push_back("HLT_noalg_L1MU10");
      triggers.push_back("HLT_noalg_L1MU12");
      triggers.push_back("HLT_noalg_L1MU14");
      triggers.push_back("HLT_noalg_mb_L1ZDC_A");
      triggers.push_back("HLT_noalg_mb_L1ZDC_C");
      triggers.push_back("HLT_noalg_mb_L1RD0_FILLED");
      triggers.push_back("HLT_noalg_mb_L1RD0_EMPTY");
      triggers.push_back("HLT_mb_sptrk_L1RD0_EMPTY");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1MBTS_2");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1MBTS_2_C");
      triggers.push_back("HLT_hi_upc_FgapC_j10_0eta490_L1MBTS_2_A");
      triggers.push_back("HLT_e5_lhloose_nod0");
      triggers.push_back("HLT_hi_upc_FgapAC_e5_lhloose_nod0");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4_L1MU4_VTE10");
      triggers.push_back("HLT_hi_upc_FgapA_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapC_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapC_j10_0eta490_L1MBTS_2_A");
    }

  if (upc2018_triggers)
    {
      triggers.push_back("HLT_mu4_L1MU4_VZDC_AORC_VTE200");
      triggers.push_back("HLT_mu4_L1MU4_VTE50");
      triggers.push_back("HLT_2mu4_L12MU4_VTE50");
      triggers.push_back("HLT_mu4_mu4noL1_L1MU4_VTE50");
      triggers.push_back("HLT_mu6_L1MU4_VTE50");
      triggers.push_back("HLT_mu8_L1MU6_VTE50");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_vetosp1500_L1VTE20");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_vetosp1500_L1VTE20");
      triggers.push_back("HLT_mb_sp_L1VTE50");
      triggers.push_back("HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L12TAU1_VTE50");
      triggers.push_back("HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L12TAU2_VTE200");
      triggers.push_back("HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose1_L1ZDC_XOR_VTE50");
      triggers.push_back("HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1ZDC_XOR_VTE50");
      triggers.push_back("HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose1_L1MU4_VTE50");
      triggers.push_back("HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1MU4_VTE50");
      triggers.push_back("HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1ZDC_A_C_VTE50");
      triggers.push_back("HLT_hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1VZDC_A_C_VTE50");
      triggers.push_back("HLT_mu4_hi_upc_FgapAC3_L1MU4_VTE50");
      
    }
  if (upc2016A_triggers)
    {
      triggers.push_back("HLT_e5_etcut_L1EM3_VZDC_A");
      triggers.push_back("HLT_e5_lhloose_nod0");
      triggers.push_back("HLT_e5_lhloose_L1EM3_VZDC_A");
      triggers.push_back("HLT_e5_lhloose_nod0_L1EM3_VZDC_A");
      triggers.push_back("HLT_2e5_etcut_L12EM3_VZDC_A");
      triggers.push_back("HLT_2e5_lhloose_L12EM3_VZDC_A");
      triggers.push_back("HLT_2e5_lhloose_nod0_L12EM3_VZDC_A");
      triggers.push_back("HLT_j10_0eta490_L1MBTS_2_C_VZDC_A");
      triggers.push_back("HLT_j10_0eta490_L1TE5_VZDC_A");
      triggers.push_back("HLT_j20_0eta490_L1TE10_VZDC_A");
      triggers.push_back("HLT_j20_L1J5_VZDC_A");
      triggers.push_back("HLT_j20_0eta490_L1TE20_VZDC_A");
      triggers.push_back("HLT_j30_L1J5_VZDC_A");
      triggers.push_back("HLT_j30_0eta490_L1TE20_VZDC_A");
      triggers.push_back("HLT_j40_L1J5_VZDC_A");
      triggers.push_back("HLT_j40_L1J10_VZDC_A");
      triggers.push_back("HLT_j60_L1J15_VZDC_A");
      triggers.push_back("HLT_j60_L1J20_VZDC_A");
      triggers.push_back("HLT_mu4_L1MU4_VTE10");
      triggers.push_back("HLT_mu4_L1MU4_VZDC_A");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10");
      triggers.push_back("HLT_2mu4_L12MU4_VZDC_A");
      triggers.push_back("HLT_mu4_L1MU4_VTE10_VZDC_A");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10_VZDC_A");
      triggers.push_back("HLT_mu6_L1MU6_VTE10_VZDC_A");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1VTE10_VZDC_A");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1RD0_FILLED");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1ZDC");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1MU4_VTE10");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_L1VTE10_VZDC_A");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_L1RD0_FILLED");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_L1ZDC");
      triggers.push_back("HLT_hi_upc_FgapA_L1RD0");
      triggers.push_back("HLT_hi_upc_FgapC_L1RD0");
      triggers.push_back("HLT_hi_upc_FgapAC_L1RD0");
      triggers.push_back("HLT_hi_upc_FgapA_L1MBTS_1");
      triggers.push_back("HLT_hi_upc_FgapC_L1MBTS_1");
      triggers.push_back("HLT_hi_upc_FgapAC_L1MBTS_1");
      triggers.push_back("HLT_noalg_L1MBTS_2_C_VZDC_A");
      triggers.push_back("HLT_noalg_L1ZDC_C_VZDC_A");
      triggers.push_back("HLT_noalg_L1EM3_VZDC_A");
      triggers.push_back("HLT_noalg_L12EM3_VZDC_A");
      triggers.push_back("HLT_noalg_L1EM5_VZDC_A");
      triggers.push_back("HLT_noalg_L1J5_VZDC_A");
      triggers.push_back("HLT_noalg_L1J10_VZDC_A");
      triggers.push_back("HLT_noalg_L1TE5_VZDC_A");
      triggers.push_back("HLT_hi_upc_FgapA_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapC_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapA_mb_sptrk_L1MBTS_2_C");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1MBTS_2");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight_L1ZDC");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1MBTS_2_C");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1TE5");
      triggers.push_back("HLT_hi_upc_FgapA_j10_L1J5");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1TE10");
      triggers.push_back("HLT_hi_upc_FgapA_j20_0eta490_L1TE10");
      triggers.push_back("HLT_hi_upc_FgapA_j30_0eta490_L1TE20");
      triggers.push_back("HLT_hi_upc_FgapA_j30_L1J5");
      triggers.push_back("HLT_hi_upc_FgapA_j40_L1J5");
      triggers.push_back("HLT_hi_upc_FgapA_j40_L1J10");
      triggers.push_back("HLT_hi_upc_FgapA_j60_L1J15");
      triggers.push_back("HLT_hi_upc_FgapA_j60_L1J20");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4");
      triggers.push_back("HLT_hi_upc_FgapAC_mu6");
      triggers.push_back("HLT_hi_upc_FgapAC_2mu4");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4_mu4noL1");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4_L1MU4_VTE10");
      triggers.push_back("HLT_hi_upc_FgapAC_e5_etcut");
      triggers.push_back("HLT_hi_upc_FgapAC_e5_lhloose_nod0");
      triggers.push_back("HLT_hi_upc_FgapAC_2e5_etcut");
      triggers.push_back("HLT_hi_upc_FgapAC_2e5_lhloose_nod0");
      triggers.push_back("HLT_hi_upc_FgapA_g5_loose");
      triggers.push_back("HLT_hi_upc_FgapA_g10_loose");
      triggers.push_back("HLT_hi_upc_FgapA_g5_etcut");
      triggers.push_back("HLT_hi_upc_FgapA_g10_etcut");
      triggers.push_back("HLT_g5_loose_L1VZDC_A");
      triggers.push_back("HLT_g10_loose_L1VZDC_A");
      triggers.push_back("HLT_g5_etcut_L1VZDC_A");
      triggers.push_back("HLT_g10_etcut_L1VZDC_A");
    }

  if (express2016B_triggers)
    {
      triggers.push_back("HLT_e7_etcut");
      triggers.push_back("HLT_e12_etcut");
      triggers.push_back("HLT_e15_etcut");
      triggers.push_back("HLT_e15_lhloose");
      triggers.push_back("HLT_e15_lhloose_nod0");
      triggers.push_back("HLT_e17_lhloose");
      triggers.push_back("HLT_e17_lhloose_nod0");
      triggers.push_back("HLT_e22_lhloose_nod0");
      triggers.push_back("HLT_g10_loose_L1VZDC_A");
      triggers.push_back("HLT_g10_etcut_L1VZDC_A");
      triggers.push_back("HLT_g10_loose");
      triggers.push_back("HLT_g15_loose");
      triggers.push_back("HLT_g13_etcut");
      triggers.push_back("HLT_g20_loose");
      triggers.push_back("HLT_g18_etcut");
      triggers.push_back("HLT_g35_loose");
      triggers.push_back("HLT_g60_loose");
      triggers.push_back("HLT_j10_0eta490_L1MBTS_2_C_VZDC_A");
      triggers.push_back("HLT_j75_ion_L1J20");
      triggers.push_back("HLT_j150_ion_L1J30");
      triggers.push_back("HLT_j175_ion_L1J50");
      triggers.push_back("HLT_j200_ion_L1J50");
      triggers.push_back("HLT_j250_ion_L1J50");
      triggers.push_back("HLT_j45_ion_n320eta490");
      triggers.push_back("HLT_j40_boffperf_split");
      triggers.push_back("HLT_mu4");
      triggers.push_back("HLT_mu4_L1MU4_VTE10");
      triggers.push_back("HLT_mu4_L1MU4_VZDC_A");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10");
      triggers.push_back("HLT_2mu4_L12MU4_VZDC_A");
      triggers.push_back("HLT_mu4_L1MU4_VTE10_VZDC_A");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10_VZDC_A");
      triggers.push_back("HLT_mu6");
      triggers.push_back("HLT_mu15_L1MU10");
      triggers.push_back("HLT_mu15_msonly");
      triggers.push_back("HLT_2mu4");
      triggers.push_back("HLT_2mu4_bJpsimumu");
      triggers.push_back("HLT_2mu4_bBmumu");
      triggers.push_back("HLT_mb_mbts_L1MBTS_1");
      triggers.push_back("HLT_mb_mbts_L1MBTS_2");
      triggers.push_back("HLT_mb_sptrk");
      triggers.push_back("HLT_mb_sptrk_L1MBTS_1");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose1_L1MU4_VTE10");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose2_L1MU4_VTE10");
      triggers.push_back("HLT_noalg_mb_L1ZDC_A");
      triggers.push_back("HLT_noalg_mb_L1ZDC_C");
      triggers.push_back("HLT_noalg_mb_L1RD0_FILLED");
      triggers.push_back("HLT_noalg_mb_L1RD0_EMPTY");
      triggers.push_back("HLT_mb_sptrk_L1RD0_EMPTY");
      triggers.push_back("HLT_noalg_L1MU4");
      triggers.push_back("HLT_noalg_L1MU6");
      triggers.push_back("HLT_noalg_L1MU10");
      triggers.push_back("HLT_noalg_L1EM10");
      triggers.push_back("HLT_noalg_L1EM12");
      triggers.push_back("HLT_noalg_L1EM14");
      triggers.push_back("HLT_noalg_idmon_L1RD0_EMPTY");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1MBTS_2");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1MBTS_2_C");
      triggers.push_back("HLT_hi_upc_FgapC_j10_0eta490_L1MBTS_2_A");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4_L1MU4_VTE10");
      triggers.push_back("HLT_hi_upc_FgapAC_e5_lhloose_nod0");
      triggers.push_back("HLT_mu4_j20_ion_dr05");
    }

  if (upc2016B_triggers)
    {
      triggers.push_back("HLT_e5_etcut_L1EM3_VZDC_A");
      triggers.push_back("HLT_e5_lhloose_nod0");
      triggers.push_back("HLT_e5_lhloose_L1EM3_VZDC_A");
      triggers.push_back("HLT_e5_lhloose_nod0_L1EM3_VZDC_A");
      triggers.push_back("HLT_2e5_etcut_L12EM3_VZDC_A");
      triggers.push_back("HLT_2e5_lhloose_L12EM3_VZDC_A");
      triggers.push_back("HLT_2e5_lhloose_nod0_L12EM3_VZDC_A");
      triggers.push_back("HLT_j10_0eta490_L1MBTS_2_C_VZDC_A");
      triggers.push_back("HLT_j10_0eta490_L1TE5_VZDC_A");
      triggers.push_back("HLT_j20_0eta490_L1TE10_VZDC_A");
      triggers.push_back("HLT_j20_L1J5_VZDC_A");
      triggers.push_back("HLT_j20_0eta490_L1TE20_VZDC_A");
      triggers.push_back("HLT_j30_L1J5_VZDC_A");
      triggers.push_back("HLT_j30_0eta490_L1TE20_VZDC_A");
      triggers.push_back("HLT_j40_L1J5_VZDC_A");
      triggers.push_back("HLT_j40_L1J10_VZDC_A");
      triggers.push_back("HLT_j60_L1J15_VZDC_A");
      triggers.push_back("HLT_j60_L1J20_VZDC_A");
      triggers.push_back("HLT_mu4_L1MU4_VTE10");
      triggers.push_back("HLT_mu4_L1MU4_VZDC_A");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10");
      triggers.push_back("HLT_2mu4_L12MU4_VZDC_A");
      triggers.push_back("HLT_mu4_L1MU4_VTE10_VZDC_A");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10_VZDC_A");
      triggers.push_back("HLT_mu6_L1MU6_VTE10_VZDC_A");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1VTE10_VZDC_A");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1RD0_FILLED");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1ZDC");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1MU4_VTE10");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_L1VTE10_VZDC_A");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_L1RD0_FILLED");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_L1ZDC");
      triggers.push_back("HLT_hi_upc_FgapA_L1RD0");
      triggers.push_back("HLT_hi_upc_FgapC_L1RD0");
      triggers.push_back("HLT_hi_upc_FgapAC_L1RD0");
      triggers.push_back("HLT_hi_upc_FgapA_L1MBTS_1");
      triggers.push_back("HLT_hi_upc_FgapC_L1MBTS_1");
      triggers.push_back("HLT_hi_upc_FgapAC_L1MBTS_1");
      triggers.push_back("HLT_noalg_L1MBTS_2_C_VZDC_A");
      triggers.push_back("HLT_noalg_L1ZDC_C_VZDC_A");
      triggers.push_back("HLT_noalg_L1EM3_VZDC_A");
      triggers.push_back("HLT_noalg_L12EM3_VZDC_A");
      triggers.push_back("HLT_noalg_L1EM5_VZDC_A");
      triggers.push_back("HLT_noalg_L1J5_VZDC_A");
      triggers.push_back("HLT_noalg_L1J10_VZDC_A");
      triggers.push_back("HLT_noalg_L1TE5_VZDC_A");
      triggers.push_back("HLT_hi_upc_FgapA_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapC_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapA_mb_sptrk_L1MBTS_2_C");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1MBTS_2");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight_L1ZDC");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1MBTS_2_C");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1TE5");
      triggers.push_back("HLT_hi_upc_FgapA_j10_L1J5");
      triggers.push_back("HLT_hi_upc_FgapA_j10_0eta490_L1TE10");
      triggers.push_back("HLT_hi_upc_FgapA_j20_0eta490_L1TE10");
      triggers.push_back("HLT_hi_upc_FgapA_j30_0eta490_L1TE20");
      triggers.push_back("HLT_hi_upc_FgapA_j30_L1J5");
      triggers.push_back("HLT_hi_upc_FgapA_j40_L1J5");
      triggers.push_back("HLT_hi_upc_FgapA_j40_L1J10");
      triggers.push_back("HLT_hi_upc_FgapA_j60_L1J15");
      triggers.push_back("HLT_hi_upc_FgapA_j60_L1J20");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4");
      triggers.push_back("HLT_hi_upc_FgapAC_mu6");
      triggers.push_back("HLT_hi_upc_FgapAC_2mu4");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4_mu4noL1");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4_L1MU4_VTE10");
      triggers.push_back("HLT_hi_upc_FgapAC_e5_etcut");
      triggers.push_back("HLT_hi_upc_FgapAC_e5_lhloose_nod0");
      triggers.push_back("HLT_hi_upc_FgapAC_2e5_etcut");
      triggers.push_back("HLT_hi_upc_FgapAC_2e5_lhloose_nod0");
      triggers.push_back("HLT_hi_upc_FgapA_g5_loose");
      triggers.push_back("HLT_hi_upc_FgapA_g10_loose");
      triggers.push_back("HLT_hi_upc_FgapA_g5_etcut");
      triggers.push_back("HLT_hi_upc_FgapA_g10_etcut");
      triggers.push_back("HLT_g5_loose_L1VZDC_A");
      triggers.push_back("HLT_g10_loose_L1VZDC_A");
      triggers.push_back("HLT_g5_etcut_L1VZDC_A");
      triggers.push_back("HLT_g10_etcut_L1VZDC_A");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose1_L1RD0_FILLED");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose1_L1MU4_VTE10");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose2_L1MU4_VTE10");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose2_L1VTE10_VZDC_A");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose2_L1RD0_FILLED");
    }

  if (upc2016C_triggers)
    {
      triggers.push_back("HLT_e5_etcut_L1EM3_VZDC_C");
      triggers.push_back("HLT_e5_lhloose_nod0");
      triggers.push_back("HLT_e5_lhloose_L1EM3_VZDC_C");
      triggers.push_back("HLT_e5_lhloose_nod0_L1EM3_VZDC_C");
      triggers.push_back("HLT_2e5_etcut_L12EM3_VZDC_C");
      triggers.push_back("HLT_2e5_lhloose_L12EM3_VZDC_C");
      triggers.push_back("HLT_2e5_lhloose_nod0_L12EM3_VZDC_C");
      triggers.push_back("HLT_j10_0eta490_L1MBTS_2_A_VZDC_C");
      triggers.push_back("HLT_j10_0eta490_L1TE5_VZDC_C");
      triggers.push_back("HLT_j20_0eta490_L1TE10_VZDC_C");
      triggers.push_back("HLT_j20_L1J5_VZDC_C");
      triggers.push_back("HLT_j20_0eta490_L1TE20_VZDC_C");
      triggers.push_back("HLT_j30_L1J5_VZDC_C");
      triggers.push_back("HLT_j30_0eta490_L1TE20_VZDC_C");
      triggers.push_back("HLT_j40_L1J5_VZDC_C");
      triggers.push_back("HLT_j40_L1J10_VZDC_C");
      triggers.push_back("HLT_j60_L1J15_VZDC_C");
      triggers.push_back("HLT_j60_L1J20_VZDC_C");
      triggers.push_back("HLT_mu4_L1MU4_VTE10");
      triggers.push_back("HLT_mu4_L1MU4_VZDC_C");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10");
      triggers.push_back("HLT_2mu4_L12MU4_VZDC_C");
      triggers.push_back("HLT_mu4_L1MU4_VTE10_VZDC_C");
      triggers.push_back("HLT_2mu4_L12MU4_VTE10_VZDC_C");
      triggers.push_back("HLT_mu6_L1MU6_VTE10_VZDC_C");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1VTE10_VZDC_C");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1RD0_FILLED");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1ZDC");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose_L1MU4_VTE10");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_L1VTE10_VZDC_C");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_L1RD0_FILLED");
      triggers.push_back("HLT_mb_sptrk_exclusivetight_L1ZDC");
      triggers.push_back("HLT_hi_upc_FgapA_L1RD0");
      triggers.push_back("HLT_hi_upc_FgapC_L1RD0");
      triggers.push_back("HLT_hi_upc_FgapAC_L1RD0");
      triggers.push_back("HLT_hi_upc_FgapA_L1MBTS_1");
      triggers.push_back("HLT_hi_upc_FgapC_L1MBTS_1");
      triggers.push_back("HLT_hi_upc_FgapAC_L1MBTS_1");
      triggers.push_back("HLT_noalg_L1MBTS_2_A_VZDC_C");
      triggers.push_back("HLT_noalg_L1ZDC_A_VZDC_C");
      triggers.push_back("HLT_noalg_L1EM3_VZDC_C");
      triggers.push_back("HLT_noalg_L12EM3_VZDC_C");
      triggers.push_back("HLT_noalg_L1EM5_VZDC_C");
      triggers.push_back("HLT_noalg_L1J5_VZDC_C");
      triggers.push_back("HLT_noalg_L1J10_VZDC_C");
      triggers.push_back("HLT_noalg_L1TE5_VZDC_C");
      triggers.push_back("HLT_hi_upc_FgapA_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapC_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapC_mb_sptrk_L1MBTS_2_A");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1MBTS_2");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight_L1ZDC");
      triggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight");
      triggers.push_back("HLT_hi_upc_FgapC_j10_0eta490_L1RD0_FILLED");
      triggers.push_back("HLT_hi_upc_FgapC_j10_0eta490_L1MBTS_2_A");
      triggers.push_back("HLT_hi_upc_FgapC_j10_0eta490_L1TE5");
      triggers.push_back("HLT_hi_upc_FgapC_j10_L1J5");
      triggers.push_back("HLT_hi_upc_FgapC_j10_0eta490_L1TE10");
      triggers.push_back("HLT_hi_upc_FgapC_j20_0eta490_L1TE10");
      triggers.push_back("HLT_hi_upc_FgapC_j30_0eta490_L1TE20");
      triggers.push_back("HLT_hi_upc_FgapC_j30_L1J5");
      triggers.push_back("HLT_hi_upc_FgapC_j40_L1J5");
      triggers.push_back("HLT_hi_upc_FgapC_j40_L1J10");
      triggers.push_back("HLT_hi_upc_FgapC_j60_L1J15");
      triggers.push_back("HLT_hi_upc_FgapC_j60_L1J20");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4");
      triggers.push_back("HLT_hi_upc_FgapAC_mu6");
      triggers.push_back("HLT_hi_upc_FgapAC_2mu4");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4_mu4noL1");
      triggers.push_back("HLT_hi_upc_FgapAC_mu4_L1MU4_VTE10");
      triggers.push_back("HLT_hi_upc_FgapAC_e5_etcut");
      triggers.push_back("HLT_hi_upc_FgapAC_e5_lhloose_nod0");
      triggers.push_back("HLT_hi_upc_FgapAC_2e5_etcut");
      triggers.push_back("HLT_hi_upc_FgapAC_2e5_lhloose_nod0");
      triggers.push_back("HLT_hi_upc_FgapC_g5_loose");
      triggers.push_back("HLT_hi_upc_FgapC_g10_loose");
      triggers.push_back("HLT_hi_upc_FgapC_g5_etcut");
      triggers.push_back("HLT_hi_upc_FgapC_g10_etcut");
      triggers.push_back("HLT_g5_loose_L1VZDC_C");
      triggers.push_back("HLT_g10_loose_L1VZDC_C");
      triggers.push_back("HLT_g5_etcut_L1VZDC_C");
      triggers.push_back("HLT_g10_etcut_L1VZDC_C");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose1_L1RD0_FILLED");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose1_L1MU4_VTE10");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose2_L1MU4_VTE10");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose2_L1VTE10_VZDC_C");
      triggers.push_back("HLT_mb_sptrk_exclusiveloose2_L1RD0_FILLED");
    }

  if (main2016B_triggers)
    {
      triggers.push_back("HLT_2e7_lhloose");
      triggers.push_back("HLT_2e7_lhloose_nod0");
      triggers.push_back("HLT_2e10_lhloose");
      triggers.push_back("HLT_2e10_lhloose_nod0");
      triggers.push_back("HLT_2g10_loose_L12EM5");
      triggers.push_back("HLT_2g15_loose");
      triggers.push_back("HLT_g35_loose");
      triggers.push_back("HLT_g60_loose");
      triggers.push_back("HLT_g33_etcut");
      triggers.push_back("HLT_j30_ion_0eta490_L1TE10");
      triggers.push_back("HLT_g58_etcut");
      triggers.push_back("HLT_j25_ion_n320eta490_L1TE5");
      triggers.push_back("HLT_j25_ion_n320eta490_L1TE10");
      triggers.push_back("HLT_j35_ion_n320eta490_L1TE10");
      triggers.push_back("HLT_j45_ion_n320eta490");
      triggers.push_back("HLT_j55_ion_n320eta490");
      triggers.push_back("HLT_j65_ion_n320eta490");
      triggers.push_back("HLT_j75_ion_n320eta490");
      triggers.push_back("HLT_2j10_ion_n320eta490_L1TE10");
      triggers.push_back("HLT_2j30_n320eta490");
      triggers.push_back("HLT_mu20_L1MU15");
      triggers.push_back("HLT_mu4");
      triggers.push_back("HLT_2mu4");
      triggers.push_back("HLT_2mu6");
      triggers.push_back("HLT_2mu4_nomucomb");
      triggers.push_back("HLT_2mu6_nomucomb");
      triggers.push_back("HLT_mb_sptrk");
      triggers.push_back("HLT_e17_lhloose");
      triggers.push_back("HLT_e17_lhloose_nod0");
      triggers.push_back("HLT_mu15_L1MU10");
      triggers.push_back("HLT_j40_bmv2c2070_split_L1J10");
      triggers.push_back("HLT_j40_bmv2c2085_split_L1J10");
      triggers.push_back("HLT_j40_boffperf_split");

      //re-run
      rerunTriggers.push_back("HLT_hi_upc_FgapA_mb_sptrk_L1MBTS_2_C");
      rerunTriggers.push_back("HLT_hi_upc_FgapA_mb_sptrk_L1RD0_FILLED");
      rerunTriggers.push_back("HLT_hi_upc_FgapC_mb_sptrk_L1RD0_FILLED");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_L1RD0_FILLED");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusiveloose");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_mb_sptrk_exclusivetight");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_mu4");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_mu6");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_2mu4");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_mu4_mu4noL1");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_mu4_L1MU4_VTE10");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_e5_etcut");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_e5_lhloose_nod0");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_2e5_etcut");
      rerunTriggers.push_back("HLT_hi_upc_FgapAC_2e5_lhloose_nod0");
 
    }

  int ic = 0;
  //char name[50];
  ANA_MSG_INFO("Adding trigger branches!");
  for(auto &trig : triggers)
    {
      auto cg = m_trigDecisionTool->getChainGroup(trig);
      m_chainGroups.push_back(cg);
      //t_decisions.push_back(0);
      //t_prescales.push_back(1.);
      if (cg->getListOfTriggers().size())
	{
	  ANA_MSG_INFO("setupTriggerHistos(): Trigger found = " << trig.c_str() << " bit " << ic);
	}
      else
	{
	  ANA_MSG_INFO("setupTriggerHistos(): Trigger NOT found = " << trig.c_str()  << " bit " << ic);
	}
      // force all triggers to show up in tree
      TString bname(trig.c_str());
      m_outputTree->Branch(bname,&(t_decisions[ic]),bname+"/O");
      m_outputTree->Branch("ps_"+bname,&(t_prescales[ic]),"ps_"+bname+"/F");
      ic++;
    }

  int irc = 0;
  ANA_MSG_INFO("Adding rerun trigger branches!");
  for(auto &trig : rerunTriggers)
    {
      auto cg = m_trigDecisionTool->getChainGroup(trig);
      m_rerunChainGroups.push_back(cg);
      //t_decisions.push_back(0);
      //t_prescales.push_back(1.);
      if (cg->getListOfTriggers().size())
	{
	  ANA_MSG_INFO("setupTriggerHistos(): Rerun trigger found = " << trig.c_str() << " bit " << irc);
	}
      else
	{
	  ANA_MSG_INFO("setupTriggerHistos(): Rerun trigger NOT found = " << trig.c_str()  << " bit " << irc);
	}
      // force all rerun triggers to show up in tree
      TString bname(trig.c_str());
      m_outputTree->Branch(bname,&(t_rerunDecisions[irc]),bname+"/O");
      irc++;
    }

  // trigger matching flags for electrons and muons

    
  ANA_MSG_INFO("Adding trig matching flags");
  t_muon_HLT_match.resize(m_muon_triggers.size());
  t_muon_HLT_dR.resize(m_muon_triggers.size());
  t_dimuon_HLT_match.resize(m_dimuon_triggers.size());
  
  if (enableMuons)
    {
      for (size_t imutr=0;imutr<m_muon_triggers.size();imutr++)
	{
	  TString bname = "muon_" + TString(m_muon_triggers.at(imutr));      
	  m_outputTree->Branch(bname,&(t_muon_HLT_match.at(imutr)),bname+"/O");
	  TString bnamedR = "muon_" + TString(m_muon_triggers.at(imutr)) + "_dR";      
	  m_outputTree->Branch(bnamedR,&(t_muon_HLT_match.at(imutr)),bnamedR+"/F");
	}
      
      for (size_t imutr=0;imutr<m_dimuon_triggers.size();imutr++)
	{
	  TString bname = "dimuon_" + TString(m_dimuon_triggers.at(imutr));      
	  m_outputTree->Branch(bname,&(t_dimuon_HLT_match.at(imutr)),bname+"/O");
	}
    }    
  
  t_el_HLT_match.resize(m_el_triggers.size());
  t_el_HLT_dR.resize(m_el_triggers.size());
  if (enableElectrons)
    {
      for (size_t ieltr=0;ieltr<m_el_triggers.size();ieltr++)
	{
	  TString bname = "el_" + TString(m_el_triggers.at(ieltr));      
	  m_outputTree->Branch(bname,&(t_el_HLT_match.at(ieltr)),bname+"/O");
	  TString bnamedR = "el_" + TString(m_el_triggers.at(ieltr)) + "_dR";      
	  m_outputTree->Branch(bnamedR,&(t_el_HLT_match.at(ieltr)),bnamedR+"/F");
	}
    }
  
  t_ph_HLT_match.resize(m_ph_triggers.size());
  t_ph_HLT_dR.resize(m_ph_triggers.size());
  if (enablePhotons)
    {
      for (size_t iphtr=0;iphtr<m_ph_triggers.size();iphtr++)
	{
	  TString bname = "ph_" + TString(m_ph_triggers.at(iphtr));      
	  m_outputTree->Branch(bname,&(t_ph_HLT_match.at(iphtr)),bname+"/O");
	  TString bnamedR = "ph_" + TString(m_ph_triggers.at(iphtr)) + "_dR";      
	  m_outputTree->Branch(bnamedR,&(t_ph_HLT_match.at(iphtr)),bnamedR+"/F");
	}
    }
  
  int ntrig = m_chainGroups.size();
  
  h_zdcTriggers = new TH1D("h_zdcTriggers","ZDC Triggers",ntrig+1,0,ntrig+1);
  h_zdcTriggersTBP = new TH1D("h_zdcTriggersTBP","ZDC Triggers",ntrig+1,0,ntrig+1);
  
  h_zdcTriggers->GetXaxis()->SetBinLabel(1,"all");
  h_zdcTriggersTBP->GetXaxis()->SetBinLabel(1,"all");

  for (int ibin=0;ibin<ntrig;ibin++)
    {
      if (m_chainGroups.at(ibin)->getListOfTriggers().size())
	{
	  h_zdcTriggers->GetXaxis()->SetBinLabel(ibin+2,m_chainGroups.at(ibin)->getListOfTriggers().at(0).c_str());
	  h_zdcTriggersTBP->GetXaxis()->SetBinLabel(ibin+2,m_chainGroups.at(ibin)->getListOfTriggers().at(0).c_str());
	}
    }
  
  //wk()->addOutput(h_zdcTriggers);
  //wk()->addOutput(h_zdcTriggersTBP);

  m_setupTrigHist = true;

  ANA_MSG_DEBUG("setupTriggerHistos(): Finished setting up trigger");

}

// from TrigMuonMatching
double ZdcNtuple::dR(const double eta1,const double phi1,const double eta2,const double phi2)
{
  double deta = fabs(eta1 - eta2);
  double dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath::Pi() - fabs(phi1 - phi2);
  return sqrt(deta*deta + dphi*dphi);
}

/*
StatusCode ZdcNtuple :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return StatusCode::SUCCESS;
}
*/


StatusCode ZdcNtuple :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

  /*
  if (enableTracks)
    {
      for (auto tool : m_trackSelectionTools)
	{
	  tool->finalize();
	  delete tool;
	}      
    }
  */

  /*
  if (m_muonSelectionTool)
    {
      delete m_muonSelectionTool;
      m_muonSelectionTool = 0;
    }

  if (enableTrigger)
    {
      if( m_trigConfigTool ) {
	delete m_trigConfigTool;
	m_trigConfigTool = 0;
      }
      
      if( m_trigDecisionTool ) {
	delete m_trigDecisionTool;
	m_trigDecisionTool = 0;
      }
    }

  if( m_jetCleaning ) {
    delete m_jetCleaning;
    m_jetCleaning = 0;
  }

  if (m_grl) {
    delete m_grl;
    m_grl = 0;
  }
  */

  return StatusCode::SUCCESS;
}


/*
StatusCode ZdcNtuple :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  return StatusCode::SUCCESS;
}
*/
