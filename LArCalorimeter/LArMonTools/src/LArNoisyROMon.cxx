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
  m_NoiseTimeTree(NULL), m_eventCounter(0), m_LArOnlineIDHelper(NULL), m_strHelper(NULL), m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"), m_EF_NoiseBurst_Triggers(), m_L1_NoiseBurst_Triggers()
{
  declareProperty("IsOnline",       m_IsOnline=false);
  declareProperty("NoisyFEBDefStr", m_NoisyFEBDefStr="");
  declareProperty("BadFEBCut",  m_BadFEBCut=999999);
  declareProperty("doTrigger",       m_doTrigger=true);
  declareProperty("EFNoiseBurstTriggers",m_EF_NoiseBurst_Triggers);
  declareProperty("L1NoiseBurstTriggers",m_L1_NoiseBurst_Triggers);
  
  m_NoiseTime.time = 0;
  m_NoiseTime.time_ns = 0;
  m_NoiseTime.algo = 0;

  h_NoisyFEB			= NULL;
  h_NoisyEventFrac		= NULL;
  h_NoisyEventTimeVetoFrac	= NULL;
  h_SaturatedTightFrac		= NULL;
  h_NoisyEvent			= NULL;
  h_NoisyEventTimeVeto		= NULL;
  h_SaturatedTight		= NULL;
  h_SaturatedTightTimeVeto	= NULL;
  h_SaturatedTightTimeVetoFrac	= NULL;
  h_LBN				= NULL;

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
    msg(MSG::FATAL) << "unable to retrieve LArOnlineID from detStore" << endreq;
    return StatusCode::FAILURE;
  }
  
  m_strHelper = new LArOnlineIDStrHelper(m_LArOnlineIDHelper);
  m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);


  if ( m_doTrigger )
  {
    StatusCode sc = m_trigDec.retrieve();
    if ( !sc.isSuccess() )
    {
      msg(MSG::FATAL) << "unable to initialize TrigDecisionTool " << endreq;
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

    // Book histograms per partitions    
    MonGroup GroupBarrel(this, "/LAr/NoisyRO/Barrel", run, ATTRIB_MANAGED );
    MonGroup GroupBarrelFrac(this, "/LAr/NoisyRO/Barrel", run, ATTRIB_MANAGED ,"", "weightedEff");
    MonGroup GroupBarrelFracBin(this, "/LAr/NoisyRO/Barrel", run, ATTRIB_MANAGED ,"", "perBinEffPerCent");
    bookPartitionHistos(m_BarrelA,"BarrelA",GroupBarrel,GroupBarrelFrac,GroupBarrelFracBin);
    bookPartitionHistos(m_BarrelC,"BarrelC",GroupBarrel,GroupBarrelFrac,GroupBarrelFracBin);
    
    MonGroup GroupEMEC(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED );
    MonGroup GroupEMECFrac(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED,"", "weightedEff" );
    MonGroup GroupEMECFracBin(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED,"", "perBinEffPerCent" );
    bookPartitionHistos(m_EMECA,"EMECA",GroupEMEC,GroupEMECFrac,GroupEMECFracBin);
    bookPartitionHistos(m_EMECC,"EMECC",GroupEMEC,GroupEMECFrac,GroupEMECFracBin);
    
    // Book general histograms
    MonGroup overall(this, "/LAr/NoisyRO", run, ATTRIB_MANAGED );
    hTitle = "Number of noisy FEB "+m_NoisyFEBDefStr+ " per event";
    h_NoisyFEB = TH1I_LW::create("NoisyFEB", hTitle.c_str(), 50,0.,50.);
    h_NoisyFEB->GetXaxis()->SetTitle("# of noisy FEB");
    overall.regHist(h_NoisyFEB).ignore();
        
    m_NoiseTimeTree = new TTree("LArNoise","LAr noisy events");
    m_NoiseTimeTree->Branch("time",&m_NoiseTime.time,"time/i");
    m_NoiseTimeTree->Branch("time_ns",&m_NoiseTime.time_ns,"time_ns/i");
    m_NoiseTimeTree->Branch("algo",&m_NoiseTime.algo,"algo/b");
    overall.regTree(m_NoiseTimeTree).ignore();
    
    MonGroup overallPerCent(this, "/LAr/NoisyRO", run, ATTRIB_MANAGED, "", "perBinEffPerCent" );
    tit << "Percentage of events flagged as noisy (> " << m_BadFEBCut << " noisy FEBs)";
    hTitle = tit.str();
    tit.str("");
    h_NoisyEventFrac = TH1F_LW::create("NoisyEvent", hTitle.c_str(), 2000, 0.5, 2000.5);
    h_NoisyEventFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(h_NoisyEventFrac).ignore();
    h_NoisyEvent = TH1I_LW::create("temp_NoisyEvent", hTitle.c_str(), 2000, 0.5, 2000.5);
    h_NoisyEvent->GetXaxis()->SetTitle("Luminosity Block");
    
    h_LBN = TH1I_LW::create("LBN", "LBN ", 2000, 0.5, 2000.5);    
    
    tit << "Percentage of events flagged as noisy outside time veto window (> " << m_BadFEBCut << " noisy FEBs)";
    hTitle = tit.str();
    tit.str("");
    h_NoisyEventTimeVetoFrac = TH1F_LW::create("NoisyEvent_TimeVeto", hTitle.c_str(), 2000, 0.5, 2000.5);
    h_NoisyEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(h_NoisyEventTimeVetoFrac).ignore();        
    h_NoisyEventTimeVeto = TH1I_LW::create("temp_NoisyEvent_TimeVeto", hTitle.c_str(), 2000, 0.5, 2000.5);
    h_NoisyEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");
    
    hTitle = "Percentage of events flagged as noisy with saturated quality cells (tight cuts)";
    h_SaturatedTightFrac = TH1F_LW::create("SaturatedTightEvent", hTitle.c_str(), 2000, 0.5, 2000.5);
    h_SaturatedTightFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(h_SaturatedTightFrac).ignore();
    h_SaturatedTight = TH1I_LW::create("temp_SaturatedTight", hTitle.c_str(), 2000, 0.5, 2000.5);
    h_SaturatedTight->GetXaxis()->SetTitle("Luminosity Block");
    
    hTitle = "Percentage of events flagged as noisy with saturated quality cells (tight cuts) outside time veto window";
    h_SaturatedTightTimeVetoFrac = TH1F_LW::create("SaturatedTightEvent_TimeVeto", hTitle.c_str(), 2000, 0.5, 2000.5);
    h_SaturatedTightTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
    overallPerCent.regHist(h_SaturatedTightTimeVetoFrac).ignore();
    h_SaturatedTightTimeVeto = TH1I_LW::create("temp_SaturatedTight_TimeVeto", hTitle.c_str(), 2000, 0.5, 2000.5);
    h_SaturatedTightTimeVeto->GetXaxis()->SetTitle("Luminosity Block");
      
  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROMon::fillHistograms()
{
  StatusCode sc(StatusCode::SUCCESS);
  
  m_eventCounter++;
  
  // retrieve 
  const LArNoisyROSummary* noisyRO;
  sc = evtStore()->retrieve(noisyRO,"LArNoisyROSummary");
  if (sc.isFailure()) 
  {
    msg(MSG::WARNING) << "Can't retrieve LArNoisyROSummary " <<endreq;
    return StatusCode::SUCCESS;
  }
  
  
  const xAOD::EventInfo* eventInfo;
  sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) 
  {
    msg(MSG::WARNING) << "Can't retrieve EventInfo " <<endreq;
    return StatusCode::SUCCESS;
  }
  
  unsigned int LBN = eventInfo->lumiBlock();
  bool burstveto = eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::NOISEBURSTVETO);

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
  if ( m_NoiseTime.algo != 0 ) 
  {
    if ( burstveto ) m_NoiseTime.algo |= 0x4;
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

  
  // event flagged by # of saturated quality cells
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ) ) 
  {
    h_SaturatedTight->Fill(LBN);
    if ( !burstveto ) h_SaturatedTightTimeVeto->Fill(LBN);
    //    uint8_t SatTightPartitions = noisyRO->SatTightFlaggedPartitions();
    // General plot on partition affected by SatTight removed
    // Should be replaced by a 
//    if ( (SatTightPartitions & LArNoisyROSummary::EMECAMask) != 0 )
//    { 
//      h_SaturatedTightPartition->Fill(0.1);
//    }
//    if ( (SatTightPartitions & LArNoisyROSummary::EMBAMask) != 0 ) 
//    { 
//      h_SaturatedTightPartition->Fill(1.1);
//    }
//    if ( (SatTightPartitions & LArNoisyROSummary::EMBCMask) != 0 ) 
//    { 
//      h_SaturatedTightPartition->Fill(2.1);
//    }
//    if ( (SatTightPartitions & LArNoisyROSummary::EMECCMask) != 0 ) 
//    { 
//      h_SaturatedTightPartition->Fill(3.1);
//    }
  }
  
  // Fill the 2D map of noisy FEBs
  const std::vector<HWIdentifier>& noisyFEB = noisyRO->get_noisy_febs();
  
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
  h_NoisyFEB->Fill(NbNoisyFEB);
  h_LBN->Fill(LBN);
  
  // Event found noisy by Std method
  uint8_t BadFEBPartitions = noisyRO->BadFEBFlaggedPartitions();
  if ( BadFEBPartitions != 0) 
  {
    h_NoisyEvent->Fill(LBN);
    if ( ! burstveto ) 
    {
      h_NoisyEventTimeVeto->Fill(LBN);
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
  if ( endOfRun || endOfEventsBlock){
    if ( m_IsOnline )
    {
      // copy the "running" histo to the final ones, to be normalised
      CopyHisto(m_BarrelA.h_NoisyFEBPerEvt,m_BarrelA.h_NoisyFEBFracPerEvt);
      CopyHisto(m_BarrelC.h_NoisyFEBPerEvt,m_BarrelC.h_NoisyFEBFracPerEvt);
      CopyHisto(m_EMECA.h_NoisyFEBPerEvt,m_EMECA.h_NoisyFEBFracPerEvt);
      CopyHisto(m_EMECC.h_NoisyFEBPerEvt,m_EMECC.h_NoisyFEBFracPerEvt);
    }
    
    double scale = 100./static_cast<double>(m_eventCounter);
    m_BarrelA.h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
    m_BarrelA.h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);
    m_BarrelC.h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
    m_BarrelC.h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);
    m_EMECA.h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
    m_EMECA.h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);
    m_EMECC.h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
    m_EMECC.h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);
    
    Divide(h_NoisyEventFrac,h_NoisyEvent,h_LBN);
    h_NoisyEventFrac->SetEntries(m_eventCounter);

    Divide(m_BarrelA.h_NoisyEventFrac,m_BarrelA.h_NoisyEvent,h_LBN);
    m_BarrelA.h_NoisyEventFrac->SetEntries(m_eventCounter);
    Divide(m_BarrelC.h_NoisyEventFrac,m_BarrelC.h_NoisyEvent,h_LBN);
    m_BarrelC.h_NoisyEventFrac->SetEntries(m_eventCounter);
    Divide(m_EMECA.h_NoisyEventFrac,m_EMECA.h_NoisyEvent,h_LBN);
    m_EMECA.h_NoisyEventFrac->SetEntries(m_eventCounter);
    Divide(m_EMECC.h_NoisyEventFrac,m_EMECC.h_NoisyEvent,h_LBN);
    m_EMECC.h_NoisyEventFrac->SetEntries(m_eventCounter);

    Divide(m_BarrelA.h_NoisyWEventFrac,m_BarrelA.h_NoisyWEvent,h_LBN);
    m_BarrelA.h_NoisyWEventFrac->SetEntries(m_eventCounter);
    Divide(m_BarrelC.h_NoisyWEventFrac,m_BarrelC.h_NoisyWEvent,h_LBN);
    m_BarrelC.h_NoisyWEventFrac->SetEntries(m_eventCounter);
    Divide(m_EMECA.h_NoisyWEventFrac,m_EMECA.h_NoisyWEvent,h_LBN);
    m_EMECA.h_NoisyWEventFrac->SetEntries(m_eventCounter);
    Divide(m_EMECC.h_NoisyWEventFrac,m_EMECC.h_NoisyWEvent,h_LBN);
    m_EMECC.h_NoisyWEventFrac->SetEntries(m_eventCounter);
    
    Divide(h_NoisyEventTimeVetoFrac,h_NoisyEventTimeVeto,h_LBN);
    h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
    Divide(m_BarrelA.h_NoisyEventTimeVetoFrac,m_BarrelA.h_NoisyEventTimeVeto,h_LBN);
    m_BarrelA.h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
    Divide(m_BarrelC.h_NoisyEventTimeVetoFrac,m_BarrelC.h_NoisyEventTimeVeto,h_LBN);
    m_BarrelC.h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
    Divide(m_EMECA.h_NoisyEventTimeVetoFrac,m_EMECA.h_NoisyEventTimeVeto,h_LBN);
    m_EMECA.h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
    Divide(m_EMECC.h_NoisyEventTimeVetoFrac,m_EMECC.h_NoisyEventTimeVeto,h_LBN);
    m_EMECC.h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
    
    Divide(m_BarrelA.h_NoisyWEventTimeVetoFrac,m_BarrelA.h_NoisyWEventTimeVeto,h_LBN);
    m_BarrelA.h_NoisyWEventTimeVetoFrac->SetEntries(m_eventCounter);
    Divide(m_BarrelC.h_NoisyWEventTimeVetoFrac,m_BarrelC.h_NoisyWEventTimeVeto,h_LBN);
    m_BarrelC.h_NoisyWEventTimeVetoFrac->SetEntries(m_eventCounter);
    Divide(m_EMECA.h_NoisyWEventTimeVetoFrac,m_EMECA.h_NoisyWEventTimeVeto,h_LBN);
    m_EMECA.h_NoisyWEventTimeVetoFrac->SetEntries(m_eventCounter);
    Divide(m_EMECC.h_NoisyWEventTimeVetoFrac,m_EMECC.h_NoisyWEventTimeVeto,h_LBN);
    m_EMECC.h_NoisyWEventTimeVetoFrac->SetEntries(m_eventCounter);
    
    Divide(h_SaturatedTightFrac,h_SaturatedTight,h_LBN);
    h_SaturatedTightFrac->SetEntries(m_eventCounter);
    Divide(h_SaturatedTightTimeVetoFrac,h_SaturatedTightTimeVeto,h_LBN);
    h_SaturatedTightTimeVetoFrac->SetEntries(m_eventCounter);
    
  }
  
  if ( endOfRun ) {
    // delete temposary histograms
    if ( h_LBN ) {
      LWHist::safeDelete(h_LBN); 
      h_LBN = NULL;
    }
    if ( h_NoisyEvent ) {
      LWHist::safeDelete(h_NoisyEvent); 
      h_NoisyEvent = NULL;
    }
    if ( m_BarrelA.h_NoisyEvent ) {
      LWHist::safeDelete(m_BarrelA.h_NoisyEvent); 
      m_BarrelA.h_NoisyEvent = NULL;
    }
    if ( m_BarrelC.h_NoisyEvent ) {
      LWHist::safeDelete(m_BarrelC.h_NoisyEvent); 
      m_BarrelC.h_NoisyEvent = NULL;
    }
    if ( m_EMECA.h_NoisyEvent ) {
      LWHist::safeDelete(m_EMECA.h_NoisyEvent); 
      m_EMECA.h_NoisyEvent = NULL;
    }
    if ( m_EMECC.h_NoisyEvent ) {
      LWHist::safeDelete(m_EMECC.h_NoisyEvent); 
      m_EMECC.h_NoisyEvent = NULL;
    }
    if ( h_NoisyEventTimeVeto ) {
      LWHist::safeDelete(h_NoisyEventTimeVeto); 
      h_NoisyEventTimeVeto = NULL;
    }
    if ( m_BarrelA.h_NoisyEventTimeVeto ) {
      LWHist::safeDelete(m_BarrelA.h_NoisyEventTimeVeto); 
      m_BarrelA.h_NoisyEventTimeVeto = NULL;
    }
    if ( m_BarrelC.h_NoisyEventTimeVeto ) {
      LWHist::safeDelete(m_BarrelC.h_NoisyEventTimeVeto); 
      m_BarrelC.h_NoisyEventTimeVeto = NULL;
    }
    if ( m_EMECA.h_NoisyEventTimeVeto ) {
      LWHist::safeDelete(m_EMECA.h_NoisyEventTimeVeto); 
      m_EMECA.h_NoisyEventTimeVeto = NULL;
    }
    if ( m_EMECC.h_NoisyEventTimeVeto ) {
      LWHist::safeDelete(m_EMECC.h_NoisyEventTimeVeto); 
      m_EMECC.h_NoisyEventTimeVeto = NULL;
    }
    if ( h_SaturatedTight ) {
      LWHist::safeDelete(h_SaturatedTight); 
      h_SaturatedTight = NULL;
    }
    if ( h_SaturatedTightTimeVeto ) {
      LWHist::safeDelete(h_SaturatedTightTimeVeto); 
      h_SaturatedTightTimeVeto = NULL;
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
  if( name == "BarrelA" || name == "BarrelC")
  {
    FEB=32, slot=14;
    FEB_low=-0.5, FEB_up=31.5, slot_low=0.5, slot_up=14.5;
  }

  // 2D FEB maps (x : slot / y : FT) with z: fraction of events for which the FEB is noisy.
  hName  = "NoisyFEBFracPerEvt_"+name;
  hTitle = "Percentage of events with FEB noisy "+m_NoisyFEBDefStr+" - "+name;
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
      
  // Fraction of events found noisy per LB - Std flag
  tit << "Percentage of events flagged as noisy (> " << m_BadFEBCut << " noisy FEBs) - " << name;
  hTitle = tit.str();
  tit.str("");
  hName = "NoisyEvent_"+name;
  partition.h_NoisyEventFrac = TH1F_LW::create(hName.c_str(), hTitle.c_str(), 2000, 0.5, 2000.5);
  partition.h_NoisyEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyEventFrac).ignore();  
  hName = "temp_NoisyEvent_"+name;
  partition.h_NoisyEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), 2000, 0.5, 2000.5);
  partition.h_NoisyEvent->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB - Weighted flag
  tit << "Percentage of events flagged as noisy (> " << m_BadFEBCut << " noisy FEBs with weighted sum) - " << name;
  hTitle = tit.str();
  tit.str("");
  hName = "NoisyWEvent_"+name;
  partition.h_NoisyWEventFrac = TH1F_LW::create(hName.c_str(), hTitle.c_str(), 2000, 0.5, 2000.5);
  partition.h_NoisyWEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyWEventFrac).ignore();  
  hName = "temp_NoisyWEvent_"+name;
  partition.h_NoisyWEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), 2000, 0.5, 2000.5);
  partition.h_NoisyWEvent->GetXaxis()->SetTitle("Luminosity Block");

  
  
  // Fraction of events found noisy per LB after time veto - Std flag
  tit << "Percentage of events flagged as noisy outside time veto window (> " << m_BadFEBCut << " noisy FEBs) - " << name;
  hTitle = tit.str();
  tit.str("");
  hName = "NoisyEvent_TimeVeto_"+name;
  partition.h_NoisyEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), hTitle.c_str(), 2000, 0.5, 2000.5);
  partition.h_NoisyEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyEventTimeVetoFrac).ignore();
  
  hName = "temp_NoisyEvent_TimeVeto_"+name;
  partition.h_NoisyEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), 2000, 0.5, 2000.5);
  partition.h_NoisyEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB after time veto - Weighted flag
  tit << "Percentage of events flagged as noisy outside time veto window (> " << m_BadFEBCut << " noisy FEBs with weighted sum) - " << name;
  hTitle = tit.str();
  tit.str("");
  hName = "NoisyWEvent_TimeVeto_"+name;
  partition.h_NoisyWEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), hTitle.c_str(), 2000, 0.5, 2000.5);
  partition.h_NoisyWEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyWEventTimeVetoFrac).ignore();
  
  hName = "temp_NoisyWEvent_TimeVeto_"+name;
  partition.h_NoisyWEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), 2000, 0.5, 2000.5);
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
