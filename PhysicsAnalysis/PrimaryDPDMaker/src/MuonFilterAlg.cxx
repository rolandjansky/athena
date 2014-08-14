/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PrimaryDPDMaker/MuonFilterAlg.h"
#include "muonEvent/MuonParamDefs.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParameters/TrackParameters.h"
//#include "IsolationTool/IIsolationTool.h"
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include "JetEvent/JetCollection.h"
#include "EventKernel/INavigable4Momentum.h"
//#include "NavFourMom/INavigable4MomentumCollection.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"




/// Standard Service Constructor
MuonFilterAlg::MuonFilterAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator), 
    m_nProcessed(0),
    m_nMuons(0),
    m_nEventMuPassEta(0),
    m_nEventMuPassPt(0),
    m_nEventMuPassAuthor(0),
    m_nEventMuPassChi2(0),
    m_nEventMuPassIsolation(0),
    m_nEventMuPassTriggerHits(0),
    m_nEventMuPassPrecisionHits(0),
    m_nEventMuPassIDHits(0),
    m_nEventMuPassd0z0Cut(0),
    m_nEventMuPassIDActivity(0),
    m_nEventMuPassHasMuon(0),
    m_IDtrackCollection(0),
    m_jetCollection(0)
    //,m_muonIsolationTool("TrackIsolationTool/MuonFilter_TrackIsolationTool")

{

  declareProperty("cutPtMinMu",         m_cutPtMinMu = 0.0);
  declareProperty("muonAuthors",        m_muonAuthors = "all"); //Possible values: 'lowpt', 'combined', 'calo', 'standalone', 'all'
  declareProperty("TrackType",          m_trackType = "default"); //Possible values: 'spectrometer', 'id','combined', 'default',some of these options are not compatible with specific MuonAuthors. Checked at initialization
  declareProperty("muonContainer",      m_muonContainer = "StacoESDMuonCollection"); 
  declareProperty("cutEtaMax",          m_cutEtaMax = 3.0);
  declareProperty("cutChi2",            m_cutChi2 = -1.0);
  declareProperty("passAll",            m_passAll = false);
  declareProperty("hasIDActivity",      m_hasIDActivity = false);
  declareProperty("IDtrackContainerName",   m_IDtrackContainerName = "TrackParticleCandidate");
  declareProperty("useIsolationFromContainer",  m_useIsolationFromContainer = true);
  declareProperty("doNoTrackIsol",      m_doNoTrackIsol = false);
  declareProperty("noTrackIsol",        m_noTrackIsol = 5);
  declareProperty("innerNoTrackRadius", m_innerNoTrackRadius = 0.);
  declareProperty("outerNoTrackRadius", m_outerNoTrackRadius = 0.4);
  declareProperty("doPtTrackIsol",      m_doPtTrackIsol = false);
  declareProperty("ptTrackIsol",        m_ptTrackIsol = 0.2);
  declareProperty("innerPtTrackRadius", m_innerPtTrackRadius = 0.);
  declareProperty("outerPtTrackRadius", m_outerPtTrackRadius = 0.4);
  declareProperty("doCaloIsol",         m_doCaloIsol = false);
  declareProperty("caloIsol",           m_caloIsol = 0.2);
  declareProperty("innerCaloRadius",    m_innerCaloRadius = 0.);
  declareProperty("outerCaloRadius",    m_outerCaloRadius = 0.4);
  declareProperty("doJetIsol",          m_doJetIsol = false);
  declareProperty("jetIsolEt",          m_jetIsolEt = 15000.);
  declareProperty("jetDistance",        m_jetDistance = 0.5);
  declareProperty("jetContainerName",   m_jetContainerName = "Cone4H1TowerJets");
  declareProperty("minRPCHits",         m_minRPCHits = 0); // Phi hits
  declareProperty("minMDTHits",         m_minMDTHits = 0);
  declareProperty("minPrecisionHits",   m_minPrecisionHits = 0);
  declareProperty("minTGCHits",         m_minTGCHits = 0);
  declareProperty("minSCTHits",         m_minSCTHits = 0);
  declareProperty("minTRTHits",         m_minTRTHits = 0);
  declareProperty("minPIXELHits",       m_minPIXELHits = 0);
  declareProperty("minTOTSIHits",       m_minTOTSIHits = 0); // SCT+PIXEL HITS
  declareProperty("minTotTrigHits",     m_minTotTrigHits = 0); // TGC+RPC HITS
  declareProperty("doz0Cut",            m_doz0Cut = false);
  declareProperty("z0MaxValue",         m_z0MaxValue = 10000);
  declareProperty("dod0Cut",            m_dod0Cut = false);
  declareProperty("d0MaxValue",         m_d0MaxValue = 10000);
  //declareProperty("MuonIsolationTool",  m_muonIsolationTool); 
  declareProperty("UseTrackContainer",  m_useTrackContainer = false);
  
}

