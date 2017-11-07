/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiCalc/LumiCalculator.h"

// GoodRunsLists
#include "GoodRunsLists/TMsgLogger.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGoodRunsListWriter.h"

// ROOT
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TROOT.h"

// MB 20100115: turn off for now, RootGraphics lib crashes on some non-cern sites.
//#include "TCanvas.h"

// stl includes
#include <iomanip>
#include <iostream>
#include <set>
#include <regex.h>

LumiCalculator::LumiCalculator()
 : m_logger( "LumiCalculator" )
 //, m_effxsec = 47.134 
 , m_lbstarttime(0.)
 , m_lbendtime(0.)

 , m_l1prescale(0.)
 , m_l2prescale(0.)
 , m_l3prescale(0.)
 , m_afterprescale(0)
 , m_livetime_beforeprescale(0)
 , m_livetime_afterprescale(0)
 , m_afterprescaleof(false)
 , m_beforeprescale(0)
 , m_beforeprescaleof(false)
 , m_runnbr(0)
 , m_lbstart(0)
 , m_lbstop(0)
 , m_lbstart_prev(0)
 , m_lbstop_prev(0)
 , m_runnbr_prev(0)
 , m_totalgoodblock(0)
 , m_totalbadblock(0)
 , m_clumiblocknbr(0)
 , m_clumiblocknbrend(0)
 , m_triglevel(0)
 , m_totalPrescaleWLiveTime(0.)
 , mt_totalPrescaleWLiveTime(0.)
 , m_totalPrescale(0.)
 , mt_totalPrescale(0.)
 , m_lumiWOPrescale(0.)
 , mt_lumiWOPrescale(0.)
 , m_lumiLAr(0.)
 , mt_lumiLAr(0.)
 , mt_totalDelL(0.)
 , mt_totalL(0.)
 , mt_totalLRun(0.)
 , mt_totaltime(0.)
 , mt_deltaT(0.)
 , mt_l1acc(0)
 , mt_l2acc(0)
 , mt_l3acc(0)
 , mt_totalgoodblock(0)
 , mt_totalbadblock(0)
 , mt_totall1befpresc(0)
 , m_lartime(0.)
 , m_larfrac(0.)
 , m_bsvalid(0.)

 , m_effxsec(1.)
 , m_l1rate(0.)
 , m_l2rate(0.)
 , m_l3rate(0.)
 , m_l1ratediveffxsec(0.)
 , m_total_l1ratediveffxsec(0.)
 , m_total_l1ratediveffxsecRun(0.)
 , m_l1ratediveffxsec_recorded(0.)
 , m_total_l1ratediveffxsec_recorded(0.)
 , m_total_l1ratediveffxsecRun_recorded(0.)
 , m_mintrigrate(5./120.)
 , m_collsgrl(0)
 , m_ntrigplb(0)
 , m_trigrateplb(0)
 , m_lumiplb(0)
 , m_lumitrigrateplb(0)
 , m_intlumi(0)
 , m_intlumitrigrate(0)
 , m_lumitrigrateplb_recorded(0)
 , m_intlumitrigrate_recorded(0)
 , m_intlumiruns(0)
 , m_intlumitrigrateruns(0)
 , m_intlumitrigrateruns_recorded(0)
 , m_avgintperbx(0)
 , m_makePlots(false)
 , m_makecollList(false)
 , Lumiid(0)
 , L3id(0)
 , L2id(0)
 , L1id(0)
 , LiveL1id(0)
 , L1Valid(false)
 , L2Valid(false)
 , L3Valid(false)
 , LiveValid(false)
 , m_onlinelumi(false)
 , m_uselar(false)
 , m_usebs(false)
 , m_minrun(0)
 , m_maxrun(0)
{

  m_trigger="COOLONL_TRIGGER/";
  m_lumioff="COOLOFL_TRIGGER/";
  m_lumionl="COOLONL_TRIGGER/";
  m_laroff="COOLOFL_LAR/"; 
  m_bsonl="COOLONL_INDET/";

  // These now need to be set after we know the run number (Run1 or Run2)
  m_data_db="";
  m_parofflumiestfolder = "";
  m_paronllumiestfolder = "";
  m_parlumiestfolder = "";
  m_parlvl1menufolder = "/TRIGGER/LVL1/Menu";
  m_parhltmenufolder = "/TRIGGER/HLT/Menu";// ChainCounter is here for COOLONL_TRIGGER/COMP20
  m_parhltprescalesfolder = "/TRIGGER/HLT/Prescales";// ChainCounter is here for COOLONL_TRIGGER/COMP20
  m_parlumilvl1folder = "/TRIGGER/LUMI/LVL1COUNTERS";
  m_parlumihltfolder = "/TRIGGER/LUMI/HLTCOUNTERS";
  m_parlvl1prescalesfolder = "/TRIGGER/LVL1/Prescales";
  m_parlvl1lblbfolder = "/TRIGGER/LUMI/LBLB";// for time information
  m_parlareventvetofolder = "/LAR/BadChannelsOfl/EventVeto"; // For LAr event veto
  m_paronlbeamspotfolder = "/Indet/Onl/Beampos"; // For invalid online beamspot

  m_uselivetrigger = false;
  m_verbose = false;
  m_lbcollname = "LumiBlocks";

  m_bstag="IndetBeamposOnl-HLT-UPD1-001-00";

  // by default we use the "offline data" database name
  lumi_database = m_lumioff + m_data_db;
  trig_database = m_trigger + m_data_db;
  lar_database = m_laroff + m_data_db;
  bs_database = m_bsonl + m_data_db;

  m_lumitag = "OflLumi-8TeV-002";// for offline:  OflLumi_CosmicFake, OflLumi_TopMix
  m_lumichannel = 0;
  m_lumimethod = "";// offline channels: ATLAS_PREFERRED, OflLumi_Fake0, OflLumi_Fake:, TopMixLumi 
  m_State = true;
  m_LumiTree = 0;
  
}


LumiCalculator::~LumiCalculator(){

  // delete collisions grl
  if (m_collsgrl!=0) { delete m_collsgrl; m_collsgrl=0; }

  std::vector<TH1F*>::iterator itr;
  for (itr=m_ntrigplbVec.begin(); itr!=m_ntrigplbVec.end(); ++itr) { delete *itr; }
  for (itr=m_trigrateplbVec.begin(); itr!=m_trigrateplbVec.end(); ++itr) { delete *itr; }
  for (itr=m_lumiplbVec.begin(); itr!=m_lumiplbVec.end(); ++itr) { delete *itr; }
  for (itr=m_lumitrigrateplbVec.begin(); itr!=m_lumitrigrateplbVec.end(); ++itr) { delete *itr; }
  for (itr=m_intlumiVec.begin(); itr!=m_intlumiVec.end(); ++itr) { delete *itr; }
  for (itr=m_intlumitrigrateVec.begin(); itr!=m_intlumitrigrateVec.end(); ++itr) { delete *itr; }
  for (itr=m_lumitrigrateplb_recordedVec.begin(); itr!=m_lumitrigrateplb_recordedVec.end(); ++itr) { delete *itr; }
  for (itr=m_intlumitrigrate_recordedVec.begin(); itr!=m_intlumitrigrate_recordedVec.end(); ++itr) { delete *itr; }

  m_ntrigplbVec.clear();
  m_trigrateplbVec.clear();
  m_lumiplbVec.clear();
  m_lumitrigrateplbVec.clear();
  m_intlumiVec.clear();
  m_intlumitrigrateVec.clear();
  m_lumitrigrateplb_recordedVec.clear();
  m_intlumitrigrate_recordedVec.clear();

  if (m_intlumiruns!=0) { delete m_intlumiruns; m_intlumiruns=0; }
  if (m_intlumitrigrateruns!=0) { delete m_intlumitrigrateruns; m_intlumitrigrateruns=0; }
  if (m_intlumitrigrateruns_recorded!=0) { delete m_intlumitrigrateruns_recorded; m_intlumitrigrateruns_recorded=0; }
  if (m_avgintperbx != 0) { delete m_avgintperbx; m_avgintperbx = 0; }

}

void LumiCalculator::setTree(TTree * tree){
  // Register branches
  m_LumiTree = tree;
  if(m_LumiTree != 0){

    m_LumiTree->Branch("LBStartTime/D", &m_lbstarttime);
    m_LumiTree->Branch("LBEndTime/D", &m_lbendtime);
    m_LumiTree->Branch("Trigger", &m_triggerchain);
    m_LumiTree->Branch("LBCollection", &m_lbcollname);
    m_LumiTree->Branch("RunNbr", &m_runnbr);
    m_LumiTree->Branch("IOVRStart", &m_lbstart);
    m_LumiTree->Branch("IOVREnd", &m_lbstop);
    m_LumiTree->Branch("LBStart", &m_clumiblocknbr);
    //  m_LumiTree->Branch("LBEnd", &m_clumiblocknbrend);
    m_LumiTree->Branch("Inst_m_Lumi", &m_instLumi);
    m_LumiTree->Branch("LiveTime", &m_livetime);
    m_LumiTree->Branch("L1Presc", &m_l1prescale);
    m_LumiTree->Branch("L2Presc", &m_l2prescale);
    m_LumiTree->Branch("L3Presc", &m_l3prescale);
    m_LumiTree->Branch("L1Count", &m_l1acc);
    m_LumiTree->Branch("L1CountOverFlow", &m_l1accof);
    m_LumiTree->Branch("L2Count", &m_l2acc);
    m_LumiTree->Branch("L3Count", &m_l3acc);
    m_LumiTree->Branch("L1AfterPrescale", &m_afterprescale);
    m_LumiTree->Branch("L1AfterPrescaleOverFlow", &m_afterprescaleof);
    m_LumiTree->Branch("L1BeforePrescale", &m_beforeprescale);
    m_LumiTree->Branch("L1BeforePrescaleOverFlow", &m_beforeprescaleof);
    m_LumiTree->Branch("Livefrac", &m_livefrac);
    m_LumiTree->Branch("LArfrac", &m_larfrac);
    m_LumiTree->Branch("DeltaT", &m_deltaT);
    m_LumiTree->Branch("L1Rate", &m_l1rate);
    m_LumiTree->Branch("IntLumi",&m_intLumi);
    m_LumiTree->Branch("L1Ratediveffxsec",&m_l1ratediveffxsec);
    m_LumiTree->Branch("TotalLumi",&m_totalL);
    m_LumiTree->Branch("Total_L1Ratediveffxsec",&m_total_l1ratediveffxsec);
    m_LumiTree->Branch("TotalLumiRun",&m_totalLRun);
    m_LumiTree->Branch("Total_L1RatediveffxsecRun",&m_total_l1ratediveffxsecRun);
    m_LumiTree->Branch("L1RatediveffxsecRecorded",&m_l1ratediveffxsec_recorded);
    m_LumiTree->Branch("Total_L1RatediveffxsecRecorded",&m_total_l1ratediveffxsec_recorded);
    m_LumiTree->Branch("Total_L1RatediveffxsecRunRecorded",&m_total_l1ratediveffxsecRun_recorded);
    m_LumiTree->Branch("AvergeInteractionPerXing",&m_AvEvtsPerBX);
    m_LumiTree->Branch("BSValid", &m_bsvalid);
  }
 
}


