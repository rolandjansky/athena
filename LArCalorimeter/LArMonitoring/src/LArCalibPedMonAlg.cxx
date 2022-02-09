/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibPedMonAlg.h"

/*---------------------------------------------------------*/
LArCalibPedMonAlg::LArCalibPedMonAlg(const std::string& name,ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator),
    m_onlineHelper(nullptr),
    m_nbOfFebBlocksTotal(-1)
{}

/*---------------------------------------------------------*/
StatusCode LArCalibPedMonAlg::initialize() {
  ATH_MSG_INFO( "Initialize LArCalibPedMonAlg"  );

/*For pedestal run ONLY, not delay and ramp*/
  ATH_MSG_INFO( "m_accDigitContainerKey.empty() " << m_accDigitContainerKey.empty() );
  ATH_CHECK( m_accDigitContainerKey.initialize(SG::AllowEmpty));
  if (m_accDigitContainerKey.empty()) {
     ATH_MSG_FATAL("LArAccumulatedDigitContainerKey must be set"); 
     return StatusCode::FAILURE;
  }
    
  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
    return StatusCode::FAILURE;
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
  return AthMonitorAlgorithm::initialize();
}


/*---------------------------------------------------------*/
StatusCode LArCalibPedMonAlg::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG( "in fillHists()"  );
  
  SG::ReadHandle<LArAccumulatedDigitContainer> pLArAccDigitContainer;
  
  bool eventRejected = false;
  std::bitset<13> rejectionBits;
  std::vector<int> febInErrorTree(0);  
  std::unordered_set<unsigned int> chanids;

  if(!m_accDigitContainerKey.empty()) {
    pLArAccDigitContainer= SG::ReadHandle<LArAccumulatedDigitContainer>{m_accDigitContainerKey,ctx};
    if(pLArAccDigitContainer.isValid()){
       ATH_MSG_DEBUG("Got LArAccumulatedDigitContainer with key "<< m_accDigitContainerKey.key());
    } else {
       ATH_MSG_WARNING("Do not have LArAccumulatedDigitContainer with key "<< m_accDigitContainerKey.key());
    }  

    if(pLArAccDigitContainer->empty()) return StatusCode::SUCCESS; // Nothing to fill

    for (LArAccumulatedDigitContainer::const_iterator itDig = pLArAccDigitContainer->begin(); itDig!=pLArAccDigitContainer->end();++itDig) {
        unsigned int id = ((*itDig)->hardwareID()).get_identifier32().get_compact();
        if(chanids.find(id) == chanids.end()) chanids.emplace(id);
    }

    ATH_MSG_DEBUG("Filling nbChan: "<<chanids.size());
    auto nbchan = Monitored::Scalar<unsigned int>("nbChan",chanids.size());
    fill(m_MonGroupName,nbchan);
  }

  SG::ReadHandle<xAOD::EventInfo> thisEvent = GetEventInfo(ctx);
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
    ATH_MSG_WARNING( "Got empty LArFebHeaderContainer. Do nothing" );
  }

  if (!lArFebErrorSummary.isValid()) {
    ATH_MSG_ERROR( "No LArFebErrorSummary found in TDS" );
    return StatusCode::FAILURE;
  }
  // Eventype = 2 : transparent/raw data - 4 : Physic - 7 : calibration - 10 : pedestal - 14 : raw data + results
  uint32_t firstEventType = (*hdrCont->begin())->DetEventType();
  
  std::vector<unsigned int> nfeb(m_partitions.size());

  float larEventSize = 0;
    
  auto slmon = Monitored::Scalar<int>("slotnb",-1);
  auto ftmon = Monitored::Scalar<int>("FTnb",-1);

  for ( LArFebHeaderContainer::const_iterator it = hdrCont->begin(); it!=hdrCont->end();++it) {
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
      
      // Eventype = 2 : transparent/raw data - 4 : Physic - 7 : calibration - 10 : pedestal - 14 : raw data + results
      uint32_t eventType = (*it)->DetEventType();      
      // If physic mode && raw data != 0, we are in rawdata+results
      if (eventType == 4 && (*it)->RodRawDataSize() != 0) eventType = 14;
      //if (firstEventType == 999) firstEventType = eventType;
      
      slmon = slot;
      ftmon = ft;
      fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitionNb_dE])],slmon,ftmon);
    } //Test on FEBid
  } //end of loop over FEB headers
  // Loop over all febs to plot the error from statusword
  // This is mandatory to also monitor the FEBs with missing headers
   
  for (std::vector<HWIdentifier>::const_iterator allFeb = m_onlineHelper->feb_begin(); allFeb != m_onlineHelper->feb_end(); ++allFeb) {
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

      if (currentFebStatus && febInErrorTree.size()<33) febInErrorTree.push_back(febid.get_identifier32().get_compact());
    }  
  }
  
  
