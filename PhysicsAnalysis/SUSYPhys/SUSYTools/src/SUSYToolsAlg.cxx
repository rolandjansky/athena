/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// SUSYToolsAlg.cxx
// Base class
#include "SUSYToolsAlg.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"

// For shallow copy containers
#include "xAODCore/ShallowCopy.h"

// For the forcing of the tau truth container build
#include "TauAnalysisTools/ITauTruthMatchingTool.h"

// GRL
#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"

// For handling systematics
#include "AsgMessaging/StatusCode.h"
#include "PATInterfaces/SystematicSet.h"

// For finding calibration files
#include "PathResolver/PathResolver.h"

// For CBK access
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#include "EventInfo/EventStreamInfo.h"
#endif

// For output of histograms
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TEfficiency.h"
#include <AnaAlgorithm/IHistogramWorker.h>

// For configuration
#include "TString.h"
#include "TEnv.h"
#include "THashList.h"

// For string manipulation
#include <regex>
#include <cmath>

//====================================================================================================
namespace Cut {
  const unsigned int NSel=5;
  enum { all=0, baseline, passOR, signal, trigmatch };
  static const TString SCut[] = {"All","Baseline","passOR","Signal","TrigM"};
}

//====================================================================================================
std::string findInReg(SG::AuxTypeRegistry& reg, xAOD::IParticle* obj, std::string searchkey);
int getSize(std::map<std::string,std::vector<std::string>> &collection, std::string object);

//====================================================================================================
// Constructor
SUSYToolsAlg::SUSYToolsAlg(const std::string& name,
                           ISvcLocator* pSvcLocator )
  : EL::AnaAlgorithm (name, pSvcLocator)
  , m_SUSYTools("")
  , m_tauTruthMatchingTool("")
  , m_Nevts(0)
  , m_kernel("")
  , m_configFile("SUSYTools/SUSYTools_Default.conf")
{

  declareProperty( "DataSource",  m_dataSource = 1 ); //default is fullsim = 1
  declareProperty( "DoSyst",      m_doSyst = false );

  declareProperty( "STConfigFile", m_configFile );
  //
  declareProperty( "PRWConfigs", m_PRWConfigs );
  declareProperty( "PRWLumiCalc", m_PRWLumiCalcFiles );
  declareProperty( "UsePRWAutoconfig", m_usePRWAutoconfig = false );
  declareProperty( "mcCampaign", m_mcCampaign = "" );
  declareProperty( "mcChannel", m_mcChannel = -99 );
  declareProperty( "GRLFiles", m_GRLFiles );
  declareProperty( "maxEvts", m_maxEvts = -999 );
  declareProperty( "LumiBlockFilter", m_lbfilter = 90 );

  // asg Tool Handles must be dealt with differently
  m_tauTruthMatchingTool.declarePropertyFor( this, "TauTruthMatchingTool", "The TTMT" );
  m_SUSYTools.declarePropertyFor( this, "SUSYTools", "The SUSYTools instance" );
}

//====================================================================================================
// Destructor
SUSYToolsAlg::~SUSYToolsAlg() { }

//====================================================================================================
// Initialize
StatusCode SUSYToolsAlg::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");

  //--- ST config and retrieval
  ATH_CHECK( m_SUSYTools.setProperty("DataSource",m_dataSource) );
  ATH_CHECK(m_SUSYTools.setProperty("PRWLumiCalcFiles", m_PRWLumiCalcFiles) );
  ATH_CHECK(m_SUSYTools.setProperty("OutputLevel", this->msg().level()) );
  ATH_CHECK(m_SUSYTools.setProperty("DebugMode", ((int)(this->msg().level())<3?true:false )));
  if (m_usePRWAutoconfig){
    ATH_CHECK(m_SUSYTools.setProperty("AutoconfigurePRWTool", true) );
    ATH_CHECK(m_SUSYTools.setProperty("mcCampaign", m_mcCampaign) );
    ATH_CHECK(m_SUSYTools.setProperty("mcChannel", m_mcChannel) );
  } else {
    ATH_CHECK(m_SUSYTools.setProperty("PRWConfigFiles", m_PRWConfigs) );
  }
  //
  ATH_CHECK(m_SUSYTools.setProperty("ConfigFile", m_configFile) );
  m_SUSYTools.setTypeAndName("ST::SUSYObjDef_xAOD/SUSYTools");
  ATH_CHECK(m_SUSYTools.retrieve());
  ATH_MSG_INFO("Retrieved tool: " << m_SUSYTools->name() );

  // Need truth matching for tau CP tools
  if( !m_SUSYTools->isData() ){
    m_tauTruthMatchingTool.setTypeAndName("TauAnalysisTools::TauTruthMatchingTool/TauTruthMatchingTool");
    ATH_CHECK( m_tauTruthMatchingTool.setProperty("WriteTruthTaus", true) );
    ATH_CHECK( m_tauTruthMatchingTool.setProperty("TruthJetContainerName", "AntiKt4TruthDressedWZJets") );
    ATH_CHECK( m_tauTruthMatchingTool.retrieve() );
    ATH_MSG_INFO("Retrieved tool: " << m_tauTruthMatchingTool->name() );
  }

  // GRL tool
  if (m_SUSYTools->isData()) {
    m_grl.setTypeAndName("GoodRunsListSelectionTool/grl");
    m_grl.isUserConfigured();

    std::vector<std::string> myGRLs;
    for ( auto x : m_GRLFiles ) myGRLs.push_back(x);

    ANA_CHECK( m_grl.setProperty("GoodRunsListVec", myGRLs) );
    ANA_CHECK( m_grl.setProperty("PassThrough", (myGRLs.size()>0?false:true)) );
    ANA_CHECK( m_grl.retrieve() );

    ATH_MSG_INFO("GRL tool retrieved & initialized: " << m_grl->name() );
  }

  // Systematics
  sysInfoList.clear();
  // this is the nominal set
  if (m_doSyst) {
    sysInfoList = m_SUSYTools->getSystInfoList();
  } else {
    ST::SystInfo infodef;
    infodef.affectsKinematics = false;
    infodef.affectsWeights = false;
    infodef.affectsType = ST::Unknown;
    sysInfoList.push_back(infodef);
  }
  // order in object groups
  groupSysts();

  // setup triggers to be checked
  m_triggers.clear();
  m_triggers["el"] = {};
  m_triggers["el"].push_back("HLT_e24_lhtight_nod0_ivarloose");
  m_triggers["el"].push_back("HLT_e60_lhmedium_nod0");
  m_triggers["el"].push_back("HLT_e26_lhtight_nod0_ivarloose");
  m_triggers["el"].push_back("HLT_e140_lhloose_nod0");

  m_triggers["ph"] = {};
  m_triggers["ph"].push_back("HLT_g120_loose");
  m_triggers["ph"].push_back("HLT_g140_loose");

  m_triggers["mu"] = {};
  m_triggers["mu"].push_back("HLT_mu24_ivarmedium");
  m_triggers["mu"].push_back("HLT_mu50");
  m_triggers["mu"].push_back("HLT_mu26_ivarmedium");

  // book histograms
  ATH_CHECK( bookHistograms() );

  // retrieve SUSYTools config file
  TEnv rEnv;
  int success = -1;
  m_configFile = (PathResolverFindCalibFile(m_configFile)).c_str();
  success = rEnv.ReadFile(m_configFile.c_str(), kEnvAll);
  if(success != 0){
    ATH_MSG_ERROR( "Cannot open config file!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Config file opened" );

  m_configDict.clear();
  m_configDict["Jet.LargeRcollection"] = rEnv.GetValue("Jet.LargeRcollection", "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" );
  m_configDict["TrackJet.Collection"]  = rEnv.GetValue("TrackJet.Collection", "AntiKtVR30Rmax4Rmin02TrackJets" );
  m_configDict["Jet.WtaggerConfig"]    = rEnv.GetValue("Jet.WtaggerConfig", "None");
  m_configDict["Jet.ZtaggerConfig"]    = rEnv.GetValue("Jet.ZtaggerConfig", "None");
  m_configDict["Jet.ToptaggerConfig"]  = rEnv.GetValue("Jet.ToptaggerConfig", "None");
  // Trim comments and extra spaces from config entries
  std::regex comment("#.*$");
  std::regex trimspaces("^ +| +$|( ) +");
  for (auto keyval : m_configDict) {
     m_configDict[keyval.first] = regex_replace(m_configDict[keyval.first], comment, "");
     m_configDict[keyval.first] = regex_replace(m_configDict[keyval.first], trimspaces, "$1");
     ATH_MSG_DEBUG("config " << keyval.first << " : " << m_configDict[keyval.first]);
  }
  m_FatJetCollection = m_configDict["Jet.LargeRcollection"];
  m_TrkJetCollection = m_configDict["TrackJet.Collection"];
  if (m_configDict["TrackJet.Collection"].find("None")!=std::string::npos) m_TrkJetCollection = "";

  m_slices.clear();
  m_slices["ele"]  = bool(rEnv.GetValue("Slices.Ele", true));
  m_slices["pho"]  = bool(rEnv.GetValue("Slices.Pho", true));
  m_slices["mu"]   = bool(rEnv.GetValue("Slices.Mu", true));
  m_slices["tau"]  = bool(rEnv.GetValue("Slices.Tau", true));
  m_slices["jet"]  = bool(rEnv.GetValue("Slices.Jet", true));
  m_slices["bjet"] = bool(rEnv.GetValue("Slices.BJet", true));
  m_slices["fjet"] = bool(rEnv.GetValue("Slices.FJet", true));
  m_slices["tjet"] = bool(rEnv.GetValue("Slices.TJet", true));
  m_slices["met"]  = bool(rEnv.GetValue("Slices.MET", true));
  for (auto x : m_slices) { ATH_MSG_DEBUG( "Slice " << x.first << ": " << ((x.second)?"true":"false")); }

  return StatusCode::SUCCESS;
}

//====================================================================================================
// Finalize
StatusCode SUSYToolsAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  ATH_MSG_INFO("Time per event:          (CPU / WALL)");
  ATH_MSG_INFO("  Entire loop           - " << m_clock0.CpuTime() * 1e3 / m_Nevts << " / " << m_clock0.RealTime() * 1e3 / m_Nevts << " ms");
  ATH_MSG_INFO("  First event           - " << m_clock1.CpuTime() * 1e3 << " / " << m_clock1.RealTime() * 1e3 << " ms");
  ATH_MSG_INFO("  Excluding first event - " << m_clock2.CpuTime() * 1e3 / (m_Nevts - 1) << " / " << m_clock2.RealTime() * 1e3 / (m_Nevts - 1) << " ms");


  // label cutflow histograms
  for (std::string hname : {"el_n_flow_nominal", "ph_n_flow_nominal", "mu_n_flow_nominal", "jet_n_flow_nominal", "bjet_n_flow_nominal", "fatjet_n_flow_nominal", "trkjet_n_flow_nominal", "tau_n_flow_nominal"} ) {
     TH1* h = hist("Cutflow/"+hname);
     for (unsigned int i=1; i < Cut::NSel+1; i++){ h->GetXaxis()->SetBinLabel(i, Cut::SCut[i-1].Data()); }
     h->GetXaxis()->SetLabelSize(0.05);
  }

  // normalize and label efficiency histograms
  for (std::string hname : {"el_trigmatch_eff_nominal", "ph_trigmatch_eff_nominal", "mu_trigmatch_eff_nominal"}) {
     std::string obj = hname.substr(0,2);
     TH1* h = hist("Trigger/"+hname);
     for (unsigned int i=1; i < m_triggers[obj].size()+1; i++){ h->GetXaxis()->SetBinLabel(i, m_triggers[obj][i-1].substr(4,std::string::npos).c_str()); }
     h->GetXaxis()->SetLabelSize(0.05);
     if (m_obj_count[obj]["sig"]>0) h->Scale(1./static_cast<float>(m_obj_count[obj]["sig"]));
  }

  // label weights histograms
  std::map<std::string,std::string> mapweights = {{"event","EventWeight"},{"electrons","Electron"},{"photons","Photon"},{"muons","Muon"},{"taus","Tau"},{"jets","Jet"},{"btags","BTag"}};
  for (auto weight : mapweights) {
     TH1* h = hist("Syst/weight_"+weight.first);
     std::vector syslist = syst_weights[weight.second];
     for (unsigned int i=1; i < syslist.size()+1; i++) { h->GetXaxis()->SetBinLabel(i, syslist.at(i-1).c_str()); }
     h->GetXaxis()->SetLabelSize(0.05);
     // average weight
     h->Scale(1./static_cast<float>(h->GetBinContent(1)));

     TH2* h2 = hist2d("Syst/syst_"+weight.first);
     syslist = syst_all[weight.second];
     for (unsigned int i=1; i < syslist.size()+1; i++) { h2->GetYaxis()->SetBinLabel(syslist.size()-i+1, syslist.at(i-1).c_str()); h2->Fill(0.5,syslist.size()-i,1.); }
     h2->GetYaxis()->SetLabelSize(0.05);
  }

  return StatusCode::SUCCESS;
}