void LumiCalculator::SetCollName(const std::string& lbcollname){
  m_lbcollname = lbcollname;
}


void LumiCalculator::Verbose(bool verbose){
  m_verbose = verbose;
}

void LumiCalculator::UseMC(bool mc){

  // Print warning
  if (mc) 
    m_logger << Root::kWARNING << "Monte Carlo mode no longer supported!" << Root::GEndl;

}

void LumiCalculator::UseOnlineLumi(bool online){

  m_onlinelumi = online;

}

void LumiCalculator::UseLumiTag(const std::string& tag){
  m_lumitag = tag;
}

void LumiCalculator::UseLumiMethod(const std::string& method){
  m_lumimethod = method;
}

void LumiCalculator::UseLumiChannel(int chan){
  m_lumichannel = chan;
  m_lumimethod = "";
}


void LumiCalculator::UseLiveTrigger(bool live, std::string& livetrigger){
  m_uselivetrigger = live;
  m_livetrigger = livetrigger;
}

void LumiCalculator::UseLArNoiseDB(bool lar, const std::string& lartag) {
  m_uselar = lar;
  m_lartag = lartag;
}

void LumiCalculator::UseBeamspot(bool bs, const std::string& bstag) {
  m_usebs = bs;
  m_bstag = bstag;
}

TTree * LumiCalculator::getTree(){
  if(m_LumiTree != 0)return m_LumiTree;
  return 0;
}

