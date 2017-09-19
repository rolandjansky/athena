/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArMonTools/LArNoisyROMon.h"

#include "LArRecEvent/LArEventBitInfo.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArMonTools/LArOnlineIDStrHelper.h"
#include "xAODEventInfo/EventInfo.h"

#include "LWHists/TH1I_LW.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "TBranch.h"


#include <sstream>
#include <stdexcept>
#include <cmath>

LArNoisyROMon::LArNoisyROMon(const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent):
  ManagedMonitorToolBase(type, name, parent), 
  m_NoiseTimeTree(NULL), m_CandidateMNBTree(NULL), m_eventCounter(0), m_LArOnlineIDHelper(NULL), m_strHelper(NULL), m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"), m_EF_NoiseBurst_Triggers(), m_L1_NoiseBurst_Triggers()
{
  declareProperty("IsOnline",       m_IsOnline=false);
  declareProperty("NoisyFEBDefStr", m_NoisyFEBDefStr="");
  declareProperty("BadFEBCut",  m_BadFEBCut=999999);
  declareProperty("doTrigger",       m_doTrigger=true);
  declareProperty("EFNoiseBurstTriggers",m_EF_NoiseBurst_Triggers);
  declareProperty("L1NoiseBurstTriggers",m_L1_NoiseBurst_Triggers);
  declareProperty("m_lumi_blocks", m_lumi_blocks = 3000 );
  declareProperty("doHisto",       m_doHisto=true);
  declareProperty("inputKey",       m_inputKey="LArNoisyROSummary");
  
  m_NoiseTime.time = 0;
  m_NoiseTime.time_ns = 0;
  m_NoiseTime.algo = 0;

  m_CandidateMNB.candidate_MNB_time = 0;
  m_CandidateMNB.candidate_MNB_time_ns = 0;
  m_CandidateMNB.n_candidate_MNBTight_FEB = 0;
  m_CandidateMNB.n_candidate_MNBLoose_FEB = 0;
  m_CandidateMNB.v_candidate_MNBTightFEB = new std::vector<int>;
  m_CandidateMNB.v_candidate_MNBLooseFEB = new std::vector<int>;

  m_h_NoisyFEB			= NULL;
  m_h_NoisyEventFrac		= NULL;
  m_h_NoisyEventTimeVetoFrac	= NULL;
  m_h_MNBTightFEB      = NULL;
  m_h_MNBTightEventFrac    = NULL;
  m_h_MNBTightEventTimeVetoFrac  = NULL;
  m_h_MNBLooseFEB      = NULL;
  m_h_MNBLooseEventFrac    = NULL;
  m_h_MNBLooseEventTimeVetoFrac  = NULL;
  m_h_CandidateMNBTightFEB      = NULL;
  m_h_CandidateMNBLooseFEB      = NULL;
  m_h_SaturatedTightFrac		= NULL;
  m_h_NoisyEvent			= NULL;
  m_h_NoisyEventTimeVeto		= NULL;
  m_h_MNBTightEvent      = NULL;
  m_h_MNBTightEventTimeVeto    = NULL;
  m_h_MNBLooseEvent      = NULL;
  m_h_MNBLooseEventTimeVeto    = NULL;
  m_h_SaturatedTight		= NULL;
  m_h_SaturatedTightTimeVeto	= NULL;
  m_h_SaturatedTightTimeVetoFrac	= NULL;
  m_h_LBN				= NULL;
}

LArNoisyROMon::~LArNoisyROMon()
{
  if ( m_strHelper ) delete m_strHelper;
  
  // don't delete m_NoiseTimeTree, it is not owned by this alg.
}