//====================================================================================================
// Execute
StatusCode SUSYToolsAlg::execute() {
  // manual event limit
  if (m_maxEvts>=0 && m_Nevts>=(unsigned int)m_maxEvts) return StatusCode::SUCCESS;

  ATH_MSG_VERBOSE("Executing " << name() << "...");
  m_clock0.Start(false);
  if (m_Nevts == 0) m_clock1.Start(false);
  else m_clock2.Start(false);

  if (m_SUSYTools->resetSystematics() != StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Cannot reset SUSYTools systematics" );
  }

  // METMaker crashes if there's no PV, check that before anything,
  // and skip to the next event if there isn't one
  if (m_SUSYTools->GetPrimVtx() == nullptr) {
    ATH_MSG_WARNING("No PV found for this event! Skipping...");
    return StatusCode::SUCCESS;
  }

  // call PRW tool to apply all relevant decorations
  ATH_CHECK( m_SUSYTools->ApplyPRWTool());

  // get EventInfo
  const xAOD::EventInfo* evtInfo(0);
  ATH_CHECK( evtStore()->retrieve(evtInfo, "EventInfo") );
  bool isData = ( m_dataSource == 0 );

  // manual max events within lumiblock selection (on top of Nevts)
  if (m_maxEvts>=-1 && (evtInfo->lumiBlock() != (unsigned int)m_lbfilter)) return StatusCode::SUCCESS;

  // get file/kernel info on first event
  if ( m_Nevts==0 ) {
    std::string stream;
#ifdef ROOTCORE
    // Read the CutBookkeeper container on the first event
    const xAOD::CutBookkeeperContainer* completeCBC = 0;
    if (!evtStore()->event()->retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()) {
      ATH_MSG_ERROR("Failed to retrieve CutBookkeepers from MetaData! Exiting.");
    }
    std::string cbkname;
    for ( auto cbk : *completeCBC ) {
      cbkname = cbk->name();
      stream = cbk->inputStream();
      ATH_MSG_INFO("cbkname: " << cbkname << ", stream: " << stream);
      if (cbkname.find("PHYSVAL")!=std::string::npos) m_kernel="PHYSVAL";
      else if (cbkname.find("PHYSLITE")!=std::string::npos) m_kernel="PHYSLITE";
      else if (cbkname.find("PHYS")!=std::string::npos) m_kernel="PHYS";
      else if (stream.find("StreamDAOD")!=std::string::npos) m_kernel=stream.replace(stream.find("Stream"),6,"");
      else m_kernel="other";
    }
#else
    // check the stream info
    const EventStreamInfo* esi = 0;
    CHECK( inputMetaStore()->retrieve(esi) );
    for ( auto stream : esi->getProcessingTags() ) {
      if (stream.find("PHYSVAL")!=std::string::npos) m_kernel="PHYSVAL";
      else if (stream.find("PHYSLITE")!=std::string::npos) m_kernel="PHYSLITE";
      else if (stream.find("PHYS")!=std::string::npos) m_kernel="PHYS";
      else if (stream.find("StreamDAOD")!=std::string::npos) m_kernel=stream.replace(stream.find("Stream"),6,"");
      else m_kernel="other";
    }
#endif
    ATH_MSG_INFO("Input file kernel type: " << m_kernel);
  }

  int period = 100;
  if (m_Nevts==0 || (m_Nevts % period == 99)) ATH_MSG_INFO("===>>>  start processing event #" << evtInfo->eventNumber() << ", run #" << evtInfo->runNumber() << " | " << m_Nevts << " events processed so far <<<===");

  //==================================================
  // General info
  TH1* info_runNo = hist("EventInfo/info_runNo");
  TH1* info_evtNo = hist("EventInfo/info_evtNo");
  TH1* info_lumiBlock = hist("EventInfo/info_lumiBlock");
  info_runNo->Fill(evtInfo->runNumber());
  info_evtNo->Fill(evtInfo->eventNumber());
  info_lumiBlock->Fill(evtInfo->lumiBlock());

  // First, create the nominal containers
  // Only create them if slice active & configured
  // Only create them freshly if affected by systematics

  xAOD::ElectronContainer* electrons_nominal(0);
  xAOD::ShallowAuxContainer* electrons_nominal_aux(0);
  if (m_slices["ele"]) {
    ATH_CHECK( m_SUSYTools->GetElectrons(electrons_nominal, electrons_nominal_aux) );
    ATH_MSG_DEBUG( "Number of electrons: " << electrons_nominal->size() );
  }

  xAOD::PhotonContainer* photons_nominal(0);
  xAOD::ShallowAuxContainer* photons_nominal_aux(0);
  if (m_slices["pho"]) {
    ATH_CHECK( m_SUSYTools->GetPhotons(photons_nominal, photons_nominal_aux) );
    ATH_MSG_DEBUG( "Number of photons: " << photons_nominal->size() );
  }

  xAOD::MuonContainer* muons_nominal(0);
  xAOD::ShallowAuxContainer* muons_nominal_aux(0);
  if (m_slices["mu"]) {
    ATH_CHECK( m_SUSYTools->GetMuons(muons_nominal, muons_nominal_aux) );
    ATH_MSG_DEBUG( "Number of muons: " << muons_nominal->size() );
  }

  xAOD::JetContainer* jets_nominal(0);
  xAOD::ShallowAuxContainer* jets_nominal_aux(0);
  if (m_slices["jet"]) {
    ATH_CHECK( m_SUSYTools->GetJets(jets_nominal, jets_nominal_aux) );
    ATH_MSG_DEBUG( "Number of jets: " << jets_nominal->size() );
  }

  static bool doFatJets = true;                   // only create if slice on && collection configured
  if (m_FatJetCollection.empty()) doFatJets = false;
  const xAOD::JetContainer* fatjets_test(0);
  xAOD::JetContainer* fatjets_nominal(0);
  xAOD::ShallowAuxContainer* fatjets_nominal_aux(0);
  if( m_slices["fjet"] && doFatJets && evtStore()->contains<xAOD::JetContainer>( m_FatJetCollection ) ){
    if( !evtStore()->retrieve( fatjets_test, m_FatJetCollection ).isSuccess() ){
      ATH_MSG_ERROR("Failed to retrieve xAOD::JetContainer with name " << m_FatJetCollection );
      return StatusCode::FAILURE;
    } else {
      ATH_CHECK( m_SUSYTools->GetFatJets(fatjets_nominal,fatjets_nominal_aux,true,m_FatJetCollection,true) );
      ATH_MSG_DEBUG( "Number of Large Radius jets: " << fatjets_nominal->size() );
    }
  } else if( m_slices["jet"] && doFatJets) {
    ATH_MSG_DEBUG("FatJets xAOD::JetContainer with name " << m_FatJetCollection <<" not found");
    doFatJets=false;
  }

  static bool doTrkJets = true;                   // only create if slice on && collection configured
  if (m_TrkJetCollection.empty()) doTrkJets = false;
  const xAOD::JetContainer* trkjets_test(0);
  xAOD::JetContainer* trkjets_nominal(0);
  xAOD::ShallowAuxContainer* trkjets_nominal_aux(0);
  if( m_slices["tjet"] && doTrkJets && evtStore()->contains<xAOD::JetContainer>( m_TrkJetCollection ) ){
    if( !evtStore()->retrieve( trkjets_test, m_TrkJetCollection ).isSuccess() ){
      ATH_MSG_ERROR("Failed to retrieve xAOD::JetContainer with name " << m_TrkJetCollection );
      return StatusCode::FAILURE;
    } else {
      ATH_CHECK( m_SUSYTools->GetTrackJets(trkjets_nominal,trkjets_nominal_aux,true,m_TrkJetCollection) );
      ATH_MSG_DEBUG( "Number of track jets: " << trkjets_nominal->size() );
    }
  } else if(m_slices["jet"] && doTrkJets) {
    ATH_MSG_DEBUG("TrkJets xAOD::JetContainer with name " << m_TrkJetCollection <<" not found");
    doTrkJets=false;
  }

  //// Tau Truth
  //if( m_slices["tau"] && !isData) {
  //  const xAOD::TauJetContainer* p_TauJets = 0;
  //  ATH_CHECK( evtStore()->retrieve(p_TauJets, "TauJets") );
  //  //if( !evtStore()->contains<xAOD::TruthParticleContainer>("TruthTaus") && p_TauJets->size()==0 ){
  //  //  // If there are no taus, then we need to force the building of the container
  //  //  ATH_MSG_DEBUG("FIXME: TruthTaus hack for empty TauJets.");
  //  //  xAOD::TruthParticleContainer* noTT = new xAOD::TruthParticleContainer;
  //  //  xAOD::TruthParticleAuxContainer* noTTAux = new xAOD::TruthParticleAuxContainer;
  //  //  noTT->setStore( noTTAux );
  //  //  ATH_CHECK( evtStore()->record(noTT, "TruthTaus") );
  //  //  ATH_CHECK( evtStore()->record(noTTAux, "TruthTausAux.") );
  //  //  ATH_CHECK( m_tauTruthMatchingTool->retrieveTruthTaus() );
  //  //}
  //  for(const auto& tau : *p_TauJets) {
  //    const xAOD::TruthParticle* trueTau = 0;
  //    trueTau = m_tauTruthMatchingTool->getTruth(*tau);
  //    if( trueTau ){
  //      ATH_MSG_DEBUG("getTruth tau " <<tau->pt() <<" " <<tau->eta()
  //                    <<" " <<tau->phi()
  //                    <<" trueTau " <<trueTau->pt() <<" " <<trueTau->eta()
  //                    <<" " <<trueTau->phi());
  //    } else {
  //      ATH_MSG_DEBUG("getTruth tau " <<tau->pt() <<" " <<tau->eta()
  //                    <<" " <<tau->phi() << "trueTau not found");
  //    }
  //  }
  //}

  const xAOD::TauJetContainer* taus_gettruth(0);
  xAOD::TauJetContainer* taus_nominal(0);
  xAOD::ShallowAuxContainer* taus_nominal_aux(0);
  if (m_slices["tau"]) {
     if (!isData) {
       ATH_CHECK( evtStore()->retrieve(taus_gettruth, "TauJets") );
       for(const auto& tau : *taus_gettruth) {
         m_tauTruthMatchingTool->getTruth(*tau);
       }
     }
     ATH_CHECK( m_SUSYTools->GetTaus(taus_nominal, taus_nominal_aux) );
     ATH_MSG_DEBUG( "Number of taus: " << taus_nominal->size() );
  }

  xAOD::MissingETContainer* metcst_nominal = new xAOD::MissingETContainer;
  xAOD::MissingETAuxContainer* metcst_nominal_aux = new xAOD::MissingETAuxContainer;
    metcst_nominal->setStore(metcst_nominal_aux);
    metcst_nominal->reserve(10);
  xAOD::MissingETContainer* mettst_nominal = new xAOD::MissingETContainer;
  xAOD::MissingETAuxContainer* mettst_nominal_aux = new xAOD::MissingETAuxContainer;
    mettst_nominal->setStore(mettst_nominal_aux);
    mettst_nominal->reserve(10);

  double metsig_cst(0.);
  double metsig_tst(0.);
  if (m_slices["met"]) {
    // (met, jets, electrons, muons, photons, taus, doTST, doJVT)
    ATH_CHECK( m_SUSYTools->GetMET(*metcst_nominal, jets_nominal, electrons_nominal, muons_nominal, photons_nominal, 0, false, false) );
    ATH_MSG_DEBUG("RefFinal CST etx="   << (*metcst_nominal)["Final"]->mpx()
                          << ", ety="   << (*metcst_nominal)["Final"]->mpy()
                          << ", et="    << (*metcst_nominal)["Final"]->met()
                          << ", sumet=" << (*metcst_nominal)["Final"]->sumet());
    ATH_CHECK( m_SUSYTools->GetMETSig(*metcst_nominal, metsig_cst, false, false) );
    ATH_MSG_DEBUG("METSignificance = " << metsig_cst);

    ATH_CHECK( m_SUSYTools->GetMET(*mettst_nominal, jets_nominal, electrons_nominal, muons_nominal, photons_nominal, 0, true, true) );
    ATH_MSG_DEBUG("RefFinal TST etx="   << (*mettst_nominal)["Final"]->mpx()
                          << ", ety="   << (*mettst_nominal)["Final"]->mpy()
                          << ", et="    << (*mettst_nominal)["Final"]->met()
                          << ", sumet=" << (*mettst_nominal)["Final"]->sumet());
    ATH_CHECK( m_SUSYTools->GetMETSig(*mettst_nominal, metsig_tst, true, true) );
    ATH_MSG_DEBUG("METSignificance = " << metsig_tst);

    hist("MET/met_et")    ->Fill( (*mettst_nominal)["Final"]->met()*0.001 );
    hist("MET/met_sumet") ->Fill( (*mettst_nominal)["Final"]->sumet()*0.001 );
    hist("MET/met_phi")   ->Fill( (*mettst_nominal)["Final"]->phi() );
    hist("MET/met_et_tst")->Fill( (*mettst_nominal)["PVSoftTrk"]->met() *0.001 );
    hist("MET/met_et_el") ->Fill( (*mettst_nominal)["RefEle"]->met()    *0.001 );
    hist("MET/met_et_ph") ->Fill( (*mettst_nominal)["RefGamma"]->met()  *0.001 );
    hist("MET/met_et_mu") ->Fill( (*mettst_nominal)["Muons"]->met()     *0.001 );
    hist("MET/met_et_jet")->Fill( (*mettst_nominal)["RefJet"]->met()    *0.001 );
    //hist("MET/met_et_tau")->Fill( (*mettst_nominal)["RefTau"]->met()    *0.001 );

    hist("MET/met_significance")->Fill( metsig_tst );
  }

  //--- Overlap Removal
  ATH_CHECK( m_SUSYTools->OverlapRemoval(electrons_nominal, muons_nominal, jets_nominal, photons_nominal, taus_nominal) );


  //--- Monitoring
  for (auto obj : m_objects) { for (auto lev : m_levels) { m_obj_count[obj][lev] = 0; } }

  //----- Electrons

  TH1* el_n_flow_nominal = hist("Cutflow/el_n_flow_nominal");
  TH1* el_trigmatch_eff_nominal = hist("Trigger/el_trigmatch_eff_nominal");

  if (m_slices["ele"]) {
    ATH_MSG_DEBUG("Processing electrons");
    m_obj_count["el"]["nom"] = electrons_nominal->size();
    for(auto el : *electrons_nominal) {
      el_n_flow_nominal->Fill(Cut::all);
      if ( el->auxdata<char>("baseline") == 1 ){
        el_n_flow_nominal->Fill(Cut::baseline);
        stdHistsForObj(el,"el","bsl");
        m_obj_count["el"]["bsl"] += 1;

        if ( el->auxdata<char>("passOR") == 1 ){
          el_n_flow_nominal->Fill(Cut::passOR);
          if ( el->auxdata<char>("signal") == 1 ){
            el_n_flow_nominal->Fill(Cut::signal);
            stdHistsForObj(el,"el","sig");
            m_obj_count["el"]["sig"] += 1;

            bool passTM=false;
            unsigned int idx=1;
            for(const auto& t : m_triggers["el"]){
              ATH_MSG_DEBUG( "  Processing trigger " << t );
              bool passit = m_SUSYTools->IsTrigMatched(el, t);
              passTM |= passit;
              if(passit) el_trigmatch_eff_nominal->SetBinContent(idx, el_trigmatch_eff_nominal->GetBinContent(idx)+1);
              m_heffs["Trigger/el_pt_"+t]->Fill(passit,el->pt()/1000.);
              m_heffs["Trigger/el_eta_"+t]->Fill(passit,el->eta());
              m_heffs["Trigger/el_phi_"+t]->Fill(passit,el->phi());
              idx++;
            }
            if(passTM) el_n_flow_nominal->Fill(Cut::trigmatch);
          }
        }
      }
      stdHistsForObj(el,"el","nom");
    }
    hist("Electron/el_nom_N")->Fill(m_obj_count["el"]["nom"]);
    hist("Electron/el_bsl_N")->Fill(m_obj_count["el"]["bsl"]);
    hist("Electron/el_sig_N")->Fill(m_obj_count["el"]["sig"]);
  }

  //----- Photons

  TH1* ph_n_flow_nominal = hist("Cutflow/ph_n_flow_nominal");
  TH1* ph_trigmatch_eff_nominal = hist("Trigger/ph_trigmatch_eff_nominal");

  if (m_slices["pho"]) {
    ATH_MSG_DEBUG("Processing photons");
    m_obj_count["ph"]["nom"] = photons_nominal->size();
    for(auto ph : *photons_nominal) {
      ph_n_flow_nominal->Fill(Cut::all);
      if ( ph->auxdata<char>("baseline") == 1 ){
        ph_n_flow_nominal->Fill(Cut::baseline);
        stdHistsForObj(ph,"ph","bsl");
        m_obj_count["ph"]["bsl"] += 1;
        if ( ph->auxdata<char>("passOR") == 1 ){
          ph_n_flow_nominal->Fill(Cut::passOR);
          if ( ph->auxdata<char>("signal") == 1 ){
            ph_n_flow_nominal->Fill(Cut::signal);
            stdHistsForObj(ph,"ph","sig");
            m_obj_count["ph"]["sig"] += 1;

            bool passTM=false;
            unsigned int idx=1;
            for(const auto& t : m_triggers["ph"]){
              bool passit = m_SUSYTools->IsTrigMatched(ph, t);
              passTM |= passit;
              if(passit) ph_trigmatch_eff_nominal->SetBinContent(idx, ph_trigmatch_eff_nominal->GetBinContent(idx)+1);
              m_heffs["Trigger/ph_pt_"+t]->Fill(passit,ph->pt()/1000.);
              m_heffs["Trigger/ph_eta_"+t]->Fill(passit,ph->eta());
              m_heffs["Trigger/ph_phi_"+t]->Fill(passit,ph->phi());
              idx++;
            }
            if (passTM) ph_n_flow_nominal->Fill(Cut::trigmatch);
          }
        }
      }
      stdHistsForObj(ph,"ph","nom");
    }
    hist("Photon/ph_nom_N")->Fill(m_obj_count["ph"]["nom"]);
    hist("Photon/ph_bsl_N")->Fill(m_obj_count["ph"]["bsl"]);
    hist("Photon/ph_sig_N")->Fill(m_obj_count["ph"]["sig"]);
  }

  //----- Muons

  TH1* mu_n_flow_nominal = hist("Cutflow/mu_n_flow_nominal");
  TH1* mu_trigmatch_eff_nominal = hist("Trigger/mu_trigmatch_eff_nominal");

  if (m_slices["mu"]) {
    ATH_MSG_DEBUG("Processing muons");
    m_obj_count["mu"]["nom"] = muons_nominal->size();
    for(auto mu : *muons_nominal) {
      mu_n_flow_nominal->Fill(Cut::all);
      if ( mu->auxdata<char>("baseline") == 1 ){
        mu_n_flow_nominal->Fill(Cut::baseline);
        stdHistsForObj(mu,"mu","bsl");
        m_obj_count["mu"]["bsl"] += 1;
        if ( mu->auxdata<char>("passOR") == 1 ){
          mu_n_flow_nominal->Fill(Cut::passOR);
          if ( mu->auxdata<char>("signal") == 1 ){
            mu_n_flow_nominal->Fill(Cut::signal);
            stdHistsForObj(mu,"mu","sig");
            m_obj_count["mu"]["sig"] += 1;

            bool passTM=false;
            unsigned int idx=1;
            for(const auto& t : m_triggers["mu"]){
              bool passit = m_SUSYTools->IsTrigMatched(mu, t);
              passTM |= passit;
              if(passit) mu_trigmatch_eff_nominal->SetBinContent(idx, mu_trigmatch_eff_nominal->GetBinContent(idx)+1);
              m_heffs["Trigger/mu_pt_"+t]->Fill(passit,mu->pt()/1000.);
              m_heffs["Trigger/mu_eta_"+t]->Fill(passit,mu->eta());
              m_heffs["Trigger/mu_phi_"+t]->Fill(passit,mu->phi());
              idx++;
            }
            if(passTM) mu_n_flow_nominal->Fill(Cut::trigmatch);
          }
        }
      }
      stdHistsForObj(mu,"mu","nom");
    }
    hist("Muon/mu_nom_N")->Fill(m_obj_count["mu"]["nom"]);
    hist("Muon/mu_bsl_N")->Fill(m_obj_count["mu"]["bsl"]);
    hist("Muon/mu_sig_N")->Fill(m_obj_count["mu"]["sig"]);
  }

  //----- Jets

  TH1* jet_n_flow_nominal = hist("Cutflow/jet_n_flow_nominal");
  TH1* bjet_n_flow_nominal = hist("Cutflow/bjet_n_flow_nominal");
  bool bjet = false;

  if (m_slices["jet"]) {
  m_obj_count["jet"]["nom"] = jets_nominal->size();
  for(auto jet : *jets_nominal) {
    ATH_MSG_DEBUG("Processing jets");
    jet_n_flow_nominal->Fill(Cut::all);
    bjet = false;
    if ( jet->auxdata<char>("bjet") == 1  ) bjet = true;
    if (bjet) bjet_n_flow_nominal->Fill(Cut::all);
    //
    if ( jet->auxdata<char>("baseline") == 1 ){
      jet_n_flow_nominal->Fill(Cut::baseline);
      if (bjet) {
         bjet_n_flow_nominal->Fill(Cut::baseline);
         stdHistsForObj(jet,"bjet","bsl");
         m_obj_count["bjet"]["bsl"] += 1;
      }
      stdHistsForObj(jet,"jet","bsl");
      m_obj_count["jet"]["bsl"] += 1;

      if ( jet->auxdata<char>("passOR") == 1 ){
        jet_n_flow_nominal->Fill(Cut::passOR);
        if (bjet) bjet_n_flow_nominal->Fill(Cut::passOR);
        if ( jet->auxdata<char>("signal") == 1 ){
          jet_n_flow_nominal->Fill(Cut::signal);
          jet_n_flow_nominal->Fill(Cut::trigmatch); //no trig matching for jets
          if (bjet) {
             bjet_n_flow_nominal->Fill(Cut::signal);
             bjet_n_flow_nominal->Fill(Cut::trigmatch); //no trig matching for (b)jets
             stdHistsForObj(jet,"bjet","sig");
             m_obj_count["bjet"]["sig"] += 1;
          }
          stdHistsForObj(jet,"jet","sig");
          m_obj_count["jet"]["sig"] += 1;
        }
      }
    }
    if (bjet) {
      stdHistsForObj(jet,"bjet","nom");
    }
    stdHistsForObj(jet,"jet","nom");
  }
  hist("Jet/jet_nom_N")->Fill(m_obj_count["jet"]["nom"]);
  hist("Jet/jet_bsl_N")->Fill(m_obj_count["jet"]["bsl"]);
  hist("Jet/jet_sig_N")->Fill(m_obj_count["jet"]["sig"]);
  }

  //----- Large R. Jets

  TH1* fatjet_n_flow_nominal = hist("Cutflow/fatjet_n_flow_nominal");

  if( m_slices["fjet"] && doFatJets ) {
    ATH_MSG_DEBUG("Processing fatjets");
    if (m_configDict.find("Jet.WtaggerConfig")!=m_configDict.end() && m_configDict.find("WtaggerKey")==m_configDict.end() && fatjets_nominal->size()>0) {
       SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
       std::string taggerKey;
       taggerKey = findInReg(reg, (*fatjets_nominal)[0], "(SmoothW.*?)_.*");
       if (!taggerKey.empty()) { m_configDict["WtaggerKey"] = taggerKey; }
    }
    if (m_configDict.find("Jet.ZtaggerConfig")!=m_configDict.end() && m_configDict.find("ZtaggerKey")==m_configDict.end() && fatjets_nominal->size()>0) {
       SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
       std::string taggerKey;
       taggerKey = findInReg(reg, (*fatjets_nominal)[0], "(SmoothZ.*?)_.*");
       if (!taggerKey.empty()) { m_configDict["ZtaggerKey"] = taggerKey; }
    }
    if (m_configDict.find("Jet.ToptaggerConfig")!=m_configDict.end() && m_configDict.find("ToptaggerKey")==m_configDict.end() && fatjets_nominal->size()>0) {
       SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
       std::string taggerKey;
       taggerKey = findInReg(reg, (*fatjets_nominal)[0], "(.*Tag.*Top.*?)_.*");
       if (!taggerKey.empty()) { m_configDict["ToptaggerKey"] = taggerKey; }
    }
    ATH_MSG_DEBUG("Tagger configuration retrieved.");
    ATH_MSG_DEBUG("WTagger:   " << m_configDict["WtaggerKey"]);
    ATH_MSG_DEBUG("ZTagger:   " << m_configDict["ZtaggerKey"]);
    ATH_MSG_DEBUG("TopTagger: " << m_configDict["ToptaggerKey"]);

    m_obj_count["fatjet"]["nom"] = fatjets_nominal->size();
    for(auto fatjet : *fatjets_nominal) {
      fatjet_n_flow_nominal->Fill(Cut::all);
      if ( fatjet->auxdata<char>("baseline") == 1 ){
        fatjet_n_flow_nominal->Fill(Cut::baseline);
        stdHistsForObj(fatjet,"fatjet","bsl",m_configDict);
        m_obj_count["fatjet"]["bsl"] += 1;
        if ( fatjet->auxdata<char>("passOR") == 1 ){
          fatjet_n_flow_nominal->Fill(Cut::passOR);
          if ( fatjet->auxdata<char>("signal") == 1 ){
            fatjet_n_flow_nominal->Fill(Cut::signal);
            fatjet_n_flow_nominal->Fill(Cut::trigmatch); //no trig matching for jets
            stdHistsForObj(fatjet,"fatjet","sig",m_configDict);
            m_obj_count["fatjet"]["sig"] += 1;
          }
        }
      }
      stdHistsForObj(fatjet,"fatjet","nom",m_configDict);
    }
    hist("LargeRJet/fatjet_nom_N")->Fill(m_obj_count["fatjet"]["nom"]);
    hist("LargeRJet/fatjet_bsl_N")->Fill(m_obj_count["fatjet"]["bsl"]);
    hist("LargeRJet/fatjet_sig_N")->Fill(m_obj_count["fatjet"]["sig"]);
  }

  //----- Track Jets

  TH1* trkjet_n_flow_nominal = hist("Cutflow/trkjet_n_flow_nominal");

  if( m_slices["tjet"] && doTrkJets ) {
    m_obj_count["trkjet"]["nom"] = trkjets_nominal->size();
    for(auto trkjet : *trkjets_nominal) {
      trkjet_n_flow_nominal->Fill(Cut::all);
      if ( trkjet->auxdata<char>("baseline") == 1 ){
        trkjet_n_flow_nominal->Fill(Cut::baseline);
        stdHistsForObj(trkjet,"trkjet","bsl");
        m_obj_count["trkjet"]["bsl"] += 1;
        if ( trkjet->auxdata<char>("passOR") == 1 ){
          trkjet_n_flow_nominal->Fill(Cut::passOR);
          if ( trkjet->auxdata<char>("signal") == 1 ){
            trkjet_n_flow_nominal->Fill(Cut::signal);
            trkjet_n_flow_nominal->Fill(Cut::trigmatch); //no trig matching for jets
            stdHistsForObj(trkjet,"trkjet","sig");
            m_obj_count["trkjet"]["sig"] += 1;
          }
        }
      }
      stdHistsForObj(trkjet,"trkjet","nom");
    }
    hist("TrackJet/trkjet_nom_N")->Fill(m_obj_count["trkjet"]["nom"]);
    hist("TrackJet/trkjet_bsl_N")->Fill(m_obj_count["trkjet"]["bsl"]);
    hist("TrackJet/trkjet_sig_N")->Fill(m_obj_count["trkjet"]["sig"]);
  }

  //----- Taus

  TH1* tau_n_flow_nominal = hist("Cutflow/tau_n_flow_nominal");

  if (m_slices["tau"]) {
    m_obj_count["tau"]["nom"] = taus_nominal->size();
    for(auto tau : *taus_nominal) {
      tau_n_flow_nominal->Fill(Cut::all);
      if ( tau->auxdata<char>("baseline") == 1 ){
        tau_n_flow_nominal->Fill(Cut::baseline);
        stdHistsForObj(tau,"tau","bsl");
        m_obj_count["tau"]["bsl"] += 1;
        if ( tau->auxdata<char>("passOR") == 1 ){
          tau_n_flow_nominal->Fill(Cut::passOR);

          if ( tau->auxdata<char>("signal") == 1 ){
            tau_n_flow_nominal->Fill(Cut::signal);
            tau_n_flow_nominal->Fill(Cut::trigmatch); //no trig matching for taus
            stdHistsForObj(tau,"tau","sig");
            m_obj_count["tau"]["sig"] += 1;
          }
        }
      }
      stdHistsForObj(tau,"tau","nom");
    }
    hist("Tau/tau_nom_N")->Fill(m_obj_count["tau"]["nom"]);
    hist("Tau/tau_bsl_N")->Fill(m_obj_count["tau"]["bsl"]);
    hist("Tau/tau_sig_N")->Fill(m_obj_count["tau"]["sig"]);
  }

  // Set up the event weights
  // Base should include all weights that do not depend on individual objects
  double base_event_weight(1.);
  if (!isData) {
    base_event_weight *= evtInfo->mcEventWeight();

    if (m_slices["met"]) {
      const xAOD::MissingETContainer* met_truth(0);
      ATH_CHECK( evtStore()->retrieve(met_truth, "MET_Truth") );

      ATH_MSG_DEBUG("Truth MET etx="   << (*met_truth)["NonInt"]->mpx()
                         << ", ety="   << (*met_truth)["NonInt"]->mpy()
                         << ", et="    << (*met_truth)["NonInt"]->met()
                         << ", sumet=" << (*met_truth)["NonInt"]->sumet());
      ATH_MSG_DEBUG("CST residuals: detx=" << (*metcst_nominal)["Final"]->mpx() - (*met_truth)["NonInt"]->mpx()
                    << ", dety=" << (*metcst_nominal)["Final"]->mpy() - (*met_truth)["NonInt"]->mpy());
      ATH_MSG_DEBUG("TST residuals: detx=" << (*mettst_nominal)["Final"]->mpx() - (*met_truth)["NonInt"]->mpx()
                    << ", dety=" << (*mettst_nominal)["Final"]->mpy() - (*met_truth)["NonInt"]->mpy());
    }
  }

  // Additionally define a nominal weight for each object type
  double electrons_weight_nominal(1.);
  double photons_weight_nominal(1.);
  double muons_weight_nominal(1.);
  double jets_weight_nominal(1.);
  //double fatjets_weight_nominal(1.);
  //double trkjets_weight_nominal(1.);
  double btag_weight_nominal(1.);
  double taus_weight_nominal(1.);
  double event_weight_nominal(1.);
  double prw_weight_nominal(1.);

  //--- Weights
  TH1* weight_event     = hist("Syst/weight_event");
  TH1* weight_electrons = hist("Syst/weight_electrons");
  TH1* weight_muons     = hist("Syst/weight_muons");
  TH1* weight_photons   = hist("Syst/weight_photons");
  TH1* weight_taus      = hist("Syst/weight_taus");
  TH1* weight_jets      = hist("Syst/weight_jets");
  TH1* weight_btags     = hist("Syst/weight_btags");

  bool isNominal(true);
  for (const auto& sysInfo : sysInfoList) {
    CP::SystematicSet sys = sysInfo.systset;
    if (m_SUSYTools->applySystematicVariation(sys) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "Cannot configure SUSYTools for systematic var. %s" << sys.name() );
    } else {
      ATH_MSG_VERBOSE( "Variation \"" << sys.name() << "\" configured..." );
    }
    if (isNominal && (sysInfo.affectsKinematics || sysInfo.affectsWeights)) {
      ATH_MSG_ERROR("Expected nominal variation!");
      return StatusCode::FAILURE;
    }
    if (sysInfo.affectsKinematics || sysInfo.affectsWeights) isNominal = false;

    double event_weight = base_event_weight;
    bool syst_affectsEventWeight = (sysInfo.affectsType == ST::SystObjType::EventWeight);

    // If nominal, compute the nominal weight, otherwise recompute the weight

    double prw_weight=1.;
    if(isNominal){
      if(!isData) prw_weight_nominal = m_SUSYTools->GetPileupWeight();
      event_weight *= prw_weight_nominal;
      weight_event->SetBinContent(1, weight_event->GetBinContent(1)+event_weight);
    }
    else if (!syst_affectsEventWeight) {
      event_weight *= prw_weight_nominal;
    }
    else {
      if(!isData){
        ATH_CHECK(m_SUSYTools->ApplyPRWTool());
        prw_weight = m_SUSYTools->GetPileupWeight();
      }
      event_weight *= prw_weight;
      size_t iwbin = find(syst_weights["EventWeight"].begin(), syst_weights["EventWeight"].end(), sys.name()) - syst_weights["EventWeight"].begin();
      if(iwbin < syst_weights["EventWeight"].size()) {  weight_event->SetBinContent(iwbin+1, weight_event->GetBinContent(iwbin+1)+event_weight); }
    }


    // Define the generic collection pointers
    xAOD::ElectronContainer* electrons(electrons_nominal);
    xAOD::PhotonContainer* photons(photons_nominal);
    xAOD::MuonContainer* muons(muons_nominal);
    xAOD::JetContainer* jets(jets_nominal);
    xAOD::JetContainer* fatjets(fatjets_nominal);
    xAOD::JetContainer* trkjets(trkjets_nominal);
    xAOD::TauJetContainer* taus(taus_nominal);
    xAOD::MissingETContainer *mettst(mettst_nominal), *metcst(metcst_nominal);
    xAOD::MissingETAuxContainer *mettst_aux(mettst_nominal_aux), *metcst_aux(metcst_nominal_aux);

    bool syst_affectsElectrons = ST::testAffectsObject(xAOD::Type::Electron, sysInfo.affectsType);
    bool syst_affectsMuons     = ST::testAffectsObject(xAOD::Type::Muon, sysInfo.affectsType);
    bool syst_affectsPhotons   = ST::testAffectsObject(xAOD::Type::Photon, sysInfo.affectsType);
    bool syst_affectsTaus      = ST::testAffectsObject(xAOD::Type::Tau, sysInfo.affectsType);
    bool syst_affectsJets      = ST::testAffectsObject(xAOD::Type::Jet, sysInfo.affectsType);
    bool syst_affectsBTag      = ST::testAffectsObject(xAOD::Type::BTag, sysInfo.affectsType);

    // If necessary (kinematics affected), make a shallow copy with the variation applied
    // otherwise set the collection pointers to the nominal
    if (sysInfo.affectsKinematics) {
      if (m_slices["el"] && syst_affectsElectrons) {
        ATH_MSG_DEBUG("Get systematics-varied electrons");
        xAOD::ElectronContainer* electrons_syst(0);
        xAOD::ShallowAuxContainer* electrons_syst_aux(0);
        ATH_CHECK( m_SUSYTools->GetElectrons(electrons_syst, electrons_syst_aux) );
        electrons = electrons_syst;
      }

      if (m_slices["pho"] && syst_affectsPhotons) {
        ATH_MSG_DEBUG("Get systematics-varied photons");
        xAOD::PhotonContainer* photons_syst(0);
        xAOD::ShallowAuxContainer* photons_syst_aux(0);
        ATH_CHECK( m_SUSYTools->GetPhotons(photons_syst, photons_syst_aux) );
        photons = photons_syst;
      }

      if (m_slices["mu"] && syst_affectsMuons) {
        ATH_MSG_DEBUG("Get systematics-varied muons");
        xAOD::MuonContainer* muons_syst(0);
        xAOD::ShallowAuxContainer* muons_syst_aux(0);
        ATH_CHECK( m_SUSYTools->GetMuons(muons_syst, muons_syst_aux) );
        muons = muons_syst;
      }

      if (syst_affectsJets) {
        if (m_slices["jet"]) {
          ATH_MSG_DEBUG("Get systematics-varied jets");
          xAOD::JetContainer* jets_syst(0);
          xAOD::ShallowAuxContainer* jets_syst_aux(0);
          ATH_CHECK( m_SUSYTools->GetJetsSyst(*jets_nominal, jets_syst, jets_syst_aux) );
          jets = jets_syst;
        }
        if (m_slices["fatjet"]) {
          ATH_MSG_DEBUG("Get systematics-varied fatjets");
          xAOD::JetContainer* fatjets_syst(0);
          xAOD::ShallowAuxContainer* fatjets_syst_aux(0);
          ATH_CHECK( m_SUSYTools->GetJetsSyst(*fatjets_nominal, fatjets_syst, fatjets_syst_aux) );
          fatjets = fatjets_syst;
        }
        if (m_slices["trkjet"]) {
          ATH_MSG_DEBUG("Get systematics-varied trkjets");
          xAOD::JetContainer* trkjets_syst(0);
          xAOD::ShallowAuxContainer* trkjets_syst_aux(0);
          ATH_CHECK( m_SUSYTools->GetJetsSyst(*trkjets_nominal, trkjets_syst, trkjets_syst_aux) );
          trkjets = trkjets_syst;
        }
      }

      if (m_slices["tau"] && syst_affectsTaus) {
        ATH_MSG_DEBUG("Get systematics-varied taus");
        xAOD::TauJetContainer* taus_syst(0);
        xAOD::ShallowAuxContainer* taus_syst_aux(0);
        ATH_CHECK( m_SUSYTools->GetTaus(taus_syst, taus_syst_aux) );
        taus = taus_syst;
      }

      ATH_MSG_DEBUG("Get systematics-varied MET");
      xAOD::MissingETContainer* mettst_syst = new xAOD::MissingETContainer;
      xAOD::MissingETAuxContainer* mettst_syst_aux = new xAOD::MissingETAuxContainer;
      xAOD::MissingETContainer* metcst_syst = new xAOD::MissingETContainer;
      xAOD::MissingETAuxContainer* metcst_syst_aux = new xAOD::MissingETAuxContainer;
      mettst_syst->setStore(mettst_syst_aux);
      metcst_syst->setStore(metcst_syst_aux);
      mettst_syst->reserve(10);
      metcst_syst->reserve(10);
      //
      if (m_slices["met"]) {
        ATH_CHECK( m_SUSYTools->GetMET(*metcst_syst, jets, electrons, muons, photons, 0) );
        ATH_CHECK( m_SUSYTools->GetMET(*mettst_syst, jets, electrons, muons, photons, 0, true, true) );
      }
      //
      mettst     = mettst_syst;
      mettst_aux = mettst_syst_aux;
      metcst     = metcst_syst;
      metcst_aux = metcst_syst_aux;
    }

    //--- Overlap Removal
    ATH_CHECK( m_SUSYTools->OverlapRemoval(electrons, muons, jets, photons, taus) );


    //--- Electrons
    if (m_slices["ele"]) {
      ATH_MSG_DEBUG("Working on electrons");
      float electrons_weight(1.);
      for ( const auto& el : *electrons ) {
        if( !isData ){
          if (isNominal || syst_affectsElectrons) {
            if ((el->auxdata< char >("signal") == 1) && (isNominal || sysInfo.affectsWeights)) {
              electrons_weight *= m_SUSYTools->GetSignalElecSF( *el ); // (*el, true, true, false, true) to switch off trigger SF
            }
          }
        }
        ATH_MSG_VERBOSE( "  Electron passing baseline selection?" << (int) el->auxdata<char>("baseline") );
        ATH_MSG_VERBOSE( "  Electron passing signal selection?" << (int) el->auxdata<char>("signal") );
        if (el->auxdata< char >("signal") == 1)
          ATH_MSG_VERBOSE( "  Electron weight " << el->auxdata<double>("effscalefact") );

      }
      if (isNominal) {
        electrons_weight_nominal = electrons_weight;
        weight_electrons->SetBinContent(1, weight_electrons->GetBinContent(1)+electrons_weight);
      }
      else if (!syst_affectsElectrons) {
        electrons_weight = electrons_weight_nominal;
      }
      else if ( sysInfo.affectsWeights ){
        size_t iwbin = find(syst_weights["Electron"].begin(), syst_weights["Electron"].end(), sys.name()) - syst_weights["Electron"].begin();
        if(iwbin < syst_weights["Electron"].size()) {  weight_electrons->SetBinContent(iwbin+1, weight_electrons->GetBinContent(iwbin+1)+electrons_weight); }
      }

      event_weight *= electrons_weight;
      ATH_MSG_DEBUG("Combined electron scale factor: " << electrons_weight);
    }

    //--- Photons
    if (m_slices["pho"]) {
      ATH_MSG_DEBUG("Working on photons");
      float photons_weight(1.);
      for ( const auto& ph : *photons ) {
        if( !isData ){
          if (isNominal || syst_affectsPhotons) {
            if ((ph->auxdata< char >("signal") == 1) && (isNominal || sysInfo.affectsWeights)) {
              photons_weight *= m_SUSYTools->GetSignalPhotonSF( *ph );
            }
          }
        }
        ATH_MSG_VERBOSE( "  Photon passing baseline selection?" << (int) ph->auxdata<char>("baseline") );
        ATH_MSG_VERBOSE( "  Photon passing signal selection?" << (int) ph->auxdata<char>("signal") );
        if (ph->auxdata< char >("signal") == 1)
          ATH_MSG_VERBOSE( "  Photon weight " << ph->auxdata<double>("effscalefact") );
      }
      if (isNominal) {
        photons_weight_nominal = photons_weight;
        weight_photons->SetBinContent(1, weight_photons->GetBinContent(1)+photons_weight);
      }
      else if (!syst_affectsPhotons) {
        photons_weight = photons_weight_nominal;
      }
      else if ( sysInfo.affectsWeights ){
        size_t iwbin = find(syst_weights["Photon"].begin(), syst_weights["Photon"].end(), sys.name()) - syst_weights["Photon"].begin();
        if(iwbin < syst_weights["Photon"].size()) {  weight_photons->SetBinContent(iwbin+1, weight_photons->GetBinContent(iwbin+1)+photons_weight); }
      }

      event_weight *= photons_weight;
      ATH_MSG_DEBUG("Combined photon scale factor: " << photons_weight);
    }

    //--- Muons
    if (m_slices["mu"]) {
      ATH_MSG_DEBUG("Working on muons");
      float muons_weight(1.);
      for ( const auto& mu : *muons ) {
        if( !isData ){
          if (isNominal || syst_affectsMuons) {
            if ((mu->auxdata< char >("signal") == 1) && (isNominal || sysInfo.affectsWeights)) {
              muons_weight *= m_SUSYTools->GetSignalMuonSF(*mu);;
            }
          }
        }
        ATH_MSG_VERBOSE( "  Muon passing baseline selection?" << (int) mu->auxdata<char>("baseline") );
        ATH_MSG_VERBOSE( "  Muon passing signal selection?" << (int) mu->auxdata<char>("signal") );
        ATH_MSG_VERBOSE( "  Muon is a cosmic ray?" << (int) mu->auxdata<char>("cosmic") );
        if (mu->auxdata< char >("signal") == 1)
          ATH_MSG_VERBOSE( "  Muon weight " << mu->auxdata<double>("effscalefact") );
      }
      if (isNominal) {
        muons_weight_nominal = muons_weight;
        weight_muons->SetBinContent(1, weight_muons->GetBinContent(1)+muons_weight);
      }
      else if (!syst_affectsMuons) {
        muons_weight = muons_weight_nominal;
      }
      else if ( sysInfo.affectsWeights ){
        size_t iwbin = find(syst_weights["Muon"].begin(), syst_weights["Muon"].end(), sys.name()) - syst_weights["Muon"].begin();
        if(iwbin < syst_weights["Muon"].size()) {  weight_muons->SetBinContent(iwbin+1, weight_muons->GetBinContent(iwbin+1)+muons_weight); }
      }

      event_weight *= muons_weight;
      ATH_MSG_DEBUG("Combined muon scale factor: " << muons_weight);
    }

    //--- Jets
    if (m_slices["jet"]) {
      ATH_MSG_DEBUG("Working on jets");
      for ( const auto& jet : *jets ) {
        ATH_MSG_VERBOSE( " Jet is bad? " << (int) jet->auxdata<char>("bad") );
        ATH_MSG_VERBOSE( " Jet is baseline ? " << (int) jet->auxdata<char>("baseline") );
        ATH_MSG_VERBOSE( " Jet passes OR ? " << (int) jet->auxdata<char>("passOR") );
        if (jet->auxdata<char>("passOR")) {
          if (m_slices["bjet"] && (isNominal || syst_affectsJets || syst_affectsBTag)) {
            m_SUSYTools->IsBJet(*jet);
          }
          ATH_MSG_VERBOSE( " Jet is bjet ? " << (int) jet->auxdata<char>("bjet") );
        }
      }

      float jet_weight(1.);
      float btag_weight(1.);
      if( !isData ) { //isMC
        if (isNominal) { //btagging
          btag_weight_nominal = btag_weight = m_SUSYTools->BtagSF(jets);
          weight_btags->SetBinContent(1, weight_btags->GetBinContent(1)+btag_weight);
        }
        else if (!syst_affectsBTag){
          btag_weight = btag_weight_nominal;
        }
        else{
          btag_weight = m_SUSYTools->BtagSF(jets);
          size_t iwbin = find(syst_weights["BTag"].begin(), syst_weights["BTag"].end(), sys.name()) - syst_weights["BTag"].begin();
          if(iwbin < syst_weights["BTag"].size()) {  weight_btags->SetBinContent(iwbin+1, weight_jets->GetBinContent(iwbin+1)+btag_weight); }
        }

        if(isNominal){ //JVT
          jets_weight_nominal = jet_weight = m_SUSYTools->JVT_SF(jets);
          weight_jets->SetBinContent(1, weight_jets->GetBinContent(1)+jet_weight);
        }
        else if (!syst_affectsJets || (syst_affectsJets && !sysInfo.affectsWeights)){
          jet_weight = jets_weight_nominal;
        }
        else if ( syst_affectsJets && sysInfo.affectsWeights ){
          jet_weight = m_SUSYTools->JVT_SF(jets);
          size_t iwbin = find(syst_weights["Jet"].begin(), syst_weights["Jet"].end(), sys.name()) - syst_weights["Jet"].begin();
          if(iwbin < syst_weights["Jet"].size()) {  weight_jets->SetBinContent(iwbin+1, weight_jets->GetBinContent(iwbin+1)+jet_weight); }
        }
      }

      event_weight *= jet_weight;
      event_weight *= btag_weight;
      ATH_MSG_DEBUG("Combined b-tagging scale factor: " << btag_weight);
      ATH_MSG_DEBUG("Combined jet scale factor: " << jet_weight);
    }

    //--- Large R. Jets

    if( m_slices["fjet"] && doFatJets ) {
      ATH_MSG_DEBUG("Working on fat jets");
      for ( const auto& fatjet : *fatjets ) {
        ATH_MSG_VERBOSE( " Jet is bad? " << (int) fatjet->auxdata<char>("bad") );
        ATH_MSG_VERBOSE( " Jet is baseline ? " << (int) fatjet->auxdata<char>("baseline") );
        ATH_MSG_VERBOSE( " Jet passes OR ? " << (int) fatjet->auxdata<char>("passOR") );
      }

      float fatjet_weight(1.);
      //disable - noJVTforfatjets if( !isData ) { //isMC
      //disable - noJVTforfatjets   if(isNominal){ //JVT - no JVT cuts for fat jets!
      //disable - noJVTforfatjets     fatjets_weight_nominal = fatjet_weight = m_SUSYTools->JVT_SF(fatjets);
      //disable - noJVTforfatjets     weight_fatjets->SetBinContent(1, weight_fatjets->GetBinContent(1)+fatjet_weight);
      //disable - noJVTforfatjets   }
      //disable - noJVTforfatjets   if (!syst_affectsJets || (syst_affectsJets && !sysInfo.affectsWeights)){
      //disable - noJVTforfatjets     fatjet_weight = fatjets_weight_nominal;
      //disable - noJVTforfatjets }
      //disable - noJVTforfatjets   else if ( syst_affectsJets && sysInfo.affectsWeights ){
      //disable - noJVTforfatjets     fatjet_weight = m_SUSYTools->JVT_SF(fatjets);
      //disable - noJVTforfatjets     size_t iwbin = find(syst_weights["Jet"].begin(), syst_weights["Jet"].end(), sys.name()) - syst_weights["Jet"].begin();
      //disable - noJVTforfatjets     if(iwbin < syst_weights["Jet"].size()) {  weight_fatjets->SetBinContent(iwbin+1, weight_fatjets->GetBinContent(iwbin+1)+fatjet_weight); }
      //disable - noJVTforfatjets   }
      //disable - noJVTforfatjets }

      ATH_MSG_DEBUG("Combined large radius jet scale factor: " << fatjet_weight);
    }

    //--- Track Jets

    if( m_slices["tjet"] && doTrkJets ) {
      ATH_MSG_DEBUG("Working on trk jets");
      for ( const auto& trkjet : *trkjets ) {
        ATH_MSG_VERBOSE( " Jet is bad? " << (int) trkjet->auxdata<char>("bad") );
        ATH_MSG_VERBOSE( " Jet is baseline ? " << (int) trkjet->auxdata<char>("baseline") );
        ATH_MSG_VERBOSE( " Jet passes OR ? " << (int) trkjet->auxdata<char>("passOR") );
      }

      float trkjet_weight(1.);
      //disable - noJVTfortrkjets if( !isData ) { //isMC
      //disable - noJVTfortrkjets   if(isNominal){ //JVT - no JVT cuts for trk jets!
      //disable - noJVTfortrkjets     trkjets_weight_nominal = trkjet_weight = 1;//m_SUSYTools->JVT_SF(trkjets);
      //disable - noJVTfortrkjets     weight_trkjets->SetBinContent(1, weight_trkjets->GetBinContent(1)+trkjet_weight);
      //disable - noJVTfortrkjets   }
      //disable - noJVTfortrkjets   else if (!syst_affectsJets || (syst_affectsJets && !sysInfo.affectsWeights)){
      //disable - noJVTfortrkjets     trkjet_weight = trkjets_weight_nominal;
      //disable - noJVTfortrkjets   }
      //disable - noJVTfortrkjets   else if ( syst_affectsJets && sysInfo.affectsWeights ){
      //disable - noJVTfortrkjets     trkjet_weight = 1;//m_SUSYTools->JVT_SF(trkjets);
      //disable - noJVTfortrkjets     size_t iwbin = find(syst_weights["Jet"].begin(), syst_weights["Jet"].end(), sys.name()) - syst_weights["Jet"].begin();
      //disable - noJVTfortrkjets     if(iwbin < syst_weights["Jet"].size()) {  weight_trkjets->SetBinContent(iwbin+1, weight_trkjets->GetBinContent(iwbin+1)+trkjet_weight); }
      //disable - noJVTfortrkjets   }
      //disable - noJVTfortrkjets }

      ATH_MSG_DEBUG("Combined track jet scale factor: " << trkjet_weight);
    }

    //--- Taus
    if (m_slices["tau"]) {
      ATH_MSG_DEBUG("Working on taus");
      float taus_weight(1.);
      for ( const auto& ta : *taus ) {
        if( !isData ){
          if (isNominal || syst_affectsTaus) {
            if ((ta->auxdata< char >("signal") == 1) && (isNominal || sysInfo.affectsWeights)) {
              taus_weight *= m_SUSYTools->GetSignalTauSF(*ta, true, true, "HLT_tau125_medium1_tracktwo");;
            }
          }
        }
        ATH_MSG_VERBOSE( "  Tau passing baseline selection?" << (int) ta->auxdata<char>("baseline") );
        ATH_MSG_VERBOSE( "  Tau passing signal selection?" << (int) ta->auxdata<char>("signal") );
        if (ta->auxdata< char >("signal") == 1)
          ATH_MSG_VERBOSE( "  Tau weight " << ta->auxdata<double>("effscalefact") );
      }

      if (isNominal) {
        taus_weight_nominal = taus_weight;
        weight_taus->SetBinContent(1, weight_taus->GetBinContent(1)+taus_weight);
      }
      else if (!syst_affectsTaus) {
        taus_weight = taus_weight_nominal;
      }
      else if ( sysInfo.affectsWeights ){
        size_t iwbin = find(syst_weights["Tau"].begin(), syst_weights["Tau"].end(), sys.name()) - syst_weights["Tau"].begin();
        if(iwbin < syst_weights["Tau"].size()) {  weight_taus->SetBinContent(iwbin+1, weight_taus->GetBinContent(iwbin+1)+taus_weight); }
      }

      event_weight *= taus_weight;
    }


    ATH_MSG_DEBUG("Full event weight: " << event_weight);
    if (isNominal) {event_weight_nominal = event_weight;}
    else if (sysInfo.affectsWeights) ATH_MSG_DEBUG("Difference with nominal weight: " << event_weight - event_weight_nominal);

    // Clean up the systematics copies
    if (sysInfo.affectsKinematics) {
      delete metcst;
      delete mettst;
      delete metcst_aux;
      delete mettst_aux;
    }
    isNominal = false;
  }

  m_clock0.Stop();
  if (m_Nevts == 0) {
    m_clock1.Stop();
  } else {
    m_clock2.Stop();
  }
  ++m_Nevts;

  return StatusCode::SUCCESS;
}

