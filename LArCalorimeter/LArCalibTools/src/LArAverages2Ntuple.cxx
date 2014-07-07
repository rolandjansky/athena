/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAverages2Ntuple.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"

#include "GaudiKernel/ToolHandle.h"

LArAverages2Ntuple::LArAverages2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  Algorithm(name, pSvcLocator),
  m_storeGateSvc(NULL),m_detStore(NULL),m_emId(NULL), m_onlineHelper(NULL),
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

  MsgStream log(msgSvc(), name());
  StatusCode sc;

  log << MSG::INFO << "in initialize" << endreq; 

  sc= service("StoreGateSvc",m_storeGateSvc);
  if(sc.isFailure()) {
    log << MSG::ERROR << "StoreGate service not found" << endreq;
    return StatusCode::FAILURE;
  }

  sc=service("DetectorStore",m_detStore);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }
  
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  if (!m_emId) {
    log << MSG::ERROR << "Could not access lar EM ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = m_detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return sc;
  }  

  // Retrieve LArCablingService
  sc = larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << " Can't get LArCablingSvc " << endreq;
    return sc;
  }

  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1) {
    log << MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE;
  }

  ntuplePath="/NTUPLES/FILE1/AVERAGES"+m_contKey;
  const std::string ntupleTitle="Averages "+m_contKey;
  NTuplePtr nt(ntupleSvc(),ntuplePath);
  if (!nt) {
    nt=ntupleSvc()->book(ntuplePath,CLID_ColumnWiseTuple,ntupleTitle);
  }
  
  if (!nt) {
    log << MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE;
  }


  sc=nt->addItem("DAC",DAC,0,65535);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'DAC' failed" << endreq;
      return StatusCode::FAILURE;
    }

  sc=nt->addItem("isPulsed",isPulsed,0,1);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'isPulsed' failed" << endreq;
      return StatusCode::FAILURE;
    }

  sc=nt->addItem("delay",delay,0,240);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'delay' failed" << endreq;
      return StatusCode::FAILURE;
    }
  

  sc=nt->addItem("Ntrigger",Ntrigger,0,500);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Ntrigger' failed" << endreq;
      return StatusCode::FAILURE;
    }
  

  sc=nt->addItem("Nsamples",Nsamples,0,32);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Nsamples' failed" << endreq;
      return StatusCode::FAILURE;
    }
  

  sc=nt->addItem("Nsteps",Nsteps,0,50);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Nsteps' failed" << endreq;
      return StatusCode::FAILURE;
    }
  

  sc=nt->addItem("StepIndex",StepIndex,0,100);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'StepIndex' failed" << endreq;
      return StatusCode::FAILURE;
    }

  static const int maxSamples = m_Nsamples;
  sc=nt->addItem("Sum",maxSamples,Sum);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Sum' failed" << endreq;
      return StatusCode::FAILURE;
    }

  sc=nt->addItem("SumSq",maxSamples,SumSq);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'SumSq' failed" << endreq;
      return StatusCode::FAILURE;
    }

  sc=nt->addItem("Mean",maxSamples,Mean);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Mean' failed" << endreq;
      return StatusCode::FAILURE;
    }

  sc=nt->addItem("RMS",maxSamples,RMS);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'RMS' failed" << endreq;
      return StatusCode::FAILURE;
    }

  sc=nt->addItem("Layer",layer,0,4);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Layer' failed" << endreq;
      return StatusCode::FAILURE;
    }
  
  sc=nt->addItem("Region",region,0,1);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Region' failed" << endreq;
      return StatusCode::FAILURE;
    }
  
  sc=nt->addItem("Eta",eta,0,510);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Eta' failed" << endreq;
      return StatusCode::FAILURE;
    }
  
  sc=nt->addItem("Phi",phi,0,1023);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Phi' failed" << endreq;
      return StatusCode::FAILURE;
    }
  
  sc=nt->addItem("Slot",slot,0,127);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Slot' failed" << endreq;
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

  sc=nt->addItem("FT",FT,0,31);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'FT' failed" << endreq;
      return StatusCode::FAILURE;
    }
  
  sc=nt->addItem("calibLine",calibLine,0,127);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'calibLine' failed" << endreq;
      return StatusCode::FAILURE;
    }
  
  sc=nt->addItem("isConnected",m_isConnected,0,1);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem 'isConnected' failed" << endreq;
    return StatusCode::FAILURE;
  }

  sc=nt->addItem("Channel",channel,0,127);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Channel' failed" << endreq;
      return StatusCode::FAILURE;
    }

  //ipass = 0;

  return StatusCode::SUCCESS;

}

StatusCode LArAverages2Ntuple::execute()
{

  MsgStream log(msgSvc(), name());
  StatusCode sc;
  
  log << MSG::DEBUG << "in execute" << endreq; 
  
  const LArAccumulatedCalibDigitContainer* m_accuDigitContainer = NULL;
  sc=m_storeGateSvc->retrieve(m_accuDigitContainer,m_contKey);  
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::WARNING << "Unable to retrieve LArAccumulatedCalibDigitContainer with key " << m_contKey << " from DetectorStore. " << endreq;
    } 
  else
    log << MSG::DEBUG << "Got LArAccumulatedCalibDigitContainer with key " << m_contKey << endreq;
  
 
 if (m_accuDigitContainer) { 
   
   LArAccumulatedCalibDigitContainer::const_iterator it=m_accuDigitContainer->begin();
   LArAccumulatedCalibDigitContainer::const_iterator it_e=m_accuDigitContainer->end();

    if(it == it_e) {
      log << MSG::DEBUG << "LArAccumulatedCalibDigitContainer with key=" << m_contKey << " is empty " << endreq;
      return StatusCode::SUCCESS;
    }else{
      log << MSG::DEBUG << "LArAccumulatedCalibDigitContainer with key=" << m_contKey << " has " <<m_accuDigitContainer->size() << " entries" <<endreq;
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
	 log << MSG::WARNING << "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " << endreq;
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

     sc=ntupleSvc()->writeRecord(ntuplePath);
     if (sc!=StatusCode::SUCCESS) {
       log << MSG::ERROR << "writeRecord failed" << endreq;
       return StatusCode::FAILURE;
     }
     cellCounter++;
   }//end loop over cells
 }//end if have accumulatedDigitContainer 
 log << MSG::DEBUG << "LArAverages2Ntuple has finished." << endreq;
 return StatusCode::SUCCESS;
}// end finalize-method.
