/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArDigits2Ntuple.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "LArRawEvent/LArDigitContainer.h"

//#include "GaudiKernel/ToolHandle.h"

LArDigits2Ntuple::LArDigits2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
  LArCond2NtupleBase(name, pSvcLocator),
  m_ipass(0),
  m_event(0), m_FTlist(0)
{
  declareProperty("ContainerKey",m_contKey);
  declareProperty("NSamples",m_Nsamples=32);
  declareProperty("FTlist",m_FTlist);
  m_ntTitle="LArDigits";
  m_ntpath="/NTUPLES/FILE1/LARDIGITS"+m_contKey;

}

LArDigits2Ntuple::~LArDigits2Ntuple() 
{}


StatusCode LArDigits2Ntuple::initialize()
{
   msg(MSG::INFO) << "in initialize" << endreq; 

   StatusCode sc=LArCond2NtupleBase::initialize();
   if (sc!=StatusCode::SUCCESS) {
     msg(MSG::ERROR) << "Base init failed" << endreq;
     return StatusCode::FAILURE;
   }

  sc=m_nt->addItem("IEvent",IEvent);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'IEvent' failed" << endreq;
      return sc;
    }

  sc=m_nt->addItem("Gain",gain,-1,3);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Gain' failed" << endreq;
      return sc;
    }

  sc=m_nt->addItem("Nsamples",Nsamples,0,32);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Nsamples' failed" << endreq;
      return sc;
    }

  sc=m_nt->addItem("samples",m_Nsamples,samples);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'samples' failed" << endreq;
      return sc;
    }
  

  m_ipass = 0;

  m_event=0;

  return StatusCode::SUCCESS;

}

StatusCode LArDigits2Ntuple::execute()
{

  StatusCode sc;
  
  msg(MSG::DEBUG) << "in execute" << endreq; 

  m_event++;
  unsigned long thisevent;
  const EventInfo* eventInfo;
  if (evtStore()->retrieve(eventInfo,"ByteStreamEventInfo").isFailure()) {
      msg(MSG::WARNING) << " Cannot access to event info " << endreq;
      thisevent=m_event;
  } else {
      thisevent = eventInfo->event_ID()->event_number();
  }
  
  const LArDigitContainer* m_DigitContainer = NULL;
  sc=evtStore()->retrieve(m_DigitContainer,m_contKey);  
  if (sc!=StatusCode::SUCCESS) {
     msg(MSG::WARNING) << "Unable to retrieve LArDigitContainer with key " << m_contKey << " from DetectorStore. " << endreq;
    } 
  else
     msg(MSG::DEBUG) << "Got LArDigitContainer with key " << m_contKey << endreq;
  
 
 if (m_DigitContainer) { 
   
   LArDigitContainer::const_iterator it=m_DigitContainer->begin();
   LArDigitContainer::const_iterator it_e=m_DigitContainer->end();

    if(it == it_e) {
      msg(MSG::DEBUG) << "LArDigitContainer with key=" << m_contKey << " is empty " << endreq;
      return StatusCode::SUCCESS;
    }else{
      msg(MSG::DEBUG) << "LArDigitContainer with key=" << m_contKey << " has " <<m_DigitContainer->size() << " entries" <<endreq;
    }

   unsigned cellCounter=0;
   for (;it!=it_e;it++) {

     unsigned int trueMaxSample = (*it)->nsamples();
     Nsamples = trueMaxSample;

     gain=(*it)->gain();
     if(gain < CaloGain::INVALIDGAIN || gain > CaloGain::LARNGAIN) gain=CaloGain::LARNGAIN;

     //     std::cout << "trigger = " << Ntrigger << ", samples = "<< Nsamples << std::endl;

     if(trueMaxSample>m_Nsamples){
       if(!m_ipass){
	 msg(MSG::WARNING) << "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " << endreq;
	 m_ipass=1;
       }
       trueMaxSample = m_Nsamples;
     }

     IEvent=thisevent;
     fillFromIdentifier((*it)->hardwareID());      
     if(m_FTlist.size() > 0) { // should do a selection
        if(std::find(std::begin(m_FTlist), std::end(m_FTlist), m_FT) == std::end(m_FTlist)) { // is our FT in list ?
           continue;
        }
     }

     const std::vector<short> sam = (*it)->samples();
     for(unsigned i=0; i<trueMaxSample;++i) samples[i]=sam[i];

     sc=ntupleSvc()->writeRecord(m_nt);
     if (sc!=StatusCode::SUCCESS) {
       msg(MSG::ERROR) << "writeRecord failed" << endreq;
       return sc;
     }
     cellCounter++;
   }// over cells 
 } 
 msg(MSG::DEBUG) << "LArDigits2Ntuple has finished." << endreq;
 return StatusCode::SUCCESS;
}// end finalize-method.