//====================================================================================================
// Book histograms
StatusCode SUSYToolsAlg::bookHistograms(void) {
  // General info
  ATH_CHECK( book(TH1D("EventInfo/info_runNo", ";RunNumber;N", 1500, 250000, 400000)) );
  ATH_CHECK( book(TH1D("EventInfo/info_evtNo", ";EventNumber;N", 600, 100000000, 400000000)) );
  ATH_CHECK( book(TH1D("EventInfo/info_lumiBlock", ";LumiBlock;N", 1000, 0, 1000)) );

  // Kinematic histograms
  m_objects = {"el", "ph", "mu", "tau", "jet", "bjet", "fatjet", "trkjet"};
  m_levels  = {"nom", "bsl", "sig"};
  m_vars    = {"pt", "eta", "phi", "e", "mass", "d0", "z0", "isolCalo", "isolTrack", "truthType", "truthOrigin", "N", "bweight", "bweightpb", "bweightpc", "bweightpu", "pid", "parentpid", "wtagged", "ztagged", "toptagged", "toptaggedscore","nTracks","nTracksCharged","nTracksIsolation","RNNJetScoreSigTrans"};
  std::map<std::string,std::vector<std::string>> cfg_hist_labels = {
     {"pt",{"p_{T} [GeV]","N"}}, {"eta",{"#eta","N"}}, {"phi",{"#phi","N"}}, {"e",{"energy","N"}}, {"mass",{"mass","N"}},
     {"d0",{"d0 significance","N"}}, {"z0",{"z0 * sin(#theta)","N"}}, {"isolCalo",{"isolation (calo) / pT","N"}}, {"isolTrack",{"isolation (track) / pT","N"}},
     {"truthType",{"truth type","N"}}, {"truthOrigin",{"truth origin","N"}}, {"N",{"count","N"}},
     {"bweight",{"b-weight","N"}}, {"bweightpb",{"b-weight (pb)","N"}}, {"bweightpc",{"b-weight (pc)","N"}}, {"bweightpu",{"b-weight (pu)","N"}},
     {"pid", {"pdg ID", "N"}}, {"parentpid", {"parent pdg ID","N"}},
     {"wtagged", {"W tagged", "N"}}, {"ztagged", {"Z tagged", "N"}}, {"toptagged", {"Top tagged", "N"}}, {"toptaggedscore", {"Top tagger score", "N"}},
     {"nTracks", {"N tracks", "N"}}, {"nTracksCharged", {"N tracks charged", "N"}}, {"nTracksIsolation", {"N tracks isolation", "N"}}, {"RNNJetScoreSigTrans", {"RNNJetScoreSigTrans","N"}} };
  std::map<std::string,int>                      cfg_hist_nbins  = {
     {"pt",100}, {"eta",40}, {"phi",64}, {"e",100}, {"mass",100},
     {"d0",50}, {"z0",60}, {"isolCalo",70}, {"isolTrack",70},
     {"truthType",50}, {"truthOrigin",50}, {"N",16},
     {"bweight",200}, {"bweightpb",200}, {"bweightpc",200}, {"bweightpu",200},
     {"pid",61}, {"parentpid",61},
     {"wtagged",3}, {"ztagged",3}, {"toptagged",3}, {"toptaggedscore",50},
     {"nTracks",20}, {"nTracksCharged",20}, {"nTracksIsolation",5}, {"RNNJetScoreSigTrans",50} };
  std::map<std::string,std::vector<float>>       cfg_hist_minmax = {
     {"pt",{0,200}}, {"eta",{-4,4}}, {"phi",{-3.2,3.2}}, {"e",{0,200}}, {"mass",{0,200}},
     {"d0",{-10,10}}, {"z0",{-1.5,1.5}}, {"isolCalo",{-0.2,0.5}}, {"isolTrack",{-0.2,0.5}},
     {"truthType",{0,50}}, {"truthOrigin",{0,50}}, {"N",{0,16}},
     {"bweight",{-10,10}}, {"bweightpb",{-0.5,1.5}}, {"bweightpc",{-0.5,1.5}}, {"bweightpu",{-0.5,1.5}},
     {"pid",{-30.5,30.5}}, {"parentpid",{-30.5,30.5}},
     {"wtagged", {-1,2}}, {"ztagged", {-1,2}}, {"toptagged", {-1,2}}, {"toptaggedscore", {0,1}},
     {"nTracks", {0,20}}, {"nTracksCharged", {0,20}}, {"nTracksIsolation", {0,5}}, {"RNNJetScoreSigTrans", {0,1}} };
  std::map<std::string,std::string>              labels_objects  = { {"el","Electron"}, {"ph","Photon"}, {"mu","Muon"}, {"jet","Jet"}, {"bjet","b-Jet"}, {"tau","Tau"}, {"fatjet","Large-R jet"}, {"trkjet","Track jet"} };
  std::map<std::string,std::string>              labels_levels   = { {"nom","Nominal"}, {"bsl","Baseline"}, {"sig","Signal"} };
  std::map<std::string,std::string>              labels_dir      = { {"el","Electron"}, {"ph","Photon"}, {"mu","Muon"}, {"jet","Jet"}, {"bjet","bJet"}, {"tau","Tau"}, {"fatjet","LargeRJet"}, {"trkjet","TrackJet"} };

  for (auto obj : m_objects) {
     m_obj_count[obj] = std::map<std::string,int>();
     for (auto lev : m_levels) {
        m_obj_count[obj][lev] = 0;
     }
  }

  for (auto obj : m_objects) {
     for (auto lev : m_levels) {
        for (auto var : m_vars) {
           if (var.find("bweight")!=std::string::npos && obj.compare("bjet")!=0) continue;    // bweights only for bjets
           if (var.find("tagged")!=std::string::npos && obj.compare("fatjet")!=0) continue;   // boson tagging only for fjets
           if ((var.find("nTracks")!=std::string::npos||var.find("RNNJetScore")!=std::string::npos) && obj.compare("tau")!=0) continue; // nTracks/RNN score only for taus
           if (var.find("isol")!=std::string::npos && !(obj.compare("el")==0||obj.compare("mu")==0||obj.compare("ph")==0)) continue;   // isol only for e/ph/mu
           std::string key = labels_dir[obj] + "/" + obj + "_" + lev + "_" + var;
           std::string labels = ";"+labels_levels[lev]+" "+labels_objects[obj]+" "+cfg_hist_labels[var][0]+";"+cfg_hist_labels[var][1];
           ATH_CHECK( book(TH1D(key.c_str(), labels.c_str(), cfg_hist_nbins[var], cfg_hist_minmax[var][0], cfg_hist_minmax[var][1])) );
           m_hists[key] = hist(key);
           ATH_MSG_INFO("Defined histogram: " << key.c_str() << ", " << m_hists[key]);
        }
     }
  }

  // Trigger histograms
  for (std::string obj : {"el","mu","ph"}) {
     for (auto trg : m_triggers[obj]) {
        for (std::string var : {"pt","eta","phi"} ) {
           std::string key = "Trigger/"+obj+"_"+var+"_"+trg;
           std::string labels = ";"+labels_objects[obj]+" "+cfg_hist_labels[var][0]+";Efficiency "+trg;
           m_heffs[key] = new TEfficiency(key.c_str(), labels.c_str(), cfg_hist_nbins[var], cfg_hist_minmax[var][0], cfg_hist_minmax[var][1]);
           histogramWorker()->addOutput(m_heffs[key]);
           ATH_MSG_INFO("Defined histogram: " << key.c_str() << ", " << m_heffs[key]);
        }
     }
  }
  ATH_CHECK( book(TH1D("Trigger/el_trigmatch_eff_nominal", "Electron Trigger Matching Efficiency (Nominal);Electron Trigger Matching Efficiency (Nominal);N", getSize(m_triggers,"el"), 0, getSize(m_triggers,"el")) ) );
  ATH_CHECK( book(TH1D("Trigger/ph_trigmatch_eff_nominal", "Photon Trigger Matching Efficiency (Nominal);Photon Trigger Matching Efficiency (Nominal);N", getSize(m_triggers,"ph"), 0, getSize(m_triggers,"ph")) ) );
  ATH_CHECK( book(TH1D("Trigger/mu_trigmatch_eff_nominal", "Muon Trigger Matching Efficiency (Nominal);Muon Trigger Matching Efficiency (Nominal);N", getSize(m_triggers,"mu"), 0, getSize(m_triggers,"mu")) ) );

  // Extra histograms
  ATH_CHECK( book(TH1D("Cutflow/el_n_flow_nominal", "Electron Cutflow (Nominal);Electron Cutflow (Nominal);N", Cut::NSel, 0, Cut::NSel) ) );
  ATH_CHECK( book(TH1D("Cutflow/ph_n_flow_nominal", "Photon Cutflow (Nominal);Photon Cutflow (Nominal);N", Cut::NSel, 0, Cut::NSel) ) );
  ATH_CHECK( book(TH1D("Cutflow/mu_n_flow_nominal", "Muon Cutflow (Nominal);Muon Cutflow (Nominal);N", Cut::NSel, 0, Cut::NSel) ) );
  ATH_CHECK( book(TH1D("Cutflow/jet_n_flow_nominal", "Jet Cutflow (Nominal);Jet Cutflow (Nominal);N", Cut::NSel, 0, Cut::NSel) ) );
  ATH_CHECK( book(TH1D("Cutflow/bjet_n_flow_nominal", "b-jet Cutflow (Nominal);b-jet Cutflow (Nominal);N", Cut::NSel, 0, Cut::NSel) ) );
  ATH_CHECK( book(TH1D("Cutflow/fatjet_n_flow_nominal", "Large R. Jet Cutflow (Nominal);Large R. Jet Cutflow (Nominal);N", Cut::NSel, 0, Cut::NSel) ) );
  ATH_CHECK( book(TH1D("Cutflow/trkjet_n_flow_nominal", "Track Jet Cutflow (Nominal);Track Jet Cutflow (Nominal);N", Cut::NSel, 0, Cut::NSel) ) );
  ATH_CHECK( book(TH1D("Cutflow/tau_n_flow_nominal", "Tau Cutflow (Nominal);Tau Cutflow (Nominal);N", Cut::NSel, 0, Cut::NSel) ) );

  ATH_CHECK( book(TH1D("MET/met_et", "MET (Nominal);MET (Nominal) [GeV];N / [10 GeV]", 50, 0, 500) ) ); //MET (+Components)
  ATH_CHECK( book(TH1D("MET/met_phi", "MET_phi (Nominal);MET_phi (Nominal);N", 50, -5, 5) ) );
  ATH_CHECK( book(TH1D("MET/met_sumet", "MET_sumet (Nominal);MET_sumet (Nominal) [GeV];N / [10 GeV]", 50, 0, 500) ) );
  ATH_CHECK( book(TH1D("MET/met_et_tst", "MET [PVSoftTrk] (Nominal);MET [PVSoftTrk] (Nominal) [GeV];N / [10 GeV]", 50, 0, 500) ) );
  ATH_CHECK( book(TH1D("MET/met_et_el", "MET [RefEle] (Nominal);MET [RefEle] (Nominal) [GeV];N / [10 GeV]", 50, 0, 500) ) );
  ATH_CHECK( book(TH1D("MET/met_et_ph", "MET [RefGamma] (Nominal);MET [RefGamma] (Nominal) [GeV];N / [10 GeV]", 50, 0, 500) ) );
  ATH_CHECK( book(TH1D("MET/met_et_mu", "MET [Muons] (Nominal);MET [Muons] (Nominal) [GeV];N / [10 GeV]", 50, 0, 500) ) );
  ATH_CHECK( book(TH1D("MET/met_et_jet", "MET [RefJet] (Nominal);MET [RefJet] (Nominal) [GeV];N / [10 GeV]", 50, 0, 500) ) );
  //ATH_CHECK( book(TH1D("MET/met_et_tau", "MET [RefTau] (Nominal);MET [RefTau] (Nominal) [GeV];N / [10 GeV]", 50, 0, 500) ) );
  ATH_CHECK( book(TH1D("MET/met_significance", "MET Significance;MET Significance;N / [0.5]", 60, 0, 30) ) );

  ATH_CHECK( book(TH1D("Syst/weight_event",     "Event weights (Nom+Systematics) [MC*PRW];Event weights (Nom+Systematics) [MC*PRW];weight",         getSize(syst_weights,"EventWeight"), 0, getSize(syst_weights,"EventWeight"))) ); //weights
  ATH_CHECK( book(TH1D("Syst/weight_electrons", "Electron total weights (Nom+Systematics);Electron total weights (Nom+Systematics);weight",         getSize(syst_weights,"Electron")   , 0, getSize(syst_weights,"Electron")   )) ); //weights
  ATH_CHECK( book(TH1D("Syst/weight_muons",     "Muon total weights (Nom+Systematics);Muon total weights (Nom+Systematics);weight",                 getSize(syst_weights,"Muon")       , 0, getSize(syst_weights,"Muon")       )) ); //weights
  ATH_CHECK( book(TH1D("Syst/weight_photons",   "Photon total weights (Nom+Systematics);Photon total weights (Nom+Systematics);weight",             getSize(syst_weights,"Photon")     , 0, getSize(syst_weights,"Photon")     )) ); //weights
  ATH_CHECK( book(TH1D("Syst/weight_taus",      "Tau total weights (Nom+Systematics);Tau total weights (Nom+Systematics);weight",                   getSize(syst_weights,"Tau")        , 0, getSize(syst_weights,"Tau")        )) ); //weights
  ATH_CHECK( book(TH1D("Syst/weight_jets",      "Jet total weights (Nom+Systematics);Jet total weights (Nom+Systematics);weight",                   getSize(syst_weights,"Jet")        , 0, getSize(syst_weights,"Jet")        )) ); //weights
  //ATH_CHECK( book(TH1D("Syst/weight_fatjets",   "Large R. jet total weights (Nom+Systematics);Large R. jet total weights (Nom+Systematics);weight", getSize(syst_weights,"Jet")        , 0, getSize(syst_weights,"Jet")        )) ); //weights
  //ATH_CHECK( book(TH1D("Syst/weight_trkjets",   "Track jet total weights (Nom+Systematics);Track jet total weights (Nom+Systematics);weight",       getSize(syst_weights,"Jet")        , 0, getSize(syst_weights,"Jet")        )) ); //weights
  ATH_CHECK( book(TH1D("Syst/weight_btags",     "Btagging total weights (Nom+Systematics);Btagging total weights (Nom+Systematics);weight",         getSize(syst_weights,"BTag")       , 0, getSize(syst_weights,"BTag")       )) ); //weights

  ATH_CHECK( book(TH2D("Syst/syst_event",      "Systematics list [MC+PRW];;Syst name;",  1,0,1, static_cast<int>(std::ceil(1.2*getSize(syst_all,"EventWeight"))),0,static_cast<int>(std::ceil(1.2*getSize(syst_all,"EventWeight"))))) ); // syst names
  ATH_CHECK( book(TH2D("Syst/syst_electrons",  "Systematics list [Electron];Syst name;", 1,0,1, static_cast<int>(std::ceil(1.2*getSize(syst_all,"Electron"))),   0,static_cast<int>(std::ceil(1.2*getSize(syst_all,"Electron"))))) ); // syst names
  ATH_CHECK( book(TH2D("Syst/syst_muons",      "Systematics list [Muon];Syst name;",     1,0,1, static_cast<int>(std::ceil(1.2*getSize(syst_all,"Muon"))),       0,static_cast<int>(std::ceil(1.2*getSize(syst_all,"Muon"))))) ); // syst names
  ATH_CHECK( book(TH2D("Syst/syst_photons",    "Systematics list [Photon];Syst name;",   1,0,1, static_cast<int>(std::ceil(1.2*getSize(syst_all,"Photon"))),     0,static_cast<int>(std::ceil(1.2*getSize(syst_all,"Photon"))))) ); // syst names
  ATH_CHECK( book(TH2D("Syst/syst_taus",       "Systematics list [Tau];Syst name;",      1,0,1, static_cast<int>(std::ceil(1.2*getSize(syst_all,"Tau"))),        0,static_cast<int>(std::ceil(1.2*getSize(syst_all,"Tau"))))) ); // syst names
  ATH_CHECK( book(TH2D("Syst/syst_jets",       "Systematics list [Jet];Syst name;",      1,0,1, static_cast<int>(std::ceil(1.2*getSize(syst_all,"Jet"))),        0,static_cast<int>(std::ceil(1.2*getSize(syst_all,"Jet"))))) ); // syst names
  ATH_CHECK( book(TH2D("Syst/syst_btags",      "Systematics list [BTag];Syst name;",     1,0,1, static_cast<int>(std::ceil(1.2*getSize(syst_all,"BTag"))),       0,static_cast<int>(std::ceil(1.2*getSize(syst_all,"BTag"))))) ); // syst names

  return StatusCode::SUCCESS;
}

