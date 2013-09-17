/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/LArDigits2Ntuple.h"
#include "CaloIdentifier/CaloGain.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "TBEvent/TBPhase.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "TBEvent/TBScintillatorCont.h"

#include "LArElecCalib/ILArPedestal.h"
#include <fstream>


LArDigits2Ntuple::LArDigits2Ntuple(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_storeGateSvc(0),
    m_detStore(0),
    m_onlineHelper(0),
    m_larCablingSvc(0),
    m_emId(0),
    m_hecId(0),
    m_fcalId(0),
    m_nt(0),
    m_nsamples(0),
    m_accept(0),
    m_reject(0),
    m_phase(0),
    m_scint(0),
    m_trigger(0),
    m_sca(0),
    m_ped(0)
{ 
  declareProperty("ContainerKey",m_contKey);
  declareProperty("NSamples",m_nsamples=5);
  // By default to get only pedestal events, we need to set m_reject=1
  // But to reject non flaged events (physics events occuring out of
  // the beam signal...) we need also to set m_accept=14.
  declareProperty("accept", m_accept=14);
  declareProperty("reject", m_reject=1);
  declareProperty("ReadPhase", m_phase=1);
  declareProperty("ReadScint", m_scint=1);
  declareProperty("ReadTrigger", m_trigger=1);
  declareProperty("ReadSCA", m_sca=1);
  declareProperty("ReadPedestal", m_ped=1);
}

LArDigits2Ntuple::~LArDigits2Ntuple() 
{}

StatusCode LArDigits2Ntuple::initialize()
{
  MsgStream log(msgSvc(), name());

  StatusCode sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure()) 
    {log << MSG::ERROR << " Cannot locate StoreGateSvc " << std::endl;
     return StatusCode::FAILURE;
    }

  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }
 
  sc= service("DetectorStore",m_detStore);
  if(sc.isFailure()) {
    log << MSG::ERROR << "DetectorStore service not found" << endreq;
    return StatusCode::FAILURE;
  }

  //Use CaloIdManager to access detector info
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  m_fcalId=caloIdMgr->getFCAL_ID();
  m_hecId=caloIdMgr->getHEC_ID();

  if (!m_emId) {
    log << MSG::ERROR << "Could not access lar EM ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (!m_fcalId) {
    log << MSG::ERROR << "Could not access lar FCAL ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (!m_hecId) {
    log << MSG::ERROR << "Could not access lar HEC ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  // get cablingSvc
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    sc = toolSvc->retrieveTool("LArCablingService",m_larCablingSvc);
    if(sc.isFailure()){
      log << MSG::ERROR << "Could not retrieve LArCablingService Tool" << endreq;
      return StatusCode::FAILURE;
    }
  } else {
    log << MSG::ERROR << "Could not retrieve ToolSvc" << endreq;
    return StatusCode::FAILURE;
  }

  // get onlineHelper
  sc = detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << " Found the LArOnlineID helper. " << endreq;
  }

  // Book ntuple
  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1)
    {log << MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE;
    }
  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE1/DIGITS");
  if (!nt) {
    nt=ntupleSvc()->book("/NTUPLES/FILE1/DIGITS",CLID_ColumnWiseTuple,"Digits");
  }
  if (!nt)
    {log << MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE;
    }

  sc=nt->addItem("event",event);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Event' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("layer",layer,0,4);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Layer' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("ieta",eta,0,510);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Eta' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("iphi",phi,0,1023);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Phi' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("region",region,0,1);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'region' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("barrel_ec",barrel_ec,0,1);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'barrel_ec' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("pos_neg",pos_neg,0,1);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'pos_neg' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("detector",detector,0,2);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Region' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("FT",FT,0,32);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'FT' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("slot",slot,1,15);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'slot' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("channel",channel,0,127);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'channel' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("gain",gain,0,3);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Gain' failed" << endreq;
    return StatusCode::FAILURE;
    }
  sc=nt->addItem("samples",m_nsamples,samples);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Ped' failed" << endreq;
    return StatusCode::FAILURE;
    }
  if(m_ped) {
    sc=nt->addItem("ped",ped);
    if (sc!=StatusCode::SUCCESS)
      {log << MSG::ERROR << "addItem 'Ped' failed" << endreq;
      return StatusCode::FAILURE;
      }
  }
  if(m_sca) {
    sc=nt->addItem("sca",m_nsamples,sca);
    if (sc!=StatusCode::SUCCESS)
      {log << MSG::ERROR << "addItem 'sca' failed" << endreq;
      return StatusCode::FAILURE;
      }
  }
  if(m_phase) {
    sc=nt->addItem("tdc",tdc);
    if (sc!=StatusCode::SUCCESS)
      {log << MSG::ERROR << "addItem 'tdc' failed" << endreq;
      return StatusCode::FAILURE;
      }
  }
  if(m_trigger) {
    sc=nt->addItem("trigger",trigger);
    if (sc!=StatusCode::SUCCESS)
      {log << MSG::ERROR << "addItem 'trigger' failed" << endreq;
      return StatusCode::FAILURE;
      }
  }
  if(m_scint) {
    sc=nt->addItem("S1",S1);
    if (sc!=StatusCode::SUCCESS)
      {log << MSG::ERROR << "addItem 'S1' failed" << endreq;
      return StatusCode::FAILURE;
      }
  }
  m_nt=nt;

  return StatusCode::SUCCESS;
}  

