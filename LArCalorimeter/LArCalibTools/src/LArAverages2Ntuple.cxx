/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAverages2Ntuple.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"

#include "GaudiKernel/ToolHandle.h"

LArAverages2Ntuple::LArAverages2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_emId(NULL), m_onlineHelper(NULL),
  larCablingSvc ("LArCablingService")
  //  m_eventCounter(0)
{
  declareProperty("ContainerKey",m_contKey);
  declareProperty("NSamples",m_Nsamples=7);
  declareProperty("KeepOnlyPulsed",m_keepPulsed=true);
  ipass=0;
}

LArAverages2Ntuple::~LArAverages2Ntuple() 
{}


StatusCode LArAverages2Ntuple::initialize()
{
  ATH_MSG_INFO ( "in initialize" );

  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  if (!m_emId) {
    ATH_MSG_ERROR ( "Could not access lar EM ID helper" );
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  // Retrieve LArCablingService
  ATH_CHECK( larCablingSvc.retrieve() );

  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1) {
    ATH_MSG_ERROR ( "Booking of NTuple failed" );
    return StatusCode::FAILURE;
  }

  ntuplePath="/NTUPLES/FILE1/AVERAGES"+m_contKey;
  const std::string ntupleTitle="Averages "+m_contKey;
  NTuplePtr nt(ntupleSvc(),ntuplePath);
  if (!nt) {
    nt=ntupleSvc()->book(ntuplePath,CLID_ColumnWiseTuple,ntupleTitle);
  }
  
  if (!nt) {
    ATH_MSG_ERROR ( "Booking of NTuple failed" );
    return StatusCode::FAILURE;
  }


  ATH_CHECK( nt->addItem("DAC",DAC,0,65535) );
  ATH_CHECK( nt->addItem("isPulsed",isPulsed,0,1) );
  ATH_CHECK( nt->addItem("delay",delay,0,240) );
  ATH_CHECK( nt->addItem("Ntrigger",Ntrigger,0,500) );
  ATH_CHECK( nt->addItem("Nsamples",Nsamples,0,32) );
  ATH_CHECK( nt->addItem("Nsteps",Nsteps,0,50) );
  ATH_CHECK( nt->addItem("StepIndex",StepIndex,0,100) );

  static const int maxSamples = m_Nsamples;
  ATH_CHECK( nt->addItem("Sum",maxSamples,Sum) );
  ATH_CHECK( nt->addItem("SumSq",maxSamples,SumSq) );
  ATH_CHECK( nt->addItem("Mean",maxSamples,Mean) );
  ATH_CHECK( nt->addItem("RMS",maxSamples,RMS) );
  ATH_CHECK( nt->addItem("Layer",layer,0,4) );
  ATH_CHECK( nt->addItem("Region",region,0,1) );
  ATH_CHECK( nt->addItem("Eta",eta,0,510) );
  ATH_CHECK( nt->addItem("Phi",phi,0,1023) );
  ATH_CHECK( nt->addItem("Slot",slot,0,127) );
  ATH_CHECK( nt->addItem("barrel_ec",barrel_ec,0,1) );
  ATH_CHECK( nt->addItem("pos_neg",pos_neg,0,1) );
  ATH_CHECK( nt->addItem("FT",FT,0,31) );
  ATH_CHECK( nt->addItem("calibLine",calibLine,0,127) );
  ATH_CHECK( nt->addItem("isConnected",m_isConnected,0,1) );
  ATH_CHECK( nt->addItem("Channel",channel,0,127) );

  return StatusCode::SUCCESS;

}