//Fill general data histos
  auto evttype = Monitored::Scalar<int>("EvtType",firstEventType);
    
  float nbOfFeb = 0.;
  for(auto nf: nfeb) nbOfFeb+=nf;// (nfeb[0]+nfeb[1]+nfeb[2]+nfeb[3]+nfeb[4]+nfeb[5]+nfeb[6]+nfeb[7]);

  bool newHighWaterMarkNFebBlocksTotal = false;
  if(m_nbOfFebBlocksTotal < nbOfFeb){ // new number of Febs
    m_nbOfFebBlocksTotal = nbOfFeb;
    newHighWaterMarkNFebBlocksTotal = true;
  }
  auto nbfeb = Monitored::Scalar<int>("nbFEB",nbOfFeb);
  fill(m_MonGroupName, evttype, nbfeb); 

  auto part = Monitored::Scalar<int>("part",-1);
  auto nbfebpart = Monitored::Scalar<int>("nbFEBpart",-1);
  for(unsigned i=0; i<m_partitions.size(); ++i) {
     part=i;
     unsigned subdet = i / 2;
     nbfebpart=nfeb[i];
     fill(m_MonGroupName,part,nbfebpart);
     fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[i])],nbfebpart);
  }
// LB histograms filling  
  auto evtrej = Monitored::Scalar<int>("EvtRej",-1);
  float evt_yield=-1.;
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
  evtyield=evt_yield;
  auto evSize = Monitored::Scalar<float>("LArEvSize",larEventSize/262144);
  auto lb0 = Monitored::Scalar<int>("LB0",lumi_block); //to avoid 'NbOfEventsVSLB' being filled multiple times
  fill(m_MonGroupName,evtrej,evtyieldout,evtyield,evtyield1D,evtoneyield,evSize,lb0);
    evtrej=7;
  fill(m_MonGroupName,evtrej);
  
  return StatusCode::SUCCESS;
}

unsigned int LArCalibPedMonAlg::returnPartition(int be,int pn,int ft,int sl) const {
  // partitionNb_dE = 0 : EMBC / 1 : EMBA / 2 : EMECC / 3 : EMECA / 4 : HECC / 5 : HECA / 6 : FCALC / 7 : FCALA
  unsigned int part = be*2+pn;
  if (be == 1){
    // This is a HEC FEB - Dirty method as IsHECOnlineFEBId is buggy!
    if ((ft == 3 || ft == 10 || ft == 16 || ft == 22) && (sl > 2)) part = be*2+pn + 2;
    if (ft == 6) part = be*2 + pn + 4; // This is FCAL FEB
  }        
  return part;
}

void LArCalibPedMonAlg::fillErrorsSummary(unsigned int partitNb_2,int ft,int slot,uint16_t error, bool lar_inerror, std::bitset<13> &rejectionBits, bool &currentFebStatus, bool &eventRejected) const
{    
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
    fill(m_MonGroupName, ferror, part);
  }
  
  if ( error & (1<<1) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotBcid",slot);
    auto ftmon = Monitored::Scalar<int>("FTBcid",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(1);
    ferror=2;
    fill(m_MonGroupName, ferror, part);
  }
  
  if ( error & (1<<2) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotRadd",slot);
    auto ftmon = Monitored::Scalar<int>("FTRadd",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(2);
    ferror=3;
    fill(m_MonGroupName, ferror, part);
  }
  
  if ( error & (1<<3) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotEvtid",slot);
    auto ftmon = Monitored::Scalar<int>("FTEvtid",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(3);
    ferror=4;
    fill(m_MonGroupName, ferror, part);
  }
  
  if ( error & (1<<4) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotScac",slot);
    auto ftmon = Monitored::Scalar<int>("FTScac",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(4);
    ferror=5;
    fill(m_MonGroupName, ferror, part);
  }
  
  if ( error & (1<<5) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotscout",slot);
    auto ftmon = Monitored::Scalar<int>("FTscout",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(5);
    ferror=6;
    fill(m_MonGroupName, ferror, part);
  }
  
  if ( error & (1<<6) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotgain",slot);
    auto ftmon = Monitored::Scalar<int>("FTgain",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(6);
    ferror=7;
    fill(m_MonGroupName, ferror, part);
  }
  
  if ( error & (1<<7) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slottype",slot);
    auto ftmon = Monitored::Scalar<int>("FTtype",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(7);
    ferror=8;
    fill(m_MonGroupName, ferror, part);
  }
  
  if ( error & (1<<8) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotsmp",slot);
    auto ftmon = Monitored::Scalar<int>("FTsmp",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(8);
    ferror=9;
    fill(m_MonGroupName, ferror, part);
  }
  
  if ( error & (1<<9) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotzero",slot);
    auto ftmon = Monitored::Scalar<int>("FTzero",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(9);
    ferror=10;
    fill(m_MonGroupName, ferror, part);
  }
  
  
  if ( error & (1<<11) ){
    unsigned subdet = partitNb_2 / 2;
    auto sl = Monitored::Scalar<int>("slotsum",slot);
    auto ftmon = Monitored::Scalar<int>("FTsum",ft);
    fill(m_tools[m_histoGroups.at(subdet).at(m_partitions[partitNb_2])],sl,ftmon);
    currentFebStatus = true;
    rejectionBits.set(10);
    ferror=11;
    fill(m_MonGroupName, ferror, part);
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
      fill(m_MonGroupName, ferror, part);
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
    fill(m_MonGroupName, ferror, part);
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