//______________________________________________________________________________
void  LumiCalculator::IntegrateLumi(const xAOD::LumiBlockRangeContainer * iovc, const std::string& triggerchain){


  CoolQuery * cq_lumi = NULL;
  CoolQuery * cq_trigger = NULL;
  CoolQuery* cq_lar = NULL;
  CoolQuery* cq_bs = NULL;

  // Peek at first run in iovc to check if we want Run1 or Run2 data
  xAOD::LumiBlockRangeContainer::const_iterator it = iovc->begin();
  //  const IOVRange * iovr = (*it);
  const IOVRange* iovr = new IOVRange(IOVTime((*it)->startRunNumber(),(*it)->startLumiBlockNumber()), 
                                      IOVTime((*it)->stopRunNumber(),(*it)->stopLumiBlockNumber()));

  std::string onlfolder;
  std::string oflfolder;
  if (iovr->start().run() > 222222) {
    m_data_db="CONDBR2";
    onlfolder = "/TRIGGER/LUMI/OnlPrefLumi";
    oflfolder = "/TRIGGER/OFLLUMI/OflPrefLumi";
  } else {
    m_data_db="COMP200";
    onlfolder = "/TRIGGER/LUMI/LBLESTONL";
    oflfolder = "/TRIGGER/OFLLUMI/LBLESTOFL";
  }

  // define all connection strings

  // check for online DB
  if(m_onlinelumi){
    lumi_database = m_lumionl + m_data_db;
    m_parlumiestfolder = onlfolder;
  } else {
    lumi_database = m_lumioff + m_data_db;
    m_parlumiestfolder = oflfolder;
  }

  trig_database = m_trigger + m_data_db;
  lar_database = m_laroff + m_data_db;
  bs_database = m_bsonl + m_data_db;


  m_logger << Root::kINFO << "Luminosity database: " << lumi_database << Root::GEndl;
  m_logger << Root::kINFO << "Trigger database: " << trig_database << Root::GEndl;

  cq_lumi = new CoolQuery(lumi_database, triggerchain);
  if (!cq_lumi->openDbConn()) {
    delete cq_lumi;
    return;
  }

  cq_trigger = new CoolQuery(trig_database, triggerchain);
  if (!cq_trigger->openDbConn()) {
    delete cq_trigger;
    return;
  }

  if (m_uselar) {
    m_logger << Root::kINFO << "LAr database: " << lar_database << Root::GEndl;

    cq_lar = new CoolQuery(lar_database, triggerchain);
    if (!cq_lar->openDbConn()) {
      delete cq_lar;
      return;
    }
  }

  if (m_usebs) {
    m_logger << Root::kINFO << "Onl beamspot database: " << bs_database << Root::GEndl;

    cq_bs = new CoolQuery(bs_database, triggerchain);
    if (!cq_bs->openDbConn()) {
      delete cq_bs;
      return;
    }
  }

  // initialize
  m_lbstarttime = 0.;
  m_lbendtime = 0.;
  m_totalDelL = 0.;
  m_totalL = 0.;
  m_totalLRun = 0.;
  m_totaltime = 0.;
  m_instLumi = 0.;
  m_delLumi = 0.;
  m_intLumi = 0.;
  m_deltaT = 0.;
  m_totalPrescaleWLiveTime = 0.;
  m_totalPrescale = 0.;
  m_lumiWOPrescale = 0.;
  m_lumiLAr = 0.;
  m_TotaldeltaT = 0.;
  m_livefrac = 0.;
  m_livetime = 0.;
  m_lartime = 0.;
  m_larfrac = 0.;
  m_bsvalid = 0.;
  m_livetime_l1acc = 0;
  m_l1acc = 0;
  m_l2acc = 0;
  m_l3acc = 0;
  m_totall1acc = 0;
  m_livtrig_totall1acc = 0;
  m_totall2acc = 0;
  m_totall3acc = 0;
  m_l1prescale = 1.;
  m_l2prescale = 1.;
  m_l3prescale = 1.;
  m_livetime_beforeprescale = 0;
  m_livetime_afterprescale = 0;
  m_afterprescale = 0;
  m_beforeprescale = 0;
  m_totall1befpresc = 0;
  m_runnbr = 0;
  m_lbstart = 0;
  m_lbstop = 0;
  m_lbstart_prev = 0;
  m_lbstop_prev = 0;
  m_totalgoodblock = 0;
  m_totalbadblock = 0;
  m_clumiblocknbr = 0;
  m_clumiblocknbrend =0;
  m_triglevel = 0;
  m_lbcollectionname = "LumiBlocks";
  m_triggerchain = triggerchain;
  m_l1rate = 0.;
  m_l2rate = 0.;
  m_l3rate = 0.;
  m_l1ratediveffxsec = 0.;
  m_total_l1ratediveffxsec = 0.;
  m_total_l1ratediveffxsecRun = 0.;
  m_l1ratediveffxsec_recorded = 0.;
  m_total_l1ratediveffxsec_recorded = 0.;
  m_total_l1ratediveffxsecRun_recorded = 0.;
  m_runnbr_prev = 0;

  // Set to extreme values, to be updated below
  m_minrun = 99999999;
  m_maxrun = 0;

  bool firstL1Missing = true;
  bool firstHLTMissing = true;

  std::set<cool::ValidityKey> lbrunset;
  lbrunset.clear();
 
  // collisions xml file settings
  if (m_collsgrl!=0) { delete m_collsgrl; m_collsgrl=0; }
  if (m_makecollList) {
    m_collsgrl = new Root::TGoodRunsList();
    m_collsgrl->SetVersion("30"); // lumicalc signature
    m_collsgrl->AddMetaData("Query","Generated by LumiCalculator");
  }
 
  // Figure out trigger level  
  m_triglevel = cq_lumi->getTriggerLevel(triggerchain);
  if (m_triglevel == 0){
    // Check if on purpose, otherwise, give a warning
    if (triggerchain == "None") {
      m_logger << Root::kINFO << "No trigger specified!" << Root::GEndl;
    } else {
      m_logger << Root::kWARNING << "Invalid trigger: [" << triggerchain << "] - will proceed with no trigger defined!" << Root::GEndl;
    }
  }

  // Look for b-jet triggers
  regex_t regex;
  int reti = regcomp(&regex, "_[[:digit:]]?b[[:digit:]]+_", REG_EXTENDED);
  if (reti) m_logger << Root::kWARNING << "Could not compile regex!" << Root::GEndl;

  reti = regexec(&regex, triggerchain.c_str(), 0, NULL, 0);
  if ( !reti && !m_usebs) {
    m_logger << Root::kWARNING << "Trigger: [" << triggerchain << "] appears to be a b-jet trigger, but online beamspot validity not included in livefraction!" << Root::GEndl;
    m_logger << Root::kWARNING << "Probably need to specify --beamspot to get accurate luminosity!" << Root::GEndl;
  }
  regfree(&regex);

  // get lumi channel id (this never changes)
  if (m_lumimethod != "") {
    Lumiid = cq_lumi->getLumiChannelId(m_lumimethod, m_parlumiestfolder);
  } else {
    Lumiid = m_lumichannel;
  }


  // do main LumiBlock loop
  //==============================

  IOVData<cool::Int32> L1preObj;
  IOVData<cool::Float> L2preObj;
  IOVData<cool::Float> L3preObj;
  IOVData<cool::UInt32> LArObj;
  IOVData<cool::Int32> BSObj;

  std::map<cool::ValidityKey, CoolQuery::LumiFolderData> LumiDataMap;

  // UTC nanoseconds since 1970
  std::map<cool::ValidityKey, cool::UInt63> L1starttime_map;
  std::map<cool::ValidityKey, cool::UInt63> L1endtime_map;

  // Livetime maps
  std::map<cool::ValidityKey, CoolQuery::L1CountFolderData> Livetime_map;
  std::map<cool::ValidityKey, CoolQuery::L1CountFolderData> L1accept_map;

  for(xAOD::LumiBlockRangeContainer::const_iterator it = iovc->begin(); it != iovc->end(); ++it){
    //    const IOVRange * iovr = (*it);
    const IOVRange* iovr = new IOVRange(IOVTime((*it)->startRunNumber(),(*it)->startLumiBlockNumber()), 
                                        IOVTime((*it)->stopRunNumber(),(*it)->stopLumiBlockNumber()));
    
    // Bookkeeping temporary results
    mt_totalDelL = 0.;
    mt_totalL = 0.;
    mt_totalLRun = 0.;
    mt_totaltime = 0.;
    mt_deltaT = 0.;
    mt_l1acc = 0;
    mt_l2acc = 0;
    mt_l3acc = 0;
    mt_totalgoodblock = 0;
    mt_totalbadblock = 0;
    mt_totall1befpresc = 0;    
    mt_totalPrescaleWLiveTime = 0.;
    mt_totalPrescale = 0.;
    mt_lumiWOPrescale = 0. ;
    mt_lumiLAr = 0.;
  
    m_runnbr = iovr->start().run();
    m_lbstart = iovr->start().event();
    m_lbstop = iovr->stop().event();

    // Look for duplicate run/LB 
    if(m_lbstart_prev == m_lbstart && m_lbstop_prev == m_lbstop && m_runnbr_prev == m_runnbr){
      m_logger << Root::kWARNING << "Skipping multiply stored IOVRange: [" << m_lbstart << "-" << m_lbstop << "]" << Root::GEndl;
      continue;
    }

    // new run, reset its lumi and reload the trigger channels
    if ( m_runnbr!=m_runnbr_prev ) {

      if (m_runnbr < m_minrun) m_minrun = m_runnbr;
      if (m_runnbr > m_maxrun) m_maxrun = m_runnbr;

      m_totalLRun=0.;
      m_total_l1ratediveffxsecRun=0.;
      m_total_l1ratediveffxsecRun_recorded=0.;

      // Set IOV range for full run
      cq_trigger->setIOVForRun (m_runnbr);
      cq_lumi->setIOVForRun(m_runnbr);

      // get trigger channel ids
      L1Valid = false;
      L2Valid = false;
      L3Valid = false;
      LiveValid = false;
      m_triggerlowerchains.clear();

      std::string lowerch = "";

      // This is really inefficient...
      if(m_triglevel == 3){
	L3id = cq_trigger->getHLTChannelId(triggerchain, m_parhltmenufolder);
	L3Valid = cq_trigger->channelIdValid();

	lowerch =  cq_trigger->getHLTLowerChainName(triggerchain, m_parhltmenufolder); 
	m_triggerlowerchains.push_back(lowerch); 
	L2id = cq_trigger->getHLTChannelId(lowerch, m_parhltmenufolder);
	L2Valid = cq_trigger->channelIdValid();

	lowerch = cq_trigger->getHLTLowerChainName(lowerch, m_parhltmenufolder); 
	m_triggerlowerchains.push_back(lowerch);
	L1id = cq_trigger->getL1ChannelId(lowerch, m_parlvl1menufolder );
	L1Valid = cq_trigger->channelIdValid();

      }else if(m_triglevel == 2){
	lowerch =  cq_trigger->getHLTLowerChainName(triggerchain, m_parhltmenufolder); 
	m_triggerlowerchains.push_back(lowerch);
	L2id = cq_trigger->getHLTChannelId(triggerchain, m_parhltmenufolder);
	L2Valid = cq_trigger->channelIdValid();

	L1id = cq_trigger->getL1ChannelId(lowerch, m_parlvl1menufolder); 
	L1Valid = cq_trigger->channelIdValid();

      }else if(m_triglevel == 1){
	L1id = cq_trigger->getL1ChannelId(triggerchain, m_parlvl1menufolder);
	L1Valid = cq_trigger->channelIdValid();
	
      }

      if (firstL1Missing && (!L1Valid && m_triglevel > 0)) {
	firstL1Missing = false;
	cq_trigger->printL1Triggers(m_parlvl1menufolder);
      }

      if (firstHLTMissing && ((!L2Valid && m_triglevel > 1) || (!L3Valid && m_triglevel > 2)) ) {
	firstHLTMissing = false;
	cq_trigger->printHLTTriggers(m_parhltmenufolder);
      }

      // Should we check here for btag trigger and onl beamspot?

      // Do we use dedicated livetime trigger?
      if(m_uselivetrigger){
	LiveL1id = cq_trigger->getL1ChannelId(m_livetrigger, m_parlvl1menufolder);
	LiveValid = cq_trigger->channelIdValid();
      }else{
	// then fall back to the original trigger
	LiveL1id = L1id;
	LiveValid = L1Valid;
      }

      if (!LiveValid) {
	if (m_uselivetrigger)
	  m_logger << Root::kWARNING << "Runnumber: [" << m_runnbr << "] can't find livefraction trigger [" << m_livetrigger << "]!  Livefraction won't be calculated!" << Root::GEndl;
	else
	  m_logger << Root::kWARNING << "Runnumber: [" << m_runnbr << "] can't find trigger [" << triggerchain << "]!  Livefraction won't be calculated!" << Root::GEndl;
      }

      // Also load all deadtime counters here

      // Need to fix this for non-MC.  HLT counters now available, but not used here...
 
      //-------------------------
      // Load livetime information
      Livetime_map.clear();
      L1accept_map.clear();

      if (LiveValid) 
	Livetime_map = cq_trigger->getL1CountFolderData(m_parlumilvl1folder, LiveL1id);

      
      if (L1Valid && m_triglevel >= 1) 
	L1accept_map = cq_trigger->getL1CountFolderData(m_parlumilvl1folder, L1id);

      // UTC nanoseconds since 1970
      L1starttime_map.clear();
      L1endtime_map.clear();
      L1starttime_map = cq_trigger->getObjMapFromFolderAtChan<cool::UInt63>("StartTime", m_parlvl1lblbfolder, 0);
      L1endtime_map = cq_trigger->getObjMapFromFolderAtChan<cool::UInt63>("EndTime", m_parlvl1lblbfolder, 0);

      //---------------------------
      // Load LAr defects
      LArObj.clear();

      if (m_uselar) {
	cool::ValidityKey runstarttime = L1starttime_map.begin()->second;
	cool::ValidityKey runendtime = L1endtime_map.rbegin()->second;

	cq_lar->setIOV(runstarttime, runendtime); 
	LArObj = cq_lar->getIOVData<cool::UInt32>("EventVeto", m_parlareventvetofolder, 0, m_lartag);
      }

      //---------------------------
      // Load Onl beamspot
      BSObj.clear();

      if (m_usebs) {
	cq_bs->setIOVForRun(m_runnbr);
	BSObj = cq_bs->getIOVData<cool::Int32>("status", m_paronlbeamspotfolder, 0, m_bstag);
      }


      //-----------------------------
      // Load luminosity for this run
      LumiDataMap.clear();
      LumiDataMap = cq_lumi->getLumiFolderData(m_parlumiestfolder, m_lumitag, Lumiid); 

    
    } // End of new run stuff

    m_lbstart_prev = m_lbstart;
    m_lbstop_prev = m_lbstop;
    m_runnbr_prev = m_runnbr;
    
    // Update DB for this specific IOV range
    cq_trigger->setIOV(iovr->start().re_time(), iovr->stop().re_time());

    //----------------------------
    // Load prescales for this IOV
    L1preObj.clear();
    L2preObj.clear();
    L3preObj.clear();
    
    if(L1Valid) {
      L1preObj = cq_trigger->getIOVData<cool::Int32>("Lvl1Prescale", m_parlvl1prescalesfolder, L1id);
    }
      
    if(L2Valid) {
      L2preObj = cq_trigger->getIOVData<cool::Float>("Prescale", m_parhltprescalesfolder, 2*L2id);
    }    
      
    if(L3Valid) {
      L3preObj = cq_trigger->getIOVData<cool::Float>("Prescale", m_parhltprescalesfolder, 2*L3id+1);
    }

    // Reload the time map to get the ATLAS range
    L1starttime_map.clear();
    L1endtime_map.clear();
    L1starttime_map = cq_trigger->getObjMapFromFolderAtChan<cool::UInt63>("StartTime", m_parlvl1lblbfolder, 0);
    L1endtime_map = cq_trigger->getObjMapFromFolderAtChan<cool::UInt63>("EndTime", m_parlvl1lblbfolder, 0);

    // Print this here (will be output for each contiguous LB range in XML file)
    m_logger << Root::kINFO << std::left << "-----------------------------------" << Root::GEndl;
    m_logger << Root::kINFO << "Beginning calculation for ";
    if (m_triglevel > 0) {
      m_logger << Root::kINFO << "Trigger " << m_triggerchain;
      if(m_triggerlowerchains.size() > 0){
	m_logger << Root::kINFO << "["; 
	for(std::vector<std::string>::iterator sit = m_triggerlowerchains.begin(); sit != m_triggerlowerchains.end(); ++sit){
	  m_logger << Root::kINFO << "==>" << *sit;
	}
	m_logger << Root::kINFO << "], "; 
      }
    }
    m_logger << Root::kINFO << "Run " << m_runnbr << " LB [" << m_lbstart << "-" << m_lbstop << "]" << Root::GEndl;

    // Restrict lb range if necessary based on actual ATLAS run/lb values
    if (L1starttime_map.begin()->first > iovr->start().re_time() || L1starttime_map.rbegin()->first < iovr->stop().re_time()) {
      m_lbstart = (L1starttime_map.begin()->first & 0xFFFFFFFF);
      m_lbstop = (L1starttime_map.rbegin()->first & 0xFFFFFFFF);
      m_logger << Root::kINFO << "Restricting to valid ATLAS lumi block range [" << m_lbstart << "-" << m_lbstop << "]" << Root::GEndl;
    }


    //
    // Dont assume that all lumi blocks in this range are present in the lumi DB.
    // Loop over all lumi blocks specifically and flag any missing lumi entries as bad lumi blocks.
    //

    // Counters to check for missing LB ranges
    int firstMissing = -1;
    int lastMissing = -1;

    for (cool::ValidityKey currentVK = L1starttime_map.begin()->first; currentVK <= L1starttime_map.rbegin()->first; currentVK++) {
      //for(; itOL != LumiDataMap.end(); ++itOL){

      // Current ValidityKey:
      //cool::ValidityKey currentVK = itOL->first;
	  
      // Current IOVTime
      IOVTime curIOV;
      curIOV.setRETime(currentVK);

      //m_clumiblocknbr = (itOL->first & 0xFFFFFFFF);
      //m_clumiblocknbrend = ((itOL->first & 0xFFFFFFFF) + 1);
      m_clumiblocknbr = curIOV.event();
      m_clumiblocknbrend = curIOV.event()+1;

      // Check here for duplicate lumi blocks by explicitly keeping track of every run/lb seen
      if (lbrunset.count(curIOV.re_time()) != 0) {
	m_logger << Root::kWARNING << "Skipping duplicate [run,lumiblock]: " << curIOV << " !" << Root::GEndl;
	continue;
      } else {
	lbrunset.insert(curIOV.re_time());
      }
      
      // Not really good, just all seen
      m_totalgoodblock += 1;
      mt_totalgoodblock += 1;
      
      // Find luminosity record
      std::map<cool::ValidityKey, CoolQuery::LumiFolderData>::iterator itOL = LumiDataMap.find(currentVK);

      // Check if we didn't find anything
      if (itOL == LumiDataMap.end()) {

	mt_totalbadblock++;
	m_totalbadblock++;

	if (firstMissing < 0) {
	  // Is this the first missing one?  If so, make a note of it and go on
	  firstMissing = curIOV.event();
	  lastMissing = firstMissing;
	} else if (int(curIOV.event()) == (lastMissing+1)) {
	  // Is this a contiguous missing lumi block?
	  lastMissing = curIOV.event();
	} else {
	  // Not contiguous, print previous
	  if (firstMissing == lastMissing) {
	    m_logger << Root::kWARNING << "Luminosity info not found for Run " << m_runnbr << " LB " << firstMissing << " !" << Root::GEndl;
	  } else {
	    m_logger << Root::kWARNING << "Luminosity info not found for Run " << m_runnbr << " LB [" << firstMissing << "-" << lastMissing << "] !" << Root::GEndl;
	  }
	  firstMissing = curIOV.event();
	  lastMissing = firstMissing;
	}

	// If last time through loop, print this also
	if (currentVK == L1starttime_map.rbegin()->first) {
	  if (firstMissing == lastMissing) {
	    m_logger << Root::kWARNING << "Luminosity info not found for Run " << m_runnbr << " LB " << firstMissing << " !" << Root::GEndl;
	  } else {
	    m_logger << Root::kWARNING << "Luminosity info not found for Run " << m_runnbr << " LB [" << firstMissing << "-" << lastMissing << "] !" << Root::GEndl;
	  }
	  firstMissing = -1;
	  lastMissing = -1;
	}

	// Skip rest of this LB
	continue;

      } else {

	// Check if we had previous missing block
	if (firstMissing >= 0) {
	  if (firstMissing == lastMissing) {
	    m_logger << Root::kWARNING << "Luminosity info not found for Run " << m_runnbr << " LB " << firstMissing << " !" << Root::GEndl;
	  } else {
	    m_logger << Root::kWARNING << "Luminosity info not found for Run " << m_runnbr << " LB [" << firstMissing << "-" << lastMissing << "] !" << Root::GEndl;
	  }

	  firstMissing = -1;
	  lastMissing = -1;
	}

      }


      // given in units of ub^{-1} = 10^{30} cm^{-2}
      m_instLumi = (itOL->second).LBAvInstLumi;
      m_AvEvtsPerBX = (itOL->second).LBAvEvtsPerBX;
      m_Valid = (itOL->second).Valid;
	  

      // Clear values in case trigger isn't defined
      m_l1acc = 0.;
      m_beforeprescale = 0.;
      m_afterprescale = 0.;
      m_l2acc = 0.;
      m_l3acc = 0.;
      
      // Store dummy prescale values at start
      m_l1prescale = -1.;
      m_l2prescale = -1.;
      m_l3prescale = -1.;
      
      // Some trigger is defined.  Get prescales and values here
       if(L1Valid && m_triglevel > 0) {
	
	 // Get L1 prescale
	 m_l1prescale = L1preObj.getValue(curIOV);

	if (m_triglevel >=2) {
	  if(L2Valid) {

	    // Get L2 prescale
	    m_l2prescale = L2preObj.getValue(curIOV);

	  }
	  // Else, prescale stays at -1.
	  
	} else {
	  // Force L2 'passthrough'
	  m_l2prescale = 1.;
	}
	
	if(m_triglevel == 3){
	  if (L3Valid) {

	    // Get L3 prescale
	    m_l3prescale = L3preObj.getValue(curIOV);

	  }
	  // Else, L3 prescale stays at -1.
	} else {
	  // Force L3 'passthrough'
	  m_l3prescale = 1.;
	}
      }
      
      //-------------------------------
      // Calculate livetime from a dedicated not rare trigger if user requested
      CoolQuery::L1CountFolderData l1count = Livetime_map.find(currentVK)->second;

      m_livetime_beforeprescale = l1count.BeforePrescale;
      m_livetime_afterprescale = l1count.AfterPrescale;
      m_livetime_l1acc = l1count.L1Accept;
      if(m_livetime_afterprescale > 0.){
	m_livefrac = m_livetime_l1acc/(float)m_livetime_afterprescale;
      }else{
	m_livefrac = 0.0;
      }
      
      // Check for low statistics in afterprescale counts
      if(m_livetime_beforeprescale > 0 && m_livetime_afterprescale <= 0 ){
	std::string ttrig = "";
	ttrig = triggerchain;
	if(m_uselivetrigger)ttrig = m_livetrigger;
	m_logger << Root::kWARNING << "L1 counts after prescale (before veto) are 0.0 for trigger " << ttrig << "! Livefraction set to zero!" << Root::GEndl;
	m_logger << Root::kWARNING << "Try using a high rate L1 trigger for livetime calculation: --livetrigger=<high rate L1 trigger> " << Root::GEndl;
	m_logger << Root::kINFO << m_runnbr << "[" << m_clumiblocknbr << "]: L1Acc: " << m_l1acc << ", AfterPrescale: " << m_afterprescale  << ", L1Presc: " << m_l1prescale << Root::GEndl;
      }	    

      //------------------------
      // Calculate LAr veto time

      cool::ValidityKey lbstarttime = L1starttime_map.find(currentVK)->second;
      cool::ValidityKey lbendtime = L1endtime_map.find(currentVK)->second;

      m_lartime = 0.; // Time to exclude in seconds
      if (m_uselar) {

	IOVTime lbstart, lbend;
	lbstart.setRETime(lbstarttime);
	lbend.setRETime(lbendtime);
	IOVRange range(lbstart, lbend);

	std::list<std::pair<IOVRange, cool::UInt32> > larlist;
	larlist = LArObj.getOverlap(range);

	for (std::list<std::pair<IOVRange, cool::UInt32> >::iterator it = larlist.begin(); it != larlist.end(); it++) {
	  if (it->second == 0) continue;
	  float dtime = (it->first.stop().re_time() - it->first.start().re_time())/1.E9;
	  m_lartime += dtime;
	  if (m_verbose == true) {
	    m_logger << Root::kINFO << "Found LAr veto from " << it->first.start().re_time() << " to " << it->first.stop().re_time() << " = " << dtime << " seconds" << Root::GEndl;
	  }
	}
      }

      //------------------------
      // Check for online BS
      m_bsvalid = 1.;
      if (m_usebs) {

	// Read beamspot validity
	bool valid = true;
	int status = BSObj.getValue(curIOV);
	//m_logger << Root::kINFO << "Found online beamspot status = " << status << Root::GEndl;

	if (status != 7) valid = false;

	if (!valid) {
	  m_bsvalid = 0.0;
	  m_livefrac = 0.0;
	  if(m_verbose == true){
	    m_logger << Root::kINFO << m_runnbr << "[" << m_clumiblocknbr << "]: Online beamspot invalid with Lumi=" << m_instLumi << " 10^30 cm-2 s-1" << Root::GEndl;
	  }
	}
      }

      //-------------------------------

      l1count = L1accept_map.find(currentVK)->second;
      m_beforeprescale = l1count.BeforePrescale;
      m_beforeprescaleof = false;
      m_afterprescale = l1count.AfterPrescale;
      m_afterprescaleof = false;
      m_l1acc = l1count.L1Accept;
      m_l1accof = false;
      
      m_deltaT = (lbendtime-lbstarttime)/1.E9;
      m_lbstarttime = lbstarttime/1.E9;
      m_lbendtime = lbendtime/1.E9;

      if (m_deltaT > 0.) m_larfrac = 1.-m_lartime/m_deltaT;

      // For online lumi database case one needs to make some changes:
      if(m_onlinelumi == true){
	// In Valid UInt32 type value some information is encoded:
	// see: https://twiki.cern.ch/twiki/bin/view/Atlas/CoolOnlineData#Folder_TRIGGER_LUMI_LBLESTONL 
	cool::UInt32 tempValid = (m_Valid & 1023);
	    
	if(tempValid == 10){
	  // in this bits, Value 301 and 302 means MBTS and LUCID lumi for which some care is needed
	  if((m_Valid >> 22) == 301
	     || (m_Valid >> 22) == 302 
	     || (m_Valid >> 22) == 101
	     || (m_Valid >> 22) == 102
	     || (m_Valid >> 22) == 103
	     || (m_Valid >> 22) == 104
	     ){
	    m_Valid = 0;
	  }else{
	    m_Valid = tempValid;
	  }
	}else{
	  m_Valid = tempValid;
	}
	
      } else {
	
	// For offline, we also need to strip out the preferred channel value from the validity word
	// if (Lumiid == 0) m_Valid &= 0x3FF;
	m_Valid &= 0x3FF;
      }

      // Dump out debugging information
      if(m_verbose == true){
	m_logger << Root::kINFO << m_runnbr << "[" << m_clumiblocknbr << "]: L1Acc: " << m_l1acc;
	if(m_uselivetrigger) m_logger << ", Livetime trigger L1Acc: " << m_livetime_l1acc;
	m_logger << ", InstLumi: " << m_instLumi << ", deltaT: " << m_deltaT << ", AvEvtsPerBX: " << m_AvEvtsPerBX << ", BeforePrescale: " << m_beforeprescale << ", AfterPrescale: " << m_afterprescale;
	if (m_uselivetrigger) m_logger  << ", Livetime trigger BeforePrescale: " << m_livetime_beforeprescale << " Livetime trigger AfterPrescale: " << m_livetime_afterprescale;
	m_logger  << ", Livefrac: " << m_livefrac << ", L1Presc: " << m_l1prescale << ", L2Presc: " << m_l2prescale << ", L3Presc: " << m_l3prescale <<  ", Valid: " << m_Valid;
	if (m_uselar) m_logger << ", LAr ready fraction: " << m_larfrac;
	m_logger << Root::GEndl;
      }

      // Check if we have valid luminosity
      // Just need to check lowest digit.  10 -> BCID blind, 100 -> mu not valid
      if(m_Valid%10 != 0){
	  
	// Invalid luminosity entry, call it bad
	m_instLumi = 0.0;
	m_totalbadblock += 1;
	mt_totalbadblock += 1;
	if(m_verbose == true)m_logger << Root::kWARNING << "Skipping lumiblock " <<  m_runnbr << "[" << m_clumiblocknbr << "] with invalid inst. lumi.!" << Root::GEndl;
	
      } else if ((m_triglevel > 0) && (m_l1prescale < 0. || m_l2prescale < 0. || m_l3prescale < 0.)) {
	
	// Disabled trigger, call bad but still record delivered luminosity
	m_totalbadblock += 1;
	mt_totalbadblock += 1;
	if(m_verbose == true)m_logger << Root::kWARNING << "Lumiblock " <<  m_runnbr << "[" << m_clumiblocknbr << "] has a disabled or incorrectly specified trigger.! " << Root::GEndl;
	
      }

      //========================================================//
      //   L U M I N O S I T Y  C A L C U L A T I O N  H E R E  //
      //========================================================//
      // The actual calculation of integrated luminosity and 
      // accumulation of some variables
      m_totalDelL += (m_deltaT*m_instLumi); // delivered lumi
      mt_totalDelL += (m_deltaT*m_instLumi); // delivered lumi
      mt_deltaT += m_deltaT;
      
      // Count up everything
      m_totall1acc += m_l1acc;
      m_livtrig_totall1acc += m_livetime_l1acc;
      mt_l1acc += m_l1acc;
      m_totall1befpresc += m_beforeprescale;
      mt_totall1befpresc += m_beforeprescale;
      m_totall2acc += m_l2acc;
      mt_l2acc += m_l2acc;
      m_totall3acc += m_l3acc;
      mt_l3acc += m_l3acc;
      m_livetime = m_livefrac*m_deltaT;
      m_totaltime += m_livetime;
      mt_totaltime += m_livetime;
      
      double totalPrescale = m_l1prescale * m_l2prescale * m_l3prescale;
	
      // Check for disabled triggers
      if (m_l1prescale < 0. ) totalPrescale = 0.;
      if (m_l2prescale < 0. ) totalPrescale = 0.;
      if (m_l3prescale < 0. ) totalPrescale = 0.;
      
      // Check also for no trigger
      if (m_triglevel == 0) totalPrescale = 1.;
	
      m_intLumi = 0.;

      m_lumiWOPrescale += m_livetime*m_instLumi ;
      mt_lumiWOPrescale += m_livetime*m_instLumi ;

      m_lumiLAr += m_livetime*m_larfrac*m_instLumi;
      mt_lumiLAr += m_livetime*m_larfrac*m_instLumi;

      if (totalPrescale > 0.) {
	m_totalPrescaleWLiveTime += m_livetime/totalPrescale; 
	mt_totalPrescaleWLiveTime += m_livetime/totalPrescale; 
	m_totalPrescale += 1./totalPrescale;
	mt_totalPrescale += 1./totalPrescale;
	m_intLumi = m_larfrac * m_livetime * m_instLumi/totalPrescale; // <<<---  T H E  F O R M U L A
      } 


      m_totalL += m_intLumi;
      mt_totalL += m_intLumi; 
      m_totalLRun += m_intLumi;

      
      // MB: trigger rates, note that livefrac drops out of ratio
      m_l1rate = ( m_livetime>0 ? m_l1acc / m_livetime : 0. );
      m_l2rate = ( m_livetime>0 ? m_l2acc / m_livetime : 0. );
      m_l3rate = ( m_livetime>0 ? m_l3acc / m_livetime : 0. );
      
      // MB: delivered lumi
      m_l1ratediveffxsec = (float)m_afterprescale/( m_deltaT*m_effxsec );
      m_total_l1ratediveffxsec += (float)m_afterprescale / m_effxsec ;
      m_total_l1ratediveffxsecRun += (float)m_afterprescale / m_effxsec ;
      
      // MB: recorded lumi
      m_l1ratediveffxsec_recorded = (float)m_l1acc /( m_deltaT*m_effxsec );
      m_total_l1ratediveffxsec_recorded += (float)m_l1acc / m_effxsec ;
      m_total_l1ratediveffxsecRun_recorded += (float)m_l1acc / m_effxsec ;
      
      if (m_collsgrl!=0) {
	if ( m_l1rate>=m_mintrigrate )
	  m_collsgrl->AddRunLumiBlock(m_runnbr,m_clumiblocknbr);
      }
      
      if (m_verbose) {
	if (m_effxsec!=1.0)
	  m_logger << Root::kINFO << "L1rate a/ prescale: " << m_afterprescale << ", Delivered LumiFromL1rate (/ub): " << m_l1ratediveffxsec << ", Delivered TotalLumiFromL1rate (/ub): " << m_total_l1ratediveffxsec
		   << Root::GEndl;
      }
      
      if(m_LumiTree != 0)m_LumiTree->Fill();
    } // End of loop over lumi blocks

    // Print IOV summary
    m_logger << Root::kINFO << std::left << "-----------------------------------" << Root::GEndl;
    m_logger << Root::kINFO<< std::setw(10) << std::left << ">== Trigger  : " << triggerchain << Root::GEndl;
    m_logger << Root::kINFO<< std::setw(10) << std::right << "Run" <<  std::setw(10) << std::right << "L1-Acc" << std::setw(10) << std::right << "L2-Acc" << std::setw(10) << std::right <<  "L3-Acc" << std::setw(10) << std::right <<  "LiveTime" << std::setw(18) << std::right <<  "IntL rec.(ub^-1)" << std::setw(18) << std::right <<  "IntL del.(ub^-1)" << Root::GEndl;
    m_logger << Root::kINFO<< std::setw(10) << std::right << m_runnbr << std::setw(10) << std::right << mt_l1acc << std::setw(10) << std::right << mt_l2acc << std::setw(10) << std::right << mt_l3acc << std::setw(10) << std::right << mt_totaltime << std::setw(18) << std::right << mt_totalL << std::setw(18) << std::right << mt_totalDelL << Root::GEndl;
    //    m_logger << Root::kINFO << std::setw(10) << std::right << "BeforePrescale" << std::setw(10) << std::right << m_totall1befpresc << std::setw(10) << std::right << "" << std::setw(10) << std::right << "" << std::setw(10) << std::right << "" << std::setw(10) << std::right << "" << std::setw(10) << std::right << "" << std::setw(10) << m_TotaldeltaT << std::setw(14) << std::right << m_totalL << Root::GEndl;
    
    m_logger << Root::kINFO<< std::setw(10) << std::left << "L1/2/3 accept: " <<  std::setw(10) << std::left << mt_l1acc <<  std::setw(10) << std::left << mt_l2acc <<  std::setw(10) << std::left << mt_l3acc << Root::GEndl;
    m_logger << Root::kINFO << std::setw(10) << std::left << "L1BeforePresc: " << std::setw(10) << std::left << mt_totall1befpresc << Root::GEndl;
    m_logger << Root::kINFO << std::setw(10) << std::left << "Livetime     : " << mt_totaltime << Root::GEndl;
    m_logger << Root::kINFO << std::setw(10) << std::left << "Prescale Weighted Livetime: " <<  mt_totalPrescaleWLiveTime << Root::GEndl; 
    m_logger << Root::kINFO<< std::setw(10)  << std::left << "Good LBs     : " << mt_totalgoodblock - mt_totalbadblock << Root::GEndl;
    m_logger << Root::kINFO<< std::setw(10)  << std::left << "Bad LBs      : " << mt_totalbadblock << Root::GEndl;
    
    if ( m_effxsec==1.0 ) {
      m_logger << Root::kINFO << std::setw(10) << std::left << "IntL delivered (ub^-1) : " << mt_totalDelL << Root::GEndl;
      m_logger << Root::kINFO << std::setw(10) << std::left << "IntL after livefraction (ub^-1):  " <<  mt_lumiWOPrescale << Root::GEndl;
      if (m_uselar)
	m_logger << Root::kINFO << std::setw(10) << std::left << "IntL after LAr fraction (ub^-1):  " <<  mt_lumiLAr << Root::GEndl;
      m_logger << Root::kINFO << std::setw(10) << std::left << "IntL recorded after prescale (ub^-1) : " << mt_totalL << Root::GEndl;
    } else {
      m_logger << Root::kINFO << std::setw(10) << std::left << "IntL delived  (ub^-1) : " << m_total_l1ratediveffxsec << Root::GEndl;
      m_logger << Root::kINFO << std::setw(10) << std::left << "IntL recorded (ub^-1) : " << m_total_l1ratediveffxsec_recorded << Root::GEndl;
    }

    // Print prescales as range of actual lumi blocks these apply to
    if(m_triglevel >= 1){
      // Print L1 Prescale values:
      m_logger << Root::kINFO << std::setw(10) << std::left << "L1 Prescales: ";

      std::list< std::pair<IOVRange, cool::Int32> >::iterator it;
      for(it = L1preObj.data.begin(); it != L1preObj.data.end(); it++) {
	m_logger << Root::kINFO << std::setw(1) << std::left << "[" << it->first.start().event() << "," << it->first.stop().event()-1 << "]:" << it->second <<  ", "; 
      }
      m_logger << Root::kINFO << Root::GEndl;
    }
    
    if(m_triglevel >= 2){
      // Print L2 Prescale values:
      m_logger << Root::kINFO << std::setw(10) << std::left << "L2 Prescales: ";

      std::list< std::pair<IOVRange, cool::Float> >::iterator it;
      for(it = L2preObj.data.begin(); it != L2preObj.data.end(); it++) {
	m_logger << Root::kINFO << std::setw(1) << std::left << "[" << it->first.start().event() << "," << it->first.stop().event()-1 << "]:" << it->second <<  ", "; 
      }
      m_logger << Root::kINFO << Root::GEndl;
    }
    
    if(m_triglevel == 3){
      // Print L3 Prescale values:
      m_logger << Root::kINFO << std::setw(10) << std::left << "L3 Prescales: ";

      std::list< std::pair<IOVRange, cool::Float> >::iterator it;
      for(it = L3preObj.data.begin(); it != L3preObj.data.end(); it++) {
	m_logger << Root::kINFO << std::setw(1) << std::left << "[" << it->first.start().event() << "," << it->first.stop().event()-1 << "]:" << it->second <<  ", "; 
      }
      m_logger << Root::kINFO << Root::GEndl;
    }  

  } // end lb collection loop


  // ------------------------------------------------------------------------------------------------
  // MB : Print total only at end of LB loop:
  m_logger << Root::kINFO << std::left << "-----------------------------------" << Root::GEndl;
  m_logger << Root::kINFO << std::left << "  LumiCalculator summary" << Root::GEndl;
  m_logger << Root::kINFO << std::left << "-----------------------------------" << Root::GEndl;
  m_logger << Root::kINFO<< std::setw(10) << std::right << "Total" <<  std::setw(10) << std::right << "L1-Acc" << std::setw(10) << std::right << "L2-Acc" << std::setw(10) << std::right <<  "L3-Acc" <<
              std::setw(10) << std::right <<  "LiveTime" << std::setw(18) << std::right <<  "IntL rec.(ub^-1)" << std::setw(18) << std::right <<  "IntL del.(ub^-1)" << Root::GEndl;
  m_logger << Root::kINFO<< std::setw(10) << std::right << "" << std::setw(10) << std::right << m_totall1acc << std::setw(10) << std::right << m_totall2acc << std::setw(10) << std::right 
           << m_totall3acc << std::setw(10) << std::right << m_totaltime << std::setw(18) << std::right << m_totalL << std::setw(18) << std::right << m_totalDelL << Root::GEndl;
  m_logger << Root::kINFO<< std::setw(10) << std::left << "Total L1/2/3 accept: " <<  std::setw(10) << std::left << m_totall1acc <<  std::setw(10) << std::left << m_totall2acc <<  std::setw(10) 
           << std::left << m_totall3acc << Root::GEndl;
  if(m_uselivetrigger)m_logger << Root::kINFO<< std::setw(10) << std::left << "Total L1 livetime trigger accept: " <<  std::setw(10) << std::left << m_livtrig_totall1acc << Root::GEndl;

  m_logger << Root::kINFO << std::setw(10) << std::left << "First Run: " << std::setw(10) << std::left << m_minrun << Root::GEndl;
  m_logger << Root::kINFO << std::setw(10) << std::left << "Last Run: " << std::setw(10) << std::left << m_maxrun << Root::GEndl;
  m_logger << Root::kINFO << std::setw(10) << std::left << "Total L1BeforePresc: " << std::setw(10) << std::left << m_totall1befpresc << Root::GEndl;
  m_logger << Root::kINFO<< std::setw(10) << std::left  << "Total Livetime     : " << m_totaltime << Root::GEndl;
  m_logger << Root::kINFO << std::setw(10) << std::left << "Total prescale weighted Livetime: " <<  m_totalPrescaleWLiveTime << Root::GEndl;
  m_logger << Root::kINFO<< std::setw(10) << std::left  << "Total Good LBs     : " << m_totalgoodblock - m_totalbadblock << Root::GEndl;
  m_logger << Root::kINFO<< std::setw(10) << std::left  << "Total Bad LBs     : " << m_totalbadblock << Root::GEndl;
  m_logger << Root::kINFO << std::setw(10) << std::left << "Total IntL delivered (ub^-1) : " << m_totalDelL << Root::GEndl;
  m_logger << Root::kINFO << std::setw(10) << std::left << "Total IntL after livefraction (ub^-1):  " <<  m_lumiWOPrescale << Root::GEndl;
  if (m_uselar)
    m_logger << Root::kINFO << std::setw(10) << std::left << "Total IntL after LAr fraction (ub^-1):  " <<  m_lumiLAr << Root::GEndl;
  m_logger << Root::kINFO << std::setw(10) << std::left << "Total IntL recorded (ub^-1) : " << m_totalL << Root::GEndl;

  // ------------------------------------------------------------------------------------------------

  if(m_makecollList == true){
    // store collisions xml file on demand
    if (m_collsgrl!=0) {
      TString collisionsxml = "collisions_" + m_collsgrl->GetSuggestedName() + ".xml";
      Root::TGoodRunsListWriter writer; 
      writer.SetGoodRunsList( *m_collsgrl );
      writer.SetFilename( collisionsxml.Data() );
      writer.WriteXMLFile();
      // can now delete grl
      delete m_collsgrl; m_collsgrl=0;
    }
  }


  // Creating monitoring plots on demand
  if(m_makePlots == true) { this->MakePlots(triggerchain); }

  delete cq_trigger;
  delete cq_lumi;
  delete cq_lar;
  delete cq_bs;
}

