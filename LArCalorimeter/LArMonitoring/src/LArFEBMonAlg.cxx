/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     LArFEBMonAlg.cxx
//
// AUTHOR:   P. Strizenec, based on LArFEBMon tool by B. Trocme
// ********************************************************************

#include "LArFEBMonAlg.h"

#include "LArRecEvent/LArEventBitInfo.h"

#include "LArRawConditions/LArDSPThresholdsComplete.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "LArCOOLConditions/LArDSPThresholdsFlat.h"

#include "LArTrigStreamMatching.h"

#include <algorithm>
#include <math.h>
#include <sys/types.h>

//const unsigned nFEBnominal=1524;

// ********************************************************************
LArFEBMonAlg::LArFEBMonAlg( const std::string& name, ISvcLocator* pSvcLocator) 
  : AthMonitorAlgorithm(name, pSvcLocator),
    //m_CorruptTree(nullptr),
    // m_eventTime(0),
    // m_eventTime_ns(0),
    m_onlineHelper(nullptr),
    m_dspThrDone(false),
    m_maskedDone(false),
    m_nbOfFebBlocksTotal(-1)
{
  /** Give the name of the streams you want to monitor, if empty, only simple profile per partition (offline case):*/
  //declareProperty("Streams",m_streams);
 
  /*
  for (unsigned i = 0;i < nFEBnominal; i++) {
    m_bfebIE[i]     = false;
  }
  FIXME*/

  //m_CorruptTree		= nullptr;

}


// ********************************************************************
StatusCode LArFEBMonAlg::initialize() {
  
  ATH_MSG_INFO( "Initializing LArFEBMonAlg " );
  
  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
    return StatusCode::FAILURE;
  }
  
  sc=m_BFKey.initialize();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not initialize Missing FEBs key " << m_BFKey.key() );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Missing FEBs key" << m_BFKey.key() << " initialized" );
  }

  ATH_CHECK( m_hdrContKey.initialize() );
  ATH_CHECK( m_lArFebErrorSummaryKey.initialize() );

  m_histoGroups.reserve(m_SubDetNames.size());
  for (unsigned i=0; i<m_SubDetNames.size(); ++i) {
    std::vector<std::string> part;
    part.push_back(m_partitions[2*i]);
    part.push_back(m_partitions[2*i+1]);
    m_histoGroups.push_back(Monitored::buildToolMap<int>(m_tools,m_SubDetNames[i],part));
  }

  ATH_CHECK( m_run1DSPThresholdsKey.initialize (SG::AllowEmpty) );
  ATH_CHECK( m_run2DSPThresholdsKey.initialize (SG::AllowEmpty) );

  return AthMonitorAlgorithm::initialize();
}