/// Destructor
MuonFilterAlg::~MuonFilterAlg()
{

}

/// Gaudi Service Interface method implementations:
StatusCode MuonFilterAlg::initialize()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);


  msg(MSG::INFO) << "==> initialize ..." << name() << endreq;
  msg(MSG::INFO)<< "******* MuonFilterAlg"<< name() <<" Configuration **********" << endreq;  
  msg(MSG::INFO)<<"cutPtMinMu          "<<         m_cutPtMinMu        <<endreq;
  msg(MSG::INFO)<<"muonAuthors         "<<        m_muonAuthors       <<endreq;
  msg(MSG::INFO)<<"TrackType           "<<          m_trackType         <<endreq;
  msg(MSG::INFO)<<"muonContainer       "<<      m_muonContainer     <<endreq;
  msg(MSG::INFO)<<"cutEtaMax           "<<          m_cutEtaMax         <<endreq;
  msg(MSG::INFO)<<"cutChi2             "<<            m_cutChi2           <<endreq;
  msg(MSG::INFO)<<"passAll             "<<            m_passAll           <<endreq;
  msg(MSG::INFO)<<"IDActivity          "<<            m_hasIDActivity     <<endreq;
  msg(MSG::INFO)<<"ID container name   "<<            m_IDtrackContainerName  <<endreq;
  msg(MSG::INFO)<<"useIsoFromContainer "<< m_useIsolationFromContainer    <<endreq;
  msg(MSG::INFO)<<"doNoTrackIsol       "<<      m_doNoTrackIsol     <<endreq;
  msg(MSG::INFO)<<"noTrackIsol         "<<        m_noTrackIsol       <<endreq;
  msg(MSG::INFO)<<"innerNoTrackRadius  "<< m_innerNoTrackRadius<<endreq;
  msg(MSG::INFO)<<"outerNoTrackRadius  "<< m_outerNoTrackRadius<<endreq;
  msg(MSG::INFO)<<"doPtTrackIsol       "<<      m_doPtTrackIsol     <<endreq;
  msg(MSG::INFO)<<"ptTrackIsol         "<<        m_ptTrackIsol       <<endreq;
  msg(MSG::INFO)<<"innerPtTrackRadius  "<< m_innerPtTrackRadius<<endreq;
  msg(MSG::INFO)<<"outerPtTrackRadius  "<< m_outerPtTrackRadius<<endreq;
  msg(MSG::INFO)<<"doCaloIsol          "<<         m_doCaloIsol        <<endreq;
  msg(MSG::INFO)<<"caloIsol            "<<           m_caloIsol          <<endreq;
  msg(MSG::INFO)<<"innerCaloRadius     "<<    m_innerCaloRadius   <<endreq;
  msg(MSG::INFO)<<"outerCaloRadius     "<<    m_outerCaloRadius   <<endreq;
  msg(MSG::INFO)<<"doJetIsol           "<<          m_doJetIsol         <<endreq;
  msg(MSG::INFO)<<"jetIsolEt           "<<          m_jetIsolEt         <<endreq;
  msg(MSG::INFO)<<"jetDistance         "<<        m_jetDistance       <<endreq;
  msg(MSG::INFO)<<"jetContainerName    "<<   m_jetContainerName  <<endreq;
  msg(MSG::INFO)<<"minRPCHits          "<<         m_minRPCHits        <<endreq;
  msg(MSG::INFO)<<"minMDTHits          "<<         m_minMDTHits        <<endreq;
  msg(MSG::INFO)<<"minPrecisionHits          "<<   m_minPrecisionHits  <<endreq;
  msg(MSG::INFO)<<"minTGCHits          "<<         m_minTGCHits        <<endreq;
  msg(MSG::INFO)<<"minSCTHits          "<<         m_minSCTHits        <<endreq;
  msg(MSG::INFO)<<"minTRTHits          "<<         m_minTRTHits        <<endreq;
  msg(MSG::INFO)<<"minPIXELHits        "<<       m_minPIXELHits      <<endreq;
  msg(MSG::INFO)<<"minTOTSIHits        "<<       m_minTOTSIHits      <<endreq;
  msg(MSG::INFO)<<"minTotTrigHits      "<<     m_minTotTrigHits    <<endreq;
  msg(MSG::INFO)<<"doz0Cut             "<<            m_doz0Cut           <<endreq;
  msg(MSG::INFO)<<"z0MaxValue          "<<         m_z0MaxValue        <<endreq;
  msg(MSG::INFO)<<"dod0Cut             "<<            m_dod0Cut           <<endreq;
  msg(MSG::INFO)<<"d0MaxValue          "<<         m_d0MaxValue        <<endreq;
  msg(MSG::INFO)<<"UseTrackContainer   "<<  m_useTrackContainer <<endreq;





