/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAverages2Ntuple.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"

#include "GaudiKernel/ToolHandle.h"

LArAverages2Ntuple::LArAverages2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_emId(NULL), m_onlineHelper(NULL)
  //  m_eventCounter(0)
{
  declareProperty("ContainerKey",m_contKey);
  declareProperty("NSamples",m_Nsamples=7);
  declareProperty("KeepOnlyPulsed",m_keepPulsed=true);
  m_ipass=0;
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

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_calibMapKey.initialize() );

  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1) {
    ATH_MSG_ERROR ( "Booking of NTuple failed" );
    return StatusCode::FAILURE;
  }

  m_ntuplePath="/NTUPLES/FILE1/AVERAGES"+m_contKey;
  const std::string ntupleTitle="Averages "+m_contKey;
  NTuplePtr nt(ntupleSvc(),m_ntuplePath);
  if (!nt) {
    nt=ntupleSvc()->book(m_ntuplePath,CLID_ColumnWiseTuple,ntupleTitle);
  }
  
  if (!nt) {
    ATH_MSG_ERROR ( "Booking of NTuple failed" );
    return StatusCode::FAILURE;
  }


  ATH_CHECK( nt->addItem("DAC",m_DAC,0,65535) );
  ATH_CHECK( nt->addItem("isPulsed",m_isPulsed,0,1) );
  ATH_CHECK( nt->addItem("delay",m_delay,0,240) );
  ATH_CHECK( nt->addItem("Ntrigger",m_Ntrigger,0,500) );
  ATH_CHECK( nt->addItem("Nsamples",m_ntNsamples,0,32) );
  ATH_CHECK( nt->addItem("Nsteps",m_Nsteps,0,50) );
  ATH_CHECK( nt->addItem("StepIndex",m_StepIndex,0,100) );

  static const int maxSamples = m_Nsamples;
  ATH_CHECK( nt->addItem("Sum",maxSamples,m_Sum) );
  ATH_CHECK( nt->addItem("SumSq",maxSamples,m_SumSq) );
  ATH_CHECK( nt->addItem("Mean",maxSamples,m_Mean) );
  ATH_CHECK( nt->addItem("RMS",maxSamples,m_RMS) );
  ATH_CHECK( nt->addItem("Layer",m_layer,0,4) );
  ATH_CHECK( nt->addItem("Region",m_region,0,1) );
  ATH_CHECK( nt->addItem("Eta",m_eta,0,510) );
  ATH_CHECK( nt->addItem("Phi",m_phi,0,1023) );
  ATH_CHECK( nt->addItem("Slot",m_slot,0,127) );
  ATH_CHECK( nt->addItem("barrel_ec",m_barrel_ec,0,1) );
  ATH_CHECK( nt->addItem("pos_neg",m_pos_neg,0,1) );
  ATH_CHECK( nt->addItem("FT",m_FT,0,31) );
  ATH_CHECK( nt->addItem("calibLine",m_calibLine,0,127) );
  ATH_CHECK( nt->addItem("isConnected",m_isConnected,0,1) );
  ATH_CHECK( nt->addItem("Channel",m_channel,0,127) );

  return StatusCode::SUCCESS;

}

StatusCode LArAverages2Ntuple::execute()
{
  ATH_MSG_DEBUG ( "in execute" );

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "DO not have cabling from the key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }
  SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_calibMapKey};
  const LArCalibLineMapping *clcabling {*clHdl};
  if(!clcabling) {
     ATH_MSG_ERROR( "Do not have calib line mapping !!!" );
     return StatusCode::FAILURE;
  }
  
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
   for (;it!=it_e;it++) {   
     // Add protection - Modif from JF. Marchand
     if ( !(*it) ) continue;

     m_isPulsed = (long)(*it)->isPulsed();
     if(m_keepPulsed && !(*it)->isPulsed()) continue;
     m_DAC = (*it)->DAC();
     m_Nsteps = (*it)->nSteps();
     m_Ntrigger = (*it)->nTriggers();
     m_delay = (*it)->delay();
     m_StepIndex=(*it)->stepIndex();
     //unsigned int max = (m_Nsteps == 1) ? 1 : (m_Nsteps+1);
     unsigned int trueMaxSample = (*it)->nsamples();
     m_ntNsamples = trueMaxSample;

     if(trueMaxSample>m_Nsamples){
       if(!m_ipass){
	 ATH_MSG_WARNING ( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
	 m_ipass=1;
       }
       trueMaxSample = m_Nsamples;
     }
   
     // std::cout << " ==> DAC Nsteps Ntrigger max " << m_DAC << " " << m_Nsteps << " " << m_Ntrigger << " " << max << " m_ntNsamples " << trueMaxSample << "/" << (*it)->nSamples() << std::endl;

  
     const std::vector<uint32_t>& sampleSum = (*it)->sampleSum();
     const std::vector<uint32_t>& sampleSum2 = (*it)->sample2Sum();
     const std::vector<float>& mean = (*it)->mean();
     const std::vector<float>& RMSv = (*it)->RMS();

     for(unsigned int j=0;j<trueMaxSample;j++){
       m_Sum[j]   = sampleSum[j];
       m_SumSq[j] = sampleSum2[j];
       m_Mean[j]  = mean[j];
       m_RMS[j]   = RMSv[j];
       //std::cout << " i/j=" << i << "/" << j << " " << mean[j] << std::endl; 
     }

     HWIdentifier chid=(*it)->channelID();
     m_channel=m_onlineHelper->channel(chid);
     m_slot=m_onlineHelper->slot(chid);
     m_FT=m_onlineHelper->feedthrough(chid);
     m_barrel_ec = m_onlineHelper->barrel_ec(chid);
     m_pos_neg   = m_onlineHelper->pos_neg(chid);

     bool isConnected = cabling->isOnlineConnected(chid);
     if(isConnected){
       Identifier id=cabling->cnvToIdentifier(chid);
       const std::vector<HWIdentifier>& calibLineV=clcabling->calibSlotLine(chid);
       std::vector<HWIdentifier>::const_iterator calibLineIt=calibLineV.begin();
       m_calibLine = m_onlineHelper->channel(*calibLineIt);
       m_eta=m_emId->eta(id); 
       m_phi=m_emId->phi(id);
       m_layer=m_emId->sampling(id);
       m_region=m_emId->region(id);
     } else {
       m_calibLine=-999;
       m_eta=-999;
       m_phi=-999;
       m_layer=-999;
       m_region=-999;
     }
     m_isConnected = (long)isConnected;

     ATH_CHECK( ntupleSvc()->writeRecord(m_ntuplePath) );
     cellCounter++;
   }//end loop over cells
 }//end if have accumulatedDigitContainer 
 ATH_MSG_DEBUG ( "LArAverages2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
