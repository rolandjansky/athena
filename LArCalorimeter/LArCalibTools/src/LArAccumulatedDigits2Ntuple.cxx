/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAccumulatedDigits2Ntuple.h"

#include "LArRawEvent/LArAccumulatedDigitContainer.h"

//#include "GaudiKernel/ToolHandle.h"

LArAccumulatedDigits2Ntuple::LArAccumulatedDigits2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
  LArCond2NtupleBase(name, pSvcLocator),
  m_ipass(0),
  m_event(0)
{
  declareProperty("ContainerKey",m_contKey);
  declareProperty("NSamples",m_Nsamples=7);
  declareProperty("Normalize",m_normalize=1);
  m_ntTitle="AccumulatedDigits";
  m_ntpath="/NTUPLES/FILE1/ACCUMULATEDDIGITS"+m_contKey;

}

LArAccumulatedDigits2Ntuple::~LArAccumulatedDigits2Ntuple() 
{}


StatusCode LArAccumulatedDigits2Ntuple::initialize()
{
   msg(MSG::INFO) << "in initialize" << endreq; 

   StatusCode sc=LArCond2NtupleBase::initialize();
   if (sc!=StatusCode::SUCCESS) {
     msg(MSG::ERROR) << "Base init failed" << endreq;
     return StatusCode::FAILURE;
   }



  sc=m_nt->addItem("IEvent",IEvent,0,3000);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'IEvent' failed" << endreq;
      return sc;
    }

  sc=m_nt->addItem("Ntrigger",Ntrigger,0,500); 
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Ntrigger' failed" << endreq;
      return sc;
    }
  
  sc=m_nt->addItem("Nsamples",Nsamples,0,32);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Nsamples' failed" << endreq;
      return sc;
    }

  sc=m_nt->addItem("sum",m_Nsamples,sum);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'sum' failed" << endreq;
      return sc;
    }
  
  sc=m_nt->addItem("sumsq",m_Nsamples,sumsq);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'sumsq' failed" << endreq;
      return sc;
    }

  sc=m_nt->addItem("mean",mean);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'mean' failed" << endreq;
      return sc;
    }

  sc=m_nt->addItem("rms",rms);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'rms' failed" << endreq;
      return sc;
    }

  sc=m_nt->addItem("covr",m_Nsamples-1,covr);
  if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'covr' failed" << endreq;
      return sc;
    }


  m_ipass = 0;

  m_event=0;

  return StatusCode::SUCCESS;

}

StatusCode LArAccumulatedDigits2Ntuple::execute()
{

  StatusCode sc;
  
  msg(MSG::DEBUG) << "in execute" << endreq; 

  m_event++;
  
  const LArAccumulatedDigitContainer* m_accuDigitContainer = NULL;
  sc=detStore()->retrieve(m_accuDigitContainer,m_contKey);  
  if (sc!=StatusCode::SUCCESS) {
     msg(MSG::WARNING) << "Unable to retrieve LArAccumulatedDigitContainer with key " << m_contKey << " from DetectorStore. " << endreq;
    } 
  else
     msg(MSG::DEBUG) << "Got LArAccumulatedDigitContainer with key " << m_contKey << endreq;
  
 
 if (m_accuDigitContainer) { 
   
   LArAccumulatedDigitContainer::const_iterator it=m_accuDigitContainer->begin();
   LArAccumulatedDigitContainer::const_iterator it_e=m_accuDigitContainer->end();

    if(it == it_e) {
      msg(MSG::DEBUG) << "LArAccumulatedDigitContainer with key=" << m_contKey << " is empty " << endreq;
      return StatusCode::SUCCESS;
    }else{
      msg(MSG::DEBUG) << "LArAccumulatedDigitContainer with key=" << m_contKey << " has " <<m_accuDigitContainer->size() << " entries" <<endreq;
    }

   unsigned cellCounter=0;
   for (;it!=it_e;it++) {

     IEvent=m_event;
     Ntrigger = (*it)->nTrigger();
     unsigned int trueMaxSample = (*it)->nsample();
     Nsamples = trueMaxSample;

     //     std::cout << "trigger = " << Ntrigger << ", samples = "<< Nsamples << std::endl;

     if(trueMaxSample>m_Nsamples){
       if(!m_ipass){
	 msg(MSG::WARNING) << "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " << endreq;
	 m_ipass=1;
       }
       trueMaxSample = m_Nsamples;
     }

     mean = (*it)->mean();
     rms  = (*it)->RMS();
     const std::vector<uint64_t> sampleSquare = (*it)->sampleSquare();
     const std::vector<uint64_t> sampleSum    = (*it)->sampleSum();
     for(unsigned i=0;i<trueMaxSample;i++) {
       sumsq[i] = sampleSquare[i];
       sum[i]   = sampleSum[i];
     }
     std::vector<float> cov;
     (*it)->getCov(cov,m_normalize);
     for(unsigned i=0;i<trueMaxSample-1;i++) {
       covr[i] = cov[i];
     }

     fillFromIdentifier((*it)->hardwareID());      
     sc=ntupleSvc()->writeRecord(m_nt);
     if (sc!=StatusCode::SUCCESS) {
       msg(MSG::ERROR) << "writeRecord failed" << endreq;
       return sc;
     }
     cellCounter++;
   } 
 } 
 msg(MSG::DEBUG) << "LArAccumulatedDigits2Ntuple has finished." << endreq;
 return StatusCode::SUCCESS;
}// end finalize-method.