#if 0
  if(m_doNoTrackIsol || m_doPtTrackIsol || m_doCaloIsol){
    if(m_muonIsolationTool.retrieve().isFailure()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Problem retrieving MuonIsolationTool pointer " << endreq;
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "setting Track and Calo isolation to False " << endreq;
      m_doNoTrackIsol = false;
      m_doPtTrackIsol = false;
      m_doCaloIsol = false;
    }
  }
#endif

  if(m_muonAuthors == "calo"){
    if(m_trackType == "combined" || m_trackType == "standalone"){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Calo algorithms only accept TrackType id, but TrackType = " << m_trackType << endreq;
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Setting TrackType to default" << endreq;
      m_trackType = "default";
    }
  }
  return sc;

}

StatusCode MuonFilterAlg::execute()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  m_nProcessed += 1;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "==> execute " << name() << " on " << m_nProcessed << " event..." << endreq;

  //If passAll is selected, accept all events
  if(m_passAll){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << name() << " event passed because passAll is true" << endreq;
    setFilterPassed(true);
    return sc;
  }
  
  if(m_doJetIsol){
    StatusCode sc = evtStore()->retrieve(m_jetCollection, m_jetContainerName);
    if(sc.isFailure()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << m_jetContainerName <<" container not found in StoreGate " << endreq;
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Turning jet isolation off" << endreq;
      m_doJetIsol = false;
    }
  }

  if(m_hasIDActivity){
    StatusCode sc = evtStore()->retrieve(m_IDtrackCollection, m_IDtrackContainerName);
    if(sc.isFailure()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << m_IDtrackContainerName <<" container not found in StoreGate " << endreq;
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Turning hasIDActivity off" << endreq;
      m_hasIDActivity = false;
    }
  }
  
  //Then... here we go!
  bool hasMuon     = false;
  //std::vector<std::string> containerNames;
  const Analysis::MuonContainer* muonCollection      = 0;
  const Rec::TrackParticleContainer* trackCollection = 0;

  sc = m_useTrackContainer ? 
    evtStore()->retrieve(trackCollection, m_muonContainer) : 
    evtStore()->retrieve(muonCollection,  m_muonContainer);
  
  if(sc.isFailure()){
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << m_muonContainer << " container not found in StoreGate." << endreq;
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Will not pass the filter for this event." << endreq;
    setFilterPassed(false);
    sc = StatusCode::SUCCESS;
    return sc;
  }

  INavigable4MomentumCollection* I4MContainer = 0;    
  if (m_useTrackContainer){
    I4MContainer = (INavigable4MomentumCollection*) trackCollection;
  } else {
    I4MContainer = (INavigable4MomentumCollection*) muonCollection;
  }
  hasMuon = hasAnyMuon(I4MContainer);
  
  if( hasMuon ){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Event passed." << endreq;
    setFilterPassed(true);
    
  }else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Event failed." << endreq;
    setFilterPassed(false);
  }
  return sc;

}

