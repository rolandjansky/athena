/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAccumulatedDigits2Ntuple.h"

#include "LArRawEvent/LArAccumulatedDigitContainer.h"

//#include "GaudiKernel/ToolHandle.h"

LArAccumulatedDigits2Ntuple::LArAccumulatedDigits2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator),
  m_storeGateSvc(0),
  m_emId(0),
  m_onlineHelper(0),
  m_larCablingSvc ("LArCablingService"),
  m_ipass(0),
  m_event(0)
{
  declareProperty("ContainerKey",m_contKey);
  declareProperty("NSamples",m_Nsamples=7);
  declareProperty("Normalize",m_normalize=1);
}

LArAccumulatedDigits2Ntuple::~LArAccumulatedDigits2Ntuple() 
{}


StatusCode LArAccumulatedDigits2Ntuple::initialize()
{

  MsgStream log(msgSvc(), name());
  StatusCode sc;

  log << MSG::INFO << "in initialize" << endreq; 

  sc= service("StoreGateSvc",m_storeGateSvc);
  if(sc.isFailure()) {
    log << MSG::ERROR << "StoreGate service not found" << endreq;
    return StatusCode::FAILURE;
  }

  StoreGateSvc* detStore = 0;
  sc=service("DetectorStore",detStore);
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
  
  sc = detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return sc;
  }  

  // Retrieve LArCablingService
  sc = m_larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << " Can't get LArCablingSvc " << endreq;
    return sc;
  }

  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1) {
    log << MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE;
  }

  m_ntuplePath="/NTUPLES/FILE1/ACCUMULATEDDIGITS"+m_contKey;
  const std::string ntupleTitle="AccumulatedDigits "+m_contKey;
  NTuplePtr nt(ntupleSvc(),m_ntuplePath);
  if (!nt) {
    nt=ntupleSvc()->book(m_ntuplePath,CLID_ColumnWiseTuple,ntupleTitle);
  }
  
  if (!nt) {
    log << MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE;
  }

  sc=nt->addItem("IEvent",IEvent,0,3000);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'IEvent' failed" << endreq;
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

  sc=nt->addItem("sum",m_Nsamples,sum);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'sum' failed" << endreq;
      return StatusCode::FAILURE;
    }
  
  sc=nt->addItem("sumsq",m_Nsamples,sumsq);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'sumsq' failed" << endreq;
      return StatusCode::FAILURE;
    }

  sc=nt->addItem("mean",mean);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'mean' failed" << endreq;
      return StatusCode::FAILURE;
    }

  sc=nt->addItem("rms",rms);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'rms' failed" << endreq;
      return StatusCode::FAILURE;
    }

  sc=nt->addItem("covr",m_Nsamples-1,covr);
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

  m_ipass = 0;

  m_event=0;

  return StatusCode::SUCCESS;

}

StatusCode LArAccumulatedDigits2Ntuple::execute()
{

  MsgStream log(msgSvc(), name());
  StatusCode sc;
  
  log << MSG::DEBUG << "in execute" << endreq; 

  m_event++;
  
  const LArAccumulatedDigitContainer* m_accuDigitContainer = NULL;
  sc=m_storeGateSvc->retrieve(m_accuDigitContainer,m_contKey);  
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::WARNING << "Unable to retrieve LArAccumulatedDigitContainer with key " << m_contKey << " from DetectorStore. " << endreq;
    } 
  else
    log << MSG::DEBUG << "Got LArAccumulatedDigitContainer with key " << m_contKey << endreq;
  
 
 if (m_accuDigitContainer) { 
   
   LArAccumulatedDigitContainer::const_iterator it=m_accuDigitContainer->begin();
   LArAccumulatedDigitContainer::const_iterator it_e=m_accuDigitContainer->end();

    if(it == it_e) {
      log << MSG::DEBUG << "LArAccumulatedDigitContainer with key=" << m_contKey << " is empty " << endreq;
      return StatusCode::SUCCESS;
    }else{
      log << MSG::DEBUG << "LArAccumulatedDigitContainer with key=" << m_contKey << " has " <<m_accuDigitContainer->size() << " entries" <<endreq;
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
	 log << MSG::WARNING << "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " << endreq;
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

     //     std::cout << "sum = "<< sum << "mean = "<< mean << ", rms = "<< rms << ", sumsq = "<< sumsq[0] << ", cov = "<< covr[0] << std::endl;
     HWIdentifier chid=(*it)->channelID();
     channel=m_onlineHelper->channel(chid);
     slot=m_onlineHelper->slot(chid);
     FT=m_onlineHelper->feedthrough(chid);
     barrel_ec = m_onlineHelper->barrel_ec(chid);
     pos_neg   = m_onlineHelper->pos_neg(chid);

     //     std::cout << "channel = "<< channel << ", slot = "<< slot << ", FT = "<< FT << std::endl;

     bool isConnected = m_larCablingSvc->isOnlineConnected(chid);
     if(isConnected){
       Identifier id=m_larCablingSvc->cnvToIdentifier(chid);
       const std::vector<HWIdentifier>& calibLineV=m_larCablingSvc->calibSlotLine(chid);
       std::vector<HWIdentifier>::const_iterator calibLineIt=calibLineV.begin();
       calibLine = m_onlineHelper->channel(*calibLineIt);
       eta=m_emId->eta(id); 
       phi=m_emId->phi(id);
       layer=m_emId->sampling(id);
       region=m_emId->region(id);
     }else{
       calibLine=-999;
       eta=-999;
       phi=-999;
       layer=-999;
       region=-999;
     }
     m_isConnected = (long)isConnected;

     //     std::cout << "calibLine =  "<< calibLine << ", eta = "<< eta << ", phi = "<< phi << std::endl;
     //     std::cout << "will write record " << cellCounter << ", sum = "<< sum << ", mean = "<< mean << std::endl;
     sc=ntupleSvc()->writeRecord(m_ntuplePath);
     if (sc!=StatusCode::SUCCESS) {
       log << MSG::ERROR << "writeRecord failed" << endreq;
       return StatusCode::FAILURE;
     }
     cellCounter++;
   } 
 } 
 log << MSG::DEBUG << "LArAccumulatedDigits2Ntuple has finished." << endreq;
 return StatusCode::SUCCESS;
}// end finalize-method.
