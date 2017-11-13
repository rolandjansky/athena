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
#include "LWHists/TH2I_LW.h"
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
  declareProperty("NoisyFEBDefStr", m_NoisyFEBDefStr="(unknown)");
  declareProperty("MNBTightFEBDefStr", m_MNBTightFEBDefStr="(unknown)");
  declareProperty("MNBLooseFEBDefStr", m_MNBLooseFEBDefStr="(unknown)");
  declareProperty("BadFEBCut",  m_BadFEBCut=999999);
  declareProperty("KnownMNBFEBs",   m_knownMNBFEBs={});
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
    m_partHistos.resize(4);
    bookPartitionHistos(m_partHistos[1],"EMBA",GroupBarrel,GroupBarrelFrac,GroupBarrelFracBin);
    bookPartitionHistos(m_partHistos[2],"EMBC",GroupBarrel,GroupBarrelFrac,GroupBarrelFracBin);
    
    MonGroup GroupEMEC(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED );
    MonGroup GroupEMECFrac(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED,"", "weightedEff" );
    MonGroup GroupEMECFracBin(this, "/LAr/NoisyRO/EMEC", run, ATTRIB_MANAGED,"", "perBinEffPerCent" );
    bookPartitionHistos(m_partHistos[0],"EMECA",GroupEMEC,GroupEMECFrac,GroupEMECFracBin);
    bookPartitionHistos(m_partHistos[3],"EMECC",GroupEMEC,GroupEMECFrac,GroupEMECFracBin);

    
    // Book general histograms - Number of RNB noisy FEBs
    hTitle = "Number of noisy FEB "+m_NoisyFEBDefStr+ " per event";
    m_h_NoisyFEB = TH1I_LW::create("NoisyFEB", hTitle.c_str(), 50,0.,50.);
    m_h_NoisyFEB->GetXaxis()->SetTitle("# of noisy FEB");
    overall.regHist(m_h_NoisyFEB).ignore();
    
    m_h_LBN = TH1I_LW::create("LBN", "Event counter per LB", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);    

    // Fill Suspicious MNB FEBs
    for (uint i=0;i<m_knownMNBFEBs.size();i++){
      const HWIdentifier& febid = HWIdentifier(m_knownMNBFEBs[i]);
      HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
      int FT = m_LArOnlineIDHelper->feedthrough(id);
      int slot = m_LArOnlineIDHelper->slot(id);
      int partition = partitionNumber(febid);
      m_partHistos[partition].h_MNBKnownFEB->Fill(slot,FT);
    }


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
  std::array<unsigned,4> partMask({{LArNoisyROSummary::EMECAMask,LArNoisyROSummary::EMBAMask,LArNoisyROSummary::EMBCMask,LArNoisyROSummary::EMECCMask}});

  for (size_t i = 0; i<noisyFEB.size(); i++)
  {
    //std::cout << "Noisy FEB " <<  noisyFEB[i].get_compact() << std::endl;
    NbNoisyFEB++;
    const HWIdentifier& febid = noisyFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
      if (m_IsOnline)
      	  m_partHistos[partition].h_NoisyFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
      	else
      	  m_partHistos[partition].h_NoisyFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
    }

  } // End of loop on all RNB - noisy FEB
  m_h_NoisyFEB->Fill(NbNoisyFEB);

  // Loop on all FEBs noisy in MNB-tight definition
  // And fill the 2D maps of fraction of fraction of noisy events
  // Fill two histograms with veto cut and all events
  for (size_t i = 0; i<mnbtightFEB.size(); i++)
  {
    //std::cout << "MNBTight FEB " <<  noisyFEB[i].get_compact() << std::endl;
    const HWIdentifier& febid = mnbtightFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
      if (m_IsOnline)
        {
	  m_partHistos[partition].h_CandidateMNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTightFlaggedPartitions() & partMask[partition]) != 0) 
            m_partHistos[partition].h_MNBTightFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
	  m_partHistos[partition].h_CandidateMNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBTightFlaggedPartitions() & partMask[partition]) != 0) 
            m_partHistos[partition].h_MNBTightFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
    }

  }// End of loop on all MNB-Tight - noisy FEB

  // Loop on all FEBs noisy in MNB-loose definition
  // And fill the 2D maps of fraction of fraction of noisy events
  // Fill two histograms with veto cut and all events
  for (size_t i = 0; i<mnblooseFEB.size(); i++)
  {
    //std::cout << "MNBLoose FEB " <<  noisyFEB[i].get_compact() << std::endl;
    const HWIdentifier& febid = mnblooseFEB[i];
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(febid,0);
    int FT = m_LArOnlineIDHelper->feedthrough(id);
    int slot = m_LArOnlineIDHelper->slot(id);
    int partition = partitionNumber(febid);

    if (partition<4){
      if (m_IsOnline)
        {
	  m_partHistos[partition].h_CandidateMNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBLooseFlaggedPartitions() & partMask[partition]) != 0) 
            m_partHistos[partition].h_MNBLooseFEBPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
        else
        {
	  m_partHistos[partition].h_CandidateMNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
          if((noisyRO->MNBLooseFlaggedPartitions() & partMask[partition]) != 0) 
            m_partHistos[partition].h_MNBLooseFEBFracPerEvt->Fill(static_cast<double>(slot), static_cast<double>(FT)+0.1);
        }
    }

  }// End of loop on all MNB-Loose - noisy FEB


  // End of 2D map of FEB found as noisy (in any definition : Std, MNB-Tight or MNB-Loose)
  // Now fill 1D histograms of fraction of events found as noisy vetoed or not
  m_h_LBN->Fill(LBN);  
  // Event found noisy by Std method
  uint8_t BadFEBPartitions = noisyRO->BadFEBFlaggedPartitions();
  if ( BadFEBPartitions != 0) {
    for (int i= 0;i<4;i++){
      if ( (BadFEBPartitions & partMask[i]) != 0 ) {
	m_partHistos[i].h_NoisyEvent->Fill(LBN);
	if ( m_doTrigger ) fillTriggerHisto(m_partHistos[i],trigbits,L1trigbits);
	if ( ! burstveto ) m_partHistos[i].h_NoisyEventTimeVeto->Fill(LBN);
      }
    }
  } // End of test on RNB

  // event flagged by # of saturated quality cells
  uint8_t SatTightPartitions = noisyRO->SatTightFlaggedPartitions();
  if ( eventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ) ) {
    for (int i= 0;i<4;i++){
      if ( (SatTightPartitions & partMask[i]) != 0 ) {
	m_partHistos[i].h_SaturatedNoisyEvent->Fill(LBN);
	if ( ! burstveto ) m_partHistos[i].h_SaturatedNoisyEventTimeVeto->Fill(LBN);
      }
    }
  } // end of test on RNB-Saturated


  // event flagged by tight-MNB
  uint8_t MNBTightPartitions = noisyRO->MNBTightFlaggedPartitions();
  if ( MNBTightPartitions != 0) {
    for (int i= 0;i<4;i++){
      if ( (MNBTightPartitions & partMask[i]) != 0 ) {
	m_partHistos[i].h_MNBTightEvent->Fill(LBN);
	if ( ! burstveto ) m_partHistos[i].h_MNBTightEventTimeVeto->Fill(LBN);
      }
    }
  } // End of test on MNB-Tight

  // event flagged by loose-MNB
  uint8_t MNBLoosePartitions = noisyRO->MNBLooseFlaggedPartitions();
  if ( MNBLoosePartitions != 0) {
    for (int i= 0;i<4;i++){
      if ( (MNBLoosePartitions & partMask[i]) != 0 ) {
	m_partHistos[i].h_MNBLooseEvent->Fill(LBN);
	if ( ! burstveto ) m_partHistos[i].h_MNBLooseEventTimeVeto->Fill(LBN);
      }
    }
  } // end fo test on MNB-Loose
    
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
    if ( m_IsOnline ){
      // copy the "running" histo to the final ones, to be normalised
      for (int i=0;i<4;i++){
	copyHisto(m_partHistos[i].h_NoisyFEBPerEvt,m_partHistos[i].h_NoisyFEBFracPerEvt);
	copyHisto(m_partHistos[i].h_MNBTightFEBPerEvt,m_partHistos[i].h_MNBTightFEBFracPerEvt);
	copyHisto(m_partHistos[i].h_MNBLooseFEBPerEvt,m_partHistos[i].h_MNBLooseFEBFracPerEvt);
	copyHisto(m_partHistos[i].h_CandidateMNBTightFEBPerEvt,m_partHistos[i].h_CandidateMNBTightFEBFracPerEvt);
	copyHisto(m_partHistos[i].h_CandidateMNBLooseFEBPerEvt,m_partHistos[i].h_CandidateMNBLooseFEBFracPerEvt);
      }
    }

    if (m_eventCounter>0) {
      const double scale =  100./static_cast<double>(m_eventCounter);

      for (int i=0;i<4;i++){
	m_partHistos[i].h_NoisyFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_NoisyFEBFracPerEvt->SetEntries(m_eventCounter);
	m_partHistos[i].h_MNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_MNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
	m_partHistos[i].h_MNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_MNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
	m_partHistos[i].h_CandidateMNBTightFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_CandidateMNBTightFEBFracPerEvt->SetEntries(m_eventCounter);
	m_partHistos[i].h_CandidateMNBLooseFEBFracPerEvt->scaleContentsAndErrors(scale);
	m_partHistos[i].h_CandidateMNBLooseFEBFracPerEvt->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_NoisyEventFrac,m_partHistos[i].h_NoisyEvent,m_h_LBN);
	m_partHistos[i].h_NoisyEventFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_SaturatedNoisyEventFrac,m_partHistos[i].h_SaturatedNoisyEvent,m_h_LBN);
	m_partHistos[i].h_SaturatedNoisyEventFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_MNBTightEventFrac,m_partHistos[i].h_MNBTightEvent,m_h_LBN);
	m_partHistos[i].h_MNBTightEventFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_MNBLooseEventFrac,m_partHistos[i].h_MNBLooseEvent,m_h_LBN);
	m_partHistos[i].h_MNBLooseEventFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_NoisyEventTimeVetoFrac,m_partHistos[i].h_NoisyEventTimeVeto,m_h_LBN);
	m_partHistos[i].h_NoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_SaturatedNoisyEventTimeVetoFrac,m_partHistos[i].h_SaturatedNoisyEventTimeVeto,m_h_LBN);
	m_partHistos[i].h_SaturatedNoisyEventTimeVetoFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_MNBTightEventTimeVetoFrac,m_partHistos[i].h_MNBTightEventTimeVeto,m_h_LBN);
	m_partHistos[i].h_MNBTightEventTimeVetoFrac->SetEntries(m_eventCounter);
	divideHisto(m_partHistos[i].h_MNBLooseEventTimeVetoFrac,m_partHistos[i].h_MNBLooseEventTimeVeto,m_h_LBN);
	m_partHistos[i].h_MNBLooseEventTimeVetoFrac->SetEntries(m_eventCounter);
      }    
    }//end if m_eventCounter>0
  }
  
  if ( endOfRunFlag() ) {
    if ( m_h_LBN ) m_h_LBN->Reset(); 
    
    for (int i=0;i<4;i++){
	
      if (m_partHistos[i].h_NoisyEvent) m_partHistos[i].h_NoisyEvent->Reset();
      if (m_partHistos[i].h_NoisyEventTimeVeto) m_partHistos[i].h_NoisyEventTimeVeto->Reset();
      if (m_partHistos[i].h_SaturatedNoisyEvent) m_partHistos[i].h_SaturatedNoisyEvent->Reset();
      if (m_partHistos[i].h_SaturatedNoisyEventTimeVeto) m_partHistos[i].h_SaturatedNoisyEventTimeVeto->Reset();
      if (m_partHistos[i].h_MNBTightEvent) m_partHistos[i].h_MNBTightEvent->Reset();
      if (m_partHistos[i].h_MNBTightEventTimeVeto) m_partHistos[i].h_MNBTightEventTimeVeto->Reset();
      if (m_partHistos[i].h_MNBLooseEvent) m_partHistos[i].h_MNBLooseEvent->Reset();
      if (m_partHistos[i].h_MNBLooseEventTimeVeto) m_partHistos[i].h_MNBLooseEventTimeVeto->Reset();
    }
  }  

  return StatusCode::SUCCESS;
}