StatusCode LArNoisyROMon::initialize()
{
  if ( !(detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID" ).isSuccess()) )
  {
    ATH_MSG_FATAL( "unable to retrieve LArOnlineID from detStore" );
    return StatusCode::FAILURE;
  }
  
  m_strHelper = new LArOnlineIDStrHelper(m_LArOnlineIDHelper);
  m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);


  if ( m_doTrigger )
  {
    StatusCode sc = m_trigDec.retrieve();
    if ( !sc.isSuccess() )
    {
      ATH_MSG_FATAL( "unable to initialize TrigDecisionTool " );
      return StatusCode::FAILURE;
    }
  }
  ManagedMonitorToolBase::initialize().ignore();

  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROMon::bookHistograms()
{
    
    m_eventCounter=0;
    std::string hTitle;
    std::stringstream tit;

    MonGroup overall(this, "/LAr/NoisyRO", run, ATTRIB_MANAGED );
    if(m_doHisto) {
    // Book histograms per partitions    
    MonGroup GroupBarrel(this, "/LAr/NoisyRO/Barrel", run, ATTRIB_MANAGED );
    MonGroup GroupBarrelFrac(this, "/LAr/NoisyRO/Barrel", run, ATTRIB_MANAGED ,"", "weightedEff");
    MonGroup GroupBarrelFracBin(this, "/LAr/NoisyRO/Barrel", run, ATTRIB_MANAGED ,"", "perBinEffPerCent");
    bookPartitionHistos(m_BarrelA,"EMBA",GroupBarrel,GroupBarrelFrac,GroupBarrelFracBin);
    bookPartitionHistos(m_BarrelC,"EMBC",GroupBarrel,GroupBarrelFrac,GroupBarrelFracBin);
    
    MonGroup GroupEMEC(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED );
    MonGroup GroupEMECFrac(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED,"", "weightedEff" );
    MonGroup GroupEMECFracBin(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED,"", "perBinEffPerCent" );
    bookPartitionHistos(m_EMECA,"EMECA",GroupEMEC,GroupEMECFrac,GroupEMECFracBin);
    bookPartitionHistos(m_EMECC,"EMECC",GroupEMEC,GroupEMECFrac,GroupEMECFracBin);


      

    
    // Book general histograms
    hTitle = "Number of noisy FEB "+m_NoisyFEBDefStr+ " per event";
    m_h_NoisyFEB = TH1I_LW::create("NoisyFEB", hTitle.c_str(), 50,0.,50.);
    m_h_NoisyFEB->GetXaxis()->SetTitle("# of noisy FEB");
    overall.regHist(m_h_NoisyFEB).ignore();

    hTitle = "Number of MNB tight noisy FEB "+m_NoisyFEBDefStr+ " per event";
    m_h_MNBTightFEB = TH1I_LW::create("NoisyFEBNMBTight", hTitle.c_str(), 50,0.,50.);
    m_h_MNBTightFEB->GetXaxis()->SetTitle("# of noisy FEB");
    overall.regHist(m_h_MNBTightFEB).ignore();

    hTitle = "Number of MNB loose noisy FEB "+m_NoisyFEBDefStr+ " per event";
    m_h_MNBLooseFEB = TH1I_LW::create("NoisyFEBMNBLoose", hTitle.c_str(), 50,0.,50.);
    m_h_MNBLooseFEB->GetXaxis()->SetTitle("# of noisy FEB");
    overall.regHist(m_h_MNBLooseFEB).ignore();

    hTitle = "Number of tight noisy FEB "+m_NoisyFEBDefStr+ " per event";
    m_h_CandidateMNBTightFEB = TH1I_LW::create("NoisyFEBMNBCandTight", hTitle.c_str(), 50,0.,50.);
    m_h_CandidateMNBTightFEB->GetXaxis()->SetTitle("# of noisy FEB");
    overall.regHist(m_h_CandidateMNBTightFEB).ignore();

    hTitle = "Number of tight noisy FEB "+m_NoisyFEBDefStr+ " per event";
    m_h_CandidateMNBLooseFEB = TH1I_LW::create("NoisyFEBMNBCandLoose", hTitle.c_str(), 50,0.,50.);
    m_h_CandidateMNBLooseFEB->GetXaxis()->SetTitle("# of noisy FEB");
    overall.regHist(m_h_CandidateMNBLooseFEB).ignore();
    }

    m_NoiseTimeTree = new TTree("LArNoise","LAr noisy events");
    m_NoiseTimeTree->Branch("time",&m_NoiseTime.time,"time/i");
    m_NoiseTimeTree->Branch("time_ns",&m_NoiseTime.time_ns,"time_ns/i");
    m_NoiseTimeTree->Branch("algo",&m_NoiseTime.algo,"algo/b");
    overall.regTree(m_NoiseTimeTree).ignore();

    m_CandidateMNBTree = new TTree("CandidateMNBFebs","Candidate MNB Febs");
    m_CandidateMNBTree->Branch("candidate_MNB_time",&m_CandidateMNB.candidate_MNB_time,"candidate_MNB_time/i");
    m_CandidateMNBTree->Branch("candidate_MNB_time_ns",&m_CandidateMNB.candidate_MNB_time_ns,"candidate_MNB_time_ns/i");
    m_CandidateMNBTree->Branch("n_candidate_MNBTight_FEB",&m_CandidateMNB.n_candidate_MNBTight_FEB,"n_candidate_MNBTight_FEB/i");
    m_CandidateMNBTree->Branch("n_candidate_MNBLoose_FEB",&m_CandidateMNB.n_candidate_MNBLoose_FEB,"n_candidate_MNBLoose_FEB/i");
    m_CandidateMNBTree->Branch("v_candidate_MNBTightFEB", "vector<int>", &m_CandidateMNB.v_candidate_MNBTightFEB);
    m_CandidateMNBTree->Branch("v_candidate_MNBLooseFEB", "vector<int>", &m_CandidateMNB.v_candidate_MNBLooseFEB);
    overall.regTree(m_CandidateMNBTree).ignore();
    
    if(m_doHisto) {
    MonGroup overallPerCent(this, "/LAr/NoisyRO", run, ATTRIB_MANAGED, "", "perBinEffPerCent" );
    m_h_NoisyEventFrac = TH1F_LW::create("NoisyEvent", "Yield of events flagged as LArNoisyRO_Std", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_NoisyEventFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(m_h_NoisyEventFrac).ignore();
    m_h_NoisyEvent = TH1I_LW::create("temp_NoisyEvent", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_NoisyEvent->GetXaxis()->SetTitle("Luminosity Block");

    m_h_MNBTightEventFrac = TH1F_LW::create("MNBTightEvent", "Yield of events flagged as LArMNBTightRO_Std", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_MNBTightEventFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(m_h_MNBTightEventFrac).ignore();
    m_h_MNBTightEvent = TH1I_LW::create("temp_MNBTightEvent", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_MNBTightEvent->GetXaxis()->SetTitle("Luminosity Block");

    m_h_MNBLooseEventFrac = TH1F_LW::create("MNBLooseEvent", "Yield of events flagged as LArMNBLooseRO_Std", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_MNBLooseEventFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(m_h_MNBLooseEventFrac).ignore();
    m_h_MNBLooseEvent = TH1I_LW::create("temp_MNBLooseEvent", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_MNBLooseEvent->GetXaxis()->SetTitle("Luminosity Block");

    m_h_LBN = TH1I_LW::create("LBN", "LBN ", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    
    m_h_NoisyEventTimeVetoFrac = TH1F_LW::create("NoisyEvent_TimeVeto", "Yield of events flagged as LArNoisyRO_Std not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_NoisyEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(m_h_NoisyEventTimeVetoFrac).ignore();        
    // Histogram below is temporary. Normalized at the end of run to produce the above histograms
    m_h_NoisyEventTimeVeto = TH1I_LW::create("temp_NoisyEvent_TimeVeto", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_NoisyEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

    m_h_MNBTightEventTimeVetoFrac = TH1F_LW::create("MNBTightEvent_TimeVeto", "Yield of events flagged as LArMNBTightRO_Std not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_MNBTightEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(m_h_MNBTightEventTimeVetoFrac).ignore();        
    // Histogram below is temporary. Normalized at the end of run to produce the above histograms
    m_h_MNBTightEventTimeVeto = TH1I_LW::create("temp_MNBTightEvent_TimeVeto", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_MNBTightEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

    m_h_MNBLooseEventTimeVetoFrac = TH1F_LW::create("MNBLooseEvent_TimeVeto", "Yield of events flagged as LArMNBLooseRO_Std not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_MNBLooseEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(m_h_MNBLooseEventTimeVetoFrac).ignore();        
    // Histogram below is temporary. Normalized at the end of run to produce the above histograms
    m_h_MNBLooseEventTimeVeto = TH1I_LW::create("temp_MNBLooseEvent_TimeVeto", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_MNBLooseEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");
    
    hTitle = "Yield of events flagged as LArNoisyROSaturated";
    m_h_SaturatedTightFrac = TH1F_LW::create("SaturatedTightEvent", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_SaturatedTightFrac->GetXaxis()->SetTitle("Luminosity Block");
    m_h_SaturatedTightFrac->GetYaxis()->SetTitle("Yield(%)");
    overallPerCent.regHist(m_h_SaturatedTightFrac).ignore();
    // Histogram below is temporary. Normalized at the end of run to produce the above histograms
    m_h_SaturatedTight = TH1I_LW::create("temp_SaturatedTight", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_SaturatedTight->GetXaxis()->SetTitle("Luminosity Block");
    
    hTitle = "Yield of events flagged as LArNoisyROSaturated not vetoed by time window";
    m_h_SaturatedTightTimeVetoFrac = TH1F_LW::create("SaturatedTightEvent_TimeVeto", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_SaturatedTightTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
    m_h_SaturatedTightTimeVetoFrac->GetYaxis()->SetTitle("Yield(%)");
    overallPerCent.regHist(m_h_SaturatedTightTimeVetoFrac).ignore();
    // Histogram below is temporary. Normalized at the end of run to produce the above histograms
    m_h_SaturatedTightTimeVeto = TH1I_LW::create("temp_SaturatedTight_TimeVeto", hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
    m_h_SaturatedTightTimeVeto->GetXaxis()->SetTitle("Luminosity Block");
    }

  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROMon::fillHistograms()
{
  StatusCode sc(StatusCode::SUCCESS);
  
  m_eventCounter++;
  
  // retrieve 
  const LArNoisyROSummary* noisyRO;
  sc = evtStore()->retrieve(noisyRO,m_inputKey);
  if (sc.isFailure()) 
  {
    ATH_MSG_WARNING( "Can't retrieve LArNoisyROSummary " );
    return StatusCode::SUCCESS;
  }
  
  
  const xAOD::EventInfo* eventInfo;
  sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) 
  {
    ATH_MSG_WARNING( "Can't retrieve EventInfo " );
    return StatusCode::SUCCESS;
  }
  
  unsigned int LBN = eventInfo->lumiBlock();
  bool burstveto = eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::NOISEBURSTVETO);

  // Filling Candidate MNB Tree
  m_CandidateMNB.candidate_MNB_time=eventInfo->timeStamp();
  m_CandidateMNB.candidate_MNB_time_ns=eventInfo->timeStampNSOffset();
  m_CandidateMNB.v_candidate_MNBTightFEB->clear();
  m_CandidateMNB.v_candidate_MNBLooseFEB->clear();
  const std::vector<HWIdentifier>& mnbtightFEB = noisyRO->get_MNBTight_febs();  
  const std::vector<HWIdentifier>& mnblooseFEB = noisyRO->get_MNBLoose_febs();  
  m_CandidateMNB.n_candidate_MNBTight_FEB = mnbtightFEB.size();
  m_CandidateMNB.n_candidate_MNBLoose_FEB = mnblooseFEB.size();  

  for(unsigned int iFeb=0; iFeb<mnbtightFEB.size(); iFeb++) 
    m_CandidateMNB.v_candidate_MNBTightFEB->push_back(mnbtightFEB.at(iFeb).get_identifier32().get_compact());

  for(unsigned int iFeb=0; iFeb<mnblooseFEB.size(); iFeb++)
    m_CandidateMNB.v_candidate_MNBLooseFEB->push_back(mnblooseFEB.at(iFeb).get_identifier32().get_compact());

  if(m_CandidateMNB.v_candidate_MNBLooseFEB->size() > 0 || m_CandidateMNB.v_candidate_MNBTightFEB->size() > 0)
    m_CandidateMNBTree->Fill();

  // Fill the noise bursts timestamp ttree
  m_NoiseTime.time = eventInfo->timeStamp();
  m_NoiseTime.time_ns = eventInfo->timeStampNSOffset(); // Modification by B.Trocme to get the ns precision
  m_NoiseTime.algo = 0;
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::BADFEBS) ) 
  {
    m_NoiseTime.algo |= 0x1;
  }
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ) ) 
  {
    m_NoiseTime.algo |= 0x2;
  }
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::BADFEBS_W) ) 
  {
    m_NoiseTime.algo |= 0x8;
  }
  if (eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::MININOISEBURSTLOOSE))
  {
    m_NoiseTime.algo |= 0x10;
  }
  if (eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT))
  {
    m_NoiseTime.algo |= 0x20;
  }

  if ( m_NoiseTime.algo != 0 ) 
  {
    if ( burstveto ) m_NoiseTime.algo |= 0x4;
    unsigned uf=m_NoiseTime.algo;
    std::cout<<"LArNoisyROMon flag: "<<uf<<std::endl;
    m_NoiseTimeTree->Fill();
  }  

  // Triggers
  uint8_t trigbits = 0;
  uint8_t L1trigbits = 0;
  if ( m_doTrigger )
  {
    for ( size_t i = 0; i < m_EF_NoiseBurst_Triggers.size(); i++)
    {
      if ( m_trigDec->isPassed(m_EF_NoiseBurst_Triggers[i]))  trigbits |= (0x1 << i);
    }
    for ( size_t i = 0; i < m_L1_NoiseBurst_Triggers.size(); i++)
    {
      if ( m_trigDec->isPassed(m_L1_NoiseBurst_Triggers[i]))  L1trigbits |= (0x1 << i);
    }
  }
  if(!m_doHisto) return sc;
  
  // Fill the 2D map of noisy and mini-noisy FEBs
  const std::vector<HWIdentifier>& noisyFEB = noisyRO->get_noisy_febs();

  // Loop on all FEBs noisy in Std definition (i.e >30 channels with q factor > 4000)
  // And fill the 2D maps of fraction of fraction of noisy events
  unsigned int NbNoisyFEB = 0;
  for (size_t i = 0; i<noisyFEB.size(); i++)
  {
    //std::cout << "Noisy FEB " <<  noisyFEB[i].get_compact() << std::endl;
    NbNoisyFEB++;
    const HWIdentifier& febid = noisyFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    if ( m_LArOnlineIDHelper->isEMBchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
      	if (m_IsOnline)
      	  m_BarrelC.h_NoisyFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      	else
      	  m_BarrelC.h_NoisyFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
      else
      {
      	if (m_IsOnline)
      	  m_BarrelA.h_NoisyFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      	else
      	  m_BarrelA.h_NoisyFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
    }
    if ( m_LArOnlineIDHelper->isEMECchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
      	if (m_IsOnline)
      	  m_EMECC.h_NoisyFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      	else
      	  m_EMECC.h_NoisyFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
      else
      {
      	if (m_IsOnline)
      	  m_EMECA.h_NoisyFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      	else
      	  m_EMECA.h_NoisyFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
    }
  }
  m_h_NoisyFEB->Fill(NbNoisyFEB);

  // Loop on all FEBs noisy in MNB-tight definition
  // And fill the 2D maps of fraction of fraction of noisy events
  // Only for events found MNB-Tight noisy first (i.e at least one FEB defined as suspicious)
  unsigned int NbMNBTightFEB = 0;
  for (size_t i = 0; i<mnbtightFEB.size(); i++)
  {
    //std::cout << "MNBTight FEB " <<  noisyFEB[i].get_compact() << std::endl;
    NbMNBTightFEB++;
    const HWIdentifier& febid = mnbtightFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    if ( m_LArOnlineIDHelper->isEMBchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
        if (m_IsOnline)
        {
          if((noisyRO->MNBTightFlaggedPartitions() & LArNoisyROSummary::EMBCMask) != 0) 
            m_BarrelC.h_MNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
          if((noisyRO->MNBTightFlaggedPartitions() & LArNoisyROSummary::EMBCMask) != 0) 
            m_BarrelC.h_MNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
      }
      else
      {
        if (m_IsOnline)
        {
          if((noisyRO->MNBTightFlaggedPartitions() & LArNoisyROSummary::EMBAMask) != 0)
            m_BarrelA.h_MNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
          if((noisyRO->MNBTightFlaggedPartitions() & LArNoisyROSummary::EMBAMask) != 0)
            m_BarrelA.h_MNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
      }
    }
    if ( m_LArOnlineIDHelper->isEMECchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
        if (m_IsOnline)
        {
          if((noisyRO->MNBTightFlaggedPartitions() & LArNoisyROSummary::EMECCMask) != 0)
            m_EMECC.h_MNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
          if((noisyRO->MNBTightFlaggedPartitions() & LArNoisyROSummary::EMECCMask) != 0)
            m_EMECC.h_MNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
      }
      else
      {
        if (m_IsOnline)
        { 
          if((noisyRO->MNBTightFlaggedPartitions() & LArNoisyROSummary::EMECAMask) != 0)
            m_EMECA.h_MNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
          if((noisyRO->MNBTightFlaggedPartitions() & LArNoisyROSummary::EMECAMask) != 0)
            m_EMECA.h_MNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
      }
    }
  }
  m_h_MNBTightFEB->Fill(NbMNBTightFEB);

  // Loop on all FEBs noisy in MNB-loose definition
  // And fill the 2D maps of fraction of fraction of noisy events
  // Only for events found MNB-Loose noisy first (i.e at least one FEB defined as suspicious)
  unsigned int NbMNBLooseFEB = 0;
  for (size_t i = 0; i<mnblooseFEB.size(); i++)
  {
    //std::cout << "MNBLoose FEB " <<  noisyFEB[i].get_compact() << std::endl;
    NbMNBLooseFEB++;
    const HWIdentifier& febid = mnblooseFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    if ( m_LArOnlineIDHelper->isEMBchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
        if (m_IsOnline)
        {
          if((noisyRO->MNBLooseFlaggedPartitions() & LArNoisyROSummary::EMBCMask) != 0) 
            m_BarrelC.h_MNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
          if((noisyRO->MNBLooseFlaggedPartitions() & LArNoisyROSummary::EMBCMask) != 0) 
            m_BarrelC.h_MNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
      }
      else
      {
        if (m_IsOnline)
        {
          if((noisyRO->MNBLooseFlaggedPartitions() & LArNoisyROSummary::EMBAMask) != 0)
            m_BarrelA.h_MNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
          if((noisyRO->MNBLooseFlaggedPartitions() & LArNoisyROSummary::EMBAMask) != 0)
            m_BarrelA.h_MNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
      }
    }
    if ( m_LArOnlineIDHelper->isEMECchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
        if (m_IsOnline)
        {
          if((noisyRO->MNBLooseFlaggedPartitions() & LArNoisyROSummary::EMECCMask) != 0)
            m_EMECC.h_MNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
          if((noisyRO->MNBLooseFlaggedPartitions() & LArNoisyROSummary::EMECCMask) != 0)
            m_EMECC.h_MNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
      }
      else
      {
        if (m_IsOnline)
        {
          if((noisyRO->MNBLooseFlaggedPartitions() & LArNoisyROSummary::EMECAMask) != 0)
            m_EMECA.h_MNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
          if((noisyRO->MNBLooseFlaggedPartitions() & LArNoisyROSummary::EMECAMask) != 0)
            m_EMECA.h_MNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
      }
    }
  }
  m_h_MNBLooseFEB->Fill(NbMNBLooseFEB);

  // Loop on all FEBs noisy in MNB-tight definition
  // And fill the 2D maps of fraction of fraction of noisy events
  // Done for all events/FEBs (i.e no filtering at all on suspicious FEBs)
  unsigned int NbCandidateMNBTightFEB = 0;
  for (size_t i = 0; i<mnbtightFEB.size(); i++)
  {
    //std::cout << "CandidateMNBTight FEB " <<  noisyFEB[i].get_compact() << std::endl;
    NbCandidateMNBTightFEB++;
    const HWIdentifier& febid = mnbtightFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    if ( m_LArOnlineIDHelper->isEMBchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
        if (m_IsOnline)
          m_BarrelC.h_CandidateMNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        else
          m_BarrelC.h_CandidateMNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
      else
      {
        if (m_IsOnline)
          m_BarrelA.h_CandidateMNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        else
          m_BarrelA.h_CandidateMNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
    }
    if ( m_LArOnlineIDHelper->isEMECchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
        if (m_IsOnline)
          m_EMECC.h_CandidateMNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        else
          m_EMECC.h_CandidateMNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
      else
      {
        if (m_IsOnline)
          m_EMECA.h_CandidateMNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        else
          m_EMECA.h_CandidateMNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
    }
  }
  m_h_CandidateMNBTightFEB->Fill(NbCandidateMNBTightFEB);

  // Loop on all FEBs noisy in MNB-loose definition
  // And fill the 2D maps of fraction of fraction of noisy events
  // Done for all events/FEBs (i.e no filtering at all on suspicious FEBs)
  unsigned int NbCandidateMNBLooseFEB = 0;
  for (size_t i = 0; i<mnblooseFEB.size(); i++)
  {
    //std::cout << "CandidateMNBLoose FEB " <<  noisyFEB[i].get_compact() << std::endl;
    NbCandidateMNBLooseFEB++;
    const HWIdentifier& febid = mnblooseFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    if ( m_LArOnlineIDHelper->isEMBchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
        if (m_IsOnline)
          m_BarrelC.h_CandidateMNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        else
          m_BarrelC.h_CandidateMNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
      else
      {
        if (m_IsOnline)
          m_BarrelA.h_CandidateMNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        else
          m_BarrelA.h_CandidateMNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
    }
    if ( m_LArOnlineIDHelper->isEMECchannel(id) )
    {
      if ( m_LArOnlineIDHelper->pos_neg(id) == 0 )
      {
        if (m_IsOnline)
          m_EMECC.h_CandidateMNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        else
          m_EMECC.h_CandidateMNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
      else
      {
        if (m_IsOnline)
          m_EMECA.h_CandidateMNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        else
          m_EMECA.h_CandidateMNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      }
    }
  }
  m_h_CandidateMNBLooseFEB->Fill(NbCandidateMNBLooseFEB);

  // End of 2D map of FEB found as noisy (in any definition : Std, MNB-Tight or MNB-Loose)
  // Now fill 1D histograms of fraction of events found as noisy vetoed or not
  m_h_LBN->Fill(LBN);  
  // Event found noisy by Std method
  uint8_t BadFEBPartitions = noisyRO->BadFEBFlaggedPartitions();
  if ( BadFEBPartitions != 0) 
  {
    m_h_NoisyEvent->Fill(LBN);
    if ( ! burstveto ) 
    {
      m_h_NoisyEventTimeVeto->Fill(LBN);
    }
    if ( (BadFEBPartitions & LArNoisyROSummary::EMECAMask) != 0 ) 
    {
      m_EMECA.h_NoisyEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_EMECA,trigbits,L1trigbits);
      if ( ! burstveto ) m_EMECA.h_NoisyEventTimeVeto->Fill(LBN);
    }
    if ( (BadFEBPartitions & LArNoisyROSummary::EMBAMask) != 0 ) 
    {
      m_BarrelA.h_NoisyEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_BarrelA,trigbits,L1trigbits);
      if ( ! burstveto ) m_BarrelA.h_NoisyEventTimeVeto->Fill(LBN);
    }
    if ( (BadFEBPartitions & LArNoisyROSummary::EMBCMask) != 0 )
    { 
      m_BarrelC.h_NoisyEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_BarrelC,trigbits,L1trigbits);
      if ( ! burstveto ) m_BarrelC.h_NoisyEventTimeVeto->Fill(LBN);
    }
    if ( (BadFEBPartitions & LArNoisyROSummary::EMECCMask) != 0 )
    {
      m_EMECC.h_NoisyEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_EMECC,trigbits,L1trigbits);
      if ( ! burstveto ) m_EMECC.h_NoisyEventTimeVeto->Fill(LBN);
    }
  }

  // event flagged by # of saturated quality cells
  uint8_t SatTightPartitions = noisyRO->SatTightFlaggedPartitions();
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ) ) 
  {
    m_h_SaturatedTight->Fill(LBN);
    if ( !burstveto ) m_h_SaturatedTightTimeVeto->Fill(LBN);
    if ( (SatTightPartitions & LArNoisyROSummary::EMECAMask) != 0 ) 
    {
      m_EMECA.h_SaturatedNoisyEvent->Fill(LBN);
      if ( ! burstveto ) m_EMECA.h_SaturatedNoisyEventTimeVeto->Fill(LBN);
    }
    if ( (SatTightPartitions & LArNoisyROSummary::EMBAMask) != 0 ) 
    {
      m_BarrelA.h_SaturatedNoisyEvent->Fill(LBN);
      if ( ! burstveto ) m_BarrelA.h_SaturatedNoisyEventTimeVeto->Fill(LBN);
    }
    if ( (SatTightPartitions & LArNoisyROSummary::EMBCMask) != 0 )
    { 
      m_BarrelC.h_SaturatedNoisyEvent->Fill(LBN);
      if ( ! burstveto ) m_BarrelC.h_SaturatedNoisyEventTimeVeto->Fill(LBN);
    }
    if ( (SatTightPartitions & LArNoisyROSummary::EMECCMask) != 0 )
    {
      m_EMECC.h_SaturatedNoisyEvent->Fill(LBN);
      if ( ! burstveto ) m_EMECC.h_SaturatedNoisyEventTimeVeto->Fill(LBN);
    }
  }


  // event flagged by tight-MNB
  uint8_t MNBTightPartitions = noisyRO->MNBTightFlaggedPartitions();
  if ( MNBTightPartitions != 0) 
  {
    m_h_MNBTightEvent->Fill(LBN);
    if ( ! burstveto ) 
    {
      m_h_MNBTightEventTimeVeto->Fill(LBN);
    }
    if ( (MNBTightPartitions & LArNoisyROSummary::EMECAMask) != 0 ) 
    {
      m_EMECA.h_MNBTightEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_EMECA,trigbits,L1trigbits);
      if ( ! burstveto ) m_EMECA.h_MNBTightEventTimeVeto->Fill(LBN);
    }
    if ( (MNBTightPartitions & LArNoisyROSummary::EMBAMask) != 0 ) 
    {
      m_BarrelA.h_MNBTightEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_BarrelA,trigbits,L1trigbits);
      if ( ! burstveto ) m_BarrelA.h_MNBTightEventTimeVeto->Fill(LBN);
    }
    if ( (MNBTightPartitions & LArNoisyROSummary::EMBCMask) != 0 )
    { 
      m_BarrelC.h_MNBTightEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_BarrelC,trigbits,L1trigbits);
      if ( ! burstveto ) m_BarrelC.h_MNBTightEventTimeVeto->Fill(LBN);
    }
    if ( (MNBTightPartitions & LArNoisyROSummary::EMECCMask) != 0 )
    {
      m_EMECC.h_MNBTightEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_EMECC,trigbits,L1trigbits);
      if ( ! burstveto ) m_EMECC.h_MNBTightEventTimeVeto->Fill(LBN);
    }
  }

  // event flagged by loose-MNB
  uint8_t MNBLoosePartitions = noisyRO->MNBLooseFlaggedPartitions();
  if ( MNBLoosePartitions != 0) 
  {
    m_h_MNBLooseEvent->Fill(LBN);
    if ( ! burstveto ) 
    {
      m_h_MNBLooseEventTimeVeto->Fill(LBN);
    }
    if ( (MNBLoosePartitions & LArNoisyROSummary::EMECAMask) != 0 ) 
    {
      m_EMECA.h_MNBLooseEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_EMECA,trigbits,L1trigbits);
      if ( ! burstveto ) m_EMECA.h_MNBLooseEventTimeVeto->Fill(LBN);
    }
    if ( (MNBLoosePartitions & LArNoisyROSummary::EMBAMask) != 0 ) 
    {
      m_BarrelA.h_MNBLooseEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_BarrelA,trigbits,L1trigbits);
      if ( ! burstveto ) m_BarrelA.h_MNBLooseEventTimeVeto->Fill(LBN);
    }
    if ( (MNBLoosePartitions & LArNoisyROSummary::EMBCMask) != 0 )
    { 
      m_BarrelC.h_MNBLooseEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_BarrelC,trigbits,L1trigbits);
      if ( ! burstveto ) m_BarrelC.h_MNBLooseEventTimeVeto->Fill(LBN);
    }
    if ( (MNBLoosePartitions & LArNoisyROSummary::EMECCMask) != 0 )
    {
      m_EMECC.h_MNBLooseEvent->Fill(LBN);
      if ( m_doTrigger ) fillTriggerHisto(m_EMECC,trigbits,L1trigbits);
      if ( ! burstveto ) m_EMECC.h_MNBLooseEventTimeVeto->Fill(LBN);
    }
  }

  // Event found noisy by weighted Std method
  uint8_t BadFEB_WPartitions = noisyRO->BadFEB_WFlaggedPartitions();
  if ( BadFEB_WPartitions != 0) 
  {
    if ( (BadFEB_WPartitions & LArNoisyROSummary::EMECAMask) != 0 ) 
    {
      m_EMECA.h_NoisyWEvent->Fill(LBN);
      if ( ! burstveto ) m_EMECA.h_NoisyWEventTimeVeto->Fill(LBN);
    }
    if ( (BadFEB_WPartitions & LArNoisyROSummary::EMBAMask) != 0 ) 
    {
      m_BarrelA.h_NoisyWEvent->Fill(LBN);
      if ( ! burstveto ) m_BarrelA.h_NoisyWEventTimeVeto->Fill(LBN);
    }
    if ( (BadFEB_WPartitions & LArNoisyROSummary::EMBCMask) != 0 )
    { 
      m_BarrelC.h_NoisyWEvent->Fill(LBN);
      if ( ! burstveto ) m_BarrelC.h_NoisyWEventTimeVeto->Fill(LBN);
    }
    if ( (BadFEB_WPartitions & LArNoisyROSummary::EMECCMask) != 0 )
    {
      m_EMECC.h_NoisyWEvent->Fill(LBN);
      if ( ! burstveto ) m_EMECC.h_NoisyWEventTimeVeto->Fill(LBN);
    }
  }
    
  return sc;
}

StatusCode LArNoisyROMon::checkHists(bool fromFinalize)
{
  if(fromFinalize){
    ///std::cout << "LArNoisyROMon::checkHists -- if statement to stop warnings"  << std::endl;
  } 
  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROMon::procHistograms()
{  
  if(!m_doHisto) return StatusCode::SUCCESS;

  if ( endOfRunFlag() || endOfEventsBlockFlag()){
    if ( m_IsOnline )
    {
      // copy the "running" histo to the final ones, to be normalised
      CopyHisto(m_BarrelA.h_NoisyFEBPerEvt,m_BarrelA.h_NoisyFEBFracPerEvt);
      CopyHisto(m_BarrelC.h_NoisyFEBPerEvt,m_BarrelC.h_NoisyFEBFracPerEvt);
      CopyHisto(m_EMECA.h_NoisyFEBPerEvt,m_EMECA.h_NoisyFEBFracPerEvt);
      CopyHisto(m_EMECC.h_NoisyFEBPerEvt,m_EMECC.h_NoisyFEBFracPerEvt);

      CopyHisto(m_BarrelA.h_MNBTightFEBPerEvt,m_BarrelA.h_MNBTightFEBFracPerEvt);
      CopyHisto(m_BarrelC.h_MNBTightFEBPerEvt,m_BarrelC.h_MNBTightFEBFracPerEvt);
      CopyHisto(m_EMECA.h_MNBTightFEBPerEvt,m_EMECA.h_MNBTightFEBFracPerEvt);
      CopyHisto(m_EMECC.h_MNBTightFEBPerEvt,m_EMECC.h_MNBTightFEBFracPerEvt);

      CopyHisto(m_BarrelA.h_MNBLooseFEBPerEvt,m_BarrelA.h_MNBLooseFEBFracPerEvt);
      CopyHisto(m_BarrelC.h_MNBLooseFEBPerEvt,m_BarrelC.h_MNBLooseFEBFracPerEvt);
      CopyHisto(m_EMECA.h_MNBLooseFEBPerEvt,m_EMECA.h_MNBLooseFEBFracPerEvt);
      CopyHisto(m_EMECC.h_MNBLooseFEBPerEvt,m_EMECC.h_MNBLooseFEBFracPerEvt);

      CopyHisto(m_BarrelA.h_CandidateMNBTightFEBPerEvt,m_BarrelA.h_CandidateMNBTightFEBFracPerEvt);
      CopyHisto(m_BarrelC.h_CandidateMNBTightFEBPerEvt,m_BarrelC.h_CandidateMNBTightFEBFracPerEvt);
      CopyHisto(m_EMECA.h_CandidateMNBTightFEBPerEvt,m_EMECA.h_CandidateMNBTightFEBFracPerEvt);
      CopyHisto(m_EMECC.h_CandidateMNBTightFEBPerEvt,m_EMECC.h_CandidateMNBTightFEBFracPerEvt);

      CopyHisto(m_BarrelA.h_CandidateMNBLooseFEBPerEvt,m_BarrelA.h_CandidateMNBLooseFEBFracPerEvt);
      CopyHisto(m_BarrelC.h_CandidateMNBLooseFEBPerEvt,m_BarrelC.h_CandidateMNBLooseFEBFracPerEvt);
      CopyHisto(m_EMECA.h_CandidateMNBLooseFEBPerEvt,m_EMECA.h_CandidateMNBLooseFEBFracPerEvt);
      CopyHisto(m_EMECC.h_CandidateMNBLooseFEBPerEvt,m_EMECC.h_CandidateMNBLooseFEBFracPerEvt);

    }
    
    if (m_eventCounter>0) {
      const double scale =  100./static_cast<double>(m_eventCounter);
      m_BarrelA.h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelA.h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);
      m_BarrelC.h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelC.h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECA.h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECA.h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECC.h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECC.h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);

      m_BarrelA.h_MNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelA.h_MNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
      m_BarrelC.h_MNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelC.h_MNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECA.h_MNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECA.h_MNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECC.h_MNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECC.h_MNBTightFEBFracPerEvt->SetEntries(m_eventCounter);

      m_BarrelA.h_MNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelA.h_MNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
      m_BarrelC.h_MNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelC.h_MNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECA.h_MNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECA.h_MNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECC.h_MNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECC.h_MNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);

      m_BarrelA.h_CandidateMNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelA.h_CandidateMNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
      m_BarrelC.h_CandidateMNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelC.h_CandidateMNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECA.h_CandidateMNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECA.h_CandidateMNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECC.h_CandidateMNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECC.h_CandidateMNBTightFEBFracPerEvt->SetEntries(m_eventCounter);

      m_BarrelA.h_CandidateMNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelA.h_CandidateMNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
      m_BarrelC.h_CandidateMNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_BarrelC.h_CandidateMNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECA.h_CandidateMNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECA.h_CandidateMNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
      m_EMECC.h_CandidateMNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
      m_EMECC.h_CandidateMNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
    
      Divide(m_h_NoisyEventFrac,m_h_NoisyEvent,m_h_LBN);
      m_h_NoisyEventFrac->SetEntries(m_eventCounter);

      Divide(m_h_MNBTightEventFrac,m_h_MNBTightEvent,m_h_LBN);
      m_h_MNBTightEventFrac->SetEntries(m_eventCounter);

      Divide(m_h_MNBLooseEventFrac,m_h_MNBLooseEvent,m_h_LBN);
      m_h_MNBLooseEventFrac->SetEntries(m_eventCounter);  

      Divide(m_BarrelA.h_NoisyEventFrac,m_BarrelA.h_NoisyEvent,m_h_LBN);
      m_BarrelA.h_NoisyEventFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_NoisyEventFrac,m_BarrelC.h_NoisyEvent,m_h_LBN);
      m_BarrelC.h_NoisyEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_NoisyEventFrac,m_EMECA.h_NoisyEvent,m_h_LBN);
      m_EMECA.h_NoisyEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_NoisyEventFrac,m_EMECC.h_NoisyEvent,m_h_LBN);
      m_EMECC.h_NoisyEventFrac->SetEntries(m_eventCounter);
      
      Divide(m_BarrelA.h_SaturatedNoisyEventFrac,m_BarrelA.h_SaturatedNoisyEvent,m_h_LBN);
      m_BarrelA.h_SaturatedNoisyEventFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_SaturatedNoisyEventFrac,m_BarrelC.h_SaturatedNoisyEvent,m_h_LBN);
      m_BarrelC.h_SaturatedNoisyEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_SaturatedNoisyEventFrac,m_EMECA.h_SaturatedNoisyEvent,m_h_LBN);
      m_EMECA.h_SaturatedNoisyEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_SaturatedNoisyEventFrac,m_EMECC.h_SaturatedNoisyEvent,m_h_LBN);
      m_EMECC.h_SaturatedNoisyEventFrac->SetEntries(m_eventCounter);

      Divide(m_BarrelA.h_MNBTightEventFrac,m_BarrelA.h_MNBTightEvent,m_h_LBN);
      m_BarrelA.h_MNBTightEventFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_MNBTightEventFrac,m_BarrelC.h_MNBTightEvent,m_h_LBN);
      m_BarrelC.h_MNBTightEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_MNBTightEventFrac,m_EMECA.h_MNBTightEvent,m_h_LBN);
      m_EMECA.h_MNBTightEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_MNBTightEventFrac,m_EMECC.h_MNBTightEvent,m_h_LBN);
      m_EMECC.h_MNBTightEventFrac->SetEntries(m_eventCounter);

      Divide(m_BarrelA.h_MNBLooseEventFrac,m_BarrelA.h_MNBLooseEvent,m_h_LBN);
      m_BarrelA.h_MNBLooseEventFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_MNBLooseEventFrac,m_BarrelC.h_MNBLooseEvent,m_h_LBN);
      m_BarrelC.h_MNBLooseEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_MNBLooseEventFrac,m_EMECA.h_MNBLooseEvent,m_h_LBN);
      m_EMECA.h_MNBLooseEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_MNBLooseEventFrac,m_EMECC.h_MNBLooseEvent,m_h_LBN);
      m_EMECC.h_MNBLooseEventFrac->SetEntries(m_eventCounter);

      Divide(m_BarrelA.h_NoisyWEventFrac,m_BarrelA.h_NoisyWEvent,m_h_LBN);
      m_BarrelA.h_NoisyWEventFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_NoisyWEventFrac,m_BarrelC.h_NoisyWEvent,m_h_LBN);
      m_BarrelC.h_NoisyWEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_NoisyWEventFrac,m_EMECA.h_NoisyWEvent,m_h_LBN);
      m_EMECA.h_NoisyWEventFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_NoisyWEventFrac,m_EMECC.h_NoisyWEvent,m_h_LBN);
      m_EMECC.h_NoisyWEventFrac->SetEntries(m_eventCounter);
    
      Divide(m_h_NoisyEventTimeVetoFrac,m_h_NoisyEventTimeVeto,m_h_LBN);
      m_h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelA.h_NoisyEventTimeVetoFrac,m_BarrelA.h_NoisyEventTimeVeto,m_h_LBN);
      m_BarrelA.h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_NoisyEventTimeVetoFrac,m_BarrelC.h_NoisyEventTimeVeto,m_h_LBN);
      m_BarrelC.h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_NoisyEventTimeVetoFrac,m_EMECA.h_NoisyEventTimeVeto,m_h_LBN);
      m_EMECA.h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_NoisyEventTimeVetoFrac,m_EMECC.h_NoisyEventTimeVeto,m_h_LBN);
      m_EMECC.h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);

      Divide(m_BarrelA.h_SaturatedNoisyEventTimeVetoFrac,m_BarrelA.h_SaturatedNoisyEventTimeVeto,m_h_LBN);
      m_BarrelA.h_SaturatedNoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_SaturatedNoisyEventTimeVetoFrac,m_BarrelC.h_SaturatedNoisyEventTimeVeto,m_h_LBN);
      m_BarrelC.h_SaturatedNoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_SaturatedNoisyEventTimeVetoFrac,m_EMECA.h_SaturatedNoisyEventTimeVeto,m_h_LBN);
      m_EMECA.h_SaturatedNoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_SaturatedNoisyEventTimeVetoFrac,m_EMECC.h_SaturatedNoisyEventTimeVeto,m_h_LBN);
      m_EMECC.h_SaturatedNoisyEventTimeVetoFrac->SetEntries(m_eventCounter);

      Divide(m_h_MNBTightEventTimeVetoFrac,m_h_MNBTightEventTimeVeto,m_h_LBN);
      m_h_MNBTightEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelA.h_MNBTightEventTimeVetoFrac,m_BarrelA.h_MNBTightEventTimeVeto,m_h_LBN);
      m_BarrelA.h_MNBTightEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_MNBTightEventTimeVetoFrac,m_BarrelC.h_MNBTightEventTimeVeto,m_h_LBN);
      m_BarrelC.h_MNBTightEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_MNBTightEventTimeVetoFrac,m_EMECA.h_MNBTightEventTimeVeto,m_h_LBN);
      m_EMECA.h_MNBTightEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_MNBTightEventTimeVetoFrac,m_EMECC.h_MNBTightEventTimeVeto,m_h_LBN);
      m_EMECC.h_MNBTightEventTimeVetoFrac->SetEntries(m_eventCounter);

      Divide(m_h_MNBLooseEventTimeVetoFrac,m_h_MNBLooseEventTimeVeto,m_h_LBN);
      m_h_MNBLooseEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelA.h_MNBLooseEventTimeVetoFrac,m_BarrelA.h_MNBLooseEventTimeVeto,m_h_LBN);
      m_BarrelA.h_MNBLooseEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_MNBLooseEventTimeVetoFrac,m_BarrelC.h_MNBLooseEventTimeVeto,m_h_LBN);
      m_BarrelC.h_MNBLooseEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_MNBLooseEventTimeVetoFrac,m_EMECA.h_MNBLooseEventTimeVeto,m_h_LBN);
      m_EMECA.h_MNBLooseEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_MNBLooseEventTimeVetoFrac,m_EMECC.h_MNBLooseEventTimeVeto,m_h_LBN);
      m_EMECC.h_MNBLooseEventTimeVetoFrac->SetEntries(m_eventCounter);
    
      Divide(m_BarrelA.h_NoisyWEventTimeVetoFrac,m_BarrelA.h_NoisyWEventTimeVeto,m_h_LBN);
      m_BarrelA.h_NoisyWEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_BarrelC.h_NoisyWEventTimeVetoFrac,m_BarrelC.h_NoisyWEventTimeVeto,m_h_LBN);
      m_BarrelC.h_NoisyWEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECA.h_NoisyWEventTimeVetoFrac,m_EMECA.h_NoisyWEventTimeVeto,m_h_LBN);
      m_EMECA.h_NoisyWEventTimeVetoFrac->SetEntries(m_eventCounter);
      Divide(m_EMECC.h_NoisyWEventTimeVetoFrac,m_EMECC.h_NoisyWEventTimeVeto,m_h_LBN);
      m_EMECC.h_NoisyWEventTimeVetoFrac->SetEntries(m_eventCounter);
    
      Divide(m_h_SaturatedTightFrac,m_h_SaturatedTight,m_h_LBN);
      m_h_SaturatedTightFrac->SetEntries(m_eventCounter);
      Divide(m_h_SaturatedTightTimeVetoFrac,m_h_SaturatedTightTimeVeto,m_h_LBN);
      m_h_SaturatedTightTimeVetoFrac->SetEntries(m_eventCounter);
    }//end if m_eventCounter>0
  }
  
  if ( endOfRunFlag() ) {
    // delete temposary histograms
    if ( m_h_LBN ) {
//      LWHist::safeDelete(m_h_LBN); 
//      m_h_LBN = NULL;
      m_h_LBN->Reset(); 
    }
    if ( m_h_NoisyEvent ) {
//      LWHist::safeDelete(m_h_NoisyEvent); 
//      m_h_NoisyEvent = NULL;
      m_h_NoisyEvent->Reset();
    }
    if ( m_BarrelA.h_NoisyEvent ) {
//      LWHist::safeDelete(m_BarrelA.h_NoisyEvent); 
//      m_BarrelA.h_NoisyEvent = NULL;
      m_BarrelA.h_NoisyEvent->Reset();
    }
    if ( m_BarrelC.h_NoisyEvent ) {
//      LWHist::safeDelete(m_BarrelC.h_NoisyEvent); 
//      m_BarrelC.h_NoisyEvent = NULL;
      m_BarrelC.h_NoisyEvent->Reset();
    }
    if ( m_EMECA.h_NoisyEvent ) {
//      LWHist::safeDelete(m_EMECA.h_NoisyEvent); 
//      m_EMECA.h_NoisyEvent = NULL;
      m_EMECA.h_NoisyEvent->Reset();
    }
    if ( m_EMECC.h_NoisyEvent ) {
//      LWHist::safeDelete(m_EMECC.h_NoisyEvent); 
//      m_EMECC.h_NoisyEvent = NULL;
      m_EMECC.h_NoisyEvent->Reset();
    }
    if ( m_h_NoisyEventTimeVeto ) {
//      LWHist::safeDelete(m_h_NoisyEventTimeVeto); 
//      m_h_NoisyEventTimeVeto = NULL;
      m_h_NoisyEventTimeVeto->Reset();
    }
    if ( m_BarrelA.h_NoisyEventTimeVeto ) {
//      LWHist::safeDelete(m_BarrelA.h_NoisyEventTimeVeto); 
//      m_BarrelA.h_NoisyEventTimeVeto = NULL;
      m_BarrelA.h_NoisyEventTimeVeto->Reset();
    }
    if ( m_BarrelC.h_NoisyEventTimeVeto ) {
//      LWHist::safeDelete(m_BarrelC.h_NoisyEventTimeVeto); 
//      m_BarrelC.h_NoisyEventTimeVeto = NULL;
      m_BarrelC.h_NoisyEventTimeVeto->Reset();
    }
    if ( m_EMECA.h_NoisyEventTimeVeto ) {
//      LWHist::safeDelete(m_EMECA.h_NoisyEventTimeVeto); 
//      m_EMECA.h_NoisyEventTimeVeto = NULL;
      m_EMECA.h_NoisyEventTimeVeto->Reset();
    }
    if ( m_EMECC.h_NoisyEventTimeVeto ) {
//      LWHist::safeDelete(m_EMECC.h_NoisyEventTimeVeto); 
//      m_EMECC.h_NoisyEventTimeVeto = NULL;
      m_EMECC.h_NoisyEventTimeVeto->Reset();
    }
    if ( m_h_MNBTightEvent ) {
//      LWHist::safeDelete(m_h_MNBTightEvent); 
//      m_h_MNBTightEvent = NULL;
      m_h_MNBTightEvent->Reset();
    }
    if ( m_BarrelA.h_MNBTightEvent ) {
//      LWHist::safeDelete(m_BarrelA.h_MNBTightEvent); 
//      m_BarrelA.h_MNBTightEvent = NULL;
      m_BarrelA.h_MNBTightEvent->Reset();
    }
    if ( m_BarrelC.h_MNBTightEvent ) {
//      LWHist::safeDelete(m_BarrelC.h_MNBTightEvent); 
//      m_BarrelC.h_MNBTightEvent = NULL;
      m_BarrelC.h_MNBTightEvent->Reset();
    }
    if ( m_EMECA.h_MNBTightEvent ) {
//      LWHist::safeDelete(m_EMECA.h_MNBTightEvent); 
//      m_EMECA.h_MNBTightEvent = NULL;
      m_EMECA.h_MNBTightEvent->Reset();
    }
    if ( m_EMECC.h_MNBTightEvent ) {
//      LWHist::safeDelete(m_EMECC.h_MNBTightEvent); 
//      m_EMECC.h_MNBTightEvent = NULL;
      m_EMECC.h_MNBTightEvent->Reset();
    }
    if ( m_h_MNBTightEventTimeVeto ) {
//      LWHist::safeDelete(h_MNBTightEventTimeVeto); 
//      h_MNBTightEventTimeVeto = NULL;
      m_h_MNBTightEventTimeVeto->Reset();
    }
    if ( m_BarrelA.h_MNBTightEventTimeVeto ) {
//      LWHist::safeDelete(m_BarrelA.h_MNBTightEventTimeVeto); 
//      m_BarrelA.h_MNBTightEventTimeVeto = NULL;
      m_BarrelA.h_MNBTightEventTimeVeto->Reset();
    }
    if ( m_BarrelC.h_MNBTightEventTimeVeto ) {
//      LWHist::safeDelete(m_BarrelC.h_MNBTightEventTimeVeto); 
//      m_BarrelC.h_MNBTightEventTimeVeto = NULL;
      m_BarrelC.h_MNBTightEventTimeVeto->Reset();
    }
    if ( m_EMECA.h_MNBTightEventTimeVeto ) {
//      LWHist::safeDelete(m_EMECA.h_MNBTightEventTimeVeto); 
//      m_EMECA.h_MNBTightEventTimeVeto = NULL;
      m_EMECA.h_MNBTightEventTimeVeto->Reset();
    }
    if ( m_EMECC.h_MNBTightEventTimeVeto ) {
//      LWHist::safeDelete(m_EMECC.h_MNBTightEventTimeVeto); 
//      m_EMECC.h_MNBTightEventTimeVeto = NULL;
      m_EMECC.h_MNBTightEventTimeVeto->Reset();
    }

    if ( m_h_MNBLooseEvent ) {
//      LWHist::safeDelete(m_h_MNBLooseEvent); 
//      m_h_MNBLooseEvent = NULL;
      m_h_MNBLooseEvent->Reset();
    }
    if ( m_BarrelA.h_MNBLooseEvent ) {
//      LWHist::safeDelete(m_BarrelA.h_MNBLooseEvent); 
//      m_BarrelA.h_MNBLooseEvent = NULL;
      m_BarrelA.h_MNBLooseEvent->Reset();
    }
    if ( m_BarrelC.h_MNBLooseEvent ) {
//      LWHist::safeDelete(m_BarrelC.h_MNBLooseEvent); 
//      m_BarrelC.h_MNBLooseEvent = NULL;
      m_BarrelC.h_MNBLooseEvent->Reset();
    }
    if ( m_EMECA.h_MNBLooseEvent ) {
//      LWHist::safeDelete(m_EMECA.h_MNBLooseEvent); 
//      m_EMECA.h_MNBLooseEvent = NULL;
      m_EMECA.h_MNBLooseEvent->Reset();
    }
    if ( m_EMECC.h_MNBLooseEvent ) {
//      LWHist::safeDelete(m_EMECC.h_MNBLooseEvent); 
//      m_EMECC.h_MNBLooseEvent = NULL;
      m_EMECC.h_MNBLooseEvent->Reset();
    }
    if ( m_h_MNBLooseEventTimeVeto ) {
//      LWHist::safeDelete(m_h_MNBLooseEventTimeVeto); 
//      m_h_MNBLooseEventTimeVeto = NULL;
      m_h_MNBLooseEventTimeVeto->Reset();
    }
    if ( m_BarrelA.h_MNBLooseEventTimeVeto ) {
//      LWHist::safeDelete(m_BarrelA.h_MNBLooseEventTimeVeto); 
//      m_BarrelA.h_MNBLooseEventTimeVeto = NULL;
      m_BarrelA.h_MNBLooseEventTimeVeto->Reset();
    }
    if ( m_BarrelC.h_MNBLooseEventTimeVeto ) {
//      LWHist::safeDelete(m_BarrelC.h_MNBLooseEventTimeVeto); 
//      m_BarrelC.h_MNBLooseEventTimeVeto = NULL;
      m_BarrelC.h_MNBLooseEventTimeVeto->Reset();
    }
    if ( m_EMECA.h_MNBLooseEventTimeVeto ) {
//      LWHist::safeDelete(m_EMECA.h_MNBLooseEventTimeVeto); 
//      m_EMECA.h_MNBLooseEventTimeVeto = NULL;
      m_EMECA.h_MNBLooseEventTimeVeto->Reset();
    }
    if ( m_EMECC.h_MNBLooseEventTimeVeto ) {
//      LWHist::safeDelete(m_EMECC.h_MNBLooseEventTimeVeto); 
//      m_EMECC.h_MNBLooseEventTimeVeto = NULL;
      m_EMECC.h_MNBLooseEventTimeVeto->Reset();
    }
    if ( m_h_SaturatedTight ) {
//      LWHist::safeDelete(m_h_SaturatedTight); 
//      m_h_SaturatedTight = NULL;
      m_h_SaturatedTight->Reset();
    }
    if ( m_BarrelA.h_SaturatedNoisyEvent ) {
//      LWHist::safeDelete(m_BarrelA.h_SaturatedNoisyEvent); 
//      m_BarrelA.h_SaturatedNoisyEvent = NULL;
      m_BarrelA.h_SaturatedNoisyEvent->Reset();
    }
    if ( m_BarrelC.h_SaturatedNoisyEvent ) {
//      LWHist::safeDelete(m_BarrelC.h_SaturatedNoisyEvent); 
//      m_BarrelC.h_SaturatedNoisyEvent = NULL;
      m_BarrelC.h_SaturatedNoisyEvent->Reset();
    }
    if ( m_EMECA.h_SaturatedNoisyEvent ) {
//      LWHist::safeDelete(m_EMECA.h_SaturatedNoisyEvent); 
//      m_EMECA.h_SaturatedNoisyEvent = NULL;
      m_EMECA.h_SaturatedNoisyEvent->Reset();
    }
    if ( m_EMECC.h_SaturatedNoisyEvent ) {
//      LWHist::safeDelete(m_EMECC.h_SaturatedNoisyEvent); 
//      m_EMECC.h_SaturatedNoisyEvent = NULL;
      m_EMECC.h_SaturatedNoisyEvent->Reset();
    }
    if ( m_h_SaturatedTightTimeVeto ) {
//      LWHist::safeDelete(m_h_SaturatedTightTimeVeto); 
//      m_h_SaturatedTightTimeVeto = NULL;
      m_h_SaturatedTightTimeVeto->Reset();
    }
    if ( m_BarrelA.h_SaturatedNoisyEventTimeVeto ) {
//      LWHist::safeDelete(m_BarrelA.h_SaturatedNoisyEventTimeVeto); 
//      m_BarrelA.h_SaturatedNoisyEventTimeVeto = NULL;
      m_BarrelA.h_SaturatedNoisyEventTimeVeto->Reset();
    }
    if ( m_BarrelC.h_SaturatedNoisyEventTimeVeto ) {
//      LWHist::safeDelete(m_BarrelC.h_SaturatedNoisyEventTimeVeto); 
//      m_BarrelC.h_SaturatedNoisyEventTimeVeto = NULL;
      m_BarrelC.h_SaturatedNoisyEventTimeVeto->Reset();
    }
    if ( m_EMECA.h_SaturatedNoisyEventTimeVeto ) {
//      LWHist::safeDelete(m_EMECA.h_SaturatedNoisyEventTimeVeto); 
//      m_EMECA.h_SaturatedNoisyEventTimeVeto = NULL;
      m_EMECA.h_SaturatedNoisyEventTimeVeto->Reset();
    }
    if ( m_EMECC.h_SaturatedNoisyEventTimeVeto ) {
//      LWHist::safeDelete(m_EMECC.h_SaturatedNoisyEventTimeVeto); 
//      m_EMECC.h_SaturatedNoisyEventTimeVeto = NULL;
      m_EMECC.h_SaturatedNoisyEventTimeVeto->Reset();
    }
  }  
  return StatusCode::SUCCESS;
}