// ---------------------------------------------------------------------------------
// Utility to print lumicalc summary results
void
LumiCalculator::printSummary(std::ostream& os) {

  os << std::left << "-----------------------------------" << std::endl;
  os << std::left << "  LumiCalculator summary" << std::endl;
  os << std::left << "-----------------------------------" << std::endl;
  os << std::setw(10) << std::left << "Trigger: " << std::setw(10) << std::left << m_triggerchain << std::endl;
  os << std::setw(10) << std::left << "First Run: " << std::setw(10) << std::left << m_minrun << std::endl;
  os << std::setw(10) << std::left << "Last Run: " << std::setw(10) << std::left << m_maxrun << std::endl;
  os << std::setw(10) << std::left << "Total L1BeforePresc: " << std::setw(10) << std::left << m_totall1befpresc << std::endl;
  os << std::setw(10) << std::left  << "Total Livetime     : " << m_totaltime << std::endl;
  os << std::setw(10) << std::left << "Total prescale weighted Livetime: " <<  m_totalPrescaleWLiveTime << std::endl;
  os << std::setw(10) << std::left  << "Total Good LBs     : " << m_totalgoodblock - m_totalbadblock << std::endl;
  os << std::setw(10) << std::left  << "Total Bad LBs     : " << m_totalbadblock << std::endl;
  os << std::setw(10) << std::left << "Total IntL delivered (ub^-1) : " << m_totalDelL << std::endl;
  os << std::setw(10) << std::left << "Total IntL after livefraction (ub^-1):  " <<  m_lumiWOPrescale << std::endl;
  if (m_uselar)
    os << std::setw(10) << std::left << "Total IntL after LAr fraction (ub^-1):  " <<  m_lumiLAr << std::endl;
  os << std::setw(10) << std::left << "Total IntL recorded (ub^-1) : " << m_totalL << std::endl;

}