StatusCode MuonFilterAlg::finalize()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // Calculate the efficiencies and their errors
  double effiMuons             = 0.0;
  double effiMuPassIDActivity  = 0.0;
  double effiMuPassEta         = 0.0;
  double effiMuPassPt          = 0.0;
  double effiMuPassAuthor      = 0.0;
  double effiMuPassChi2        = 0.0;
  double effiMuPassIsolation   = 0.0;
  double effiMuPassTriggerHits = 0.0;
  double effiMuPassPrecisionHits = 0.0;
  double effiMuPassIDHits = 0.0;
  double effiMuPassd0z0Cut = 0.0;
  double effiMuPassHasMuon = 0.0;

  double effiErrMuons             = 0.0;
  double effiErrMuPassIDActivity      = 0.0;
  double effiErrMuPassEta         = 0.0;
  double effiErrMuPassPt          = 0.0;
  double effiErrMuPassAuthor      = 0.0;
  double effiErrMuPassChi2        = 0.0;
  double effiErrMuPassIsolation   = 0.0;
  double effiErrMuPassTriggerHits = 0.0;
  double effiErrMuPassPrecisionHits = 0.0;
  double effiErrMuPassIDHits = 0.0;
  double effiErrMuPassd0z0Cut = 0.0;
  double effiErrMuPassHasMuon = 0.0;
  double eftmp;
  double ertmp;
  double rootn;

  // Efficiencies
  effiMuons             = m_nProcessed != 0 ? m_nMuons / (1.0*m_nProcessed) : 0;
  effiMuPassHasMuon     = m_nProcessed != 0 ? 100.0*m_nEventMuPassHasMuon / m_nProcessed : 0;
  effiMuPassd0z0Cut     = m_nProcessed != 0 ? 100.0*m_nEventMuPassd0z0Cut / m_nProcessed : 0;
  effiMuPassIDActivity  = m_nProcessed != 0 ? 100.0*m_nEventMuPassIDActivity / m_nProcessed : 0;
  effiMuPassEta         = m_nProcessed != 0 ? 100.0*m_nEventMuPassEta / m_nProcessed : 0;
  effiMuPassPt          = m_nProcessed != 0 ? 100.0*m_nEventMuPassPt / m_nProcessed : 0;
  effiMuPassAuthor      = m_nProcessed != 0 ? 100.0*m_nEventMuPassAuthor / m_nProcessed : 0;
  effiMuPassChi2        = m_nProcessed != 0 ? 100.0*m_nEventMuPassChi2 / m_nProcessed : 0;
  effiMuPassIsolation   = m_nProcessed != 0 ? 100.0*m_nEventMuPassIsolation / m_nProcessed : 0;
  effiMuPassTriggerHits = m_nProcessed != 0 ? 100.0*m_nEventMuPassTriggerHits / m_nProcessed : 0;
  effiMuPassPrecisionHits = m_nProcessed != 0 ? 100.0*m_nEventMuPassPrecisionHits / m_nProcessed : 0;

  effiMuPassIDHits = m_nProcessed != 0 ? 100.0*m_nEventMuPassIDHits / m_nProcessed : 0;

  // Error on efficiencies

  rootn    = m_nProcessed != 0 ? sqrt((double)m_nProcessed) : 0;

  effiErrMuons             = rootn != 0 ? sqrt(effiMuons) / rootn : 0;
  
  eftmp= effiMuPassHasMuon/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassHasMuon         = rootn != 0 ? 100 * ertmp / rootn : 0;


  eftmp= effiMuPassd0z0Cut/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassd0z0Cut         = rootn != 0 ? 100 * ertmp / rootn : 0;

  eftmp= effiMuPassIDActivity/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassIDActivity         = rootn != 0 ? 100 * ertmp / rootn : 0;


  eftmp= effiMuPassEta/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassEta         = rootn != 0 ? 100 * ertmp / rootn : 0;

  eftmp= effiMuPassPt/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassPt          = rootn != 0 ? 100 * ertmp / rootn : 0;

  eftmp= effiMuPassAuthor/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassAuthor      = rootn != 0 ? 100 * ertmp / rootn : 0;

  eftmp= effiMuPassChi2/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassChi2        = rootn != 0 ? 100 * ertmp / rootn : 0;

  eftmp= effiMuPassIsolation/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassIsolation   = rootn != 0 ? 100 * ertmp / rootn : 0;

  eftmp= effiMuPassTriggerHits/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassTriggerHits = rootn != 0 ? 100 * ertmp / rootn : 0;
  
  eftmp= effiMuPassPrecisionHits/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassPrecisionHits = rootn != 0 ? 100 * ertmp / rootn : 0;

  eftmp= effiMuPassIDHits/100;
  ertmp= eftmp*(1-eftmp) >=0 ?sqrt(eftmp*(1-eftmp)) : 0;  
  effiErrMuPassIDHits = rootn != 0 ? 100 * ertmp / rootn : 0;

  if (msgLvl(MSG::INFO)){
    msg(MSG::INFO) << "==> finalize " << name() << endreq;
    msg(MSG::INFO) << "***************************************************************" << endreq;
    msg(MSG::INFO) << "Cut-flow table of " << name() << " skimming algorithm:" << endreq;
    msg(MSG::INFO) << "-------------" << endreq;
    msg(MSG::INFO) << " Number of processed events: " << m_nProcessed << endreq;
    msg(MSG::INFO) << " Events with ID Activity:  " << m_nEventMuPassIDActivity << " and resulting efficiency = (" << effiMuPassIDActivity << "+/-" << effiErrMuPassIDActivity << ")" << endreq;
    msg(MSG::INFO) << " Number of all muons:  " << m_nMuons << " and number of muons per event: " << effiMuons << "+/-" << effiErrMuons << endreq;
    msg(MSG::INFO) << " Events with  >0 muons: " << m_nEventMuPassHasMuon << " and resulting efficiency = (" << effiMuPassHasMuon << "+/-" << effiErrMuPassHasMuon << ")" << endreq;
    msg(MSG::INFO) << " Events after d0z0 cut:    " << m_nEventMuPassd0z0Cut << " and resulting efficiency = (" << effiMuPassd0z0Cut << "+/-" << effiErrMuPassd0z0Cut << ")" << endreq;
    msg(MSG::INFO) << " Events after eta cut:     " << m_nEventMuPassEta << " and resulting efficiency = (" << effiMuPassEta << "+/-" << effiErrMuPassEta << ")" << endreq;
    msg(MSG::INFO) << " Events after p_T cut:     " << m_nEventMuPassPt << " and resulting efficiency = (" << effiMuPassPt << "+/-" << effiErrMuPassPt << ")" << endreq;
    msg(MSG::INFO) << " Events after author cut:  " << m_nEventMuPassAuthor << " and resulting efficiency = (" << effiMuPassAuthor << "+/-"  << effiErrMuPassAuthor << ")" << endreq;
    msg(MSG::INFO) << " Events after chi2 cut:  "   << m_nEventMuPassChi2   << " and resulting efficiency = (" << effiMuPassChi2   << "+/-"  << effiErrMuPassChi2   << ")" << endreq;
    msg(MSG::INFO) << " Events after isolation cut:  " << m_nEventMuPassIsolation << " and resulting efficiency = (" << effiMuPassIsolation << "+/-"  << effiErrMuPassIsolation << ")" << endreq;
    msg(MSG::INFO) << " Events after trigger hits cut:  " << m_nEventMuPassTriggerHits << " and resulting efficiency = (" << effiMuPassTriggerHits << "+/-"  << effiErrMuPassTriggerHits << ")" << endreq;
    msg(MSG::INFO) << " Events after precision hits cut:  " << m_nEventMuPassPrecisionHits << " and resulting efficiency = (" << effiMuPassPrecisionHits << "+/-"  << effiErrMuPassPrecisionHits << ")" << endreq;
    msg(MSG::INFO) << " Events after ID hits cut:  " << m_nEventMuPassIDHits << " and resulting efficiency = (" << effiMuPassIDHits << "+/-"  << effiErrMuPassIDHits << ")" << endreq;
    msg(MSG::INFO) << "***************************************************************" << endreq;
  }
  
  return sc;
  
}