void LArNoisyROMon::CopyHisto(LWHist2D* from,LWHist2D* to)
{
  unsigned xbin, ybin;
  double numer, error;
  from->resetActiveBinLoop();
  
  while(from->getNextActiveBin(xbin,ybin,numer,error)) 
  {    
    to->SetBinContentAndError(xbin,ybin,numer,error);       
  }
}


void LArNoisyROMon::CopyHisto(LWHist1D* from,LWHist1D* to)
{
  unsigned xbin;
  double numer, error;
  from->resetActiveBinLoop();
  
  while(from->getNextActiveBin(xbin,numer,error)) 
  {    
    to->SetBinContentAndError(xbin,numer,error);       
  }
}


void LArNoisyROMon::Divide(LWHist1D* to,LWHist1D* num,LWHist1D* denom)
{
  static double OneSigOneSided = 0.159; // 0.5*(1-0.681) where 0.681 means 68%CL
  
  to->Reset();
  unsigned Nbins = num->GetNbinsX();
  
  // can I throw an exception in the monitoring ? 
  if ( to->GetNbinsX() != Nbins || denom->GetNbinsX() != Nbins )
    throw(std::logic_error("Trying to divide histograms with different number of bins !"));
  
  for ( unsigned i = 0; i < Nbins; i++ ) 
  {
    double N = denom->GetBinContent(i);
    if ( N > 0)
    {
      double eps = num->GetBinContent(i) / N;
      double err = std::sqrt(eps*(1.-eps)/N);
      // specific cases for efficiency 0 or 1
      if ( eps == 0 ) err = 1.-std::pow(OneSigOneSided,1./N);
      else if ( eps > 0.99 ) err = 1.-std::pow(OneSigOneSided,1./N);
      eps *= 100.;
      err *= 100.;
      to->SetBinContentAndError(i,eps,err);
    }
  }
}


