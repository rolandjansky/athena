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
   ATH_MSG_INFO( "in initialize" ); 

   StatusCode sc=LArCond2NtupleBase::initialize();
   if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_ERROR( "Base init failed" );
     return StatusCode::FAILURE;
   }



  sc=m_nt->addItem("IEvent",m_IEvent,0,3000);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'IEvent' failed" );
      return sc;
    }

  sc=m_nt->addItem("Ntrigger",m_Ntrigger,0,500); 
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Ntrigger' failed" );
      return sc;
    }
  
  sc=m_nt->addItem("Nsamples",m_ntNsamples,0,32);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Nsamples' failed" );
      return sc;
    }

  sc=m_nt->addItem("sum",m_Nsamples,m_sum);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'sum' failed" );
      return sc;
    }
  
  sc=m_nt->addItem("sumsq",m_Nsamples,m_sumsq);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'sumsq' failed" );
      return sc;
    }

  sc=m_nt->addItem("mean",m_mean);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'mean' failed" );
      return sc;
    }

  sc=m_nt->addItem("rms",m_rms);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'rms' failed" );
      return sc;
    }

  sc=m_nt->addItem("covr",m_Nsamples-1,m_covr);
  if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'covr' failed" );
      return sc;
    }


  m_ipass = 0;

  m_event=0;

  return StatusCode::SUCCESS;

}

StatusCode LArAccumulatedDigits2Ntuple::execute()
{

  StatusCode sc;
  
  ATH_MSG_DEBUG( "in execute" ); 

  m_event++;
  
  const LArAccumulatedDigitContainer* accuDigitContainer = NULL;
  sc=detStore()->retrieve(accuDigitContainer,m_contKey);  
  if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_WARNING( "Unable to retrieve LArAccumulatedDigitContainer with key " << m_contKey << " from DetectorStore. " );
    } 
  else
     ATH_MSG_DEBUG( "Got LArAccumulatedDigitContainer with key " << m_contKey );
  
 
 if (accuDigitContainer) { 
   
   LArAccumulatedDigitContainer::const_iterator it=accuDigitContainer->begin();
   LArAccumulatedDigitContainer::const_iterator it_e=accuDigitContainer->end();

    if(it == it_e) {
      ATH_MSG_DEBUG( "LArAccumulatedDigitContainer with key=" << m_contKey << " is empty " );
      return StatusCode::SUCCESS;
    }else{
      ATH_MSG_DEBUG( "LArAccumulatedDigitContainer with key=" << m_contKey << " has " <<accuDigitContainer->size() << " entries" );
    }

   unsigned cellCounter=0;
   for (;it!=it_e;it++) {

     m_IEvent=m_event;
     m_Ntrigger = (*it)->nTrigger();
     unsigned int trueMaxSample = (*it)->nsample();
     m_ntNsamples = trueMaxSample;

     //     std::cout << "trigger = " << m_Ntrigger << ", samples = "<< m_ntNsamples << std::endl;

     if(trueMaxSample>m_Nsamples){
       if(!m_ipass){
	 ATH_MSG_WARNING( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
	 m_ipass=1;
       }
       trueMaxSample = m_Nsamples;
     }

     m_mean = (*it)->mean();
     m_rms  = (*it)->RMS();
     const std::vector<uint64_t> sampleSquare = (*it)->sampleSquare();
     const std::vector<uint64_t> sampleSum    = (*it)->sampleSum();
     for(unsigned i=0;i<trueMaxSample;i++) {
       m_sumsq[i] = sampleSquare[i];
       m_sum[i]   = sampleSum[i];
     }
     std::vector<float> cov;
     (*it)->getCov(cov,m_normalize);
     for(unsigned i=0;i<trueMaxSample-1;i++) {
       m_covr[i] = cov[i];
     }

     fillFromIdentifier((*it)->hardwareID());      
     sc=ntupleSvc()->writeRecord(m_nt);
     if (sc!=StatusCode::SUCCESS) {
       ATH_MSG_ERROR( "writeRecord failed" );
       return sc;
     }
     cellCounter++;
   } 
 } 
 ATH_MSG_DEBUG( "LArAccumulatedDigits2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
