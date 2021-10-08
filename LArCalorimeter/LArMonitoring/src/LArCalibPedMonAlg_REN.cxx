/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibPedMonAlg_REN.h"

/*---------------------------------------------------------*/
LArCalibPedMonAlgREN::LArCalibPedMonAlgREN(const std::string& name,ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator),
  m_nbOfFebBlocksTotal(-1)
{}

/*---------------------------------------------------------*/
LArCalibPedMonAlgREN::~LArCalibPedMonAlgREN()
{}

/*---------------------------------------------------------*/
StatusCode LArCalibPedMonAlgREN::initialize() {
  ATH_MSG_INFO( "Initialize LArCalibPedMonAlgREN"  );

  ATH_MSG_INFO( "m_accDigitContainerKey.empty() " << m_accDigitContainerKey.empty()
        );
  if(!m_calibDigitContainerKey.empty()) {
    ATH_CHECK( m_calibDigitContainerKey.initialize() );
  } else if(!m_accDigitContainerKey.empty()) {
    ATH_CHECK( m_accDigitContainerKey.initialize() );
  } else if(!m_accCalibDigitContainerKey.empty()) {
    ATH_CHECK( m_accCalibDigitContainerKey.initialize() );
  } else {
     ATH_MSG_FATAL("Either LArCalibDigitContainerKey or LArAccumulatedDigitContainerKey or LArAccumulatedCalibDigitContainerKey must be set");
     return StatusCode::FAILURE;
  }
  
  ATH_CHECK( m_hdrContKey.initialize() );
  
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
StatusCode LArCalibPedMonAlgREN::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG( "in fillHists()"  );
  
  SG::ReadHandle<LArCalibDigitContainer> pLArCalibDigitContainer;
  SG::ReadHandle<LArAccumulatedDigitContainer> pLArAccDigitContainer;
  SG::ReadHandle<LArAccumulatedCalibDigitContainer> pLArAccCalibDigitContainer;
    
  std::unordered_set<unsigned int> chanids;

  if(!m_calibDigitContainerKey.empty()) {
    pLArCalibDigitContainer= SG::ReadHandle<LArCalibDigitContainer>{m_calibDigitContainerKey,ctx};
    if(pLArCalibDigitContainer.isValid()){
       ATH_MSG_DEBUG("Got LArCalibDigitContainer with key "<< m_calibDigitContainerKey.key());
    } else {
       ATH_MSG_WARNING("Do not have LArCalibDigitContainer with key "<< m_calibDigitContainerKey.key());
    }  
  }

  if(!m_accDigitContainerKey.empty()) {
    pLArAccDigitContainer= SG::ReadHandle<LArAccumulatedDigitContainer>{m_accDigitContainerKey,ctx};
    if(pLArAccDigitContainer.isValid()){
       ATH_MSG_DEBUG("Got LArAccumulatedDigitContainer with key "<< m_accDigitContainerKey.key());
    } else {
       ATH_MSG_WARNING("Do not have LArAccumulatedDigitContainer with key "<< m_accDigitContainerKey.key());
    }  

    if(pLArAccDigitContainer->empty()) return StatusCode::SUCCESS; // Nothing to fill

    LArAccumulatedDigitContainer::const_iterator itDig = pLArAccDigitContainer->begin();
    LArAccumulatedDigitContainer::const_iterator itDig_e = pLArAccDigitContainer->end();
    const LArAccumulatedDigit* pLArDigit;
    for ( ; itDig!=itDig_e;++itDig) {
        pLArDigit = *itDig;
        unsigned int id = (pLArDigit->hardwareID()).get_identifier32().get_compact();
        if(chanids.find(id) == chanids.end()) chanids.emplace(id);
    }

    ATH_MSG_DEBUG("Filling nbChan: "<<chanids.size());

    auto nbchan = Monitored::Scalar<unsigned int>("nbChan",chanids.size());
    fill(m_MonGroupName,nbchan);
    
  }

  if(!m_accCalibDigitContainerKey.empty()) {
    pLArAccCalibDigitContainer= SG::ReadHandle<LArAccumulatedCalibDigitContainer>{m_accCalibDigitContainerKey,ctx};
    if(pLArAccCalibDigitContainer.isValid()){
       ATH_MSG_DEBUG("Got LArAccumulatedCalibDigitContainer with key "<< m_accCalibDigitContainerKey.key());
    } else {
       ATH_MSG_WARNING("Do not have LArAcumulatedCalibDigitContainer with key "<< m_accCalibDigitContainerKey.key());
    }  

    if(pLArAccCalibDigitContainer->empty()) return StatusCode::SUCCESS; // Nothing to fill

    LArAccumulatedCalibDigitContainer::const_iterator itDig = pLArAccCalibDigitContainer->begin();
    LArAccumulatedCalibDigitContainer::const_iterator itDig_e = pLArAccCalibDigitContainer->end();
    const LArAccumulatedCalibDigit* pLArDigit;
    for ( ; itDig!=itDig_e;++itDig) {
        pLArDigit = *itDig;
        unsigned int id = (pLArDigit->hardwareID()).get_identifier32().get_compact();
        if(chanids.find(id) == chanids.end()) chanids.emplace(id);
    }

//     ATH_MSG_DEBUG("Filling nbChan: "<<chanids.size());

    auto nbchan = Monitored::Scalar<unsigned int>("nbChan",chanids.size());
    fill(m_MonGroupName,nbchan);
    
  }
  
//   int firstEventNbSp=-1;
  
  SG::ReadHandle<xAOD::EventInfo> thisEvent = GetEventInfo(ctx);
  SG::ReadHandle<LArFebHeaderContainer> hdrCont(m_hdrContKey, ctx);
  if (!hdrCont.isValid()) {
    ATH_MSG_ERROR( "No LArFebHeaderContainer found in TDS" ); 
    return StatusCode::FAILURE;
  }
  
  if (hdrCont->size()==0) {
    ATH_MSG_ERROR( "Got empty LArFebHeaderContainer. Do nothing" );
    return StatusCode::FAILURE;
  }
  
  uint32_t firstEventType = (*hdrCont->begin())->DetEventType();
  
//Fill general data histos
  auto evttype = Monitored::Scalar<int>("EvtType",firstEventType);

  std::vector<unsigned int> nfeb(m_partitions.size()); 
  float nbOfFeb = 0.;
  for(auto nf: nfeb) nbOfFeb+=nf;// (nfeb[0]+nfeb[1]+nfeb[2]+nfeb[3]+nfeb[4]+nfeb[5]+nfeb[6]+nfeb[7]);
//   if (firstEventType == 2 || firstEventType == 4 || firstEventType == 14) {
//      auto nbsamp = Monitored::Scalar<int>("NbOfSamp",firstEventNbSp);
//      fill(m_MonGroupName, nbsamp);
//   }
  
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
  
  return StatusCode::SUCCESS;
}



