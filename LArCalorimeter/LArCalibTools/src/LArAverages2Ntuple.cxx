/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAverages2Ntuple.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

LArAverages2Ntuple::LArAverages2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator)
{
  declareProperty("ContainerKey",m_contKey);
  declareProperty("NSamples",m_Nsamples=50);
  declareProperty("KeepOnlyPulsed",m_keepPulsed=true);
  declareProperty("KeepFT",m_keepFT);
  m_ipass=0;
}

LArAverages2Ntuple::~LArAverages2Ntuple() 
{}


StatusCode LArAverages2Ntuple::initialize()
{
  ATH_MSG_INFO ( "in initialize" );

  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  
  StatusCode sc;
  if ( m_isSC ){
    const LArOnline_SuperCellID* ll;
    sc = detStore()->retrieve(ll, "LArOnline_SuperCellID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
      return StatusCode::FAILURE;
    }
    else {
      m_onlineHelper = dynamic_cast<const LArOnlineID_Base*>(ll);
      ATH_MSG_DEBUG("Found the LArOnlineID helper");
    }
  } else { // m_isSC
    const LArOnlineID* ll;
    sc = detStore()->retrieve(ll, "LArOnlineID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
      return StatusCode::FAILURE;
    }
    else {
      m_onlineHelper = dynamic_cast<const LArOnlineID_Base*>(ll);
      ATH_MSG_DEBUG(" Found the LArOnlineID helper. ");
    }
  }
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
  
  const EventContext& ctx = Gaudi::Hive::currentContext();

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{cablingKey(), ctx};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "DO not have cabling from the key " << cablingKey().key() );
     return StatusCode::FAILURE;
  }
  SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_calibMapKey, ctx};
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
     
       const std::vector<unsigned long>& sampleSum = (*it)->sampleSum();
       const std::vector<unsigned long>& sampleSum2 = (*it)->sample2Sum();
       const std::vector<float>& mean = (*it)->mean();
       const std::vector<float>& RMSv = (*it)->RMS();
 
       for(unsigned int j=0;j<trueMaxSample;j++){
         m_Sum[j]   = sampleSum[j];
         m_SumSq[j] = sampleSum2[j];
         m_Mean[j]  = mean[j];
         m_RMS[j]   = RMSv[j];
       }
 
       m_onlChanId = chid.get_identifier32().get_compact();
       m_channel=m_onlineHelper->channel(chid);
       m_slot=m_onlineHelper->slot(chid);
       m_FT=m_onlineHelper->feedthrough(chid);
       if(m_keepFT.size() > 0) {
          if(std::find(std::begin(m_keepFT), std::end(m_keepFT), m_FT) == std::end(m_keepFT)) continue;
       }
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
 
       fillFromIdentifier(chid);       
       ATH_CHECK( ntupleSvc()->writeRecord(m_nt) );
       cellCounter++;
     }//end loop over cells
  }//end if have accumulatedDigitContainer 
  ATH_MSG_DEBUG ( "LArAverages2Ntuple has finished." );
  return StatusCode::SUCCESS;
}// end execute method.
