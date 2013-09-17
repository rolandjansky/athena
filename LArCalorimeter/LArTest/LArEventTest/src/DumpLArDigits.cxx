/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/DumpLArDigits.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//#include "testpack/compdigit.h"
#include <stdlib.h>

#include <vector>

DumpLArDigits::DumpLArDigits(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_chan(0),
    m_storeGateSvc(0),
    m_onlineHelper(0),
    m_larCablingSvc(0),
    m_emId(0)
{m_count=0;
 declareProperty("LArDigitContainerName",m_key=""); 
 declareProperty("OutputFileName",m_FileName="LArDigits.txt");
}

DumpLArDigits::~DumpLArDigits() 
{//empty
}

StatusCode DumpLArDigits::initialize()
{ MsgStream log(msgSvc(), name());
  log << MSG::INFO << "======== initialize DumpLArDigits ========" << endreq;
  StatusCode sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure()) 
    {log << MSG::ERROR << " Cannot locate StoreGateSvc " << std::endl;
     return StatusCode::FAILURE;
    }

  
  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore );
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Could not locate DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  sc = detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return sc;
  } 
  log << MSG::DEBUG << " Found the LArOnlineID helper. " << endreq;


  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  if (!caloIdMgr)
    {log << MSG::ERROR << "Unable to get instance of CaloIdManager" << endreq;
     return StatusCode::FAILURE;
    }
  m_emId=caloIdMgr->getEM_ID();
  if (!m_emId) 
    {log << MSG::ERROR << "Unable to get EM_ID" << endreq;
     return StatusCode::FAILURE;
    }

  //compdigit::m_emId=m_emId;


  IToolSvc* toolSvc;
  sc=service( "ToolSvc",toolSvc  );
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve ToolSvc" << endreq;
      return StatusCode::FAILURE;
    }

  sc=toolSvc->retrieveTool("LArCablingService",m_larCablingSvc);
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve LArCablingService" << endreq;
      return StatusCode::FAILURE;
    }
  //compdigit::m_larCablingSvc=m_larCablingSvc;



  m_outfile.open(m_FileName.c_str(),std::ios::out);

  log << MSG::INFO << "======== test-stuff initialize successfully ========" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode DumpLArDigits::execute()
{MsgStream log(msgSvc(), name());
 m_count++; 
 log << MSG::INFO << "======== executing event "<< m_count << " ========" << endreq;
 EventID *thisEvent;           //EventID is a part of EventInfo
 const DataHandle<EventInfo> thisEventInfo;
 StatusCode sc=m_storeGateSvc->retrieve(thisEventInfo);
 if (sc!=StatusCode::SUCCESS)
   log << MSG::WARNING << "No EventInfo object found!" << endreq;
 else
   {thisEvent=thisEventInfo->event_ID();
   std::cout << "*** Event #" << std::dec << thisEvent->run_number() << "/" << thisEvent->event_number() << std::endl;
   }

 const DataHandle < LArDigitContainer > digit_cont;
 
 if (m_key.size())
   sc = m_storeGateSvc->retrieve(digit_cont,m_key);
 else
   sc = m_storeGateSvc->retrieve(digit_cont);
 if (sc.isFailure()) 
   {log << MSG::ERROR << " Cannot read LArDigitContainer from StoreGate! key=" << m_key << endreq;
    return StatusCode::FAILURE;
   }
 log << MSG::INFO << "Retrieved LArDigitContainer from StoreGate! key=" << m_key << endreq;
 //int nColl=0;
 short layer,eta,phi;
 m_chan=0;
 //int Nchan=0;
 typedef std::vector< const LArDigit* > LArDigitVector;
 //std::vector< const LArDigit* > digitVector;
 LArDigitVector digitVector;
 LArDigitContainer::const_iterator it = digit_cont->begin(); 
 LArDigitContainer::const_iterator it_e = digit_cont->end(); 
 //Nchan=0;
 for(; it!=it_e; ++it){
   m_chan++;
   digitVector.push_back(*it);
 }
 //std::cout << "End of outer loop" <<std::endl;

 m_outfile << "Event " << m_count << " contains " << m_chan << " digits\n";
 std::cout << "Event " << m_count << " contains " << m_chan << " digits\n";
 
 mySort aSort;
 std::sort(digitVector.begin(),digitVector.end(),aSort);
 LArDigitVector::const_iterator vec_it=digitVector.begin();
 LArDigitVector::const_iterator vec_it_e=digitVector.end();
 for (;vec_it!=vec_it_e;vec_it++)
   {
     //if ((*vec_it)->energy()==0)
     //continue;
     const HWIdentifier chid=(*vec_it)->channelID();//hardwareID();
     const HWIdentifier febid=m_onlineHelper->feb_Id(chid);
     std::cout << "FEB_ID: 0x" << std::hex << febid.get_compact() 
	       << " channel: " << std::dec <<  m_onlineHelper->channel(chid) 
	       << " (" << m_onlineHelper->channelInSlotMax(febid) << "/FEB), ";
     m_outfile << "FEB_ID: 0x" << std::hex << febid.get_compact() 
	       << " channel: " << std::dec <<  m_onlineHelper->channel(chid) 
	       << " (" << m_onlineHelper->channelInSlotMax(febid) << "/FEB), ";
     if (!m_larCablingSvc->isOnlineConnected(chid))
       {std::cout << "disconnected: \n";
       m_outfile << "disconnected: \n";
       continue;
       }
     const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);
     //std::cout << "Compact onlineid=" << id.get_compact() << std::endl;
     eta=m_emId->eta(id); 
     phi=m_emId->phi(id);
     layer=m_emId->sampling(id);
     //std::cout << "LArDigit #" << chid.get_compact();
     if(!m_emId->is_lar_em(id))
      {std::cout << "Is not EM! \n" << std::endl; 
      m_outfile << "Is not EM! \n" << std::endl;
       continue;
      }
    if (m_onlineHelper->isEmEndcapOnline(chid))
     {std::cout << "Endcap ";
      m_outfile << "Endcap ";
     }
    if (m_onlineHelper->isEmBarrelOnline(chid))
      {std::cout << "Barrel ";
       m_outfile << "Barrel ";
      }
    std::cout << "  l/e/p= " << layer << "/" << eta << "/" << phi << ":";
    for(unsigned int i=0;i<(*vec_it)->samples().size();i++)
      std::cout << " " << (*vec_it)->samples()[i];
    std::cout << " G="  << (*vec_it)->gain() << std::endl;
    
    m_outfile << "l/e/p= " << layer << "/" << eta << "/" << phi << ":";
    for(unsigned int i=0;i<(*vec_it)->samples().size();i++)
      m_outfile << " " << (*vec_it)->samples()[i];
    m_outfile << " G="  << (*vec_it)->gain() << std::endl;
   }
 //std::cout << "Collection #" << ++nColl << " contains " << chan_coll->size() << " elementes." << std::endl;
 std::cout << "Event " << m_count << " contains " << m_chan << " (" <<digitVector.size() <<")  channels\n";
 return StatusCode::SUCCESS;
}

StatusCode DumpLArDigits::finalize()
{ MsgStream log(msgSvc(), name());
 m_outfile.close(); 
  log << MSG::INFO << "======== finalize DumpLArDigit ========" << endreq;
  return StatusCode::SUCCESS;
}

bool DumpLArDigits::mySort::operator()(const LArDigit* a, const LArDigit* b)
{return a->channelID() < b->channelID();}