void 
LumiCalculator::DoHistogramAdmin(const uint32_t& runnbr, const TString& trigName, const float& effxsec)
{
  // rebin the histograms once number of LBs is known
  int maxlb = 5000;

  m_ntrigplbVec.push_back(        new TH1F(Form("run%d_ntrigplb",runnbr),            Form("Run %d",runnbr) , maxlb, 0., float(maxlb)) );
  m_trigrateplbVec.push_back(     new TH1F(Form("run%d_trigrateplb",runnbr),         Form("Run %d",runnbr) , maxlb, 0., float(maxlb)) );
  m_lumiplbVec.push_back(         new TH1F(Form("run%d_peaklumiplb",runnbr),         Form("Run %d",runnbr) , maxlb, 0., float(maxlb)) );
  m_lumitrigrateplbVec.push_back( new TH1F(Form("run%d_peaklumitrigrateplb",runnbr), Form("Run %d",runnbr) , maxlb, 0., float(maxlb)) );
  m_intlumiVec.push_back(         new TH1F(Form("run%d_intlumi",runnbr),             Form("Run %d",runnbr) , maxlb, 0., float(maxlb)) );
  m_intlumitrigrateVec.push_back( new TH1F(Form("run%d_intlumitrigrate",runnbr),     Form("Run %d",runnbr) , maxlb, 0., float(maxlb)) );
  m_lumitrigrateplb_recordedVec.push_back( new TH1F(Form("run%d_peakrecordedlumitrigrateplb",runnbr), Form("Run %d",runnbr) , maxlb, 0., float(maxlb)) );
  m_intlumitrigrate_recordedVec.push_back( new TH1F(Form("run%d_intrecordedlumitrigrate",runnbr),     Form("Run %d",runnbr) , maxlb, 0., float(maxlb)) );

  m_ntrigplb        = *(m_ntrigplbVec.rbegin());
  m_trigrateplb     = *(m_trigrateplbVec.rbegin());
  m_lumiplb         = *(m_lumiplbVec.rbegin());
  m_lumitrigrateplb = *(m_lumitrigrateplbVec.rbegin());
  m_intlumi         = *(m_intlumiVec.rbegin());
  m_intlumitrigrate = *(m_intlumitrigrateVec.rbegin());
  m_lumitrigrateplb_recorded = *(m_lumitrigrateplb_recordedVec.rbegin());
  m_intlumitrigrate_recorded = *(m_intlumitrigrate_recordedVec.rbegin());

  this->SetHistogramStyle(m_ntrigplb,        Form("Run = %d",runnbr), "Luminosity block number", Form("# %s triggers / LB",trigName.Data()));
  this->SetHistogramStyle(m_trigrateplb,     Form("Run = %d",runnbr), "Luminosity block number", Form("%s trigger rate / LB",trigName.Data()));
  this->SetHistogramStyle(m_lumiplb,         Form("Run = %d",runnbr), "Luminosity block number", "Delivered luminosity (#mub^{-1}/s)");
  this->SetHistogramStyle(m_lumitrigrateplb, Form("Efficiency * x-sec = %.1f #mub, Run = %d",effxsec,runnbr), "Luminosity block number", Form("%s luminosity (#mub^{-1}/s)",trigName.Data()));
  this->SetHistogramStyle(m_intlumi,         Form("Run = %d",runnbr), "Luminosity block number", "Integrated delivered luminosity (#mub^{-1})");
  this->SetHistogramStyle(m_intlumitrigrate, Form("Efficiency * x-sec = %.1f #mub, Run = %d",effxsec,runnbr), "Luminosity block number", Form("%s Integrated luminosity (#mub^{-1})",trigName.Data()));
  this->SetHistogramStyle(m_lumitrigrateplb_recorded, Form("Efficiency * x-sec = %.1f #mub, Run = %d",effxsec,runnbr), "Luminosity block number", Form("%s Recorded luminosity (#mub^{-1}/s)",trigName.Data()));
  this->SetHistogramStyle(m_intlumitrigrate_recorded, Form("Efficiency * x-sec = %.1f #mub, Run = %d",effxsec,runnbr), "Luminosity block number", Form("%s Integrated recorded luminosity (#mub^{-1})",trigName.Data()));
}