//====================================================================================================
// collect systematics per object (all / weight-only)
void SUSYToolsAlg::groupSysts(void) {
  syst_all.clear();
  syst_weights.clear();
  for (const auto& sysInfo : sysInfoList) {
    std::string sys_name = sysInfo.systset.name();
    std::string sys_affects = ST::testAffectsObject(sysInfo.affectsType);
    ATH_MSG_DEBUG("Syst " << sys_name << " affects " << sys_affects);

    bool sys_affects_weights = sysInfo.affectsWeights;

    // collect all syst names
    // per affected object
    if (syst_all.find(sys_affects) == syst_all.end()) syst_all[sys_affects] = {"Nominal"};
    syst_all[sys_affects].push_back(sys_name);

    // weight related syst
    if (sys_affects_weights) {
       if (syst_weights.find(sys_affects) == syst_weights.end()) syst_weights[sys_affects] = {"Nominal"};
       syst_weights[sys_affects].push_back(sys_name);
    }
  }
}

//====================================================================================================
// Fill object histograms consistently for different selection levels / object types
void SUSYToolsAlg::stdHistsForObj(xAOD::IParticle *obj, std::string objtype, std::string objlevel, std::map<std::string,std::string> config) {
   std::map<std::string,std::string> labels_dir = { {"el","Electron"}, {"ph","Photon"}, {"mu","Muon"}, {"jet","Jet"}, {"bjet","bJet"}, {"tau","Tau"}, {"fatjet","LargeRJet"}, {"trkjet","TrackJet"} };
   std::string dir = labels_dir[objtype]+"/";

   ATH_MSG_VERBOSE("Filling histograms for " << obj->type() << ", " << objlevel);
   hist(dir+objtype+"_"+objlevel+"_pt")->Fill( obj->pt()/1000. );
   hist(dir+objtype+"_"+objlevel+"_eta")->Fill( obj->eta() );
   hist(dir+objtype+"_"+objlevel+"_phi")->Fill( obj->phi() );
   hist(dir+objtype+"_"+objlevel+"_e")->Fill( obj->e()/1000. );
   hist(dir+objtype+"_"+objlevel+"_mass")->Fill( obj->m()/1000. );
   //
   if (objtype=="el" || objtype=="mu" || objtype=="ph") {
      hist(dir+objtype+"_"+objlevel+"_d0")->Fill( obj->auxdata<float>("d0sig") );
      hist(dir+objtype+"_"+objlevel+"_z0")->Fill( obj->auxdata<float>("z0sinTheta") );
      if (objtype=="el") {
         hist(dir+objtype+"_"+objlevel+"_isolCalo")->Fill( obj->auxdata<float>("topoetcone20")/obj->pt() );
         hist(dir+objtype+"_"+objlevel+"_isolTrack")->Fill( obj->auxdata<float>("ptvarcone20_Nonprompt_All_MaxWeightTTVALooseCone_pt1000")/obj->pt() );
      } else if (objtype=="mu") {
         hist(dir+objtype+"_"+objlevel+"_isolCalo")->Fill( obj->auxdata<float>("topoetcone20")/obj->pt() );
         hist(dir+objtype+"_"+objlevel+"_isolTrack")->Fill( obj->auxdata<float>("ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt1000")/obj->pt() );
      } else if (objtype=="ph") {
         hist(dir+objtype+"_"+objlevel+"_isolCalo")->Fill( (obj->auxdata<float>("topoetcone40")-2450)/obj->pt() );
         hist(dir+objtype+"_"+objlevel+"_isolTrack")->Fill( obj->auxdata<float>("ptcone20")/obj->pt() );
      }
   }
   //
   if (objtype=="el" || objtype=="mu" || objtype=="ph" || objtype=="tau") {
      if ( objtype!="tau" ) {
        //ATH_MSG_DEBUG(objtype << ", " << objlevel << ": " << obj->auxdata<int>("truthType") << ", " << obj->auxdata<int>("truthOrigin"));
        hist(dir+objtype+"_"+objlevel+"_truthType")->Fill( obj->auxdata<int>("truthType") );
        hist(dir+objtype+"_"+objlevel+"_truthOrigin")->Fill( obj->auxdata<int>("truthOrigin") );
      } else {
        bool istruthmatched = (bool)obj->auxdata<char>("IsTruthMatched");
        int pid(0),ppid(0);
        if (istruthmatched && obj->isAvailable<ElementLink<xAOD::TruthParticleContainer>>("truthParticleLink")) {
           auto *tp = *(obj->auxdata<ElementLink<xAOD::TruthParticleContainer>>("truthParticleLink"));
           if (tp) {
             pid = tp->pdgId();
             ppid = (tp->nParents()>0)?tp->parent(0)->pdgId():0;
           }
        }
        hist(dir+objtype+"_"+objlevel+"_pid")->Fill( pid );
        hist(dir+objtype+"_"+objlevel+"_parentpid")->Fill( ppid );
      }
   }
   if (objtype=="tau") {
      hist(dir+objtype+"_"+objlevel+"_nTracks")->Fill( dynamic_cast<xAOD::TauJet*>(obj)->nTracks() );
      hist(dir+objtype+"_"+objlevel+"_nTracksCharged")->Fill( dynamic_cast<xAOD::TauJet*>(obj)->nTracksCharged() );
      hist(dir+objtype+"_"+objlevel+"_nTracksIsolation")->Fill( dynamic_cast<xAOD::TauJet*>(obj)->nTracksIsolation() );
      hist(dir+objtype+"_"+objlevel+"_RNNJetScoreSigTrans")->Fill( dynamic_cast<xAOD::TauJet*>(obj)->auxdata<float>("RNNJetScoreSigTrans") );
   }
   //
   if (objtype=="bjet") {
      hist(dir+objtype+"_"+objlevel+"_bweight")->Fill( obj->auxdata<double>("btag_weight") );
      hist(dir+objtype+"_"+objlevel+"_bweightpb")->Fill( obj->auxdata<float>("btag_dl1pb") );
      hist(dir+objtype+"_"+objlevel+"_bweightpc")->Fill( obj->auxdata<float>("btag_dl1pc") );
      hist(dir+objtype+"_"+objlevel+"_bweightpu")->Fill( obj->auxdata<float>("btag_dl1pu") );
   }
   //
   if (objtype=="fatjet") {
      bool kin = obj->auxdata<bool>(config["ToptaggerKey"]+"_ValidPtRangeLow") && obj->auxdata<bool>(config["ToptaggerKey"]+"_ValidPtRangeHigh") && obj->auxdata<bool>(config["ToptaggerKey"]+"_ValidEtaRange");
      if (config.find("WtaggerKey")!=config.end())   hist(dir+objtype+"_"+objlevel+"_wtagged")  ->Fill( obj->isAvailable<bool>(config["WtaggerKey"]+"_Tagged")   ? kin && obj->auxdata<bool>(config["WtaggerKey"]+"_Tagged")   : -1);
      if (config.find("ZtaggerKey")!=config.end())   hist(dir+objtype+"_"+objlevel+"_ztagged")  ->Fill( obj->isAvailable<bool>(config["ZtaggerKey"]+"_Tagged")   ? kin && obj->auxdata<bool>(config["ZtaggerKey"]+"_Tagged")   : -1);
      if (config.find("ToptaggerKey")!=config.end()) hist(dir+objtype+"_"+objlevel+"_toptagged")->Fill( obj->isAvailable<bool>(config["ToptaggerKey"]+"_Tagged") ? kin && obj->auxdata<bool>(config["ToptaggerKey"]+"_Tagged") : -1);
      if (config.find("ToptaggerKey")!=config.end()) hist(dir+objtype+"_"+objlevel+"_toptaggedscore")->Fill( obj->isAvailable<float>(config["ToptaggerKey"]+"_Score") ? obj->auxdata<float>(config["ToptaggerKey"]+"_Score") : -1);
   }
}


//====================================================================================================
std::string findInReg(SG::AuxTypeRegistry& reg, xAOD::IParticle* obj, std::string searchkey) {
   std::regex re_tag(searchkey);
   std::smatch matches;
   for (auto x : obj->getAuxIDs()) {
      std::string xname = reg.getName(x);
      std::regex_search(xname, matches, re_tag);
      if (matches.size()>1 and !matches[1].str().empty()) {
         return matches[1].str();
      }
   }
   return "";
}

//====================================================================================================
int getSize(std::map<std::string,std::vector<std::string>> &collection, std::string object) {
   if (collection.find(object) == collection.end()) return 1;
   return static_cast<int>(collection[object].size());
}
