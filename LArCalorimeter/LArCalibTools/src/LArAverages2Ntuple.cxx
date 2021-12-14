/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAverages2Ntuple.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"

LArAverages2Ntuple::LArAverages2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator)
{
  declareProperty("ContainerKey",m_contKey);
  declareProperty("NSamples",m_Nsamples=50);
  declareProperty("KeepOnlyPulsed",m_keepPulsed=true);
  m_ipass=0;
}

LArAverages2Ntuple::~LArAverages2Ntuple() 
{}


StatusCode LArAverages2Ntuple::initialize()
{
  ATH_MSG_INFO ( "in initialize" );
  m_ntName = "AVERAGES";
  m_ntTitle="Averages";
  m_ntpath=std::string("/NTUPLES/FILE1/")+m_ntName+m_contKey;

  ATH_CHECK(LArCond2NtupleBase::initialize());

  ATH_CHECK( m_nt->addItem("DAC",m_DAC,0,65535) );
  ATH_CHECK( m_nt->addItem("isPulsed",m_isPulsed,0,1) );
  ATH_CHECK( m_nt->addItem("delay",m_delay,0,240) );
  ATH_CHECK( m_nt->addItem("Ntrigger",m_Ntrigger,0,500) );
  ATH_CHECK( m_nt->addItem("Nsamples",m_ntNsamples,0,32) );
  ATH_CHECK( m_nt->addItem("Nsteps",m_Nsteps,0,50) );
  ATH_CHECK( m_nt->addItem("StepIndex",m_StepIndex,0,100) );

  static const int maxSamples = m_Nsamples;
  ATH_CHECK( m_nt->addItem("Sum",maxSamples,m_Sum) );
  ATH_CHECK( m_nt->addItem("SumSq",maxSamples,m_SumSq) );
  ATH_CHECK( m_nt->addItem("Mean",maxSamples,m_Mean) );
  ATH_CHECK( m_nt->addItem("RMS",maxSamples,m_RMS) );

  return StatusCode::SUCCESS;

}

StatusCode LArAverages2Ntuple::execute()
{
  ATH_MSG_DEBUG ( "in execute" );
  
  const LArAccumulatedCalibDigitContainer* accuDigitContainer = NULL;
  StatusCode sc=evtStore()->retrieve(accuDigitContainer,m_contKey);  
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_WARNING ( "Unable to retrieve LArAccumulatedCalibDigitContainer with key " << m_contKey << " from DetectorStore. " );
    } 
  else
    ATH_MSG_DEBUG ( "Got LArAccumulatedCalibDigitContainer with key " << m_contKey );
  
 
 if (accuDigitContainer) { 
   
   LArAccumulatedCalibDigitContainer::const_iterator it=accuDigitContainer->begin();
   LArAccumulatedCalibDigitContainer::const_iterator it_e=accuDigitContainer->end();

    if(it == it_e) {
      ATH_MSG_DEBUG ( "LArAccumulatedCalibDigitContainer with key=" << m_contKey << " is empty " );
      return StatusCode::SUCCESS;
    }else{
      ATH_MSG_DEBUG ( "LArAccumulatedCalibDigitContainer with key=" << m_contKey << " has " <<accuDigitContainer->size() << " entries" );
    }

   unsigned cellCounter=0;
   for (;it!=it_e;++it) {   
     // Add protection - Modif from JF. Marchand
     if ( !(*it) ) continue;

     HWIdentifier chid=(*it)->channelID();
     m_isPulsed = (long)(*it)->isPulsed();
     if(m_keepPulsed && !(*it)->isPulsed()) continue;
     m_DAC = (*it)->DAC();
     m_Nsteps = (*it)->nSteps();
     m_Ntrigger = (*it)->nTriggers();
     m_delay = (*it)->delay();
     m_StepIndex=(*it)->stepIndex();
     unsigned int trueMaxSample = (*it)->nsamples();
     m_ntNsamples = trueMaxSample;

     if(trueMaxSample>m_Nsamples){
       if(!m_ipass){
	 ATH_MSG_WARNING ( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
	 m_ipass=1;
       }
       trueMaxSample = m_Nsamples;
     }
   
  
     const std::vector<uint64_t>& sampleSum = (*it)->sampleSum();
     const std::vector<uint64_t>& sampleSum2 = (*it)->sample2Sum();
     const std::vector<float>& mean = (*it)->mean();
     const std::vector<float>& RMSv = (*it)->RMS();

     for(unsigned int j=0;j<trueMaxSample;j++){
       m_Sum[j]   = sampleSum[j];
       m_SumSq[j] = sampleSum2[j];
       m_Mean[j]  = mean[j];
       m_RMS[j]   = RMSv[j];
     }

     fillFromIdentifier(chid);       
     ATH_CHECK( ntupleSvc()->writeRecord(m_nt) );
     cellCounter++;
   }//end loop over cells
 }//end if have accumulatedDigitContainer 
 ATH_MSG_DEBUG ( "LArAverages2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