void LArNoisyROMon::bookPartitionHistos(partitionHistos& partition, const std::string& name, MonGroup& group, MonGroup& groupfrac, MonGroup& groupfracbin )
{
  std::string hName, hTitle;
  std::stringstream tit;
  
  // declare histograms properties
  // Endcaps
  int FEB=25,slot=15;
  double FEB_low=-0.5, FEB_up=24.5, slot_low=0.5, slot_up=15.5;
  // for Barrel
  if( name == "EMBA" || name == "EMBC")
  {
    FEB=32, slot=14;
    FEB_low=-0.5, FEB_up=31.5, slot_low=0.5, slot_up=14.5;
  }

  // 2D FEB maps (x : slot / y : FT) with z: fraction of events for which the FEB is noisy or mini-noisy.
  hName  = "NoisyFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB noisy "+m_NoisyFEBDefStr+" - "+name;
  partition.h_NoisyFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_NoisyFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_NoisyFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_NoisyFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_NoisyFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_NoisyFEBPerEvt).ignore();
  }
   
  hName  = "MNBTightFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB Tight (>17 chan with Q>4000) - "+name;
  partition.h_MNBTightFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_MNBTightFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_MNBTightFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_MNBTightFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_MNBTightFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_MNBTightFEBPerEvt).ignore();
  }
      
  hName  = "MNBLooseFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB Loose (>5 chan with Q>4000) - "+name;
  partition.h_MNBLooseFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_MNBLooseFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_MNBLooseFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_MNBLooseFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_MNBLooseFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_MNBLooseFEBPerEvt).ignore();
  }

  hName  = "CandidateMNBTightFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB Tight Candidate (>17 chan with Q>4000) - "+name;
  partition.h_CandidateMNBTightFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_CandidateMNBTightFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_CandidateMNBTightFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_CandidateMNBTightFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_CandidateMNBTightFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_CandidateMNBTightFEBPerEvt).ignore();
  }
      
  hName  = "CandidateMNBLooseFEBFracPerEvt_"+name;
  hTitle = "Yield of events with FEB MNB Loose Candidate (>5 chan with Q>4000) - "+name;
  partition.h_CandidateMNBLooseFEBFracPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_CandidateMNBLooseFEBFracPerEvt->GetXaxis()->SetTitle("Slot");
  partition.h_CandidateMNBLooseFEBFracPerEvt->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_CandidateMNBLooseFEBFracPerEvt).ignore();
  
  if ( m_IsOnline )
  {
    hName="temp_"+hName;
    partition.h_CandidateMNBLooseFEBPerEvt = TH2F_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
    group.regHist(partition.h_CandidateMNBLooseFEBPerEvt).ignore();
  }

  // Fraction of events found noisy per LB - Std flag
  hName = "NoisyEvent_"+name;
  partition.h_NoisyEventFrac = TH1F_LW::create(hName.c_str(),"Yield of events flagged as LArNoisyRO_Std", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_NoisyEvent_"+name;
  partition.h_NoisyEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEvent->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB - Std flag
  hName = "SaturatedNoisyEvent_"+name;
  partition.h_SaturatedNoisyEventFrac = TH1F_LW::create(hName.c_str(),"Yield of events flagged as LArNoisyRO Saturated", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_SaturatedNoisyEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_NoisyEvent_"+name;
  partition.h_SaturatedNoisyEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEvent->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBTightEvent_"+name;
  partition.h_MNBTightEventFrac = TH1F_LW::create(hName.c_str(),"Yield of events flagged as LArMNBTight", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBTightEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_MNBTightEvent_"+name;
  partition.h_MNBTightEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEvent->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBLooseEvent_"+name;
  partition.h_MNBLooseEventFrac = TH1F_LW::create(hName.c_str(),"Yield of events flagged as LArMNBLoose", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBLooseEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_MNBLooseEvent_"+name;
  partition.h_MNBLooseEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEvent->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB - Weighted flag
  hName = "NoisyWEvent_"+name;
  partition.h_NoisyWEventFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as LArNoisyRO_StdOpt", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyWEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyWEventFrac).ignore();  
  hName = "temp_NoisyWEvent_"+name;
  partition.h_NoisyWEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyWEvent->GetXaxis()->SetTitle("Luminosity Block");
  
  // Fraction of events found noisy per LB after time veto - Std flag
  hName = "NoisyEvent_TimeVeto_"+name;
  partition.h_NoisyEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as LArNoisyRO_Std not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyEventTimeVetoFrac).ignore();

  hName = "temp_NoisyEvent_TimeVeto_"+name;
  partition.h_NoisyEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB after time veto - Sat flag
  hName = "SaturatedNoisyEvent_TimeVeto_"+name;
  partition.h_SaturatedNoisyEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as LArNoisyRO Sat not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_SaturatedNoisyEventTimeVetoFrac).ignore();

  hName = "temp_SaturatedNoisyEvent_TimeVeto_"+name;
  partition.h_SaturatedNoisyEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBTightEvent_TimeVeto_"+name;
  partition.h_MNBTightEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as LArMNBTight not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBTightEventTimeVetoFrac).ignore();
    
  hName = "temp_MNBTightEvent_TimeVeto_"+name;
  partition.h_MNBTightEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBLooseEvent_TimeVeto_"+name;
  partition.h_MNBLooseEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as LArMNBLoose not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBLooseEventTimeVetoFrac).ignore();
    
  hName = "temp_MNBLooseEvent_TimeVeto_"+name;
  partition.h_MNBLooseEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB after time veto - Weighted flag
  hName = "NoisyWEvent_TimeVeto_"+name;
  partition.h_NoisyWEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as LArNoisyRO_StdOpt not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyWEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyWEventTimeVetoFrac).ignore();
  
  hName = "temp_NoisyWEvent_TimeVeto_"+name;
  partition.h_NoisyWEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyWEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");
  
  if ( m_doTrigger ) 
  {
    unsigned int siz = m_EF_NoiseBurst_Triggers.size();
    hName = "NoisyEventTrigger_"+name;
    hTitle = "Trigger fired for noisy event - "+name;
    partition.h_NoisyEventTrigger = TH1I_LW::create(hName.c_str(), hTitle.c_str(), siz+1, 0.5, siz+1.5);
    LWHist::LWHistAxis* axis = partition.h_NoisyEventTrigger->GetXaxis();
    axis->SetTitle("Special trigger fired");
    for ( size_t i = 0; i < siz; i++)
    {
      axis->SetBinLabel(i+1,m_EF_NoiseBurst_Triggers[i].c_str());
    }
    axis->SetBinLabel(siz+1,"NONE");
    group.regHist(partition.h_NoisyEventTrigger).ignore();


    siz = m_L1_NoiseBurst_Triggers.size();
    hName = "NoisyEventL1Term_"+name;
    hTitle = "L1 term fired for noisy event - "+name;
    partition.h_NoisyEventTriggerL1 = TH1I_LW::create(hName.c_str(), hTitle.c_str(), siz+1, 0.5, siz+1.5);
    axis = partition.h_NoisyEventTriggerL1->GetXaxis();
    axis->SetTitle("L1 term fired");
    for ( size_t i = 0; i < siz; i++)
    {
      axis->SetBinLabel(i+1,m_L1_NoiseBurst_Triggers[i].c_str());
    }
    axis->SetBinLabel(siz+1,"NONE");
    group.regHist(partition.h_NoisyEventTriggerL1).ignore();
  }
}