void
LumiCalculator::RebinHistograms(const int& nbins, const double& start, const double& end)
{
  m_ntrigplb->SetBins(nbins,start,end);        
  m_trigrateplb->SetBins(nbins,start,end);     
  m_lumiplb->SetBins(nbins,start,end);         
  m_lumitrigrateplb->SetBins(nbins,start,end); 
  m_intlumi->SetBins(nbins,start,end);         
  m_intlumitrigrate->SetBins(nbins,start,end); 
  m_lumitrigrateplb_recorded->SetBins(nbins,start,end);
  m_intlumitrigrate_recorded->SetBins(nbins,start,end);
}


void 
LumiCalculator::SetHistogramStyle(TH1F* hist, const char* title, const char* xaxis, const char* yaxis)
{
  hist->SetFillColor(33); // light grey, blueish
  if (title!=0) hist->SetTitle(title);
  if (xaxis!=0) hist->GetXaxis()->SetTitle(xaxis);
  if (yaxis!=0) hist->GetYaxis()->SetTitle(yaxis);

  hist->GetXaxis()->SetLabelFont(52);
  hist->GetXaxis()->SetLabelSize(0.04);
  hist->GetXaxis()->SetTitleSize(0.05);
  hist->GetXaxis()->SetTitleOffset(1.28);
  hist->GetXaxis()->SetTitleFont(42);

  hist->GetYaxis()->SetLabelFont(52);
  hist->GetYaxis()->SetLabelSize(0.04);
  hist->GetYaxis()->SetTitleSize(0.05);
  hist->GetYaxis()->SetTitleOffset(1.25);
  hist->GetYaxis()->SetTitleFont(42);

  hist->SetStats(false);

  hist->SetLineWidth(2);
}