StatusCode LArDigits2Ntuple::execute()
{
  int eventnumber,triggerword;
  double S1Adc,tdcphase;

  MsgStream log(msgSvc(), name());

  // Retrieve EventInfo
  const DataHandle<EventInfo> thisEventInfo;
  StatusCode sc=m_storeGateSvc->retrieve(thisEventInfo);
  eventnumber=0;
  if (sc!=StatusCode::SUCCESS)
    log << MSG::WARNING << "No EventInfo object found!" << endreq;
  else {
    EventID *thisEvent=thisEventInfo->event_ID();
    eventnumber=thisEvent->event_number();
  }

  // Retrieve the TBScintillators
  if(m_scint) {
    S1Adc=-999.0;
    const TBScintillatorCont * theTBScint;
    sc = m_storeGateSvc->retrieve(theTBScint,"ScintillatorCont");
    if (sc.isFailure()) 
      {
	log << MSG::ERROR << " Cannot read TBScintillatorCont from StoreGate! " << endreq;
	//return StatusCode::FAILURE;
      }
    else {
      TBScintillatorCont::const_iterator it_scint = theTBScint->begin();
      TBScintillatorCont::const_iterator last_scint = theTBScint->end();
      for(;it_scint!=last_scint;it_scint++) {
	const TBScintillator * scint = (*it_scint);
	const std::string name = scint->getDetectorName();
	if (name=="S1") {
	  S1Adc = scint->getSignal();
	  break;
	}
      } //end loop over scintillator-container
    }
  }
  else S1Adc=0; 

  //Retrieve the TBPhase
  if(m_phase) {
    const TBPhase* theTBPhase;
    sc = m_storeGateSvc->retrieve(theTBPhase, "TBPhase");
    
    if (sc.isFailure()) {
      tdcphase = -999.0;
      log << MSG::ERROR
	  << "cannot allocate TBPhase "
	  << endreq;
      //return StatusCode::FAILURE;
    } else {
      tdcphase = theTBPhase->getPhase();
    }
  }
  else tdcphase = 0;

  //Retrieve the TBTriggerPatternUnit
  if(m_trigger) {
    const TBTriggerPatternUnit* theTBTriggerPatternUnit;
    sc = m_storeGateSvc->retrieve(theTBTriggerPatternUnit, "TBTrigPat");

    if (sc.isFailure()) {
      triggerword = 0;
      log << MSG::ERROR
	  << "cannot allocate TBTriggerPatternUnit"
	  << endreq;
      //return StatusCode::FAILURE;
    } else {
      triggerword = theTBTriggerPatternUnit->getTriggerWord();
    }

    // As a trigger can be both physics and pedestal (Indeed!)
    // we need to make two checks to cover all possibilities
    // Check whether we should reject this trigger
    if(m_reject>0) if(triggerword&m_reject) return StatusCode::SUCCESS;
    // Check whether we should accept this trigger
    if(m_accept>0) if((triggerword&m_accept)==0) return StatusCode::SUCCESS;
  }
  else triggerword=0;

  // Retrieve the pedestal
  //Pointer to conditions data objects 
  const ILArPedestal* larPedestal=NULL;
  if (m_ped) {
    sc=m_detStore->retrieve(larPedestal);
    if (sc.isFailure()) {
      larPedestal=NULL;
      log << MSG::INFO << "No pedestal found in database. Use default values." << endreq;
    }
  }

  // Retrieve LArDigitContainer
  const DataHandle < LArDigitContainer > digit_cont;
  if (m_contKey.size())
    sc = m_storeGateSvc->retrieve(digit_cont,m_contKey);
  else
    sc = m_storeGateSvc->retrieve(digit_cont);
  if (sc.isFailure()) 
    {log << MSG::ERROR << " Cannot read LArDigitContainer from StoreGate! key=" << m_contKey << endreq;
    return StatusCode::FAILURE;
    }
  log << MSG::INFO << "Retrieved LArDigitContainer from StoreGate! key=" << m_contKey << endreq;

  // Retrieve LArFebHeaderContainer
  const LArFebHeaderContainer *larFebHeaderContainer;
  if(m_sca) {
    sc= m_storeGateSvc->retrieve(larFebHeaderContainer);
    if (sc.isFailure() || !larFebHeaderContainer) {
      log << MSG::DEBUG << "Cannot read LArFebHeaderContainer from StoreGate!" << endreq;
      return StatusCode::FAILURE;
    }
  }

  // Fill ntuple
  LArDigitContainer::const_iterator it = digit_cont->begin(); 
  LArDigitContainer::const_iterator it_e = digit_cont->end(); 
  if(it==it_e) {
    log << MSG::DEBUG << "LArDigitContainer is empty..." << endreq;
  }
  for(; it!=it_e; ++it){
    const HWIdentifier hwid=(*it)->channelID();//hardwareID();
    // Fill detector geometry information
    event   = eventnumber;
    if(m_phase)   tdc     = tdcphase;
    if(m_trigger) trigger = triggerword;
    if(m_scint)   S1      = S1Adc;
    try {
      Identifier id=m_larCablingSvc->cnvToIdentifier(hwid);
      if (m_emId->is_lar_em(id)) {
	eta       = m_emId->eta(id);
	phi       = m_emId->phi(id);
	layer     = m_emId->sampling(id);
	region    = m_emId->region(id);
	detector  = 0;
      }
      else if (m_hecId->is_lar_hec(id)) {
	eta       = m_hecId->eta(id);
	phi       = m_hecId->phi(id);
	layer     = m_hecId->sampling(id);
	region    = m_hecId->region(id);
	detector  = 1;
      }
      else if (m_fcalId->is_lar_fcal(id)) {
	eta       = m_fcalId->eta(id);
	phi       = m_fcalId->phi(id);
	layer     = m_fcalId->module(id);
	region    = 0;
	detector  = 2;
      }
    }
    catch (LArID_Exception & except) {
      eta       = -1;
      phi       = -1;
      layer     = -1;
      region    = -1;
      detector  = -1;
    }
    // Fill hardware information
    barrel_ec = m_onlineHelper->barrel_ec(hwid);
    pos_neg   = m_onlineHelper->pos_neg(hwid);
    FT        = m_onlineHelper->feedthrough(hwid);
    slot      = m_onlineHelper->slot(hwid);
    channel   = m_onlineHelper->channel(hwid);

    // Fill pedestal information
    float thePedestal=-1;    
    if (larPedestal) {
      float  DBpedestal=larPedestal->pedestal(hwid,(*it)->gain());
      if (DBpedestal >= (1.0+LArElecCalib::ERRORCODE))
	thePedestal=DBpedestal;
    }
    if (thePedestal<0) {
      thePedestal = -999;
      log << MSG::DEBUG << "No pedestal found for this cell. Use default value " << thePedestal << endreq;
    }
    ped = thePedestal;

    // Fill raw data samples and gain
    for(unsigned int i=0;i<(*it)->samples().size();i++) {
      if((int)i>=m_nsamples) break;
      samples[i]=(*it)->samples()[i];
    }
    gain=(*it)->gain();

    // Fill SCA numbers
    if(m_sca) {
      const HWIdentifier febid=m_onlineHelper->feb_Id(hwid);
      LArFebHeaderContainer::const_iterator feb_it=larFebHeaderContainer->begin();
      LArFebHeaderContainer::const_iterator feb_it_e=larFebHeaderContainer->end();
      for (;feb_it!=feb_it_e;feb_it++) {
	const HWIdentifier this_febid=(*feb_it)->FEBId();
	
	if(this_febid!=febid) continue;
	for(unsigned int i=0;i<(*feb_it)->SCA().size();i++) {
	  if((int)i>=m_nsamples) break;
	  sca[i]=(*feb_it)->SCA()[i];
	}
	break;
      } // End FebHeader loop
    }
    sc=ntupleSvc()->writeRecord(m_nt);
    
    if (sc!=StatusCode::SUCCESS) {
      log << MSG::ERROR << "writeRecord failed" << endreq;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode LArDigits2Ntuple::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "LArDigits2Ntuple has finished." << endreq;
  return StatusCode::SUCCESS;
}// end finalize-method.