void LArNoisyROMon::copyHisto(LWHist2D* from,LWHist2D* to)
{
  unsigned xbin, ybin;
  double numer, error;
  from->resetActiveBinLoop();
  
  while(from->getNextActiveBin(xbin,ybin,numer,error)) 
  {    
    to->SetBinContentAndError(xbin,ybin,numer,error);       
  }
}


void LArNoisyROMon::copyHisto(LWHist1D* from,LWHist1D* to)
{
  unsigned xbin;
  double numer, error;
  from->resetActiveBinLoop();
  
  while(from->getNextActiveBin(xbin,numer,error)) 
  {    
    to->SetBinContentAndError(xbin,numer,error);       
  }
}


void LArNoisyROMon::divideHisto(LWHist1D* to,LWHist1D* num,LWHist1D* denom)
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
  hName  = "MNBKnownFEB_"+name;
  hTitle = "Suspicious MNB FEBs - "+name;
  partition.h_MNBKnownFEB = TH2I_LW::create(hName.c_str(), hTitle.c_str(), slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  partition.h_MNBKnownFEB->GetXaxis()->SetTitle("Slot");
  partition.h_MNBKnownFEB->GetYaxis()->SetTitle("Feedthrough");
  groupfrac.regHist(partition.h_MNBKnownFEB).ignore();
  

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
  hTitle = "Yield of events with FEB MNB-Tight " + m_MNBTightFEBDefStr+ " - "+name+" (only vetoed events)";
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
  hTitle = "Yield of events with FEB MNB-Loose " + m_MNBLooseFEBDefStr + " - "+name+" (only vetoed events)";
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
  hTitle = "Yield of events with FEB MNB-Tight "+ m_MNBTightFEBDefStr+ " - "+name;
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
  hTitle = "Yield of events with FEB MNB-Loose " + m_MNBLooseFEBDefStr + " - "+name;
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

  // Fraction of events found noisy per LB - Regular Noise Burst(RNB) Standard flag
  hName = "NoisyEvent_"+name;
  hTitle = "Yield of events flagged as RNB-Standard - " + name;
  partition.h_NoisyEventFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_NoisyEvent_"+name;
  partition.h_NoisyEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEvent->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB - RNB-Saturated flag
  hName = "SaturatedNoisyEvent_"+name;
  hTitle = "Yield of events flagged as RNB-Saturated - "+name;
  partition.h_SaturatedNoisyEventFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_SaturatedNoisyEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_NoisyEvent_"+name;
  partition.h_SaturatedNoisyEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEvent->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBTightEvent_"+name;
  hTitle = "Yield of events flagged as MNB-Tight - "+name;
  partition.h_MNBTightEventFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBTightEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_MNBTightEvent_"+name;
  partition.h_MNBTightEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEvent->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBLooseEvent_"+name;
  hTitle = "Yield of events flagged as MNB-Loose - "+name;
  partition.h_MNBLooseEventFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEventFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBLooseEventFrac).ignore();  
  // Histogram below is temporary. Normalized at the end of run to produce the above histograms
  hName = "temp_MNBLooseEvent_"+name;
  partition.h_MNBLooseEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEvent->GetXaxis()->SetTitle("Luminosity Block");