/*
// MB 20100115: turn off for now, RootGraphics lib crashes on some non-cern sites.
TCanvas* 
LumiCalculator::GetNiceCanvas(const char* name, const char* title)
{
  TCanvas *tcan = new TCanvas(name,title,4,45,800,600);
  //gStyle->SetOptStat(0);
  //tcan->SetHighLightColor(1);
  tcan->Range(500.0, 100.0, 1000.,1000.);
  //Int_t ci = TColor::GetColor("#ffffff");
  tcan->SetFillColor(0);
  tcan->SetBorderMode(0);
  tcan->SetBorderSize(0);
  tcan->SetGridx();
  tcan->SetGridy();
  tcan->SetLeftMargin(0.14);
  tcan->SetRightMargin(0.14);
  tcan->SetBottomMargin(0.15);
  tcan->SetFrameFillColor(0);

  return tcan;
}
*/

void
LumiCalculator::MakePlots(const std::string& triggerchain)
{
  if(m_LumiTree != 0){
    // rebin and fill histograms:
    m_LumiTree->SetBranchAddress("LBStart", &m_clumiblocknbr);
    m_LumiTree->SetBranchAddress("L1AfterPrescale", &m_afterprescale);
    m_LumiTree->SetBranchAddress("L1Rate", &m_l1rate);
    m_LumiTree->SetBranchAddress("IntLumi",&m_intLumi);
    m_LumiTree->SetBranchAddress("L1Ratediveffxsec",&m_l1ratediveffxsec);
    m_LumiTree->SetBranchAddress("TotalLumi",&m_totalL);
    m_LumiTree->SetBranchAddress("Total_L1Ratediveffxsec",&m_total_l1ratediveffxsec);
    m_LumiTree->SetBranchAddress("TotalLumiRun",&m_totalLRun);
    m_LumiTree->SetBranchAddress("Total_L1RatediveffxsecRun",&m_total_l1ratediveffxsecRun);
    m_LumiTree->SetBranchAddress("RunNbr", &m_runnbr);
    m_LumiTree->SetBranchAddress("L1RatediveffxsecRecorded",&m_l1ratediveffxsec_recorded);
    m_LumiTree->SetBranchAddress("Total_L1RatediveffxsecRecorded",&m_total_l1ratediveffxsec_recorded);
    m_LumiTree->SetBranchAddress("Total_L1RatediveffxsecRunRecorded",&m_total_l1ratediveffxsecRun_recorded);
    m_LumiTree->SetBranchAddress("AvergeInteractionPerXing",&m_AvEvtsPerBX);
    
    // get first and last run number
    m_LumiTree->GetEntry(0); 
    int runnbrstart = m_runnbr;
    m_LumiTree->GetEntry(m_LumiTree->GetEntries()-1); 
    int runnbrend = m_runnbr;
    
    // makeup of integrated lumi histograms
    m_intlumiruns = new TH1F("intlumiruns","Luminosity",1,0.,1.);
    m_intlumitrigrateruns = new TH1F("intlumitrigrateruns","Delivered luminosity",1,0.,1.);
    m_intlumitrigrateruns_recorded = new TH1F("intlumitrigrateruns_recorded","Recorded luminosity",1,0.,1.);
    m_intlumiruns->SetBins(runnbrend-runnbrstart+10,float(runnbrstart),float(runnbrend+10));
    m_intlumitrigrateruns->SetBins(runnbrend-runnbrstart+10,float(runnbrstart),float(runnbrend+10));
    m_intlumitrigrateruns_recorded->SetBins(runnbrend-runnbrstart+10,float(runnbrstart),float(runnbrend+10));

    // Lumi-weighted average interactions per crossing
    m_avgintperbx = new TH1F("avgintperbx", "Avg Int/BX", 1000, 0., 100.);
    m_avgintperbx->SetTitle("Lumi-weighted Interactions per BX");
    this->SetHistogramStyle(m_avgintperbx, "Lumi-weighted Average Interactions per BX", "Average Interactions per BX", "Recorded Luminosity (mb-1)");
 
    // loop over the lumi tree    
    m_runnbr_prev = 0;
    int nlbs(0);
    float totalL(0.), total_l1ratediveffxsec(0.), total_l1ratediveffxsec_recorded(0.), totalLRun(0.), total_l1ratediveffxsecRun(0.), total_l1ratediveffxsecRun_recorded(0.);
    for(int i=0; i < m_LumiTree->GetEntries(); i++){
      m_LumiTree->GetEntry(i);
      
      // do histogram admin first
      if ( m_runnbr!=m_runnbr_prev ) {
	// first rebin prev histograms
	if (m_runnbr_prev>0) { 
	  m_intlumi->SetTitle(Form("Delivered luminosity = %.1f /#mu b, Run = %d",totalLRun,m_runnbr_prev));
	  m_intlumitrigrate->SetTitle(Form("Delivered luminosity = %.1f /#mu b, Efficiency * x-sec = %.1f #mu b, Run = %d",total_l1ratediveffxsecRun,m_effxsec,m_runnbr_prev));
	  m_intlumitrigrate_recorded->SetTitle(Form("Recorded luminosity = %.1f /#mu b, Efficiency * x-sec = %.1f #mu b, Run = %d",total_l1ratediveffxsecRun_recorded,m_effxsec,m_runnbr_prev));
	  this->RebinHistograms(nlbs+10,0,double(nlbs+10)); 
	}
	// create new histograms
	this->DoHistogramAdmin(m_runnbr,triggerchain,m_effxsec);
	// fill cullumative luminosity
	if (m_runnbr_prev>0) {
	  for (uint32_t j=m_runnbr_prev; j<m_runnbr; ++j) {
	    m_intlumiruns->Fill(j,totalL);
	    m_intlumitrigrateruns->Fill(j,total_l1ratediveffxsec);
	    m_intlumitrigrateruns_recorded->Fill(j,total_l1ratediveffxsec_recorded);
	  }
	}
	m_runnbr_prev = m_runnbr ;
      }
      totalL = m_totalL;
      totalLRun = m_totalLRun;
      total_l1ratediveffxsec = m_total_l1ratediveffxsec;
      total_l1ratediveffxsecRun = m_total_l1ratediveffxsecRun;      
      total_l1ratediveffxsec_recorded = m_total_l1ratediveffxsec_recorded;
      total_l1ratediveffxsecRun_recorded = m_total_l1ratediveffxsecRun_recorded;
      nlbs = m_clumiblocknbr;
      
      m_ntrigplb->Fill(m_clumiblocknbr,(float)m_afterprescale);
      m_trigrateplb->Fill(m_clumiblocknbr,m_l1rate);
      m_lumiplb->Fill(m_clumiblocknbr,m_instLumi);
      m_lumitrigrateplb->Fill(m_clumiblocknbr,m_l1ratediveffxsec);
      m_intlumi->Fill(m_clumiblocknbr,m_totalLRun);
      m_intlumitrigrate->Fill(m_clumiblocknbr,m_total_l1ratediveffxsecRun);
      m_lumitrigrateplb_recorded->Fill(m_clumiblocknbr,m_l1ratediveffxsec_recorded);
      m_intlumitrigrate_recorded->Fill(m_clumiblocknbr,m_total_l1ratediveffxsecRun_recorded);
      m_avgintperbx->Fill(m_AvEvtsPerBX, m_intLumi);  // Lumi-weighted mu

    } // end tree loop
    m_intlumiruns->Fill(runnbrend,totalL);
    m_intlumitrigrateruns->Fill(runnbrend,total_l1ratediveffxsec);
    m_intlumitrigrateruns_recorded->Fill(runnbrend,total_l1ratediveffxsec_recorded);
    
    // finish histograms make-up
    m_intlumiruns->SetMinimum(0.);
    m_intlumitrigrateruns->SetMinimum(0.);
    this->RebinHistograms(nlbs+10,0,double(nlbs+10));
    this->SetHistogramStyle(m_intlumiruns, Form("Delivered luminosity = %.1f /ub",totalL), "Run number", "Luminosity (#mu b^{-1})");
    this->SetHistogramStyle(m_intlumitrigrateruns, Form("Delivered luminosity = %.1f /#mu b, Recorded luminosity = %.1f /#mu b", //, Efficiency * x-sec = %.1f mb",
							total_l1ratediveffxsec,total_l1ratediveffxsec_recorded/*,m_effxsec*/), "Run number", Form("%s Luminosity (#mu b^{-1})",triggerchain.c_str()));
    this->SetHistogramStyle(m_intlumitrigrateruns_recorded, Form("Delivered luminosity = %.1f /#mub, Recorded luminosity = %.1f /#mub", //, Efficiency * x-sec = %.1f #mub",
								 total_l1ratediveffxsec,total_l1ratediveffxsec_recorded/*,m_effxsec*/), "Run number", Form("%s Luminosity (#mu b^{-1})",triggerchain.c_str()));
    
    std::vector<TH1F*>::iterator itr, itr2;
    
    /*
    // MB 20100115: turn off for now, RootGraphics lib crashes on some non-cern sites.
    // save pictures
    m_logger << Root::kINFO << "Saving plots (this may take a while)." << Root::GEndl;
    gROOT->SetBatch(true); // batch mode, don't draw pictures
    TCanvas* tcan = GetNiceCanvas(); tcan->cd();
    if (m_effxsec==1.0) {
    m_intlumiruns->Draw(); tcan->SaveAs(Form("%s.png",m_intlumiruns->GetName()));
    for (itr=m_lumiplbVec.begin(); itr!=m_lumiplbVec.end(); ++itr)                 { (*itr)->Draw(); tcan->SaveAs(Form("%s.png",(*itr)->GetName())); }
    for (itr=m_intlumiVec.begin(); itr!=m_intlumiVec.end(); ++itr)                 { (*itr)->Draw(); tcan->SaveAs(Form("%s.png",(*itr)->GetName())); }
    }
    for (itr=m_ntrigplbVec.begin(); itr!=m_ntrigplbVec.end(); ++itr)                 { (*itr)->Draw(); tcan->SaveAs(Form("%s.png",(*itr)->GetName())); }
    for (itr=m_trigrateplbVec.begin(); itr!=m_trigrateplbVec.end(); ++itr)           { (*itr)->Draw(); tcan->SaveAs(Form("%s.png",(*itr)->GetName())); }
    if (m_effxsec!=1.0) { // results only make sense when proper xsec is provided externally
    m_intlumitrigrateruns->SetFillColor(0);
    m_intlumitrigrateruns->Draw(); //tcan->SaveAs(Form("%s.png",m_intlumitrigrateruns->GetName()));
    m_intlumitrigrateruns_recorded->Draw("SAME"); tcan->SaveAs(Form("%s.png",m_intlumitrigrateruns->GetName()));
    for (itr=m_lumitrigrateplbVec.begin(), itr2=m_lumitrigrateplb_recordedVec.begin(); itr!=m_lumitrigrateplbVec.end(); ++itr, ++itr2) { 
    (*itr)->SetFillColor(0);
  (*itr)->Draw(); //tcan->SaveAs(Form("%s.png",(*itr)->GetName())); 
  (*itr2)->Draw("SAME"); tcan->SaveAs(Form("%s.png",(*itr)->GetName()));
  }
  for (itr=m_intlumitrigrateVec.begin(), itr2=m_intlumitrigrate_recordedVec.begin(); itr!=m_intlumitrigrateVec.end(); ++itr, ++itr2) { 
  (*itr)->SetFillColor(0);
  (*itr)->Draw(); //tcan->SaveAs(Form("%s.png",(*itr)->GetName())); 
  (*itr2)->Draw("SAME"); tcan->SaveAs(Form("%s.png",(*itr)->GetName()));
  }
  }
  delete tcan;
    */
  
  // and store the histograms
  TString histFileName = TString("ilumicalc_histograms_") + TString(triggerchain) + ( runnbrstart==runnbrend ? Form("_%d.root",runnbrstart) : Form("_%d-%d.root",runnbrstart,runnbrend) );
  TFile *ff = new TFile(histFileName.Data(),"recreate");
  m_avgintperbx->Write();
  if (m_effxsec==1.0) {
    m_intlumiruns->Write();
    for (itr=m_lumiplbVec.begin(); itr!=m_lumiplbVec.end(); ++itr)                 { (*itr)->Write(); }
    for (itr=m_intlumiVec.begin(); itr!=m_intlumiVec.end(); ++itr)                 { (*itr)->Write(); }
  }
  for (itr=m_ntrigplbVec.begin(); itr!=m_ntrigplbVec.end(); ++itr)                 { (*itr)->Write(); }
  for (itr=m_trigrateplbVec.begin(); itr!=m_trigrateplbVec.end(); ++itr)           { (*itr)->Write(); }
  if (m_effxsec!=1.0) { // results only make sense when proper xsec is provided externally
    m_intlumitrigrateruns->Write();
    m_intlumitrigrateruns_recorded->Write();
    for (itr=m_lumitrigrateplbVec.begin(); itr!=m_lumitrigrateplbVec.end(); ++itr) { (*itr)->Write(); }
    for (itr=m_intlumitrigrateVec.begin(); itr!=m_intlumitrigrateVec.end(); ++itr) { (*itr)->Write(); }
    for (itr=m_lumitrigrateplb_recordedVec.begin(); itr!=m_lumitrigrateplb_recordedVec.end(); ++itr) { (*itr)->Write(); }
    for (itr=m_intlumitrigrate_recordedVec.begin(); itr!=m_intlumitrigrate_recordedVec.end(); ++itr) { (*itr)->Write(); }
  }
  m_LumiTree->Write();
  ff->Close();
  delete ff;
  m_logger << Root::kINFO << "Histograms stored as          : " << histFileName << Root::GEndl;

  }else{
    m_logger << Root::kWARNING << "LumiTree pointer does not exist!          : " << Root::GEndl;
  }


}