// ********************************************************************
StatusCode LArFEBMonAlg::fillHistograms(const EventContext& ctx) const {
  
  bool eventRejected = false;
  std::bitset<13> rejectionBits;
  // for TTree
  std::vector<int> febInErrorTree(0);
  std::vector<int> febErrorTypeTree(0);
  
  // Retrieve event info to get event time,trigger type...
  // Retrieved at beg of method now to get the LVL1 type
  // to check consistency with DSP trigger type

  SG::ReadHandle<xAOD::EventInfo> thisEvent = GetEventInfo(ctx);

  unsigned int l1Trig = thisEvent->level1TriggerType();
  auto l1 = Monitored::Scalar<int>("LVL1Trig",l1Trig);
  fill(m_monGroupName,l1); 

  auto eventTime = Monitored::Scalar<int>("timestamp",thisEvent->timeStamp());
  auto eventTime_ns = Monitored::Scalar<int>("time_ns",thisEvent->timeStampNSOffset());

  unsigned lumi_block = thisEvent->lumiBlock();
  bool lar_inerror = (thisEvent->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error) ? true : false;
  
  ATH_MSG_DEBUG( "LArFEBMonAlg Lumi block: "<<lumi_block);

  SG::ReadHandle<LArFebHeaderContainer> hdrCont(m_hdrContKey, ctx);
  SG::ReadHandle<LArFebErrorSummary> lArFebErrorSummary(m_lArFebErrorSummaryKey, ctx);
  if (!hdrCont.isValid()) {
    ATH_MSG_ERROR( "No LArFebHeaderContainer found in TDS" ); 
    return StatusCode::FAILURE;
  }
  
  if (hdrCont->size()==0) {
    ATH_MSG_ERROR( "Got empty LArFebHeaderContainer. Do nothing" );
    return StatusCode::FAILURE;
  }

  if (!lArFebErrorSummary.isValid()) {
    ATH_MSG_ERROR( "No LArFebErrorSummary found in TDS" );
    return StatusCode::FAILURE;
  }
  
  bool trigok=true;
  if (environment() == Environment_t::online) {
    const ToolHandle<Trig::TrigDecisionTool> trigTool=getTrigDecisionTool();
    //const Trig::TrigDecisionTool *trigTool = dynamic_cast<const Trig::TrigDecisionTool * > (&*trigHdl);
    std::vector<std::string> l1triggers;
    if(trigTool) {   
       const Trig::ChainGroup* allL1 = trigTool->getChainGroup("L1_.*");
       l1triggers = allL1->getListOfTriggers();
       ATH_MSG_DEBUG( "lvl1 item names: [" );
       for (unsigned int i=0;i< l1triggers.size();i++) {
            ATH_MSG_DEBUG( i << " " << l1triggers.at(i) << " , " );
       }
       ATH_MSG_DEBUG( "] " );
    }
    if (l1triggers.size()>0) {trigok=true;} else {trigok=false;}
  }
  
  uint32_t firstEventType = (*hdrCont->begin())->DetEventType(); 

  { // adding scope for locking
    std::lock_guard<std::mutex> lock(m_mut);
    if(!m_maskedDone) { // should be done once at the beginning
      plotMaskedFEB();
      m_maskedDone=true;
    }
  }

  // At 1st event, retrieve DSP thresholds and fill histogram with values for all channels
  { // adding scope for locking
   std::lock_guard<std::mutex> lock(m_mut);
   if (!m_dspThrDone && firstEventType == 4) {
    
    auto dspADC = Monitored::Scalar<unsigned int>("dspThrADC",-1);
    auto dspQT = Monitored::Scalar<unsigned int>("dspThrQT",-1);
    if (!m_run1DSPThresholdsKey.empty()) {
      ATH_MSG_DEBUG("Loading run1 version of LAr DSP Thresholds");
      SG::ReadCondHandle<LArDSPThresholdsComplete> dspThresh (m_run1DSPThresholdsKey, ctx);
      for (HWIdentifier ch : m_onlineHelper->channel_range()) {
        dspADC=dspThresh->samplesThr(ch);
        dspQT=dspThresh->tQThr(ch);
        fill(m_monGroupName, dspADC, dspQT);
      }//end loop over channels
    }
    else if (!m_run2DSPThresholdsKey.empty()) {
      ATH_MSG_DEBUG("Loading run2 version of LAr DSP Thresholds");
      SG::ReadCondHandle<AthenaAttributeList> dspThrshAttr (m_run2DSPThresholdsKey, ctx);
      LArDSPThresholdsFlat dspThreshFlat(*dspThrshAttr);
      if (!dspThreshFlat.good()) {
        ATH_MSG_WARNING( "Failed to initialize LArDSPThresholdFlat from attribute list loaded from " << m_run2DSPThresholdsKey.key()
                         << ". Will not fill histograms." ); 
      }//end if not good
      const IdentifierHash chanMax=m_onlineHelper->channelHashMax();
      for (unsigned iChan=0;iChan<chanMax;++iChan) {
        dspADC=dspThreshFlat.samplesThrByHash(iChan);
        dspQT=dspThreshFlat.tQThrByHash(iChan);
        fill(m_monGroupName, dspADC, dspQT);
      }
    }
    else 
      ATH_MSG_WARNING( "No LArDSPThresholds key specified. Will not fill these histograms" );
    m_dspThrDone=true;
   }//m_dspThrDone
  } // locking scope


  //Adjust event type if we have raw data in the ROD block
  if (firstEventType == 4 && (*hdrCont->begin())->RodRawDataSize() != 0) firstEventType = 14;

  std::vector<unsigned int> nfeb(m_partitions.size());
  int firstEventNbSp=-1;
  unsigned int totNbOfSweet2 = 0;
  float larEventSize = 0;
  float larEventSize_part[8] = {0.,0.,0.,0.,0.,0.,0.,0.};

  LArFebHeaderContainer::const_iterator it = hdrCont->begin(); 
  LArFebHeaderContainer::const_iterator it_e = hdrCont->end(); 
  
  auto l1word = Monitored::Scalar<int>("LVL1TrigAllDSP",0);
  // Loop on all available FEBHeader to fill basic parameters histogram : event type, event size, nb of sweet cells...
  auto slmon = Monitored::Scalar<int>("slotnb",-1);
  auto ftmon = Monitored::Scalar<int>("FTnb",-1);
  auto sw1 = Monitored::Scalar<int>("weightsweet1",-1);
  auto sw2 = Monitored::Scalar<int>("weightsweet2",-1);
  auto slmis = Monitored::Scalar<int>("slotmist",-1);
  auto ftmis = Monitored::Scalar<int>("FTmist",-1);
  for ( ; it!=it_e;++it) {
    HWIdentifier febid=(*it)->FEBId();
    
    if (febid.get_identifier32().get_compact() >= 0x38000000 && febid.get_identifier32().get_compact() <= 0x3bc60000 && !(febid.get_identifier32().get_compact() & 0xFFF)) {
      int barrel_ec = m_onlineHelper->barrel_ec(febid);
      int pos_neg   = m_onlineHelper->pos_neg(febid);
      int ft        = m_onlineHelper->feedthrough(febid);
      int slot      = m_onlineHelper->slot(febid);      
      unsigned int partitionNb_dE = returnPartition(barrel_ec,pos_neg,ft,slot);
      unsigned int subdet = partitionNb_dE / 2;

      if (partitionNb_dE < m_partitions.size()) {
        nfeb[partitionNb_dE]++ ;
      }else{
         ATH_MSG_WARNING("Unknown partition number: "<< partitionNb_dE << " not filling !");
         continue;
      }
      larEventSize += (float) ((*it)->RodRawDataSize() + (*it)->RodResults1Size() + (*it)->RodResults2Size()); // This quantity is now in megabytes
      if(partitionNb_dE<8) larEventSize_part[partitionNb_dE] += (float) ((*it)->RodRawDataSize() + (*it)->RodResults1Size() + (*it)->RodResults2Size());
      
      // Eventype = 2 : transparent/raw data - 4 : Physic - 7 : calibration - 10 : pedestal - 14 : raw data + results
      uint32_t eventType = (*it)->DetEventType();      
      // If physic mode && raw data != 0, we are in rawdata+results
      if (eventType == 4 && (*it)->RodRawDataSize() != 0) eventType = 14;
      //if (firstEventType == 999) firstEventType = eventType;
      if (firstEventNbSp < 0)
      	if ((*it)->NbSamples() != 0) 
	  firstEventNbSp=(*it)->NbSamples();

      
      l1word = (*it)->LVL1TigType();
      fill(m_monGroupName,l1word);

      if (firstEventType == 4) totNbOfSweet2 = totNbOfSweet2+(*it)->NbSweetCells2();
      // Fill (nb of evnts and sweet cells per FEB) histos
      slmon = slot;
      ftmon = ft;
      sw1 = (*it)->NbSweetCells1();
      sw2 = (*it)->NbSweetCells2();
      fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitionNb_dE])],slmon,ftmon,sw1,sw2);
      if ((*it)->LVL1TigType() == 0 || (*it)->LVL1TigType() == 170 || (*it)->LVL1TigType() != l1Trig){
         fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitionNb_dE])],slmis,ftmis);  
      }
    }//Test on FEBid
  }//end of loop over FEB headers
  
  // Loop over all febs to plot the error from statusword
  // This is mandatory to also monitor the FEBs with missing headers
   
  bool anyfebIE = false; 
  for (std::vector<HWIdentifier>::const_iterator allFeb = m_onlineHelper->feb_begin(); 
       allFeb != m_onlineHelper->feb_end(); ++allFeb) {
    HWIdentifier febid = HWIdentifier(*allFeb);
    bool currentFebStatus = false;
    uint16_t feberrorSummary = lArFebErrorSummary->feb_error(febid);
    
    if ( feberrorSummary != 0 ){
      int barrel_ec = m_onlineHelper->barrel_ec(febid);
      int pos_neg   = m_onlineHelper->pos_neg(febid);
      int ft        = m_onlineHelper->feedthrough(febid);
      int slot      = m_onlineHelper->slot(febid);      
      unsigned int partitionNb_dE = returnPartition(barrel_ec,pos_neg,ft,slot);
      
      if (partitionNb_dE < m_partitions.size()) {
         // Fill the errors in partition histograms
         fillErrorsSummary(partitionNb_dE,ft,slot,feberrorSummary,lar_inerror, rejectionBits, currentFebStatus, eventRejected);
      }else{
         ATH_MSG_WARNING("Unknown partition number: "<< partitionNb_dE << " not filling !");
      }

      if (currentFebStatus && febInErrorTree.size()<33){
	febInErrorTree.push_back(febid.get_identifier32().get_compact());
	febErrorTypeTree.push_back(rejectionBits.to_ulong()); 
      }
    }  
    if(currentFebStatus) anyfebIE = currentFebStatus;
  }
  
  //Fill general data histos
  auto evttype = Monitored::Scalar<int>("EvtType",firstEventType);

  float nbOfFeb = 0.;
  for(auto nf: nfeb) nbOfFeb+=nf;// (nfeb[0]+nfeb[1]+nfeb[2]+nfeb[3]+nfeb[4]+nfeb[5]+nfeb[6]+nfeb[7]);
  if (firstEventType == 2 || firstEventType == 4 || firstEventType == 14) {
     auto nbsamp = Monitored::Scalar<int>("NbOfSamp",firstEventNbSp);
     fill(m_monGroupName, nbsamp);
  }
  
  bool newHighWaterMarkNFebBlocksTotal = false;
  if(m_nbOfFebBlocksTotal < nbOfFeb){ // new number of Febs
    m_nbOfFebBlocksTotal = nbOfFeb;
    newHighWaterMarkNFebBlocksTotal = true;
  }
  auto nbfeb = Monitored::Scalar<int>("nbFEB",nbOfFeb);
  fill(m_monGroupName, evttype, nbfeb); 

  auto part = Monitored::Scalar<int>("part",-1);
  auto nbfebpart = Monitored::Scalar<int>("nbFEBpart",-1);
  for(unsigned i=0; i<m_partitions.size(); ++i) {
     part=i;
     unsigned subdet = i / 2;
     nbfebpart=nfeb[i];
     fill(m_monGroupName,part,nbfebpart);
     fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[i])],nbfebpart);
  }
  
  // If the nb of DSP headers is lower than the maximum, this means that there are some missing FEBs, probably
  // due to missing ROS fragment
  // This assumes that the maximum of DSP headers found is the expected one
  //  ATH_MSG_ERROR( "ICI" << nbOfFeb << " " << m_nbOfFebBlocksTotal->GetBinLowEdge(m_nbOfFebBlocksTotal->GetMaximumBin()) );
  
  auto evtrej = Monitored::Scalar<int>("EvtRej",-1);
  float evt_yield=-1;
  auto evtyield = Monitored::Scalar<float>("EvtRejYield",-1);
  auto evtyield1D = Monitored::Scalar<float>("EvtRejYield1D",-1);
  auto evtoneyield = Monitored::Scalar<float>("EvtOneYield",-1);
  auto evtyieldout = Monitored::Scalar<float>("EvtRejYieldOut",-1);
  if (febInErrorTree.size()>=1 || newHighWaterMarkNFebBlocksTotal || nbOfFeb < m_nbOfFebBlocksTotal ){
    evtrej=1; evt_yield = 100.; evtoneyield =100.;
    if (febInErrorTree.size()>=4) evtrej=2;
  } else{
     evtoneyield=0.;
  }

  if (thisEvent->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error || nbOfFeb != m_nbOfFebBlocksTotal || nbOfFeb < m_nFEBnominal){ // Event in error (whatever is the cause)
    if (thisEvent->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::DATACORRUPTED) || nbOfFeb != m_nbOfFebBlocksTotal){ // Event corrupted (>=1/4 FEBs in error)
      evtrej=3; evt_yield = 100.;
      if (thisEvent->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::DATACORRUPTEDVETO)) evtyieldout=0.; // Vetoed 
      else evtyieldout=100.; // not vetoed
      auto rbits = Monitored::Scalar<unsigned long>("rejBits", rejectionBits.to_ulong());
      fill(m_monGroupName, rbits);
    }
    else{ // Event in error but not corrupted
       evt_yield = 50.; evtyieldout=100.;
    }
    if (thisEvent->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::DATACORRUPTEDVETO)) evtrej=4;
    if (thisEvent->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::NOISEBURSTVETO)) evtrej=5;
    evtyield1D=evt_yield;
  } else{ // The event is NOT in error. Fill per LB TProfile
    evtrej=6; evt_yield = 0.; evtyield1D=100; evtyieldout=0.;
  }
  evtyield=evt_yield;
  auto evSize = Monitored::Scalar<float>("LArEvSize",larEventSize/262144);
  auto sweet2 = Monitored::Scalar<int>("NbOfSweet2",totNbOfSweet2);
  auto lb0 = Monitored::Scalar<int>("LB0",lumi_block); //to avoid 'NbOfEventsVSLB' being filled multiple times
  fill(m_monGroupName,evtrej,evtyieldout,evtyield,evtyield1D,evtoneyield,evSize, sweet2, lb0);
  evtrej=7;
  fill(m_monGroupName,evtrej);
  if(environment() == Environment_t::online) {
     auto lbfake = Monitored::Scalar<int>("LBf",0);
     fill(m_monGroupName,evtyield,lbfake);
  }

  if(anyfebIE) { 
     //Fill LArCorrupted tree
     auto mon_febInErrorTree = Monitored::Collection("febHwId", febInErrorTree);
     auto mon_febErrorTypeTree = Monitored::Collection("febErrorType", febErrorTypeTree);
     fill(m_monGroupName,mon_febInErrorTree,mon_febErrorTypeTree,eventTime,eventTime_ns);
  }
  
  // Now we could fill the event size
  auto lb = Monitored::Scalar<int>("LB",lumi_block);
  auto evsize=Monitored::Scalar<float>("LArEvSizePart",-1);
  if(environment() == Environment_t::online && m_streams.size() > 0) {
    std::vector<unsigned int> streamsThisEvent=LArMon::trigStreamMatching(m_streams,thisEvent->streamTags());
    if(streamsThisEvent.size()==1 && streamsThisEvent[0] == m_streams.size()) streamsThisEvent[0]=m_streams.size()-1; // assuming others are last in the list of streams
 
    auto streambin=Monitored::Scalar<float>("streamBin",-1);
      
    unsigned ie;
    for(unsigned int str=0; str<streamsThisEvent.size(); ++str) {
      if(streamsThisEvent[str] > m_streams.size()) break;
      if(trigok && streamsThisEvent[str] < m_streams.size() && (m_streams[streamsThisEvent[str]].find("CosmicCalo") != std::string::npos)) { // test excluding events
	for(ie=0; ie<m_excoscalo.size(); ++ie) {
	  if(getTrigDecisionTool()->isPassed(m_excoscalo[ie])) break;
	}
	if(ie<m_excoscalo.size()) {
	  ATH_MSG_INFO("Skipping "<<m_excoscalo[ie]<<" for CosmicCalo ");
	  continue; // we should skip this trigger
	}
      }
      streambin =  streamsThisEvent[str];
      evsize = larEventSize;
      fill(m_monGroupName,lb,streambin,evsize);
      
      for(unsigned i=0; i <m_partitions.size(); ++i){
        unsigned subdet = i / 2;
        evsize =  larEventSize_part[i]/262144;
        fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[i])],lb,streambin,evsize);
      }

    }
  } else { // we are filling only simple profiles
    for(unsigned i=0; i<m_partitions.size(); ++i) { 
       unsigned subdet = i / 2;
       evsize=larEventSize_part[i]/262144;
       fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[i])],lb,evsize);
    }
  }

   
  return StatusCode::SUCCESS;
}


