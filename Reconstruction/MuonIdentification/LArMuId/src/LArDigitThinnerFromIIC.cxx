/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArMuId/LArDigitThinnerFromIIC.h"

#include <iostream>

#include "TrackToCalo/ImpactInCaloCollection.h"
#include "Identifier/HWIdentifier.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArDigit.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"


using std::cout;
using std::endl;


LArDigitThinnerFromIIC::LArDigitThinnerFromIIC(const std::string& name,
				 ISvcLocator* pSvcLocator) :
    Algorithm(name, pSvcLocator)
    , m_log(0)
    , m_storeGate("StoreGateSvc", name)
    , m_detStore("DetectorStore", name)
    , m_larCablingSvc("LArCablingService") 
    , m_inputContainerName("FREE")
    , m_outputContainerName("LArDigitContainer_IIC")
    , m_cablingCache(nChannels), m_dr_min(0.1)
{
  declareProperty("InputContainerName", m_inputContainerName);
  declareProperty("OutputContainerName",m_outputContainerName);
  declareProperty("ImpactInCaloName",m_impactInCaloName);
  declareProperty("deltaR",m_dr_min);
}

LArDigitThinnerFromIIC::~LArDigitThinnerFromIIC() {
  if (m_log) {
    delete m_log;
    m_log =0;
  }
}


