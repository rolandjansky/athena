/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "WZSkims/WZSkimsMain.h"
#include "GaudiKernel/MsgStream.h"

/////////////////////////////////////////////////////////////////////////////

WZSkimsMain::WZSkimsMain(const std::string& name, ISvcLocator* pSvcLocator) : AthFilterAlgorithm(name, pSvcLocator)
{
  declareProperty("doJpsieeSkim"           , m_doJpsieeSkim           = false);
  declareProperty("doJpsimumuSkim"         , m_doJpsimumuSkim         = false);
  declareProperty("doZeeSkim"              , m_doZeeSkim              = false);
  declareProperty("doZmumuSkim"            , m_doZmumuSkim            = false);
  declareProperty("doWenuSkim"             , m_doWenuSkim             = false);
  declareProperty("doWmunuSkim"            , m_doWmunuSkim            = false);
  declareProperty("doWenujjSkim"           , m_doWenujjSkim           = false);
  declareProperty("doWmunujjSkim"          , m_doWmunujjSkim          = false);
  declareProperty("doZtautauSkim"          , m_doZtautauSkim          = false);
  declareProperty("doTagAndProbeElecSkim"  , m_doTagAndProbeElecSkim  = false);
  declareProperty("doTagAndProbeMuonSkim"  , m_doTagAndProbeMuonSkim  = false);
  declareProperty("doBackgroundElecSkim"   , m_doBackgroundElecSkim   = false);
  declareProperty("doBackgroundMuonSkim"   , m_doBackgroundMuonSkim   = false);
  declareProperty("doZeeWithTagAndProbeSkim"   , m_doZeeWithTagAndProbeSkim   = false);
  declareProperty("doZmumuWithTagAndProbeSkim" , m_doZmumuWithTagAndProbeSkim = false);
  declareProperty("doTwoLeptonsSkim"       , m_doTwoLeptonsSkim       = false);
  declareProperty("doThreeLeptonsSkim"     , m_doThreeLeptonsSkim     = false);
  declareProperty("doCustomSkim"           , m_doCustomSkim           = false);
  
  declareProperty("electronContainerName"  , m_electronContainerName  = "ElectronAODCollection");
  declareProperty("muonContainerName"      , m_muonContainerName      = "BOTH");
  declareProperty("trackContainerName"     , m_trackContainerName     = "TrackParticleCandidate");
  declareProperty("missingEtObjectName"    , m_missingEtObjectName    = "ALL");
  declareProperty("jetContainerName"       , m_jetContainerName       = "AntiKt4TopoEMJets");

  declareProperty("electronNumber"         , m_electronNumber         = 0);
  declareProperty("electronNumber2"        , m_electronNumber2        = 0);
  declareProperty("electronPtCut"          , m_electronPtCut          = 0.);
  declareProperty("electronPtCut2"         , m_electronPtCut2         = 0.);
  declareProperty("electronEtaCut"         , m_electronEtaCut         = 0.);
  declareProperty("electronEtaCut2"        , m_electronEtaCut2        = 0.);
  declareProperty("electronType"           , m_electronType           = "NOCHECK");
  declareProperty("electronType2"          , m_electronType2          = "NOCHECK");
 
  declareProperty("muonNumber"             , m_muonNumber             = 0);
  declareProperty("muonNumber2"            , m_muonNumber2            = 0);
  declareProperty("muonPtCut"              , m_muonPtCut              = 0.);
  declareProperty("muonPtCut2"             , m_muonPtCut2             = 0.);
  declareProperty("muonEtaCut"             , m_muonEtaCut             = 0.);
  declareProperty("muonEtaCut2"            , m_muonEtaCut2            = 0.);
  declareProperty("muonType"               , m_muonType               = "NOCHECK");
  declareProperty("muonType2"              , m_muonType2              = "NOCHECK");

  declareProperty("jetNumber"              , m_jetNumber              = 0);
  declareProperty("jetPtCut"               , m_jetPtCut               = 0.);
  declareProperty("jetEtaCut"              , m_jetEtaCut              = 0.);
  declareProperty("jetOverlapRemovalWithTight", m_jetOverlapRemovalWithTight = false);
  declareProperty("jetOverlapRemovalWithMedium", m_jetOverlapRemovalWithMedium = false);
  declareProperty("jetOverlapRemovalWithMediumIfNoTight", m_jetOverlapRemovalWithMediumIfNoTight = false);
  declareProperty("jetOverlapRemovalEtaRange", m_jetOverlapEtaRange);
  declareProperty("jetOverlapRemovalDrCut",    m_jetOverlapDrCut      = 0.);

  declareProperty("doTrackCut"             , m_doTrackCut             = false);
  declareProperty("TrackPtCut"             , m_TrackPtCut             = 0.);
  declareProperty("TrackEtaCut"            , m_TrackEtaCut            = 0.);
 
  declareProperty("doEtmissCut"            , m_doEtmissCut            = false);
  declareProperty("EtmissCut"              , m_EtmissCut              = 0.);

  declareProperty("doInvMassCut"           , m_doInvMassCut           = false);
  declareProperty("Mmin"                   , m_Mmin                   = 0.);
  declareProperty("Mmax"                   , m_Mmax                   = 1000000.0);

  declareProperty("doTriggerCut"           , m_doTriggerCut           = false);
  declareProperty("TriggerName"            , m_triggerName            = "");
  declareProperty("TriggerName2"           , m_triggerName2           = "");

  declareProperty("doPrescale"             , m_doPrescale             = false);
  declareProperty("prescaleRate"           , m_prescaleRate           = 1);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode WZSkimsMain::initialize(){

  ATH_MSG_DEBUG ("initialize()");

  //locate the StoreGateSvc and initialize our local ptr
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (!sc.isSuccess() || 0 == m_storeGate) 
    ATH_MSG_ERROR("Could not find StoreGateSvc");

  if ( (m_doWenujjSkim || m_doWmunujjSkim) && m_muonContainerName == "BOTH" ) {
    m_muonContainerName = "StacoMuonCollection";
    ATH_MSG_INFO("For Wlnujj skims changing name of muon container from BOTH to " << m_muonContainerName);
  }
  
  ATH_MSG_DEBUG("** WZSkim input parameters **");

  ATH_MSG_DEBUG("doing Jpsiee Skim : "	         << m_doJpsieeSkim);
  ATH_MSG_DEBUG("doing Jpsimumu Skim : "         << m_doJpsimumuSkim);
  ATH_MSG_DEBUG("doing Zee Skim : "	         << m_doZeeSkim);
  ATH_MSG_DEBUG("doing Zmumu Skim : "            << m_doZmumuSkim);
  ATH_MSG_DEBUG("doing Wenu Skim : "	         << m_doWenuSkim);
  ATH_MSG_DEBUG("doing Wmunu Skim : "	         << m_doWmunuSkim);
  ATH_MSG_DEBUG("doing Wenujj Skim : "	         << m_doWenujjSkim);
  ATH_MSG_DEBUG("doing Wmunujj Skim : "	         << m_doWmunujjSkim);
  ATH_MSG_DEBUG("doing Ztautau Skim : "	         << m_doZtautauSkim);
  ATH_MSG_DEBUG("doing T&P Electron Skim : "     << m_doTagAndProbeElecSkim);
  ATH_MSG_DEBUG("doing T&P Muon Skim : "         << m_doTagAndProbeMuonSkim);
  ATH_MSG_DEBUG("doing backgd Electron Skim : "  << m_doBackgroundElecSkim);
  ATH_MSG_DEBUG("doing backgd Muon Skim : "      << m_doBackgroundMuonSkim);
  ATH_MSG_DEBUG("doing Zee + T&P Elec Skim : "   << m_doZeeWithTagAndProbeSkim);
  ATH_MSG_DEBUG("doing Zmumu + T&P Muon Skim : " << m_doZmumuWithTagAndProbeSkim);
  ATH_MSG_DEBUG("doing TwoLeptons Skim : "	 << m_doTwoLeptonsSkim);
  ATH_MSG_DEBUG("doing ThreeLeptons Skim : "	 << m_doThreeLeptonsSkim);
  ATH_MSG_DEBUG("doing custom Skim : "	         << m_doCustomSkim);

  ATH_MSG_DEBUG("electron container name : "  << m_electronContainerName);
  ATH_MSG_DEBUG("muon container name : "      << m_muonContainerName);
  ATH_MSG_DEBUG("track container name :"      << m_trackContainerName);
  ATH_MSG_DEBUG("missing Et object name : "   << m_missingEtObjectName);
  ATH_MSG_DEBUG("jet container name : "       << m_jetContainerName);

  ATH_MSG_DEBUG("* For custom skim *");
  ATH_MSG_DEBUG("electron number : "	      << m_electronNumber);
  ATH_MSG_DEBUG("electron number 2 : "        << m_electronNumber2);
  ATH_MSG_DEBUG("electron Pt cut : "	      << m_electronPtCut);
  ATH_MSG_DEBUG("electron Pt cut 2 : "	      << m_electronPtCut2);
  ATH_MSG_DEBUG("electron Eta cut : "	      << m_electronEtaCut);
  ATH_MSG_DEBUG("electron Eta cut 2 : "	      << m_electronEtaCut2);
  ATH_MSG_DEBUG("electron Type : " 	      << m_electronType);
  ATH_MSG_DEBUG("electron Type 2 : " 	      << m_electronType2);

  ATH_MSG_DEBUG("muon number : "              << m_muonNumber);
  ATH_MSG_DEBUG("muon number 2 : "            << m_muonNumber2);
  ATH_MSG_DEBUG("muon Pt cut : "	      << m_muonPtCut);
  ATH_MSG_DEBUG("muon Pt cut 2 : "	      << m_muonPtCut2);
  ATH_MSG_DEBUG("muon Eta cut : "	      << m_muonEtaCut);
  ATH_MSG_DEBUG("muon Eta cut 2 : "	      << m_muonEtaCut2);
  ATH_MSG_DEBUG("muon Type : " 	              << m_muonType);
  ATH_MSG_DEBUG("muon Type 2 : " 	      << m_muonType2);

  ATH_MSG_DEBUG("jet number : "               << m_jetNumber);
  ATH_MSG_DEBUG("jet Pt cut : "	              << m_jetPtCut);
  ATH_MSG_DEBUG("jet Eta cut : "	      << m_jetEtaCut);
  ATH_MSG_DEBUG("Overlap removal with tight lepton : " << m_jetOverlapRemovalWithTight);
  ATH_MSG_DEBUG("Overlap removal with medium lepton : " << m_jetOverlapRemovalWithMedium);
  ATH_MSG_DEBUG("Overlap removal with medium if no tight : " << m_jetOverlapRemovalWithMediumIfNoTight);
  ATH_MSG_DEBUG("DR for jet-lepton pverlap removal : " << m_jetOverlapDrCut);
  for (size_t q=0; q<m_jetOverlapEtaRange.size(); ++q) {
    if (q%2) ATH_MSG_DEBUG("Lepton abs(eta) for overlap removal : to " << m_jetOverlapEtaRange[q]);
    else ATH_MSG_DEBUG("Lepton abs(eta) for overlap removal : from " << m_jetOverlapEtaRange[q]);
  }

  ATH_MSG_DEBUG("doing track cut : " 	      << m_doTrackCut);
  ATH_MSG_DEBUG("Track Pt cut : " 	      << m_TrackPtCut);
  ATH_MSG_DEBUG("Track Eta cut : " 	      << m_TrackEtaCut);

  ATH_MSG_DEBUG("doing Etmiss cut : "         << m_doEtmissCut);
  ATH_MSG_DEBUG("Etmiss cut : "               << m_EtmissCut);

  ATH_MSG_DEBUG("doing Inv mass cut : "       << m_doInvMassCut);
  ATH_MSG_DEBUG("Inv mass cut minimum : "     << m_Mmin);
  ATH_MSG_DEBUG("Inv mass cut maximum : "     << m_Mmax);

  ATH_MSG_DEBUG("doing trigger cut : " 	      << m_doTriggerCut);
  ATH_MSG_DEBUG("Trigger type : "	      << m_triggerName);
  ATH_MSG_DEBUG("Trigger type 2 : "	      << m_triggerName2);
  
  ATH_MSG_DEBUG("Prescaling : "               << m_doPrescale);
  ATH_MSG_DEBUG("Prescale rate : "            << m_prescaleRate);

  n_events = 0;
  n_good_events = 0;
  m_prescaleCounter = 0;

  bool CutsOk = CheckCuts();
  if (!CutsOk) {
    ATH_MSG_WARNING("SKIM NOT PERFORMED, all events accepted !");
    m_doSkim = false;
  }
  else {
    m_doSkim = true;
    ATH_MSG_INFO("** WZSkim object containers **");
    ATH_MSG_INFO("electron container name : "  << m_electronContainerName);
    ATH_MSG_INFO("muon container name : "      << m_muonContainerName);
    ATH_MSG_INFO("track container name :"      << m_trackContainerName);
    ATH_MSG_INFO("missing Et object name : "   << m_missingEtObjectName);
    ATH_MSG_INFO("jet container name : "       << m_jetContainerName);
  }
  
  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode WZSkimsMain::execute() {

  n_events++;
  ATH_MSG_DEBUG("*************** Event number " << n_events << " ****************");

  if (!m_doSkim) {
    setFilterPassed(true);
    return StatusCode::SUCCESS;
  }
  
  bool isGoodEvent = false;
  
  if (m_skimVec[Jpsiee]) isGoodEvent = isGoodEvent || performSkim("JpsieeSkim");

  if (m_skimVec[Jpsimumu]) isGoodEvent = isGoodEvent || performSkim("JpsimumuSkim");

  if (m_skimVec[Zee]) isGoodEvent = isGoodEvent || performSkim("ZeeSkim");

  if (m_skimVec[Zmumu]) isGoodEvent = isGoodEvent || performSkim("ZmumuSkim");

  if (m_skimVec[Wenu]) isGoodEvent = isGoodEvent || performSkim("WenuSkim");

  if (m_skimVec[Wmunu]) isGoodEvent = isGoodEvent || performSkim("WmunuSkim");

  if (m_skimVec[Wenujj]) isGoodEvent = isGoodEvent || performSkim("WenujjSkim");

  if (m_skimVec[Wmunujj]) isGoodEvent = isGoodEvent || performSkim("WmunujjSkim");

  if (m_skimVec[TagAndProbeElec]) isGoodEvent = isGoodEvent || performSkim("TagAndProbeElecSkim");

  if (m_skimVec[TagAndProbeMuon]) isGoodEvent = isGoodEvent || performSkim("TagAndProbeMuonSkim");

  if (m_skimVec[BackgroundElec]) isGoodEvent = isGoodEvent || performSkim("BackgroundElecSkim");

  if (m_skimVec[BackgroundMuon]) isGoodEvent = isGoodEvent || performSkim("BackgroundMuonSkim");

  if (m_skimVec[Ztautau]) isGoodEvent = isGoodEvent || performSkim("ZtautauSkim");

  if (m_skimVec[TwoElectrons]) isGoodEvent = isGoodEvent || performSkim("TwoElectronsSkim");

  if (m_skimVec[TwoMuons]) isGoodEvent = isGoodEvent || performSkim("TwoMuonsSkim");

  if (m_skimVec[eee]) isGoodEvent = isGoodEvent || performSkim("eeeSkim");

  if (m_skimVec[eemu]) isGoodEvent = isGoodEvent || performSkim("eemuSkim");

  if (m_skimVec[emumu]) isGoodEvent = isGoodEvent || performSkim("emumuSkim");

  if (m_skimVec[mumumu]) isGoodEvent = isGoodEvent || performSkim("mumumuSkim");

  if (m_skimVec[Custom]) isGoodEvent = isGoodEvent || performSkim("CustomSkim");

  setFilterPassed(isGoodEvent);

  if (isGoodEvent) {
    ATH_MSG_DEBUG("Event accepted");
    n_good_events++;
  }


  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::performSkim(std::string thisSkim = "") {
	
  bool isGoodEvent = false;
  
  bool CutsOK = SetCuts(thisSkim);
  if (!CutsOK) return true;  // If cuts are wrong, event is always kept
  
  bool passElectrons = true, passMuons = true, passJets = true, passTrack = true, passInvMass = true, passEtmiss = true, passTrigger = true, passEvent = true;
  std::vector<struct lepton> leptons;

  if ((m_electronNumber > 0) || (m_electronNumber2 > 0)) {

    std::vector<const Electron*> AllElectronsVector = OpenElectrons();
    
    if (AllElectronsVector.size() > 0) {
      ReadElectrons(AllElectronsVector,leptons);
      std::vector<bool> GoodElectronsFlag1, GoodElectronsFlag2;
  
      GoodElectronsFlag1.clear();
      GoodElectronsFlag2.clear();
  
      if (m_electronNumber > 0) GoodElectronsFlag1 = CheckElectronVector(AllElectronsVector, m_electronPtCut, m_electronEtaCut, m_electronType);
      if (m_electronNumber2 > 0) GoodElectronsFlag2 = CheckElectronVector(AllElectronsVector, m_electronPtCut2, m_electronEtaCut2, m_electronType2);
    
      passElectrons = CheckLeptonCuts(m_electronNumber, m_electronNumber2, GoodElectronsFlag1, GoodElectronsFlag2);

      if (m_doInvMassCut) passInvMass = CheckInvMass(AllElectronsVector, GoodElectronsFlag1, GoodElectronsFlag2);
    }
    else passElectrons = false;

  }

  if ((m_muonNumber > 0) || (m_muonNumber2 > 0)) {

    bool passMuonsStaco = false, passMuonsMuid = false, passInvMassStaco = false, passInvMassMuid = false;
    std::vector<const Analysis::Muon*> AllMuonsVector;
    
    if (m_muonContainerName == "BOTH" || m_muonContainerName == "StacoMuonCollection") { 
    
      AllMuonsVector = OpenMuons("StacoMuonCollection");
    
      if (AllMuonsVector.size() > 0) {
        ReadMuons(AllMuonsVector,leptons);
        std::vector<bool> GoodMuonsFlag1, GoodMuonsFlag2;
  
        GoodMuonsFlag1.clear();
        GoodMuonsFlag2.clear();
  
        if (m_muonNumber > 0) GoodMuonsFlag1 = CheckMuonVector(AllMuonsVector, m_muonPtCut, m_muonEtaCut, m_muonType);
        if (m_muonNumber2 > 0) GoodMuonsFlag2 = CheckMuonVector(AllMuonsVector, m_muonPtCut2, m_muonEtaCut2, m_muonType2);
    
        passMuonsStaco = CheckLeptonCuts(m_muonNumber, m_muonNumber2, GoodMuonsFlag1, GoodMuonsFlag2);

        if (m_doInvMassCut) passInvMassStaco = CheckInvMass(AllMuonsVector, GoodMuonsFlag1, GoodMuonsFlag2);
      }
      else passMuonsStaco = false;
      AllMuonsVector.clear();
    }

    if (m_muonContainerName == "BOTH" || m_muonContainerName == "MuidMuonCollection") {
    
      AllMuonsVector = OpenMuons("MuidMuonCollection");
      
      if (AllMuonsVector.size() > 0) {
        if (m_muonContainerName != "BOTH") ReadMuons(AllMuonsVector,leptons);
        std::vector<bool> GoodMuonsFlag1, GoodMuonsFlag2;
    
        GoodMuonsFlag1.clear();
        GoodMuonsFlag2.clear();
    
        if (m_muonNumber > 0) GoodMuonsFlag1 = CheckMuonVector(AllMuonsVector, m_muonPtCut, m_muonEtaCut, m_muonType);
        if (m_muonNumber2 > 0) GoodMuonsFlag2 = CheckMuonVector(AllMuonsVector, m_muonPtCut2, m_muonEtaCut2, m_muonType2);
      
        passMuonsMuid = CheckLeptonCuts(m_muonNumber, m_muonNumber2, GoodMuonsFlag1, GoodMuonsFlag2);
  
        if (m_doInvMassCut) passInvMassMuid = CheckInvMass(AllMuonsVector, GoodMuonsFlag1, GoodMuonsFlag2);
      }
      else passMuonsMuid = false;
      AllMuonsVector.clear();
    }
    
    passMuons = passMuonsStaco || passMuonsMuid;
    if (m_doInvMassCut) passInvMass = passInvMassStaco || passInvMassMuid;
    
  }

  if (m_jetNumber > 0) {

    std::vector<const Jet*> jets = OpenJets();
    
    passJets = (jets.size() >= leptons.size()+m_jetNumber || CheckJetLeptonOverlap(jets, leptons));
    //passJets = CheckJetLeptonOverlap(jets, leptons); // use this line to see DEBUG messages for all events

  }
 
  if (m_doTrackCut) passTrack = SelectTrack();

  if (m_doEtmissCut) {
    if (m_missingEtObjectName == "ALL") { passEtmiss = SelectEtmiss(); }
    else { passEtmiss = SelectEtmiss(m_missingEtObjectName); }
  }

  if (m_doTriggerCut) passTrigger = SelectTrigger();

  //if (m_doGlobalCut) passEvent = SelectEvent();

  isGoodEvent = passElectrons && passMuons && passJets && passInvMass && passTrack && passEtmiss && passTrigger && passEvent;

  if (m_doPrescale && isGoodEvent) isGoodEvent = Prescale();
  
  return isGoodEvent;  
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode WZSkimsMain::finalize() {
    
    ATH_MSG_DEBUG ("finalize()");

    ATH_MSG_INFO ("Skimmable events " << n_events);
    ATH_MSG_INFO ("Skimmed events " << n_good_events);

    
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::CheckCuts() {
  
  for (int i = 0; i < 20; i++) m_skimVec[i] = false;
  
  int check_skims = m_doJpsieeSkim + m_doJpsimumuSkim + m_doZeeSkim + m_doZmumuSkim + m_doWenuSkim + m_doWmunuSkim + m_doTagAndProbeElecSkim + m_doTagAndProbeMuonSkim
                  + m_doBackgroundElecSkim + m_doBackgroundMuonSkim + m_doZtautauSkim + m_doZeeWithTagAndProbeSkim + m_doZmumuWithTagAndProbeSkim
		  + m_doTwoLeptonsSkim + m_doThreeLeptonsSkim + m_doWenujjSkim + m_doWmunujjSkim + m_doCustomSkim;

  ATH_MSG_DEBUG ("Number of active skims: " << check_skims);
  
  if (check_skims > 1) {
    ATH_MSG_WARNING ("More than one active skim, will not skim. Please check the job options file");
    return false;
  }
  
  if (check_skims == 0) {
    ATH_MSG_WARNING ("No skim active, will not skim. Please check the job options file");
    return false;  
  }
  
  m_skimVec[Jpsiee] = m_doJpsieeSkim;
  m_skimVec[Jpsimumu] = m_doJpsimumuSkim;
  m_skimVec[Zee] = m_doZeeSkim;
  m_skimVec[Zmumu] = m_doZmumuSkim;
  m_skimVec[Wenu] = m_doWenuSkim;
  m_skimVec[Wmunu] = m_doWmunuSkim;
  m_skimVec[Wenujj] = m_doWenujjSkim;
  m_skimVec[Wmunujj] = m_doWmunujjSkim;
  m_skimVec[TagAndProbeElec] = m_doTagAndProbeElecSkim;
  m_skimVec[TagAndProbeMuon] = m_doTagAndProbeMuonSkim;
  m_skimVec[BackgroundElec] = m_doBackgroundElecSkim;
  m_skimVec[BackgroundMuon] = m_doBackgroundMuonSkim;
  m_skimVec[Ztautau] = m_doZtautauSkim;
  m_skimVec[Custom] = m_doCustomSkim;
  
  if (m_doZeeWithTagAndProbeSkim) {
    m_skimVec[Zee] = true;
    m_skimVec[TagAndProbeElec] = true;  
  }
  if (m_doZmumuWithTagAndProbeSkim) {
    m_skimVec[Zmumu] = true;
    m_skimVec[TagAndProbeMuon] = true;  
  }
  if (m_doTwoLeptonsSkim) {
    m_skimVec[Ztautau] = true;
    m_skimVec[TwoElectrons] = true;  
    m_skimVec[TwoMuons] = true;  
  }
  if (m_doThreeLeptonsSkim) {
    m_skimVec[eee] = true;
    m_skimVec[eemu] = true;  
    m_skimVec[emumu] = true;  
    m_skimVec[mumumu] = true;  
  }

  if (m_skimVec[Jpsiee]) 		ATH_MSG_INFO ("**** Doing Jpsiee skim ****");
  if (m_skimVec[Jpsimumu]) 		ATH_MSG_INFO ("**** Doing Jpsimumu skim ****");
  if (m_skimVec[Zee]) 			ATH_MSG_INFO ("**** Doing Zee skim ****");
  if (m_skimVec[Zmumu]) 		ATH_MSG_INFO ("**** Doing Zmumu skim ****");
  if (m_skimVec[Wenu])		 	ATH_MSG_INFO ("**** Doing Wenu skim ****");
  if (m_skimVec[Wmunu]) 		ATH_MSG_INFO ("**** Doing Wmunu skim ****");
  if (m_skimVec[Wenujj])	 	ATH_MSG_INFO ("**** Doing Wenujj skim ****");
  if (m_skimVec[Wmunujj]) 		ATH_MSG_INFO ("**** Doing Wmunujj skim ****");
  if (m_skimVec[TagAndProbeElec]) 	ATH_MSG_INFO ("**** Doing TagAndProbeElec skim ****");
  if (m_skimVec[TagAndProbeMuon]) 	ATH_MSG_INFO ("**** Doing TagAndProbeMuon skim ****");
  if (m_skimVec[BackgroundElec]) 	ATH_MSG_INFO ("**** Doing BackgroundElec skim ****");
  if (m_skimVec[BackgroundMuon]) 	ATH_MSG_INFO ("**** Doing BackgroundMuon skim ****");
  if (m_skimVec[Ztautau]) 		ATH_MSG_INFO ("**** Doing Ztautau skim ****");
  if (m_skimVec[TwoElectrons]) 		ATH_MSG_INFO ("**** Doing TwoElectrons skim ****");
  if (m_skimVec[TwoMuons]) 		ATH_MSG_INFO ("**** Doing TwoMuons skim ****");
  if (m_skimVec[eee]) 			ATH_MSG_INFO ("**** Doing eee skim ****");
  if (m_skimVec[eemu]) 			ATH_MSG_INFO ("**** Doing eemu skim ****");
  if (m_skimVec[emumu]) 		ATH_MSG_INFO ("**** Doing emumu skim ****");
  if (m_skimVec[mumumu]) 		ATH_MSG_INFO ("**** Doing mumumu skim ****");
  if (m_skimVec[Custom]) 		ATH_MSG_INFO ("**** Doing Custom skim ****");

  return true;

}
  
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::SetCuts(std::string thisSkim = "") {
  
  // clean all jet variables, since they are dummy most of the time
  if (thisSkim != "CustomSkim") {
    m_jetNumber = 0;
    m_jetPtCut = 0.;
    m_jetEtaCut = 0.;
    m_jetOverlapRemovalWithTight = false;
    m_jetOverlapRemovalWithMedium = false;
    m_jetOverlapRemovalWithMediumIfNoTight = false;
    m_jetOverlapEtaRange.clear();
    m_jetOverlapDrCut = 0.;
  }
  
  if (thisSkim == "JpsieeSkim") {
    ATH_MSG_VERBOSE ("**** Doing Jpsiee skim ****");
    m_electronNumber = 2;
    m_electronNumber2 = 0;
    m_electronPtCut = 4000.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 9.9;
    m_electronType = "MEDIUM";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 0;
    m_muonNumber2 = 0;
    m_muonPtCut = 0.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "NOCHECK";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
   
  if (thisSkim == "ZeeSkim") {
    ATH_MSG_VERBOSE ("**** Doing Zee skim ****");
    m_electronNumber = 2;
    m_electronNumber2 = 0;
    m_electronPtCut = 20000.;
    m_electronPtCut2 = 20000.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 9.9;
    m_electronType = "LOOSE";
    m_electronType2 = "LOOSE";
    m_muonNumber = 0;
    m_muonNumber2 = 0;
    m_muonPtCut = 0.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "NOCHECK";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 60000.0;
    m_Mmax = 120000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
  
  if (thisSkim == "WenuSkim") {
    ATH_MSG_VERBOSE ("**** Doing Wenu skim ****");
    m_electronNumber = 1;
    m_electronNumber2 = 0;
    m_electronPtCut = 20000.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 9.9;
    m_electronType = "MEDIUM";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 0;
    m_muonNumber2 = 0;
    m_muonPtCut = 0.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "NOCHECK";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = true;
    m_EtmissCut = 20000.0; 
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
  
  if (thisSkim == "WenujjSkim") {
    ATH_MSG_VERBOSE ("**** Doing Wenujj skim ****");
    m_electronNumber = 1;
    m_electronNumber2 = 0;
    m_electronPtCut = 17000.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 9.9;
    m_electronType = "MEDIUM";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 0;
    m_muonNumber2 = 0;
    m_muonPtCut = 0.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "NOCHECK";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 150000.0; // use it for high-pt jet cut
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
    m_jetNumber = 2;
    m_jetPtCut = 20000.;
    m_jetEtaCut = 2.8;
    m_jetOverlapRemovalWithTight = false;
    m_jetOverlapRemovalWithMedium = true;
    m_jetOverlapRemovalWithMediumIfNoTight = false;
    m_jetOverlapEtaRange.push_back(-0.001);
    m_jetOverlapEtaRange.push_back(1.37);
    m_jetOverlapEtaRange.push_back(1.52);
    m_jetOverlapEtaRange.push_back(2.47);
    m_jetOverlapDrCut = 0.2;
  }
  
  if (thisSkim == "TagAndProbeElecSkim") {
    ATH_MSG_VERBOSE ("**** Doing Tag and Probe skim, electrons ****");
    m_electronNumber = 1;
    m_electronNumber2 = 1;
    m_electronPtCut = 25000.;
    m_electronPtCut2 = 15000.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 9.9;
    m_electronType = "MEDIUM";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 0;
    m_muonNumber2 = 0;
    m_muonPtCut = 0.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "NOCHECK";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
  
  if (thisSkim == "BackgroundElecSkim") {
    ATH_MSG_VERBOSE ("**** Doing Background skim, electrons ****");
    m_electronNumber = 0;
    m_electronNumber2 = 0;
    m_electronPtCut = 0.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 0.0;
    m_electronEtaCut2 = 0.0;
    m_electronType = "NOCHECK";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 0;
    m_muonNumber2 = 0;
    m_muonPtCut = 0.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "NOCHECK";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = true;
    m_triggerName = "EF_g20_etcut||EF_g20_loose";
    m_triggerName2 = "EF_e20_loose||EF_e20_loose1||EF_e20_looseTrk||EF_e22_loose||EF_e22_loose1||EF_e22_looseTrk||EF_e22vh_loose||EF_e22vh_loose1||EF_e22vh_looseTrk";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
  
  if (thisSkim == "JpsimumuSkim") {
    ATH_MSG_VERBOSE ("**** Doing Jpsimumu skim ****");
    m_electronNumber = 0;
    m_electronNumber2 = 0;
    m_electronPtCut = 0.0;
    m_electronPtCut2 = 0.0;
    m_electronEtaCut = 0.0;
    m_electronEtaCut2 = 0.0;
    m_electronType = "NOCHECK";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 2;
    m_muonNumber2 = 0;
    m_muonPtCut = 6000.;
    m_muonPtCut2 = 0.;
    m_muonEtaCut = 9.9;
    m_muonEtaCut2 = 9.9;
    m_muonType = "TIGHT";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
  
  if (thisSkim == "ZmumuSkim") {
    ATH_MSG_VERBOSE ("**** Doing Zmumu skim ****");
    m_electronNumber = 0;
    m_electronNumber2 = 0;
    m_electronPtCut = 0.0;
    m_electronPtCut2 = 0.0;
    m_electronEtaCut = 0.0;
    m_electronEtaCut2 = 0.0;
    m_electronType = "NOCHECK";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 2;
    m_muonNumber2 = 0;
    m_muonPtCut = 20000.;
    m_muonPtCut2 = 0.;
    m_muonEtaCut = 9.9;
    m_muonEtaCut2 = 9.9;
    m_muonType = "LOOSE";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 60000.0;
    m_Mmax = 120000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  } 
  
  if (thisSkim == "WmunuSkim") {
    ATH_MSG_VERBOSE ("**** Doing Wmunu skim ****");
    m_electronNumber = 0;
    m_electronNumber2 = 0;
    m_electronPtCut = 0.0;
    m_electronPtCut2 = 0.0;
    m_electronEtaCut = 0.0;
    m_electronEtaCut2 = 0.0;
    m_electronType = "NOCHECK";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 1;
    m_muonNumber2 = 0;
    m_muonPtCut = 20000.;
    m_muonPtCut2 = 0.;
    m_muonEtaCut = 9.9;
    m_muonEtaCut2 = 9.9;
    m_muonType = "TIGHT";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = true;
    m_EtmissCut = 20000.0; 
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }

  if (thisSkim == "WmunujjSkim") {
    ATH_MSG_VERBOSE ("**** Doing Wmunujj skim ****");
    m_electronNumber = 0;
    m_electronNumber2 = 0;
    m_electronPtCut = 0.0;
    m_electronPtCut2 = 0.0;
    m_electronEtaCut = 0.0;
    m_electronEtaCut2 = 0.0;
    m_electronType = "NOCHECK";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 1;
    m_muonNumber2 = 0;
    m_muonPtCut = 17000.;
    m_muonPtCut2 = 0.;
    m_muonEtaCut = 9.9;
    m_muonEtaCut2 = 9.9;
    m_muonType = "TIGHT";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 150000.0; // use it for high-pt jet cut
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
    m_jetNumber = 2;
    m_jetPtCut = 20000.;
    m_jetEtaCut = 2.8;
    m_jetOverlapRemovalWithTight = true;
    m_jetOverlapRemovalWithMedium = false;
    m_jetOverlapRemovalWithMediumIfNoTight = false;
    m_jetOverlapEtaRange.push_back(-0.001);
    m_jetOverlapEtaRange.push_back(2.4);
    m_jetOverlapDrCut = 0.2;
  }
  
  if (thisSkim == "TagAndProbeMuonSkim") {
    ATH_MSG_VERBOSE ("**** Doing Tag and Probe skim, muons ****");
    m_electronNumber = 0;
    m_electronNumber2 = 0;
    m_electronPtCut = 0.0;
    m_electronPtCut2 = 0.0;
    m_electronEtaCut = 0.0;
    m_electronEtaCut2 = 0.0;
    m_electronType = "NOCHECK";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 1;
    m_muonNumber2 = 0;
    m_muonPtCut = 25000.;
    m_muonPtCut2 = 0.;
    m_muonEtaCut = 9.9;
    m_muonEtaCut2 = 9.9;
    m_muonType = "TIGHT";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = true;
    m_TrackPtCut = 15000.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  } 
  
  if (thisSkim == "BackgroundMuonSkim") {
    ATH_MSG_VERBOSE ("**** Doing Background skim, muons ****");
    m_electronNumber = 0;
    m_electronNumber2 = 0;
    m_electronPtCut = 0.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 0.0;
    m_electronEtaCut2 = 0.0;
    m_electronType = "NOCHECK";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 1;
    m_muonNumber2 = 0;
    m_muonPtCut = 15000.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 9.9;
    m_muonEtaCut2 = 0.0;
    m_muonType = "NOCHECK";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = true;
    m_prescaleRate = 10;
  }
  
  if (thisSkim == "ZtautauSkim") {
    ATH_MSG_VERBOSE ("**** Doing Ztautau skim ****");
    m_electronNumber = 1;
    m_electronNumber2 = 0;
    m_electronPtCut = 10000.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 0.0;
    m_electronType = "MEDIUM";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 1;
    m_muonNumber2 = 0;
    m_muonPtCut = 10000.;
    m_muonPtCut2 = 0.;
    m_muonEtaCut = 9.9;
    m_muonEtaCut2 = 0.0;
    m_muonType = "LOOSE";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
  
  if (thisSkim == "TwoElectronsSkim") {
    ATH_MSG_VERBOSE ("**** Doing TwoElectrons skim ****");
    m_electronNumber = 2;
    m_electronNumber2 = 0;
    m_electronPtCut = 10000.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 0.0;
    m_electronType = "MEDIUM";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 0;
    m_muonNumber2 = 0;
    m_muonPtCut = 0.;
    m_muonPtCut2 = 0.;
    m_muonEtaCut = 9.9;
    m_muonEtaCut2 = 0.0;
    m_muonType = "NOCHECK";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }

  if (thisSkim == "TwoMuonsSkim") {
    ATH_MSG_VERBOSE ("**** Doing TwoMuons skim ****");
    m_electronNumber = 0;
    m_electronNumber2 = 0;
    m_electronPtCut = 0.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 0.0;
    m_electronType = "MEDIUM";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 2;
    m_muonNumber2 = 0;
    m_muonPtCut = 10000.;
    m_muonPtCut2 = 0.;
    m_muonEtaCut = 9.9;
    m_muonEtaCut2 = 0.0;
    m_muonType = "LOOSE";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }

    if (thisSkim == "eeeSkim") {
    ATH_MSG_VERBOSE ("**** Doing eee skim ****");
    m_electronNumber = 3;
    m_electronNumber2 = 0;
    m_electronPtCut = 10000.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 9.9;
    m_electronType = "LOOSE";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 0;
    m_muonNumber2 = 0;
    m_muonPtCut = 10000.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "LOOSE";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
   
  if (thisSkim == "eemuSkim") {
    ATH_MSG_VERBOSE ("**** Doing eemu skim ****");
    m_electronNumber = 2;
    m_electronNumber2 = 0;
    m_electronPtCut = 10000.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 9.9;
    m_electronType = "LOOSE";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 1;
    m_muonNumber2 = 0;
    m_muonPtCut = 10000.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "LOOSE";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
   
  if (thisSkim == "emumuSkim") {
    ATH_MSG_VERBOSE ("**** Doing emumu skim ****");
    m_electronNumber = 1;
    m_electronNumber2 = 0;
    m_electronPtCut = 10000.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 9.9;
    m_electronType = "LOOSE";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 2;
    m_muonNumber2 = 0;
    m_muonPtCut = 10000.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "LOOSE";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }
   
  if (thisSkim == "mumumuSkim") {
    ATH_MSG_VERBOSE ("**** Doing mumumu skim ****");
    m_electronNumber = 0;
    m_electronNumber2 = 0;
    m_electronPtCut = 10000.;
    m_electronPtCut2 = 0.;
    m_electronEtaCut = 9.9;
    m_electronEtaCut2 = 9.9;
    m_electronType = "LOOSE";
    m_electronType2 = "NOCHECK";
    m_muonNumber = 3;
    m_muonNumber2 = 0;
    m_muonPtCut = 10000.0;
    m_muonPtCut2 = 0.0;
    m_muonEtaCut = 0.0;
    m_muonEtaCut2 = 0.0;
    m_muonType = "LOOSE";
    m_muonType2 = "NOCHECK";
    m_doTrackCut = false;
    m_TrackPtCut = 0.0;
    m_TrackEtaCut = 9.9;
    m_doInvMassCut = false;
    m_Mmin = 0.0;
    m_Mmax = 20000000.0;
    m_doEtmissCut = false;
    m_EtmissCut = 0.0;
    m_doTriggerCut = false;
    m_triggerName = "";
    m_triggerName2 = "";
    m_doPrescale = false;
    m_prescaleRate = 1;
  }

  if (thisSkim == "CustomSkim") {
    ATH_MSG_VERBOSE ("**** Doing custom skim ****");
  }
 
  if ( (m_doTriggerCut) && (m_triggerName == "") && (m_triggerName2 == "") ) {
    ATH_MSG_DEBUG("No trigger name selected, will not cut on trigger");
    m_doTriggerCut = false;
  }

  if ( (m_doPrescale) && (m_prescaleRate <= 1) ) {
    ATH_MSG_DEBUG("Improper prescale rate selected (" << m_prescaleRate << "), will not prescale");
    m_doPrescale = false;
  }

  if ( (m_doInvMassCut) && (m_electronNumber == 0) && (m_electronNumber2 == 0) && (m_muonNumber == 0) && (m_muonNumber2 == 0)) {
    ATH_MSG_DEBUG("No cuts selected for leptons, invariant mass cut will have no effect");
    m_doInvMassCut = false;
  }

  if ( (m_doInvMassCut) && ((m_electronNumber > 0) || (m_electronNumber2 > 0)) && ((m_muonNumber > 0) || (m_muonNumber2 > 0)))  {
    ATH_MSG_DEBUG("Electron and muon cuts selected, invariant mass cut will have no effect");
    m_doInvMassCut = false;
  }
 
  if ( (m_electronNumber == 0) && (m_electronNumber2 == 0) && (m_muonNumber == 0) && (m_muonNumber2 == 0) && !m_doTrackCut && !m_doEtmissCut && !m_doTriggerCut && !m_doPrescale) {
    ATH_MSG_DEBUG("No cuts set at all ! Will not skim");
    return false;
  }
  
  ATH_MSG_VERBOSE("**** Skim will be performed with parameters: ****");	
  ATH_MSG_VERBOSE("electron number : "	     << m_electronNumber);
  ATH_MSG_VERBOSE("electron number 2 : "     << m_electronNumber2);
  ATH_MSG_VERBOSE("electron Pt cut : "	     << m_electronPtCut);
  ATH_MSG_VERBOSE("electron Pt cut 2 : "     << m_electronPtCut2);
  ATH_MSG_VERBOSE("electron Eta cut : "	     << m_electronEtaCut);
  ATH_MSG_VERBOSE("electron Eta cut 2 : "    << m_electronEtaCut2);
  ATH_MSG_VERBOSE("electron Type : "	     << m_electronType);
  ATH_MSG_VERBOSE("electron Type 2 : "	     << m_electronType2);

  ATH_MSG_VERBOSE("muon number : " 	     << m_muonNumber);
  ATH_MSG_VERBOSE("muon number 2 : "	     << m_muonNumber2);
  ATH_MSG_VERBOSE("muon Pt cut : " 	     << m_muonPtCut);
  ATH_MSG_VERBOSE("muon Pt cut 2 : "	     << m_muonPtCut2);
  ATH_MSG_VERBOSE("muon Eta cut : "	     << m_muonEtaCut);
  ATH_MSG_VERBOSE("muon Eta cut 2 : "	     << m_muonEtaCut2);
  ATH_MSG_VERBOSE("muon Type : "	     << m_muonType);
  ATH_MSG_VERBOSE("muon Type 2 : " 	     << m_muonType2);

  ATH_MSG_VERBOSE("jet number : "	     << m_jetNumber);
  ATH_MSG_VERBOSE("jet Pt cut : "	     << m_jetPtCut);
  ATH_MSG_VERBOSE("jet Eta cut : "	     << m_jetEtaCut);
  ATH_MSG_VERBOSE("Overlap removal with tight lepton : " << m_jetOverlapRemovalWithTight);
  ATH_MSG_VERBOSE("Overlap removal with medium lepton : " << m_jetOverlapRemovalWithMedium);
  ATH_MSG_VERBOSE("Overlap removal with medium if no tight : " << m_jetOverlapRemovalWithMediumIfNoTight);
  ATH_MSG_VERBOSE("DR for jet-lepton pverlap removal : " << m_jetOverlapDrCut);
  for (size_t q=0; q<m_jetOverlapEtaRange.size(); ++q) {
    if (q%2) ATH_MSG_VERBOSE("Lepton abs(eta) for overlap removal : to " << m_jetOverlapEtaRange[q]);
    else ATH_MSG_VERBOSE("Lepton abs(eta) for overlap removal : from " << m_jetOverlapEtaRange[q]);
  }
  
  ATH_MSG_VERBOSE("doing track cut : "	     << m_doTrackCut);
  ATH_MSG_VERBOSE("Track Pt cut : "	     << m_TrackPtCut);
  ATH_MSG_VERBOSE("Track Eta cut : "	     << m_TrackEtaCut);

  ATH_MSG_VERBOSE("doing Etmiss cut : "	     << m_doEtmissCut);
  ATH_MSG_VERBOSE("Etmiss cut : "  	     << m_EtmissCut);

  ATH_MSG_VERBOSE("doing Inv mass cut : "    << m_doInvMassCut);
  ATH_MSG_VERBOSE("Inv mass cut minimum : "  << m_Mmin);
  ATH_MSG_VERBOSE("Inv mass cut maximum : "  << m_Mmax);

  ATH_MSG_VERBOSE("doing trigger cut : "     << m_doTriggerCut);
  ATH_MSG_VERBOSE("Trigger type : "	     << m_triggerName);
  ATH_MSG_VERBOSE("Trigger type 2 : "	     << m_triggerName2);

  ATH_MSG_VERBOSE("Prescaling : "  	     << m_doPrescale);
  ATH_MSG_VERBOSE("Prescale rate : "	     << m_prescaleRate);
  
  return true;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<const Electron*> WZSkimsMain::OpenElectrons() {
  ATH_MSG_DEBUG("Opening electron container: " << m_electronContainerName);

  std::vector<const Electron*> l_Electrons;
  l_Electrons.clear();

  const ElectronContainer* importedElectronColl;
  StatusCode sc = m_storeGate->retrieve(importedElectronColl, m_electronContainerName);
  if (sc.isFailure() ) {
    ATH_MSG_WARNING("No Electron Collection of type " << m_electronContainerName << " found in StoreGate");
    return l_Electrons;
  }
  else ATH_MSG_DEBUG("You have " << importedElectronColl->size() << " electrons in this event");

  ElectronContainer::const_iterator electronIter;

  for (electronIter=importedElectronColl->begin(); electronIter!=importedElectronColl->end(); ++electronIter) {
    l_Electrons.push_back(*electronIter);
  }

  return l_Electrons;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<const Analysis::Muon*> WZSkimsMain::OpenMuons(std::string l_muonContainerName) {
  ATH_MSG_DEBUG("Opening muon container: " << l_muonContainerName);

  std::vector<const Analysis::Muon*> l_Muons;
  l_Muons.clear();

  const MuonContainer* importedMuonColl;
  StatusCode sc = m_storeGate->retrieve(importedMuonColl, l_muonContainerName);
  if (sc.isFailure() ) {
    ATH_MSG_WARNING("No Muon Collection of type " << l_muonContainerName << " found in StoreGate");
    return l_Muons;
  }
  else ATH_MSG_DEBUG("You have " << importedMuonColl->size() << " muons in this event");

  MuonContainer::const_iterator muonIter;

  for (muonIter=importedMuonColl->begin(); muonIter!=importedMuonColl->end(); ++muonIter) {
    l_Muons.push_back(*muonIter);
  }

  return l_Muons;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<const Jet*> WZSkimsMain::OpenJets() {
  ATH_MSG_DEBUG("Opening jet container: " << m_jetContainerName);

  std::vector<const Jet*> l_Jets;

  const JetCollection* jetcol;
  StatusCode sc = m_storeGate->retrieve(jetcol,m_jetContainerName);
  if (sc.isFailure() ) {
    ATH_MSG_WARNING("No Jet Collection of type " << m_jetContainerName << " found in StoreGate");
    return l_Jets;
  }
  else ATH_MSG_DEBUG("You have " << jetcol->size() << " jets in this event");

  for (JetCollection::const_iterator jetIter=jetcol->begin(); jetIter!=jetcol->end(); ++jetIter) {
    double jetPt = (*jetIter)->pt();
    double jetEta = (*jetIter)->eta();
    if (jetPt > m_jetPtCut && fabs(jetEta) < m_jetEtaCut){
      l_Jets.push_back(*jetIter);
      ATH_MSG_DEBUG("jet Pt, Eta: " << jetPt << " " << jetEta << " - accepted ");
    } else {
      ATH_MSG_DEBUG("jet Pt, Eta: " << jetPt << " " << jetEta << " - rejected ");
    }
  }
  ATH_MSG_DEBUG("You have " << l_Jets.size() << " jets which passed Pt and Eta cuts ");

  return l_Jets;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void WZSkimsMain::ReadElectrons(const std::vector<const Electron*> & electrons, std::vector<struct lepton> & leptons) 
{
  struct lepton l;
  for (std::vector<const Electron*>::const_iterator electronIter=electrons.begin(); electronIter!=electrons.end(); ++electronIter) {
    l.medium = (*electronIter)->isElectron(egammaPID::ElectronMedium);
    if (l.medium) { // keep only medium leptons
      l.pt  = (*electronIter)->cluster()->pt();
      if (l.pt>m_electronPtCut) { // keep only leptons with good pt
        l.eta = (*electronIter)->cluster()->eta();
        l.phi = (*electronIter)->cluster()->phi();
        l.tight  = (*electronIter)->isElectron(egammaPID::ElectronTight);
        leptons.push_back(l);
      }
    }
  }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void WZSkimsMain::ReadMuons(const std::vector<const Analysis::Muon*> & muons, std::vector<struct lepton> & leptons) 
{
  struct lepton l;
  for (std::vector<const Analysis::Muon*>::const_iterator muonIter=muons.begin(); muonIter!=muons.end(); ++muonIter) {
    l.medium = (*muonIter)->isMedium();
    if (l.medium) { // keep only medium leptons
      l.pt  = (*muonIter)->pt();
      if (l.pt>m_muonPtCut) { // keep only leptons with good pt
        l.eta = (*muonIter)->eta();
        l.phi = (*muonIter)->phi();
        l.tight  = (*muonIter)->isTight();
        leptons.push_back(l);
      }
    }
  }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::CheckJetLeptonOverlap(const std::vector<const Jet*> & jets, const std::vector<struct lepton> & leptons) 
{
  const struct lepton * firstMedium = NULL;
  std::vector<const struct lepton *> goodleptons;
  bool ok = false;

  ATH_MSG_DEBUG("Check eta range of " << leptons.size() << " leptons for overlap removal with jets");

  for (std::vector<struct lepton>::const_iterator lepIter=leptons.begin(); lepIter!=leptons.end(); ++lepIter) {
    double eta=fabs(lepIter->eta);
    // look for leptons with special eta range
    if (m_jetOverlapEtaRange.size()>1) {
      ok=false;
      for (size_t q=1; q<m_jetOverlapEtaRange.size(); q+=2) {
        if (eta<m_jetOverlapEtaRange[q] && eta>m_jetOverlapEtaRange[q-1]) {
          ok=true;
          break;
        }
      }
      if (!ok) {
        ATH_MSG_DEBUG("lepton eta is "<<eta<<" do not use it for overlap removal");
        continue; // can't use this lepton - it's outside valid eta range
      }
    }
    if (m_jetOverlapRemovalWithMedium && lepIter->medium) {
      goodleptons.push_back(&(*lepIter));
    } else if (m_jetOverlapRemovalWithTight && lepIter->tight) {
      goodleptons.push_back(&(*lepIter));
    }
    if (!lepIter->tight && !firstMedium) {
      firstMedium=&(*lepIter);
    }
  }

  size_t nlep=goodleptons.size();
  if (m_jetOverlapRemovalWithMediumIfNoTight && nlep == 0 && firstMedium) {
    goodleptons.push_back(firstMedium);
    nlep=1;
    ATH_MSG_DEBUG("lepton-jet overlap removal with 'one medium because no tight' lepton");
  } else {
    ATH_MSG_DEBUG("lepton-jet overlap removal with "<<nlep
                  <<((m_jetOverlapRemovalWithMedium)?" medium leptons":" tight leptons"));
  }
  
  if (nlep>0) {
    // we are looking here for at least one combination
    // of one lepton and N jets which do not overlap with this lepton
    // we don't care if those jets overlap with other leptons
    ok = false;
    for (std::vector<const struct lepton *>::const_iterator lep=goodleptons.begin(); 
         lep!=goodleptons.end(); ++lep) {
      unsigned int nj = 0;
      for (std::vector<const Jet *>::const_iterator jetIter=jets.begin(); jetIter!=jets.end(); ++jetIter){
        double dphi = fabs((*jetIter)->phi() - (*lep)->phi);
        if (dphi>M_PI) dphi -= 2*M_PI;
        double deta = (*jetIter)->eta() - (*lep)->eta;
        double dr = hypot(dphi,deta);
        if (dr<m_jetOverlapDrCut) continue;
        if (++nj >= m_jetNumber) {
          ok = true;
          break;
        }
      }
      if (ok) break;
    }
  } else { // no leptons for overlap removal
    ok = ( jets.size() >= m_jetNumber );
  }

  // check if there is one high-pt jet
  // use m_EtmissCut as a threshold (since Et miss cut is not applied)
  if (! (ok || m_doEtmissCut) ) {
    for (std::vector<const Jet *>::const_iterator jetIter=jets.begin(); jetIter!=jets.end(); ++jetIter){
      if ( (*jetIter)->pt() > m_EtmissCut) {
        ok = true;
        ATH_MSG_DEBUG("Found one high-pt jet, pt "<<(*jetIter)->pt());
        break;
      }
    }
  }
  
  if (ok)
    ATH_MSG_DEBUG("Passed CheckJetLeptonOverlap");
  else
    ATH_MSG_DEBUG("Not enough good jets in event");

  return ok;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<bool> WZSkimsMain::CheckElectronVector(std::vector<const Electron*> AllElectronsVector, double l_electronPtCut, double l_electronEtaCut, std::string l_electronType) {
  ATH_MSG_DEBUG("Checking electron vector with cuts: Pt: " << l_electronPtCut << ", Eta: " << l_electronEtaCut << ", type: " << l_electronType);

  std::vector<bool> l_GoodElectronsFlag;
  l_GoodElectronsFlag.clear();

  std::vector<const Electron*>::iterator electronIter;

  for (electronIter=AllElectronsVector.begin(); electronIter!=AllElectronsVector.end(); ++electronIter) {
    double electronPt = fabs((*electronIter)->cluster()->pt());
    double electronEta = fabs((*electronIter)->cluster()->eta());
    bool isElecLoose = (*electronIter)->isElectron(egammaPID::ElectronLoose);
    bool isElecMedium = (*electronIter)->isElectron(egammaPID::ElectronMedium);
    bool isElecTight = (*electronIter)->isElectron(egammaPID::ElectronTight);
    bool isFwdElecLoose = (*electronIter)->isElectron(egammaPID::frwdElectronLoose);
    bool isFwdElecTight = (*electronIter)->isElectron(egammaPID::frwdElectronTight);
    ATH_MSG_DEBUG("electron Pt, Eta, Type: " << electronPt << " " << electronEta << " " << isElecLoose << isElecMedium << isElecTight << isFwdElecLoose << isFwdElecTight);
    // if (isFwdElecLoose) isElecLoose = true; // Loose forward electrons bring too many events: not included in LOOSE
    if (isFwdElecTight) isElecTight = true;
    if ((electronPt >= l_electronPtCut) && (electronEta <= l_electronEtaCut) 
    && ((l_electronType == "NOCHECK") || ((l_electronType == "LOOSE") && isElecLoose) || ((l_electronType == "MEDIUM") && isElecMedium) || ((l_electronType == "TIGHT") && isElecTight))) {
      l_GoodElectronsFlag.push_back(true);
    }
    else l_GoodElectronsFlag.push_back(false);
  }
  
  return l_GoodElectronsFlag;
  
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
std::vector<bool> WZSkimsMain::CheckMuonVector(std::vector<const Analysis::Muon*> AllMuonsVector, double l_muonPtCut, double l_muonEtaCut, std::string l_muonType) {
  ATH_MSG_DEBUG("Checking muon vector with cuts: Pt: " << l_muonPtCut << ", Eta: " << l_muonEtaCut << ", type: " << l_muonType);

  std::vector<bool> l_GoodMuonsFlag;
  l_GoodMuonsFlag.clear();

  std::vector<const Analysis::Muon*>::iterator muonIter;

  for (muonIter=AllMuonsVector.begin(); muonIter!=AllMuonsVector.end(); ++muonIter) {
    double muonPt = fabs((*muonIter)->pt());
    double muonEta = fabs((*muonIter)->eta());
        
    bool isMuonLoose = (*muonIter)->isLoose();
    bool isMuonMedium = (*muonIter)->isMedium();
    bool isMuonTight = (*muonIter)->isTight();

    ATH_MSG_DEBUG("muon Pt, Eta, Type: " << muonPt << " " << muonEta << " " << isMuonLoose << isMuonMedium << isMuonTight);
    if ((muonPt >= l_muonPtCut) && (muonEta <= l_muonEtaCut) 
    && ((l_muonType == "NOCHECK") || ((l_muonType == "LOOSE") && isMuonLoose) || ((l_muonType == "MEDIUM") && isMuonMedium) || ((l_muonType == "TIGHT") && isMuonTight))) {
      l_GoodMuonsFlag.push_back(true);
    }
    else l_GoodMuonsFlag.push_back(false);
  }
  
  return l_GoodMuonsFlag;
  
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::CheckLeptonCuts(unsigned int l_leptonNumber1, unsigned int l_leptonNumber2, std::vector<bool> l_GoodLeptonsFlag1, std::vector<bool> l_GoodLeptonsFlag2) {

  bool hasPassed = false;

  if ((l_leptonNumber1 > 0) && (l_leptonNumber2 == 0)) {
    ATH_MSG_DEBUG ("Checking only 1st set of cuts for leptons");
    unsigned int number_good_leptons = 0;
    for (unsigned int iter = 0; iter < l_GoodLeptonsFlag1.size(); iter++) {
      if (l_GoodLeptonsFlag1[iter]) number_good_leptons++;
    }
    if (number_good_leptons >= l_leptonNumber1) hasPassed = true;
  }
  
  else if ((l_leptonNumber1 == 0) && (l_leptonNumber2 > 0)){
    ATH_MSG_DEBUG ("Checking only 2nd set of cuts for leptons");
    unsigned int number_good_leptons = 0;
    for (unsigned int iter = 0; iter < l_GoodLeptonsFlag2.size(); iter++) {
      if (l_GoodLeptonsFlag2[iter]) number_good_leptons++;
    }
    if (number_good_leptons >= l_leptonNumber2) hasPassed = true;

  }
  
  else {
    ATH_MSG_DEBUG ("Checking both sets of cuts for leptons");
    int number_leptons_only_1st_cut = 0;
    int number_leptons_only_2nd_cut = 0;
    int number_leptons_both_cuts = 0;
   
    for (unsigned int iter = 0; iter < l_GoodLeptonsFlag1.size(); iter++) {
      if ((l_GoodLeptonsFlag1[iter]) && !(l_GoodLeptonsFlag2[iter])) number_leptons_only_1st_cut++;
      if (!(l_GoodLeptonsFlag1[iter]) && (l_GoodLeptonsFlag2[iter])) number_leptons_only_2nd_cut++;
      if ((l_GoodLeptonsFlag1[iter]) && (l_GoodLeptonsFlag2[iter])) number_leptons_both_cuts++;
    }
    
    int number_leptons_rem_1st_cut = (int) l_leptonNumber1 - number_leptons_only_1st_cut;
    int number_leptons_rem_2nd_cut = (int) l_leptonNumber2 - number_leptons_only_2nd_cut;
    
    if (number_leptons_rem_1st_cut < 0) number_leptons_rem_1st_cut = 0;
    if (number_leptons_rem_2nd_cut < 0) number_leptons_rem_2nd_cut = 0;
    
    if (number_leptons_both_cuts >= (number_leptons_rem_1st_cut + number_leptons_rem_2nd_cut)) hasPassed = true;
             
  }

  if (hasPassed) ATH_MSG_DEBUG ("Passed CheckLeptonCuts");
  return hasPassed;

}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::CheckInvMass(std::vector<const Electron*> ElectronV, std::vector<bool> l_GoodElectronsFlag1, std::vector<bool> l_GoodElectronsFlag2) {
  ATH_MSG_DEBUG("Checking invariant mass for electrons");

  ATH_MSG_DEBUG(ElectronV.size() << ", " << l_GoodElectronsFlag1.size() << ", " << l_GoodElectronsFlag2.size());
	
  bool hasPassed = false;
 
  unsigned int sizeV = ElectronV.size();
  
  if (l_GoodElectronsFlag1.size() == 0) l_GoodElectronsFlag1.assign(sizeV, false);
  if (l_GoodElectronsFlag2.size() == 0) l_GoodElectronsFlag2.assign(sizeV, false);

  std::vector<const Electron*>::iterator electronIter_1, electronIter_2;
  std::vector<bool>::iterator Flag1_Iter_1, Flag1_Iter_2, Flag2_Iter_1, Flag2_Iter_2;
 		
  if (sizeV >= 2) {

    for (electronIter_1 = ElectronV.begin(), Flag1_Iter_1 = l_GoodElectronsFlag1.begin(), Flag2_Iter_1 = l_GoodElectronsFlag2.begin(); electronIter_1 !=ElectronV.end(); ++electronIter_1, ++Flag1_Iter_1, ++Flag2_Iter_1) {
      //double electronCharge_1 = (*electronIter_1)->charge();
      for (electronIter_2 = electronIter_1 + 1, Flag1_Iter_2 = Flag1_Iter_1 + 1, Flag2_Iter_2 = Flag2_Iter_1 + 1; electronIter_2 !=ElectronV.end(); ++electronIter_2, ++Flag1_Iter_2, ++Flag2_Iter_2) {
      	//double electronCharge_2 = (*electronIter_2)->charge();
	
      	if (  ((*Flag1_Iter_1) || (*Flag2_Iter_1))  &&  ((*Flag1_Iter_2) || (*Flag2_Iter_2))  ) { // &&  ((electronCharge_1*electronCharge_2) == -1.0)  ) { // opp. sign leptons not required

          double pxSum = (*electronIter_1)->px() + (*electronIter_2)->px();
          double pySum = (*electronIter_1)->py() + (*electronIter_2)->py();
          double pzSum = (*electronIter_1)->pz() + (*electronIter_2)->pz();
	  double eSum = (*electronIter_1)->e() + (*electronIter_2)->e();

	  double invMass = sqrt(eSum*eSum - pxSum*pxSum - pySum*pySum - pzSum*pzSum);

          if ((invMass >= m_Mmin) && (invMass <= m_Mmax)) {
            ATH_MSG_DEBUG ("Good electron pair found with invariant mass: " << invMass);
	    hasPassed = true;
	    break;

          }
      	}
      }
      if (hasPassed == true) break;
    }
  }
 
  return hasPassed;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::CheckInvMass(std::vector<const Analysis::Muon*> MuonV, std::vector<bool> l_GoodMuonsFlag1, std::vector<bool> l_GoodMuonsFlag2) {
  ATH_MSG_DEBUG("Checking invariant mass for muons");

  ATH_MSG_DEBUG(MuonV.size() << ", " << l_GoodMuonsFlag1.size() << ", " << l_GoodMuonsFlag2.size());
	
  bool hasPassed = false;
 
  unsigned int sizeV = MuonV.size();
  
  if (l_GoodMuonsFlag1.size() == 0) l_GoodMuonsFlag1.assign(sizeV, false);
  if (l_GoodMuonsFlag2.size() == 0) l_GoodMuonsFlag2.assign(sizeV, false);

  std::vector<const Analysis::Muon*>::iterator muonIter_1, muonIter_2;
  std::vector<bool>::iterator Flag1_Iter_1, Flag1_Iter_2, Flag2_Iter_1, Flag2_Iter_2;
 		
  if (sizeV >= 2) {

    for (muonIter_1 = MuonV.begin(), Flag1_Iter_1 = l_GoodMuonsFlag1.begin(), Flag2_Iter_1 = l_GoodMuonsFlag2.begin(); muonIter_1 !=MuonV.end(); ++muonIter_1, ++Flag1_Iter_1, ++Flag2_Iter_1) {
      //double muonCharge_1 = (*muonIter_1)->charge();
      for (muonIter_2 = muonIter_1 + 1, Flag1_Iter_2 = Flag1_Iter_1 + 1, Flag2_Iter_2 = Flag2_Iter_1 + 1; muonIter_2 !=MuonV.end(); ++muonIter_2, ++Flag1_Iter_2, ++Flag2_Iter_2) {
      	//double muonCharge_2 = (*muonIter_2)->charge();
	
      	if (  ((*Flag1_Iter_1) || (*Flag2_Iter_1))  &&  ((*Flag1_Iter_2) || (*Flag2_Iter_2))  ) { // &&  ((muonCharge_1*muonCharge_2) == -1.0)  ) { // opp. sign leptons not required

          double pxSum = (*muonIter_1)->px() + (*muonIter_2)->px();
          double pySum = (*muonIter_1)->py() + (*muonIter_2)->py();
          double pzSum = (*muonIter_1)->pz() + (*muonIter_2)->pz();
	  double eSum = (*muonIter_1)->e() + (*muonIter_2)->e();

	  double invMass = sqrt(eSum*eSum - pxSum*pxSum - pySum*pySum - pzSum*pzSum);

          if ((invMass >= m_Mmin) && (invMass <= m_Mmax)) {
            ATH_MSG_DEBUG ("Good muon pair found with invariant mass: " << invMass);
	    hasPassed = true;
	    break;

          }
      	}
      }
      if (hasPassed == true) break;
    }
  }
 
  return hasPassed;
}


  

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::Prescale() {
  bool keeping = false;
  m_prescaleCounter++;
  if (m_prescaleCounter == 1) {
    ATH_MSG_DEBUG("Prescaling : event 1 out of " << m_prescaleRate << ", keeping");
    keeping = true;
  }
  else {
    ATH_MSG_DEBUG("Prescaling : event " << m_prescaleCounter << " out of " << m_prescaleRate << ", rejecting");
    if (m_prescaleCounter > m_prescaleRate) { 
      ATH_MSG_WARNING("Prescale counter (" << m_prescaleCounter << ") above prescale rate (" << m_prescaleRate << ") ! This should not happen, STOP prescaling from now on");
      m_prescaleCounter = 0; 
      m_doPrescale = false;
    }
    if (m_prescaleCounter == m_prescaleRate) m_prescaleCounter = 0;
    keeping = false;
  }
  
  return keeping;

}


  

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::SelectTrack() {
  ATH_MSG_DEBUG("Checking tracks with cuts: Pt: " << m_TrackPtCut << ", Eta: " << m_TrackEtaCut);

  bool hasPassed = false;

  const Rec::TrackParticleContainer* importedTrackColl;
  StatusCode sc = m_storeGate->retrieve(importedTrackColl, m_trackContainerName);  
  if (sc.isFailure()) {
    ATH_MSG_WARNING("No track collection of type " << m_trackContainerName);
    return hasPassed;
  }
  else ATH_MSG_DEBUG("You have " << importedTrackColl->size() << " tracks in this event");

  Rec::TrackParticleContainer::const_iterator trackIter;

  for (trackIter=importedTrackColl->begin(); trackIter!=importedTrackColl->end(); ++trackIter) {      
    double trackPt = fabs((*trackIter)->pt());
    double trackEta = fabs((*trackIter)->eta());
    
    ATH_MSG_DEBUG("track Pt, Eta: " << trackPt << " " << trackEta);
    
    if ( (trackPt >= m_TrackPtCut) && (trackEta <= m_TrackEtaCut) ) {
      ATH_MSG_DEBUG("Good track found");
      hasPassed = true;
      break;
    }
  }

  if (hasPassed == true) ATH_MSG_DEBUG ("Passed SelectTrack");

  return hasPassed;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::SelectEtmiss(std::string l_missingEtObjectName) {
  ATH_MSG_DEBUG("Checking missing Et");
  bool hasPassed = false;

  const MissingET* missEt;   

  StatusCode sc = m_storeGate->retrieve(missEt, l_missingEtObjectName);
  if (sc.isFailure()) {
     ATH_MSG_WARNING("No Missing Et object of type " << l_missingEtObjectName);
     return hasPassed;
  }
  double ExMiss=missEt->etx();
  double EyMiss=missEt->ety();
  //double EtSum=missEt->sumet();

  double etmiss = sqrt(ExMiss*ExMiss + EyMiss*EyMiss); 
  
  ATH_MSG_DEBUG("Etmiss (from " << l_missingEtObjectName << "): " << etmiss << endreq);

  if (etmiss >= m_EtmissCut) {
    ATH_MSG_DEBUG (" --> cut passed for this Etmiss");
    hasPassed = true;
  }
  

  return hasPassed;

}
   

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::SelectEtmiss() {
  ATH_MSG_DEBUG("Checking missing Et (ALL)");
  bool hasPassed = false;

  const MissingET* missEt;   
  StatusCode sc = m_storeGate->retrieve(missEt, "MET_RefFinal");
  if (sc.isFailure()) {
     ATH_MSG_WARNING("No Missing Et object of type MET_RefFinal");
     return hasPassed;
  }
  double ExMiss=missEt->etx();
  double EyMiss=missEt->ety();
  //double EtSum=missEt->sumet();

  double etmiss = sqrt(ExMiss*ExMiss + EyMiss*EyMiss); 
  
  ATH_MSG_DEBUG("Etmiss (from MET_RefFinal): " << etmiss << endreq);

  if (etmiss >= m_EtmissCut) {
    ATH_MSG_DEBUG (" --> cut passed for this Etmiss");
    hasPassed = true;
  }
  
  if (hasPassed) return hasPassed;  


  
  const MissingET* MET_LocHadTopo;   
  sc = m_storeGate->retrieve(MET_LocHadTopo, "MET_LocHadTopo");
  if (sc.isFailure()) {
     ATH_MSG_WARNING("No Missing Et object of type MET_LocHadTopo");
     return hasPassed;
  }
  
  const MissingET* MET_MuonBoy;   
  sc = m_storeGate->retrieve(MET_MuonBoy, "MET_MuonBoy");
  if (sc.isFailure()) {
     ATH_MSG_WARNING("No Missing Et object of type MET_MuonBoy");
     return hasPassed;
  }
  
  const MissingET* MET_RefMuon_Track;   
  sc = m_storeGate->retrieve(MET_RefMuon_Track, "MET_RefMuon_Track");
  if (sc.isFailure()) {
     ATH_MSG_WARNING("No Missing Et object of type MET_RefMuon_Track");
     return hasPassed;
  }

  ExMiss = MET_LocHadTopo->etx() + MET_MuonBoy->etx() - MET_RefMuon_Track->etx();
  EyMiss = MET_LocHadTopo->ety() + MET_MuonBoy->ety() - MET_RefMuon_Track->ety();

  etmiss = sqrt(ExMiss*ExMiss + EyMiss*EyMiss); 
  
  ATH_MSG_DEBUG("Etmiss (from MET_LocHadTopo + MET_MuonBoy - MET_RefMuon_Track): " << etmiss << endreq);

  if (etmiss >= m_EtmissCut) {
    ATH_MSG_DEBUG (" --> cut passed for this Etmiss");
    hasPassed = true;
  }

  return hasPassed;

}
   

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool WZSkimsMain::SelectTrigger() {
  ATH_MSG_DEBUG("Checking trigger");

  bool hasPassed = false;

  // get the trigger tool
  if (m_doTriggerCut) {
    StatusCode sc = toolSvc()->retrieveTool("Trig::TrigDecisionTool/TrigDecisionTool",m_trigDec);
    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Can't get handle on the trigger decision tool - will not cut on trigger");
      m_doTriggerCut = false;
    }
  }
  
  bool trigpass = false, trigpass2 = false;
  if (m_triggerName != "") trigpass = m_trigDec->isPassed(m_triggerName);
  if (m_triggerName2 != "") trigpass2 = m_trigDec->isPassed(m_triggerName2);
   
  if (trigpass) ATH_MSG_DEBUG("Passes trigger " << m_triggerName);
  if (trigpass2) ATH_MSG_DEBUG("Passes trigger " << m_triggerName2);
  
  hasPassed = trigpass || trigpass2;

  if (hasPassed == true) ATH_MSG_DEBUG ("Passed SelectTrigger");

  return hasPassed;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//bool WZSkimsMain::SelectEvent() {
//}