// ********************************************************************
void LArFEBMonAlg::fillErrorsSummary(unsigned int partitNb_2,int ft,int slot,uint16_t error, bool lar_inerror, std::bitset<13> &rejectionBits, bool &currentFebStatus, bool &eventRejected) const
{  
  if (m_badFebs.count(std::make_pair(slot,ft)) != 0) return;
  
  auto part = Monitored::Scalar<int>("part",partitNb_2);
  auto ferror = Monitored::Scalar<int>("febError",-1);
  if ( error & (1<<0) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotPar",slot);
    auto ftmon = Monitored::Scalar<int>("FTPar",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(0);
    ferror=1;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<1) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotBcid",slot);
    auto ftmon = Monitored::Scalar<int>("FTBcid",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(1);
    ferror=2;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<2) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotRadd",slot);
    auto ftmon = Monitored::Scalar<int>("FTRadd",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(2);
    ferror=3;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<3) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotEvtid",slot);
    auto ftmon = Monitored::Scalar<int>("FTEvtid",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(3);
    ferror=4;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<4) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotScac",slot);
    auto ftmon = Monitored::Scalar<int>("FTScac",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(4);
    ferror=5;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<5) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotscout",slot);
    auto ftmon = Monitored::Scalar<int>("FTscout",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(5);
    ferror=6;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<6) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotgain",slot);
    auto ftmon = Monitored::Scalar<int>("FTgain",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(6);
    ferror=7;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<7) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slottype",slot);
    auto ftmon = Monitored::Scalar<int>("FTtype",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(7);
    ferror=8;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<8) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotsmp",slot);
    auto ftmon = Monitored::Scalar<int>("FTsmp",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(8);
    ferror=9;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<9) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotzero",slot);
    auto ftmon = Monitored::Scalar<int>("FTzero",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(9);
    ferror=10;
    fill(m_monGroupName, ferror, part);
  }
  
  
  if ( error & (1<<11) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotsum",slot);
    auto ftmon = Monitored::Scalar<int>("FTsum",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(10);
    ferror=11;
    fill(m_monGroupName, ferror, part);
  }
  
  if ( error & (1<<12) ){
    // Check whether this error can be ignored. Useful for calibration run of PS or EMB
    if (!((m_ignoreMissingHeaderEMB && partitNb_2<2 && slot>=2) || (m_ignoreMissingHeaderPS && partitNb_2<2 && slot==1))){
      unsigned subdet = partitNb_2 / 2;
      auto sl = Monitored::Scalar<int>("slotmis",slot);
      auto ftmon = Monitored::Scalar<int>("FTmis",ft);
      fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
      currentFebStatus = true;
      rejectionBits.set(11);
      ferror=12;
      fill(m_monGroupName, ferror, part);
    }
  }
  
  if ( error & (1<<13) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotgain",slot);
    auto ftmon = Monitored::Scalar<int>("FTgain",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(12);
    ferror=13;
    fill(m_monGroupName, ferror, part);
  }
  

  if (currentFebStatus){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotabs",slot);
    auto ftmon = Monitored::Scalar<int>("FTabs",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    float ferr=0.;
    if (lar_inerror) {// LArinError
       eventRejected = true;
       if(environment() == Environment_t::online) ferr=100.;
    } else {
       if(environment() == Environment_t::online) ferr=50.;
    }
    if(environment() == Environment_t::online) {
       auto lbf = Monitored::Scalar<float>("LBf",0.5);
       auto erry = Monitored::Scalar<float>("erronl",ferr);
       fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],lbf,erry);
    }
  }
  
  return;
}

/*---------------------------------------------------------*/
void 
LArFEBMonAlg::plotMaskedFEB() const{
  
  SG::ReadCondHandle<LArBadFebCont> badFebHdl{m_BFKey};
  const LArBadFebCont* badFebCont=(*badFebHdl);
  if(!badFebCont) {
     ATH_MSG_WARNING( "Do not have Misisng FEB container, no plots !!!" );
     return;
  }
  m_badFebs.clear();
  // Loop over all FEBs
  for (std::vector<HWIdentifier>::const_iterator allFeb = m_onlineHelper->feb_begin(); 
       allFeb != m_onlineHelper->feb_end(); ++allFeb) {
    HWIdentifier febid = HWIdentifier(*allFeb);
    const LArBadFeb febStatus = badFebCont->status(febid);
    unsigned int binContent = 0;
    if (febStatus.inError() || febStatus.deadReadout() || febStatus.deadAll() || febStatus.deactivatedInOKS()) binContent = 2;
    if (febStatus.ignoreErrors() > 0)  binContent = 1;
    if (binContent != 0){
      int barrel_ec = m_onlineHelper->barrel_ec(febid);
      int pos_neg   = m_onlineHelper->pos_neg(febid);
      int ft        = m_onlineHelper->feedthrough(febid);
      int slot      = m_onlineHelper->slot(febid);  
      unsigned int partitionNb_dE = returnPartition(barrel_ec,pos_neg,ft,slot);
      int subdet = partitionNb_dE / 2;
      
      m_badFebs[std::make_pair(slot,ft)] = binContent;
      if (partitionNb_dE < m_partitions.size()) {
         auto sl = Monitored::Scalar<int>("slotMasked",slot);
         auto ftmon = Monitored::Scalar<int>("FTMasked",ft);
         ATH_MSG_INFO("filling for " << partitionNb_dE << " partition, slot: " << slot << "FT: " << ft);
         fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitionNb_dE])],sl,ftmon);
      }else{
         ATH_MSG_WARNING("Unknown partition number: "<< partitionNb_dE << " not filling !");
      }
    }
  }
}


