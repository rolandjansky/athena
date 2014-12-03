/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBCnv/ReadTBLArDigits.h"
#include "GaudiKernel/AlgFactory.h"
#include "LArRawEvent/LArDigit.h"
#include <vector>
#include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloIdManager.h"

ReadTBLArDigits::SortDigits::SortDigits(const LArOnlineID* onlineHelper)
{
  m_onlineHelper = onlineHelper;
}

ReadTBLArDigits::ReadTBLArDigits(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_count(0),
    m_larCablingSvc(nullptr),
    m_emId(0),
    m_fcalId(0),
    m_hecId(0),
    m_onlineHelper(0),
    m_ntuplePtr(0)
{
 declareProperty("ContainerKey",m_containerKey="");
 declareProperty("DumpFile",m_dumpFile="");
 declareProperty("PrintCellLocation",m_printCellLoc=false);
 declareProperty("PrintFebChan",m_printFebChan=true);
 
}

ReadTBLArDigits::~ReadTBLArDigits() 
{//empty
}

StatusCode ReadTBLArDigits::initialize()
{ MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize" << endreq;
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  m_fcalId=caloIdMgr->getFCAL_ID();
  m_hecId=caloIdMgr->getHEC_ID();
  IToolSvc* toolSvc;
  StatusCode sc=service( "ToolSvc",toolSvc  );
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve ToolSvc" << endreq;
      return StatusCode::FAILURE;
    }

  sc=toolSvc->retrieveTool("LArCablingService",m_larCablingSvc);
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve LArCablingService" << endreq;
      return StatusCode::FAILURE;
    }

  sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  if (m_dumpFile.size()>0)
    m_outfile.open(m_dumpFile.c_str(),std::ios::out);
  //Ntuple booking
 

  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1)
    {log << MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE;
   }
  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE1/LARDIGITS");
  if (!nt) {
    nt=ntupleSvc()->book("/NTUPLES/FILE1/LARDIGITS",CLID_ColumnWiseTuple,"LArDigits");
  }
  if (!nt)
    {log << MSG::ERROR << "Booking of NTuple failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("icell",m_cellIndex,0,3600);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Cell Index' failed" << endreq;
     return StatusCode::FAILURE;
    }

  //sc=nt->addItem("layer",m_layer,0,4);
  sc=nt->addItem("layer",m_cellIndex,m_layer);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Layer' failed" << endreq;
     return StatusCode::FAILURE;
    }

  //sc=nt->addItem("ieta",m_eta,0,510);
  sc=nt->addItem("ieta",m_cellIndex,m_eta);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Eta' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("iphi",m_cellIndex,m_phi);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Phi' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("barrel_ec",m_cellIndex,m_barrel_ec);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'barrel_ec' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("pos_neg",m_cellIndex,m_pos_neg);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'pos_neg' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("FT",m_cellIndex,m_FT);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'FT' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("slot",m_cellIndex,m_slot);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'slot' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("channel",m_cellIndex,m_channel);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'channel' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("gain",m_cellIndex,m_gain);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Gain' failed" << endreq;
     return StatusCode::FAILURE;
    }  
  
  sc=nt->addItem("NSamples",m_Nsamples,0,32);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem 'sampleIndex' failed" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc=nt->addItem("Samples",m_cellIndex,m_samples,32);
  //sc=nt->addItem("Samples",m_cellIndex,m_samples,m_Nsamples);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem failed" << endreq;
    return StatusCode::FAILURE;
  }
  
  m_ntuplePtr=nt;
  m_count=0;
  log << MSG::INFO << "======== ReadTBLArDigits initialize successfully ========" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode ReadTBLArDigits::execute()
{MsgStream log(msgSvc(), name());
 m_count++; 
 StatusCode sc; 
 log << MSG::DEBUG << "======== executing event "<< m_count << " ========" << endreq;
 log << MSG::DEBUG << "Retrieving TBLArDigitContainer. Key= " << m_containerKey << endreq; 
 TBLArDigitContainer* larDigitCont;
 if (m_containerKey.size())
   sc = evtStore()->retrieve(larDigitCont ,m_containerKey);
 else
   sc = evtStore()->retrieve(larDigitCont);
 if (sc.isFailure()) 
   {log << MSG::FATAL << " Cannot read TBLArDigitContainer from StoreGate! key=" << m_containerKey << endreq;
    return StatusCode::FAILURE;
   }
 

 if (m_outfile.is_open()) {
   log << MSG::DEBUG << "Sorting digits" << endreq; 
   SortDigits sortDigits(m_onlineHelper);
   std::sort(larDigitCont->begin(),larDigitCont->end(),sortDigits);
 }
 
 log << MSG::DEBUG << "Finished sorting" << endreq; 
 unsigned cellCounter=0;
 TBLArDigitContainer::const_iterator it=larDigitCont->begin();
 TBLArDigitContainer::const_iterator it_end=larDigitCont->end();
 if (larDigitCont->size()>0)
   m_Nsamples=(*it)->samples().size();
 else                       
   m_Nsamples=0;
 log << MSG::DEBUG << "Now loop over digits" << endreq; 
 for (;it!=it_end;it++) {
   HWIdentifier chid=(*it)->hardwareID();
   log << MSG::DEBUG << "Get offline ID" << endreq; 
   const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);
   const std::vector<short>& vSamples=(*it)->samples();
   m_cellIndex=cellCounter;
   log << MSG::DEBUG << "Now find eta/phi (EM only right now)" << endreq; 
   if (m_emId->is_lar_em(id)) {
     m_eta[m_cellIndex]=m_emId->eta(id); 
     m_phi[m_cellIndex]=m_emId->phi(id);
     m_layer[m_cellIndex]=m_emId->sampling(id);
   } else if (m_fcalId->is_lar_fcal(id)) {
     m_eta[m_cellIndex]=m_fcalId->eta(id); 
     m_phi[m_cellIndex]=m_fcalId->phi(id);
     m_layer[m_cellIndex]=m_fcalId->module(id);     
   } else if (m_hecId->is_lar_hec(id)) {
     m_eta[m_cellIndex]=m_hecId->eta(id); 
     m_phi[m_cellIndex]=m_hecId->phi(id);
     m_layer[m_cellIndex]=m_hecId->sampling(id);
   } else {
     m_eta[m_cellIndex]=0;
     m_phi[m_cellIndex]=0;
     m_layer[m_cellIndex]=0;
   }
   log << MSG::DEBUG << "Store identifier arrays" << endreq;
   m_barrel_ec[m_cellIndex]=m_onlineHelper->barrel_ec(chid);
   m_pos_neg[m_cellIndex] = m_onlineHelper->pos_neg(chid);
   m_FT[m_cellIndex] = m_onlineHelper->feedthrough(chid);
   m_slot[m_cellIndex] = m_onlineHelper->slot(chid);
   m_channel[m_cellIndex]   = m_onlineHelper->channel(chid);
   m_gain[m_cellIndex]=(*it)->gain();
   
   log << MSG::DEBUG << "Loop over samples" << endreq; 
   int nSamples=vSamples.size();
   for (int i=0;i<nSamples && i<32;i++)
     m_samples[m_cellIndex][i]=vSamples[i];
   
   if (m_outfile.is_open()) {
     m_outfile << "Evt="<< m_count << " ";
     if (m_printCellLoc)
       m_outfile << "Cell l/e/p= " << m_layer[m_cellIndex] << "/" << m_eta[m_cellIndex] 
                 << "/" << m_phi[m_cellIndex] << " ";
     if (m_printFebChan)
       m_outfile << "FebId= 0x" << std::hex << m_onlineHelper->feb_Id(chid).get_compact()
                 << std::dec << " Ch= " << m_onlineHelper->channel(chid) << " ";
     for (int i=0;i<nSamples;i++) {
       m_outfile << " " << vSamples[i];
     }
     m_outfile << " G=" << m_gain[m_cellIndex] << std::endl;
   }
   cellCounter++;
 }
 sc=ntupleSvc()->writeRecord(m_ntuplePtr);
 if (sc!=StatusCode::SUCCESS) {
   log << MSG::ERROR << "writeRecord failed" << endreq;
   return StatusCode::FAILURE;
 }
 
 if (m_count%1000==0) 
   log << MSG::INFO  << "Event " << m_count << " contains " << cellCounter << " channels\n";
 return StatusCode::SUCCESS;
}

StatusCode ReadTBLArDigits::finalize()
{ MsgStream log(msgSvc(), name());
  if (m_outfile.is_open()) 
    m_outfile.close();
  log << MSG::INFO << "finalize ReadTBLArDigits" << endreq;
  return StatusCode::SUCCESS;
}