StatusCode LArAverages2Ntuple::execute()
{
  ATH_MSG_DEBUG ( "in execute" );
  
  const LArAccumulatedCalibDigitContainer* m_accuDigitContainer = NULL;
  StatusCode sc=evtStore()->retrieve(m_accuDigitContainer,m_contKey);  
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_WARNING ( "Unable to retrieve LArAccumulatedCalibDigitContainer with key " << m_contKey << " from DetectorStore. " );
    } 
  else
    ATH_MSG_DEBUG ( "Got LArAccumulatedCalibDigitContainer with key " << m_contKey );
  
 
 if (m_accuDigitContainer) { 
   
   LArAccumulatedCalibDigitContainer::const_iterator it=m_accuDigitContainer->begin();
   LArAccumulatedCalibDigitContainer::const_iterator it_e=m_accuDigitContainer->end();

    if(it == it_e) {
      ATH_MSG_DEBUG ( "LArAccumulatedCalibDigitContainer with key=" << m_contKey << " is empty " );
      return StatusCode::SUCCESS;
    }else{
      ATH_MSG_DEBUG ( "LArAccumulatedCalibDigitContainer with key=" << m_contKey << " has " <<m_accuDigitContainer->size() << " entries" );
    }

   unsigned cellCounter=0;
   for (;it!=it_e;it++) {   
     // Add protection - Modif from JF. Marchand
     if ( !(*it) ) continue;

     isPulsed = (long)(*it)->isPulsed();
     if(m_keepPulsed && !(*it)->isPulsed()) continue;
     DAC = (*it)->DAC();
     Nsteps = (*it)->nSteps();
     Ntrigger = (*it)->nTriggers();
     delay = (*it)->delay();
     StepIndex=(*it)->stepIndex();
     //unsigned int max = (Nsteps == 1) ? 1 : (Nsteps+1);
     unsigned int trueMaxSample = (*it)->nsamples();
     Nsamples = trueMaxSample;

     if(trueMaxSample>m_Nsamples){
       if(!ipass){
	 ATH_MSG_WARNING ( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
	 ipass=1;
       }
       trueMaxSample = m_Nsamples;
     }
   
     // std::cout << " ==> DAC Nsteps Ntrigger max " << DAC << " " << Nsteps << " " << Ntrigger << " " << max << " Nsamples " << trueMaxSample << "/" << (*it)->nSamples() << std::endl;

  
     const std::vector<uint32_t>& sampleSum = (*it)->sampleSum();
     const std::vector<uint32_t>& sampleSum2 = (*it)->sample2Sum();
     const std::vector<float>& mean = (*it)->mean();
     const std::vector<float>& RMSv = (*it)->RMS();

     for(unsigned int j=0;j<trueMaxSample;j++){
       Sum[j]   = sampleSum[j];
       SumSq[j] = sampleSum2[j];
       Mean[j]  = mean[j];
       RMS[j]   = RMSv[j];
       //std::cout << " i/j=" << i << "/" << j << " " << mean[j] << std::endl; 
     }

     HWIdentifier chid=(*it)->channelID();
     channel=m_onlineHelper->channel(chid);
     slot=m_onlineHelper->slot(chid);
     FT=m_onlineHelper->feedthrough(chid);
     barrel_ec = m_onlineHelper->barrel_ec(chid);
     pos_neg   = m_onlineHelper->pos_neg(chid);

     bool isConnected = larCablingSvc->isOnlineConnected(chid);
     if(isConnected){
       Identifier id=larCablingSvc->cnvToIdentifier(chid);
       const std::vector<HWIdentifier>& calibLineV=larCablingSvc->calibSlotLine(chid);
       std::vector<HWIdentifier>::const_iterator calibLineIt=calibLineV.begin();
       calibLine = m_onlineHelper->channel(*calibLineIt);
       eta=m_emId->eta(id); 
       phi=m_emId->phi(id);
       layer=m_emId->sampling(id);
       region=m_emId->region(id);
     } else {
       calibLine=-999;
       eta=-999;
       phi=-999;
       layer=-999;
       region=-999;
     }
     m_isConnected = (long)isConnected;

     ATH_CHECK( ntupleSvc()->writeRecord(ntuplePath) );
     cellCounter++;
   }//end loop over cells
 }//end if have accumulatedDigitContainer 
 ATH_MSG_DEBUG ( "LArAverages2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