bool MuonFilterAlg::hasAnyMuon(const INavigable4MomentumCollection* I4MContainer)
{

  int nMuPassEta         = 0;
  int nMuPassPt          = 0;
  int nMuPassAuthor      = 0;
  int nMuPassIsolation   = 0;
  int nMuPassChi2        = 0;
  int nMuPassTriggerHits = 0;
  int nMuPassPrecisionHits = 0;
  int nMuPassIDHits = 0;
  int nMuPassd0z0Cut = 0;
  int nMuPassHasMuon = 0;
  bool hasMuonInThisCollection = false;


  /** ID activity requirement **/
  bool IDActivityFlag= true;
  if (m_hasIDActivity) IDActivityFlag= hasIDActivity();    
  if (IDActivityFlag)  m_nEventMuPassIDActivity += 1;
  else return hasMuonInThisCollection;
  
  INavigable4MomentumCollection::const_iterator I4MItr = I4MContainer->begin();
  INavigable4MomentumCollection::const_iterator I4MItrE = I4MContainer->end();
  
  const Rec::TrackParticle* tp = 0;
  for(; I4MItr != I4MItrE; ++I4MItr){
    m_nMuons += 1;
    if (!m_useTrackContainer) {	
      const Analysis::Muon* muon = dynamic_cast<const Analysis::Muon*>(*I4MItr);
      if ( !muon ) continue;
      tp = getTrackParticle(muon, m_muonAuthors, m_trackType);
      if(tp == 0) continue;
    } else {
      tp = dynamic_cast<const Rec::TrackParticle*> (*I4MItr);
    }
    if(tp == 0) continue;
    nMuPassHasMuon += 1;
    /*test NAN*/
    float tmp_pt=(float)tp->pt();
    if (tmp_pt!=tmp_pt){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Container "<<m_muonContainer
                                                  <<" has variables with NAN values, skip event"<< endreq;    
      return false;
    }
    // Cut on d0 and z0
    if (!passd0z0Cut(tp)) continue;
    nMuPassd0z0Cut += 1;
    
    if(fabs(tp->eta()) < m_cutEtaMax){
      nMuPassEta += 1;
      //If the Pt of this muon is larger than the minimum required Pt
      // of the high-Pt cut, then the high-Pt muon is found
      if(tp->pt() > m_cutPtMinMu){
        nMuPassPt += 1;
	bool goodAuthor=false;
	if (!m_useTrackContainer) {	
	  //Check that the muon in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
	  const Analysis::Muon* muon = dynamic_cast<const Analysis::Muon*>(*I4MItr);
	  if(checkMuonAuthor( muon, m_muonAuthors )){goodAuthor=true;}
	} else {goodAuthor=true;}
	if(goodAuthor){
	  nMuPassAuthor += 1;	  
          if(m_cutChi2 == -1.0 || tp->fitQuality()->chiSquared() < m_cutChi2){
            nMuPassChi2 += 1;
	    bool goodIsolation=false;
	    if (!m_useTrackContainer&&m_useIsolationFromContainer) {	
	      const Analysis::Muon* muon = dynamic_cast<const Analysis::Muon*>(*I4MItr);
         if ( !muon ) continue;
	      if (passCaloIsol(muon) && passNoTracksIsol(muon) && passPtTracksIsol(muon) && passJetIsol(tp)) goodIsolation=true;
	    } else if (passCaloIsol(tp) && passNoTracksIsol(tp) && passPtTracksIsol(tp) && passJetIsol(tp))  goodIsolation=true;
	    
	    if (goodIsolation) {
              nMuPassIsolation += 1;
	      
              int rpcHits = tp->trackSummary() ? tp->trackSummary()->get(Trk::numberOfRpcPhiHits) : 10;
              int mdtHits = tp->trackSummary() ? tp->trackSummary()->get(Trk::numberOfMdtHits) : 20;
              int cscHits = tp->trackSummary() ? tp->trackSummary()->get(Trk::numberOfCscPhiHits) : 20;
              int tgcHits = tp->trackSummary() ? tp->trackSummary()->get(Trk::numberOfTgcPhiHits) : 10;
              int sctHits = tp->trackSummary() ? tp->trackSummary()->get(Trk::numberOfSCTHits) : 10;
              int trtHits = tp->trackSummary() ? tp->trackSummary()->get(Trk::numberOfTRTHits) : 10;
              int pxlHits = tp->trackSummary() ? tp->trackSummary()->get(Trk::numberOfPixelHits) : 10;
	      
	      
              if(rpcHits<0) rpcHits=0;
              if(mdtHits<0) mdtHits=0;
              if(cscHits<0) cscHits=0;
              if(tgcHits<0) tgcHits=0;
              if(sctHits<0) sctHits=0;
              if(trtHits<0) trtHits=0;
              if(pxlHits<0) pxlHits=0;
	      
              if(tp->trackSummary() == 0 && 
                 (m_minRPCHits != 0 || m_minMDTHits != 0 || m_minTGCHits != 0 || 
                  m_minSCTHits != 0 || m_minTRTHits != 0 || m_minPIXELHits != 0 ||
                  m_minTOTSIHits !=0 || m_minTotTrigHits !=0 || m_minPrecisionHits != 0 ) ){
		
                if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "TrackParticle does not have a TrackSummary, but cut on Trigger hits requested." << endreq;
                if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Muon will pass the cuts" << endreq;		
              }
              if(rpcHits >= m_minRPCHits && tgcHits >= m_minTGCHits
                 && (rpcHits+tgcHits) >= m_minTotTrigHits ){
                nMuPassTriggerHits += 1;
                if (mdtHits+cscHits >= m_minPrecisionHits && mdtHits >= m_minMDTHits ){
                  nMuPassPrecisionHits += 1;
                  if (sctHits >= m_minSCTHits && 
                      trtHits >= m_minTRTHits && 
                      pxlHits >= m_minPIXELHits && 
                      (sctHits+pxlHits) >= m_minTOTSIHits ){
                    nMuPassIDHits += 1;
                    hasMuonInThisCollection = true;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  // Translate the muon pass counters into event pass counters    
  
  
  if(nMuPassHasMuon     > 0)
    m_nEventMuPassHasMuon += 1;


  if(nMuPassd0z0Cut  > 0)
    m_nEventMuPassd0z0Cut += 1;

  if(nMuPassEta > 0)
    m_nEventMuPassEta += 1;
  
  if(nMuPassPt > 0)
    m_nEventMuPassPt += 1;

  if(nMuPassAuthor > 0) 
    m_nEventMuPassAuthor += 1;
  
  if(nMuPassChi2 > 0) 
    m_nEventMuPassChi2 += 1;

  if(nMuPassIsolation > 0) 
    m_nEventMuPassIsolation += 1;

  if(nMuPassTriggerHits > 0) 
    m_nEventMuPassTriggerHits += 1;

  if(nMuPassPrecisionHits > 0) 
    m_nEventMuPassPrecisionHits += 1;

  if(nMuPassIDHits > 0) 
    m_nEventMuPassIDHits += 1;
  
  
  return hasMuonInThisCollection;
  
}

bool MuonFilterAlg::passCaloIsol(const Rec::TrackParticle* /*muon*/)
{
  if(!m_doCaloIsol)
    return true;
#if 0
  double outerConeEt = m_muonIsolationTool->caloIsolationEnergy(muon, m_outerCaloRadius,false);
  double innerConeEt = m_muonIsolationTool->caloIsolationEnergy(muon, m_innerCaloRadius,false);
  if (muon->pt()>0) {
    if((outerConeEt-innerConeEt)/muon->pt() < m_caloIsol)
      return true;
  }
#endif
  return false;
}

bool MuonFilterAlg::passCaloIsol(const Analysis::Muon* muon)
{
  if(!m_doCaloIsol) return true;
  float etCone10 = (float) muon->parameter(MuonParameters::etcone10);
  float etCone20 = (float) muon->parameter(MuonParameters::etcone20);
  float etCone30 = (float) muon->parameter(MuonParameters::etcone30);
  float etCone40 = (float) muon->parameter(MuonParameters::etcone40);
  double innerConeEt=0; 
  if (m_innerCaloRadius<0.1) innerConeEt=0; 
  else if (m_innerCaloRadius<0.2) innerConeEt=etCone10; 
  else if (m_innerCaloRadius<0.3) innerConeEt=etCone20; 
  else if (m_innerCaloRadius<0.4) innerConeEt=etCone30; 
  else innerConeEt=etCone40; 
  double outerConeEt=0; 
  if (m_outerCaloRadius<0.1) outerConeEt=0; 
  else if (m_outerCaloRadius<0.2) outerConeEt=etCone10; 
  else if (m_outerCaloRadius<0.3) outerConeEt=etCone20; 
  else if (m_outerCaloRadius<0.4) outerConeEt=etCone30; 
  else outerConeEt=etCone40; 
  
  if (muon->pt()>0) {
    if((outerConeEt-innerConeEt)/muon->pt() < m_caloIsol)
      return true;
  }
  return false;
}


bool MuonFilterAlg::passNoTracksIsol(const Rec::TrackParticle* /*muon*/)
{
  if(!m_doNoTrackIsol)
    return true;
#if 0
  int outerConeNoTracks = m_muonIsolationTool->trackIsolationNumberOfTrack(muon,m_outerNoTrackRadius,false);
  int innerConeNoTracks = m_muonIsolationTool->trackIsolationNumberOfTrack(muon,m_innerNoTrackRadius,false);
  if((outerConeNoTracks-innerConeNoTracks) < m_noTrackIsol)
    return true;
#endif
  return false;
}


bool MuonFilterAlg::passNoTracksIsol(const Analysis::Muon* muon)
{
  if(!m_doNoTrackIsol) return true;
  float nuCone10 = (float) muon->parameter(MuonParameters::nucone10);
  float nuCone20 = (float) muon->parameter(MuonParameters::nucone20);
  float nuCone30 = (float) muon->parameter(MuonParameters::nucone30);
  float nuCone40 = (float) muon->parameter(MuonParameters::nucone40);
  double innerConeNoTracks=0; 
  if (m_innerNoTrackRadius<0.1)      innerConeNoTracks=0; 
  else if (m_innerNoTrackRadius<0.2) innerConeNoTracks=nuCone10; 
  else if (m_innerNoTrackRadius<0.3) innerConeNoTracks=nuCone20; 
  else if (m_innerNoTrackRadius<0.4) innerConeNoTracks=nuCone30; 
  else innerConeNoTracks=nuCone40; 
  double outerConeNoTracks=0; 
  if (m_outerNoTrackRadius<0.1)      outerConeNoTracks=0; 
  else if (m_outerNoTrackRadius<0.2) outerConeNoTracks=nuCone10; 
  else if (m_outerNoTrackRadius<0.3) outerConeNoTracks=nuCone20; 
  else if (m_outerNoTrackRadius<0.4) outerConeNoTracks=nuCone30; 
  else outerConeNoTracks=nuCone40; 
  
  if((outerConeNoTracks-innerConeNoTracks) < m_noTrackIsol)
    return true;
  return false;    
}



bool MuonFilterAlg::passPtTracksIsol(const Rec::TrackParticle* /*muon*/)
{
  if(!m_doPtTrackIsol)
    return true;
#if 0
  double outerConePtTracks = m_muonIsolationTool->trackIsolationEnergy(muon,m_outerPtTrackRadius, false);
  double innerConePtTracks = m_muonIsolationTool->trackIsolationEnergy(muon,m_innerPtTrackRadius, false);
  if((outerConePtTracks-innerConePtTracks) < m_ptTrackIsol)
    return true;
#endif
  return false;
}

bool MuonFilterAlg::passPtTracksIsol(const Analysis::Muon* muon)
{
  if(!m_doPtTrackIsol) return true;
  float ptCone10 = (float) muon->parameter(MuonParameters::ptcone10);
  float ptCone20 = (float) muon->parameter(MuonParameters::ptcone20);
  float ptCone30 = (float) muon->parameter(MuonParameters::ptcone30);
  float ptCone40 = (float) muon->parameter(MuonParameters::ptcone40);

  double innerConePtTracks=0; 
  if (m_innerPtTrackRadius<0.1)      innerConePtTracks=0; 
  else if (m_innerPtTrackRadius<0.2) innerConePtTracks=ptCone10; 
  else if (m_innerPtTrackRadius<0.3) innerConePtTracks=ptCone20; 
  else if (m_innerPtTrackRadius<0.4) innerConePtTracks=ptCone30; 
  else innerConePtTracks=ptCone40; 
  double outerConePtTracks=0; 
  if (m_outerPtTrackRadius<0.1) outerConePtTracks=0; 
  else if (m_outerPtTrackRadius<0.2) outerConePtTracks=ptCone10; 
  else if (m_outerPtTrackRadius<0.3) outerConePtTracks=ptCone20; 
  else if (m_outerPtTrackRadius<0.4) outerConePtTracks=ptCone30; 
  else outerConePtTracks=ptCone40; 
  
  if (muon->pt()>0) {
    if((outerConePtTracks-innerConePtTracks)/muon->pt() < m_ptTrackIsol) return true;
  }
  return false;
  
}


bool MuonFilterAlg::passJetIsol(const Rec::TrackParticle* muon)
{

  if(!m_doJetIsol)
    return true;
  JetCollection::const_iterator jet, jetE = m_jetCollection->end();
  for(jet = m_jetCollection->begin(); jet != jetE; jet++){
    if(deltaR(*jet,muon) < m_jetDistance && (*jet)->et() > m_jetIsolEt)
      return false;
  }
  return true;

}

bool MuonFilterAlg::checkMuonAuthor(const Analysis::Muon* muon, std::string authorName)
{
  if(authorName == "all")
    return true;
  
  std::vector<MuonParameters::Author> authors ;
  if(authorName == "combined"){
    authors.push_back(MuonParameters::STACO);
    authors.push_back(MuonParameters::MuidCo);
  }else if(authorName == "standalone"){
    authors.push_back(MuonParameters::Muonboy);
    authors.push_back(MuonParameters::MuidSA);
  }else if(authorName == "lowpt"){
    authors.push_back(MuonParameters::MuTag);
    authors.push_back(MuonParameters::MuTagIMO);
    authors.push_back(MuonParameters::MuGirl);
  }else if(authorName == "calo"){
    authors.push_back(MuonParameters::CaloMuonId);
    authors.push_back(MuonParameters::CaloTag);
    authors.push_back(MuonParameters::CaloLikelihood);
  }
  std::vector<MuonParameters::Author>::iterator author, authorE = authors.end();
  for(author = authors.begin(); author != authorE; author++){
    if(muon->isAuthor(*author)) return true;
  }


  return false;
}    

double MuonFilterAlg::deltaR(const INavigable4Momentum* part1, const Rec::TrackParticle* part2)
{
  double deltaEta = part1->eta()-part2->eta();
  double deltaPhi = part1->phi()-part2->phi();
  deltaPhi = deltaPhi < -3.141592 ? deltaPhi+2*3.141592 : deltaPhi;
  deltaPhi = deltaPhi > 3.141592 ? deltaPhi-2*3.141592 : deltaPhi;
  return sqrt(deltaPhi*deltaPhi+deltaEta*deltaEta);
}

const Rec::TrackParticle* MuonFilterAlg::getTrackParticle(const Analysis::Muon* muon, std::string authorName, std::string trackType)
{
  const Rec::TrackParticle* tp = muon->track();
  if(authorName == "standalone" || trackType == "spectrometer") tp = muon->muonExtrapolatedTrackParticle();
  else if(trackType =="id") tp = muon->inDetTrackParticle();
  else if(trackType =="combined") tp = muon->combinedMuonTrackParticle();
  return tp;
}


bool MuonFilterAlg::passd0z0Cut(const Rec::TrackParticle* muon)
{
  
  const Trk::Perigee* perigee = muon->measuredPerigee();
  if(perigee) {
    float d0= (float) perigee->parameters()[Trk::d0];
    float z0= (float) perigee->parameters()[Trk::z0];
    if (m_doz0Cut && fabs(z0)>m_z0MaxValue) return false;
    if (m_dod0Cut && fabs(d0)>m_d0MaxValue) return false;
  } else {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "MeasuredPerigee not found: returning true" << endreq;    
    return true;
  }
  return true;
  
}

bool MuonFilterAlg::hasIDActivity()
{
  if(!m_hasIDActivity) return true;
  int NumIDtracks = m_IDtrackCollection->size();
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Number of ID tracks" << NumIDtracks<< endreq;
  if (NumIDtracks>0) return true;
  
  return false;
}