/*---------------------------------------------------------*/
/*
 DEV this needs to be migrated, but so far we do not have posibility to dynamically create the histograms....
void 
LArFEBMonAlg::fillFebInError(const summaryPartition& summ,int errorType,int barrel_ec,int pos_neg,std::string summName)
{
  ATH_MSG_DEBUG( "In fillFebInError" );
  
  //  TH2I* tempHisto = TH2I_LW::create(*summ.parity);
  
  std::string hName = "/LAr/FEBMon/perPartition/FebInErrors/" + summName;
  
  MonGroup generalGroup( this, hName.c_str(), run, ATTRIB_MANAGED);
  
  // Loop on TH2D to find which FEB is in error
  // parity is used to extract binning range. Ok as this is uniform along different errors
  int nbOfFEBErrors = 0;
  
  for (unsigned int ix=1; ix <= (summ.parity)->GetNbinsX();ix++){
    for (unsigned int iy=1; iy <= (summ.parity)->GetNbinsY();iy++){
      // Found a faulty FEB
      // If more than 15 FEBs in error in a partition, ignore other FEBs (mandatory to avoid 
      // creation of 1500  histos when a run is bad!).
      double binContent =0;
      switch (errorType){
	case 1:
	  binContent = (summ.parity)->GetBinContent(ix,iy);
	  break;
	case 2:
	  binContent = (summ.BCID_2halves)->GetBinContent(ix,iy);
	  break;
	case 3:
	  binContent = (summ.RADD_2halves)->GetBinContent(ix,iy);
	  break;
	case 4:
	  binContent = (summ.EVTID_2halves)->GetBinContent(ix,iy);
	  break;
	case 5:
	  binContent = (summ.SCACStatus)->GetBinContent(ix,iy);
	  break;
	case 6:
	  binContent = (summ.scaOutOfRange)->GetBinContent(ix,iy);
	  break;
	case 7:
	  binContent = (summ.gainMismatch)->GetBinContent(ix,iy);
	  break;
	case 8:
	  binContent = (summ.typeMismatch)->GetBinContent(ix,iy);
	  break;
	case 9:
	  binContent = (summ.badNbOfSamp)->GetBinContent(ix,iy);
	  break;
	case 10:
	  binContent = (summ.zeroSamp)->GetBinContent(ix,iy);
	  break;
	case 11:
	  binContent = (summ.checkSum)->GetBinContent(ix,iy);
	  break;
	case 12:
	  binContent = (summ.missingHeader)->GetBinContent(ix,iy);
	  break;
	case 13:
	  binContent = (summ.badGain)->GetBinContent(ix,iy);
	  break;
      }
      
      if (binContent>0 && nbOfFEBErrors < 15){
	HWIdentifier errorFebId = m_onlineHelper->feb_Id(barrel_ec,pos_neg,iy-1,ix);
	IdentifierHash hashId = m_onlineHelper->feb_Hash(errorFebId);
	if (!m_bfebIE[hashId]) {
	  m_febInError[hashId] = TH1F_LW::create((m_strHelper->feb_str(errorFebId)).c_str(),(m_strHelper->feb_str(errorFebId)+"(UNRELIABLE if # of faulty FEBs>15)").c_str(),13,0.5,13.5);
	  //	  tempHisto->Print();	
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(1,"Parity");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(2,"BCID");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(3,"Sample Header");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(4,"EVTID");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(5,"SCAC status");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(6,"Sca out of range");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(7,"Gain mismatch");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(8,"Type mismatch");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(9,"# of samples");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(10,"Empty data block");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(11,"Checksum / block size");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(12,"Missing header");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(13,"Bad gain");
	  (m_febInError[hashId]->GetYaxis())->SetTitle("Number of errors");
	  
	  StatusCode sc = generalGroup.regHist(m_febInError[hashId]);
	  if (sc.isFailure()) ATH_MSG_ERROR( "Failed to register Feb histogram!" );
	  else m_bfebIE[hashId] = true;
	}
	m_febInError[hashId]->SetBinContent(errorType,binContent);
	m_febInError[hashId]->SetEntries(m_eventsCounter);
        nbOfFEBErrors++;
      }
    }
  }
  
  fillYieldHistos(summ.LArAllErrors,summ.LArAllErrorsYield);
  
  return;
}
*/
/*---------------------------------------------------------*/
unsigned int LArFEBMonAlg::returnPartition(int be,int pn,int ft,int sl) const {
  // partitionNb_dE = 0 : EMBC / 1 : EMBA / 2 : EMECC / 3 : EMECA / 4 : HECC / 5 : HECA / 6 : FCALC / 7 : FCALA
  unsigned int part = be*2+pn;
  if (be == 1){
    // This is a HEC FEB - Dirty method as IsHECOnlineFEBId is buggy!
    if ((ft == 3 || ft == 10 || ft == 16 || ft == 22) && (sl > 2)) part = be*2+pn + 2;
    if (ft == 6) part = be*2 + pn + 4; // This is FCAL FEB
  }        
  return part;
}


