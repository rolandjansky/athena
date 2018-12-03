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
   ATH_MSG_INFO( "in initialize" ); 

   StatusCode sc=LArCond2NtupleBase::initialize();
   if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_ERROR( "Base init failed" );
     return StatusCode::FAILURE;
   }

  sc=m_nt->addItem("IEvent",m_IEvent);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'IEvent' failed" );
      return sc;
    }

  sc=m_nt->addItem("Gain",m_gain,-1,3);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Gain' failed" );
      return sc;
    }

  sc=m_nt->addItem("Nsamples",m_ntNsamples,0,32);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Nsamples' failed" );
      return sc;
    }

  sc=m_nt->addItem("samples",m_Nsamples,m_samples);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'samples' failed" );
      return sc;
    }
  

  m_ipass = 0;

  m_event=0;

  return StatusCode::SUCCESS;

}

StatusCode LArDigits2Ntuple::execute()
{

  StatusCode sc;
  
  ATH_MSG_DEBUG( "in execute" ); 

  m_event++;
  unsigned long thisevent;
  const EventInfo* eventInfo;
  if (evtStore()->retrieve(eventInfo,"ByteStreamEventInfo").isFailure()) {
      ATH_MSG_WARNING( " Cannot access to event info " );
      thisevent=m_event;
  } else {
      thisevent = eventInfo->event_ID()->event_number();
  }
  
  const LArDigitContainer* DigitContainer = NULL;
  sc=evtStore()->retrieve(DigitContainer,m_contKey);  
  if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_WARNING( "Unable to retrieve LArDigitContainer with key " << m_contKey << " from DetectorStore. " );
    } 
  else
     ATH_MSG_DEBUG( "Got LArDigitContainer with key " << m_contKey );
  
 
 if (DigitContainer) { 
   
   LArDigitContainer::const_iterator it=DigitContainer->begin();
   LArDigitContainer::const_iterator it_e=DigitContainer->end();

    if(it == it_e) {
      ATH_MSG_DEBUG( "LArDigitContainer with key=" << m_contKey << " is empty " );
      return StatusCode::SUCCESS;
    }else{
      ATH_MSG_DEBUG( "LArDigitContainer with key=" << m_contKey << " has " <<DigitContainer->size() << " entries" );
    }

   unsigned cellCounter=0;
   for (;it!=it_e;it++) {

     unsigned int trueMaxSample = (*it)->nsamples();
     m_ntNsamples = trueMaxSample;

     m_gain=(*it)->gain();
     if(m_gain < CaloGain::INVALIDGAIN || m_gain > CaloGain::LARNGAIN) m_gain=CaloGain::LARNGAIN;

     //     std::cout << "trigger = " << Ntrigger << ", m_samples = "<< m_ntNsamples << std::endl;

     if(trueMaxSample>m_Nsamples){
       if(!m_ipass){
	 ATH_MSG_WARNING( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
	 m_ipass=1;
       }
       trueMaxSample = m_Nsamples;
     }

     m_IEvent=thisevent;
     fillFromIdentifier((*it)->hardwareID());      
     if(m_FTlist.size() > 0) { // should do a selection
        if(std::find(std::begin(m_FTlist), std::end(m_FTlist), m_FT) == std::end(m_FTlist)) { // is our FT in list ?
           continue;
        }
     }

     const std::vector<short> sam = (*it)->samples();
     for(unsigned i=0; i<trueMaxSample;++i) m_samples[i]=sam[i];

     sc=ntupleSvc()->writeRecord(m_nt);
     if (sc!=StatusCode::SUCCESS) {
       ATH_MSG_ERROR( "writeRecord failed" );
       return sc;
     }
     cellCounter++;
   }// over cells 
 } 
 ATH_MSG_DEBUG( "LArDigits2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