// 	  // Reporting also lumi algorithm - for future...
// 	  std::map<int, std::string> m_lumialgo_chan;
	  
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(0, "ATLAS_PREFERRED")); 
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(1, "LHC")); 
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(101, "LUCID_ZEROS_OR"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(102, "LUCID_ZEROS_AND"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(103, "LUCID_HITS_OR"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(104, "LUCID_HITS_AND"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(201, "BCM_H_ZEROS_AND"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(202, "BCM_H_EVENTS_AND"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(203, "BCM_H_XORA"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(204, "BCM_H_EVENTS_XORC"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(301, "MBTS_ZEROS_AND"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(302, "MBTS_ZEROS_OR"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(303, "MBTS_HITS_OR"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(401, "ZDC_EVENTS_AND"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(402, "ZDC_EVENTS_ORA")); 
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(403, "ZDC_EVENTS_ORC"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(501, "FCAL"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(601, "HLT"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(901, "OflLumi_LArTime_Events"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(998, "OflLumi_Fake0"));
// 	  m_lumialgo_chan.insert(std::pair<int, std::string>(999, "OflLumi_Fake1"));
	  
//	  m_logger << Root::kINFO << "Algorithm: " << m_lumialgo_chan.find((m_Valid >> 22))->second << Root::GEndl;
	  