StatusCode LArDigitThinnerFromIIC::initialize()
{
  m_log = new MsgStream(msgSvc(), name());
  log() << MSG::VERBOSE << "LArDigitThinnerFromIIC Init" << endreq;
  StatusCode sc;
  
  //Initialize stores, services & tools:
  sc = m_storeGate.retrieve();
  if (sc.isFailure()) {
    log() << MSG::ERROR << "StoreGate service not found" << endreq;
    return sc;
  }

  sc = m_detStore.retrieve();
  if (sc.isFailure()) {
    log() << MSG::ERROR << "DetectorStore service not found" << endreq;
    return sc;
  }
 
  sc = m_larCablingSvc.retrieve();
  if (sc.isFailure()) {
    log() << MSG::ERROR << "Could not retrieve LArCablingService Tool" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = m_detStore->retrieve(m_onlineID, "LArOnlineID");
  if (sc.isFailure()) {
    log() << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return sc;
  } 
  
  sc = m_detStore->retrieve(m_ddm);
  if (sc.isFailure()) {
    log() << MSG::ERROR << "Could not get CaloDetDescrManager  !" << endreq;
    return sc;
  } 
  
  sc = m_detStore->regFcn(&LArCablingService::iovCallBack,&(*m_larCablingSvc),
                           &LArDigitThinnerFromIIC::iovCallBack,
                           this,true) ;
  if (sc.isFailure()) {
    log() << MSG::ERROR << "Could not register callback to cabling service." << endreq;
    return sc;
  }
    
  m_cablingCache.clear();
  m_cablingCache.resize(nChannels);
 
  m_seeds.resize(4); 

  return StatusCode::SUCCESS;
}


StatusCode LArDigitThinnerFromIIC::iovCallBack(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{
  log() << MSG::INFO << " iovCallBack " << endreq;
  m_cablingCache.clear();
  m_cablingCache.resize(nChannels);
  return StatusCode::SUCCESS;
}



StatusCode LArDigitThinnerFromIIC::finalize()
{
  log() << MSG::INFO << "LArDigitThinnerFromIIC Finalize" << endreq;  

  return StatusCode::SUCCESS;
}


StatusCode LArDigitThinnerFromIIC::execute()
{
  log() << MSG::VERBOSE << "LArDigitThinnerFromIIC Exec" << endreq;
  StatusCode sc;

  // Create the new digit container
  LArDigitContainer* outputContainer = new LArDigitContainer(SG::VIEW_ELEMENTS);
  if (!outputContainer){
    log() << MSG::WARNING << "Could not allocate a new LArDigitContainer" << endreq;
    return StatusCode::SUCCESS;	  
  }
  
  sc = m_storeGate->record(outputContainer , m_outputContainerName);
  if (sc.isFailure()) {
    log() << MSG::WARNING << "Could not record output LArDigitContainer with key " 
        << m_outputContainerName << endreq;
    return StatusCode::SUCCESS;
  }

  const LArDigitContainer* inputContainer = 0;
  sc = m_storeGate->retrieve(inputContainer, m_inputContainerName);
  
  if (sc.isFailure()) { 
    log() << MSG::WARNING << "Input LArDigitContainer not found with key"
        << m_inputContainerName << endreq;
    return StatusCode::SUCCESS;
  }


  sc = getEtaPhi(); 
  if (sc.isFailure()) { 
    return StatusCode::SUCCESS;
  }

  if (m_seeds[0].size()==0 ){

    log() << MSG::DEBUG << " There is no seeds from ImpactInCaloCollection, nothing in output container " << endreq;
    return StatusCode::SUCCESS;
  }


  for (LArDigitContainer::const_iterator chan = inputContainer->begin(); 
       chan != inputContainer->end(); ++chan) 
  {       

    HWIdentifier channelID = (*chan)->channelID();
 
    if (!channelSelection(channelID)) continue;

    outputContainer->push_back(*chan);
  }

  
  log() << MSG::DEBUG << "Copied " << outputContainer->size() << " of " 
      << inputContainer->size() << " digits." << endmsg;
  
  return StatusCode::SUCCESS;
}
  


bool LArDigitThinnerFromIIC::channelSelection(const HWIdentifier& channelID) 
{

  const CaloDetDescrElement* dde = getDDE(channelID) ; 

  if (!dde) return false; 
  
  CaloCell_ID::CaloSample sam = dde->getSampling(); 
  int i=-1;
  if (sam==CaloCell_ID::PreSamplerB || sam==CaloCell_ID::PreSamplerE)
    {i=0;}
  else if(sam==CaloCell_ID::EME1 || sam==CaloCell_ID::EMB1)
    {i=1;}
  else if(sam==CaloCell_ID::EME2 || sam==CaloCell_ID::EMB2)
    {i=2;}
  else if(sam==CaloCell_ID::EME3 || sam==CaloCell_ID::EMB3)
    {i=3;}
    
  if (i==-1) return false;

  std::vector<EtaPhi>& v=m_seeds[i]; 
  std::vector<EtaPhi>::const_iterator it= v.begin();
  std::vector<EtaPhi>::const_iterator it_e= v.end();
  for (; it!=it_e;++it){
    float d_eta = (*it).first-dde->eta();
    float d_phi = (*it).second-dde->phi();
    d_phi=std::fmod(d_phi+3*M_PI,2*M_PI)-M_PI;
    float dR= sqrt(d_eta*d_eta+d_phi*d_phi);
    if (dR<m_dr_min) return true; 
  }

  return false; 

}


StatusCode  LArDigitThinnerFromIIC::getEtaPhi() 
{
  const ImpactInCaloCollection* impactincalocollec ;
  StatusCode sc = m_storeGate->retrieve(impactincalocollec, m_impactInCaloName) ; 
    
  if ( sc.isFailure()) {
    log() << MSG::ERROR
	  << " No impact in Calo found "
	  << endreq;
    return StatusCode::FAILURE;
  }
  log() << MSG::DEBUG << " retrieved ImpactInCaloCollection " << endreq;

  for (int i=0; i<4;++i)
    m_seeds[i].clear() ;
    
  for (ImpactInCaloCollection::const_iterator itr  = 
	 (*impactincalocollec).begin(); itr != (*impactincalocollec).end(); itr++){
    
      m_seeds[0].push_back(EtaPhi((*itr)->etaCaloLocal_0(),(*itr)->phiCaloLocal_0() ) ); 
      m_seeds[1].push_back(EtaPhi((*itr)->etaCaloLocal_1(),(*itr)->phiCaloLocal_1() ) ); 
      m_seeds[2].push_back(EtaPhi((*itr)->etaCaloLocal_2(),(*itr)->phiCaloLocal_2() ) ); 
      m_seeds[3].push_back(EtaPhi((*itr)->etaCaloLocal_3(),(*itr)->phiCaloLocal_3() ) ); 
      
  }

  log() << MSG::DEBUG << " obtained "<< m_seeds[0].size() << " ImpactInCalos " << endreq;

  return StatusCode::SUCCESS; 

}

const CaloDetDescrElement*  LArDigitThinnerFromIIC::getDDE(const HWIdentifier& channelID) const
{

  IdentifierHash hash = m_onlineID->channel_Hash(channelID);
  if (hash >= m_cablingCache.size()) {
    log() << MSG::ERROR << "Found channel with hash = " << hash 
          << " greater than the expected highest value " << m_cablingCache.size() << endreq;
    return 0;
  }
  
  CaloPosition& pos = m_cablingCache[hash];
  if (pos.cached) return pos.element;  // the good case : the position was processed before and the result is cached

  pos.cached=true; 
  // This repeats the tests done below for each subdetector, but keep it for safety
  if (!m_larCablingSvc->isOnlineConnected(channelID)) return 0; 
  
  // This is slow - which is why we cache the results.
  Identifier id = m_larCablingSvc->cnvToIdentifier(channelID);
  if (!id.is_valid()) {
    log() << MSG::ERROR << "LArCablingSvc returned invalid Identifier for channel with hash = " << hash << endreq;
    return 0; 
  }

  const CaloDetDescrElement* element = m_ddm->get_element(id); 
  pos.element=element; 

  return element;

}