//  // Fraction of events found noisy per LB - Weighted flag
//  hName = "NoisyWEvent_"+name;
//  partition.h_NoisyWEventFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as LArNoisyRO_StdOpt", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
//  partition.h_NoisyWEventFrac->GetXaxis()->SetTitle("Luminosity Block");
//  groupfracbin.regHist(partition.h_NoisyWEventFrac).ignore();  
//  hName = "temp_NoisyWEvent_"+name;
//  partition.h_NoisyWEvent = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
//  partition.h_NoisyWEvent->GetXaxis()->SetTitle("Luminosity Block");
  
  // Fraction of events found noisy per LB after time veto - Std flag
  hName = "NoisyEvent_TimeVeto_"+name;
  hTitle = "Yield of events flagged as RNB-Standard not vetoed by time window - "+name;
  partition.h_NoisyEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_NoisyEventTimeVetoFrac).ignore();

  hName = "temp_NoisyEvent_TimeVeto_"+name;
  partition.h_NoisyEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_NoisyEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  // Fraction of events found noisy per LB after time veto - Sat flag
  hName = "SaturatedNoisyEvent_TimeVeto_"+name;
  hTitle = "Yield of events flagged as RNB-Saturated not vetoed by time window - "+ name;
  partition.h_SaturatedNoisyEventTimeVetoFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_SaturatedNoisyEventTimeVetoFrac).ignore();

  hName = "temp_SaturatedNoisyEvent_TimeVeto_"+name;
  partition.h_SaturatedNoisyEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_SaturatedNoisyEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBTightEvent_TimeVeto_"+name;
  hTitle =  "Yield of events flagged as MNB-Tight not vetoed by time window - " + name;
  partition.h_MNBTightEventTimeVetoFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBTightEventTimeVetoFrac).ignore();
    
  hName = "temp_MNBTightEvent_TimeVeto_"+name;
  partition.h_MNBTightEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBTightEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

  hName = "MNBLooseEvent_TimeVeto_"+name;
  hTitle = "Yield of events flagged as MNB-Loose not vetoed by time window - " + name;
  partition.h_MNBLooseEventTimeVetoFrac = TH1F_LW::create(hName.c_str(),hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
  groupfracbin.regHist(partition.h_MNBLooseEventTimeVetoFrac).ignore();
    
  hName = "temp_MNBLooseEvent_TimeVeto_"+name;
  partition.h_MNBLooseEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
  partition.h_MNBLooseEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");

//  // Fraction of events found noisy per LB after time veto - Weighted flag
//  hName = "NoisyWEvent_TimeVeto_"+name;
//  partition.h_NoisyWEventTimeVetoFrac = TH1F_LW::create(hName.c_str(), "Yield of events flagged as RNB-StandardOpt not vetoed by time window", m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
//  partition.h_NoisyWEventTimeVetoFrac->GetXaxis()->SetTitle("Luminosity Block");
//  groupfracbin.regHist(partition.h_NoisyWEventTimeVetoFrac).ignore();
//  
//  hName = "temp_NoisyWEvent_TimeVeto_"+name;
//  partition.h_NoisyWEventTimeVeto = TH1I_LW::create(hName.c_str(), hTitle.c_str(), m_lumi_blocks+1, -0.5, (float)m_lumi_blocks+0.5);
//  partition.h_NoisyWEventTimeVeto->GetXaxis()->SetTitle("Luminosity Block");
  
  if ( m_doTrigger ) 
  {
    unsigned int siz = m_EF_NoiseBurst_Triggers.size();
    hName = "NoisyEventTrigger_"+name;
    hTitle = "Trigger fired for RNB flagged events - "+name;
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
    hTitle = "L1 term fired for RNB flagged events - "+name;
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
  ATH_MSG_INFO(  " in LArNoisyROMon::finalize() " );
     // delete temposary histograms
/*
  if ( m_h_LBN ) {
    LWHist::safeDelete(m_h_LBN); 
    m_h_LBN = nullptr;
  }
    
  //  for (partitionHistos* part : {&m_BarrelA, &m_BarrelC, &m_EMECA, &m_EMECC}) {
  for (int i=0;i<4;i++){
     if(m_partHistos[i].h_NoisyEvent) LWHist::safeDelete(m_partHistos[i].h_NoisyEvent); 
     //m_partHistos[i].h_NoisyEvent = nullptr;
     
//     LWHist::safeDelete(part->h_NoisyWEvent); 
//     part->h_NoisyWEvent = nullptr;

     if(m_partHistos[i].h_NoisyEventTimeVeto) LWHist::safeDelete(m_partHistos[i].h_NoisyEventTimeVeto); 
     //m_partHistos[i].h_NoisyEventTimeVeto = nullptr;

     if(m_partHistos[i].h_MNBTightEvent) LWHist::safeDelete(m_partHistos[i].h_MNBTightEvent); 
     //m_partHistos[i].h_MNBTightEvent = nullptr;

     if(m_partHistos[i].h_MNBTightEventTimeVeto) LWHist::safeDelete(m_partHistos[i].h_MNBTightEventTimeVeto); 
     //m_partHistos[i].h_MNBTightEventTimeVeto = nullptr;

     if(m_partHistos[i].h_MNBLooseEvent) LWHist::safeDelete(m_partHistos[i].h_MNBLooseEvent); 
     //m_partHistos[i].h_MNBLooseEvent = nullptr;

     if(m_partHistos[i].h_MNBLooseEventTimeVeto) LWHist::safeDelete(m_partHistos[i].h_MNBLooseEventTimeVeto); 
     //m_partHistos[i].h_MNBLooseEventTimeVeto = nullptr;

     if(m_partHistos[i].h_SaturatedNoisyEvent) LWHist::safeDelete(m_partHistos[i].h_SaturatedNoisyEvent); 
     //m_partHistos[i].h_SaturatedNoisyEvent = nullptr;

     if(m_partHistos[i].h_SaturatedNoisyEventTimeVeto) LWHist::safeDelete(m_partHistos[i].h_SaturatedNoisyEventTimeVeto); 
     //m_partHistos[i].h_SaturatedNoisyEventTimeVeto = nullptr;

//     LWHist::safeDelete(part->h_NoisyWEventTimeVeto);
//     part->h_NoisyWEventTimeVeto=nullptr;
  }

*/

  return StatusCode::SUCCESS;
}