void LArNoisyROMon::fillTriggerHisto(partitionHistos& partition, uint8_t triggerbits, uint8_t L1triggerbits)
{
  if ( triggerbits ==0 )
  {
    partition.h_NoisyEventTrigger->Fill(double(m_EF_NoiseBurst_Triggers.size()+1));
  }
  else
  {
    for ( size_t i = 0; i < m_EF_NoiseBurst_Triggers.size(); i++)
    {
      if ( triggerbits & (0x1 << i) ) partition.h_NoisyEventTrigger->Fill(double(i+1));
    }
  }

  if ( L1triggerbits ==0 )
  {
    partition.h_NoisyEventTriggerL1->Fill(double(m_L1_NoiseBurst_Triggers.size()+1));
  }
  else
  {
    for ( size_t i = 0; i < m_L1_NoiseBurst_Triggers.size(); i++)
    {
      if ( L1triggerbits & (0x1 << i) ) partition.h_NoisyEventTriggerL1->Fill(double(i+1));
    }
  }

}

StatusCode LArNoisyROMon::finalize()
{
  ATH_MSG_INFO(  " in LArNoisyROMon::finalHists() " );
  // delete temposary histograms

  if ( m_h_LBN ) {
    LWHist::safeDelete(m_h_LBN); 
    m_h_LBN = nullptr;
  }

  if ( m_h_NoisyEvent ) {
    LWHist::safeDelete(m_h_NoisyEvent); 
    m_h_NoisyEvent = nullptr;
  }

  if ( m_h_NoisyEventTimeVeto ) {
    LWHist::safeDelete(m_h_NoisyEventTimeVeto); 
    m_h_NoisyEventTimeVeto = nullptr;
  }
  
  if ( m_h_MNBTightEvent ) {
    LWHist::safeDelete(m_h_MNBTightEvent); 
    m_h_MNBTightEvent = nullptr;
  }

  if ( m_h_MNBTightEventTimeVeto ) {
    LWHist::safeDelete(m_h_MNBTightEventTimeVeto); 
    m_h_MNBTightEventTimeVeto = nullptr;
  }
  
  if ( m_h_MNBLooseEvent ) {
    LWHist::safeDelete(m_h_MNBLooseEvent); 
    m_h_MNBLooseEvent = nullptr;
  }

  if ( m_h_MNBLooseEventTimeVeto ) {
    LWHist::safeDelete(m_h_MNBLooseEventTimeVeto); 
    m_h_MNBLooseEventTimeVeto = nullptr;
  }

  if ( m_h_SaturatedTight ) {
    LWHist::safeDelete(m_h_SaturatedTight); 
    m_h_SaturatedTight = nullptr;
  }

  if ( m_h_SaturatedTightTimeVeto ) {
    LWHist::safeDelete(m_h_SaturatedTightTimeVeto); 
    m_h_SaturatedTightTimeVeto = nullptr;
  }


  for (partitionHistos* part : {&m_BarrelA, &m_BarrelC, &m_EMECA, &m_EMECC}) {
     LWHist::safeDelete(part->h_NoisyEvent); 
     part->h_NoisyEvent = nullptr;
     
     LWHist::safeDelete(part->h_NoisyWEvent); 
     part->h_NoisyWEvent = nullptr;

     LWHist::safeDelete(part->h_NoisyEventTimeVeto); 
     part->h_NoisyEventTimeVeto = nullptr;

     LWHist::safeDelete(part->h_MNBTightEvent); 
     part->h_MNBTightEvent = nullptr;

     LWHist::safeDelete(part->h_MNBTightEventTimeVeto); 
     part->h_MNBTightEventTimeVeto = nullptr;

     LWHist::safeDelete(part->h_MNBLooseEvent); 
     part->h_MNBLooseEvent = nullptr;

     LWHist::safeDelete(part->h_MNBLooseEventTimeVeto); 
     part->h_MNBLooseEventTimeVeto = nullptr;

     LWHist::safeDelete(part->h_SaturatedNoisyEvent); 
     part->h_SaturatedNoisyEvent = nullptr;

     LWHist::safeDelete(part->h_SaturatedNoisyEventTimeVeto); 
     part->h_SaturatedNoisyEventTimeVeto = nullptr;

     LWHist::safeDelete(part->h_NoisyWEventTimeVeto);
     part->h_NoisyWEventTimeVeto=nullptr;
  }



  return StatusCode::SUCCESS;
}
