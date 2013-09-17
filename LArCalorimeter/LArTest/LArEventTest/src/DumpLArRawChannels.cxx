/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/DumpLArRawChannels.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//#include "testpack/compdigit.h"
#include <stdlib.h>

#include <vector>

DumpLArRawChannels::DumpLArRawChannels(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_chan(0),
    m_storeGateSvc(0),
    m_onlineHelper(0),
    m_larCablingSvc(0),
    m_emId(0)
{m_count=0;
 declareProperty("LArRawChannelContainerName",m_key=""); 
 declareProperty("OutputFileName",m_FileName="LArRawChannels.txt");
}

DumpLArRawChannels::~DumpLArRawChannels() 
{//empty
}

StatusCode DumpLArRawChannels::initialize()
{ MsgStream log(msgSvc(), name());
  log << MSG::INFO << "======== initialize DumpLArRawChanels ========" << endreq;
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


StatusCode DumpLArRawChannels::execute()
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

 const DataHandle < LArRawChannelContainer > channel_cont;
 
 if (m_key.size())
   sc = m_storeGateSvc->retrieve(channel_cont,m_key);
 else
   sc = m_storeGateSvc->retrieve(channel_cont);
 if (sc.isFailure()) 
   {log << MSG::ERROR << " Cannot read LArRawChannelContainer from StoreGate! key=" << m_key << endreq;
    return StatusCode::FAILURE;
   }
 log << MSG::INFO << "Retrieved LArRawChannelContainer from StoreGate! key=" << m_key << endreq;
 //int nColl=0;
 short layer,eta,phi;
 m_chan=0;
 //int Nchan=0;
 typedef std::vector< const LArRawChannel* > LArRawChannelVector;
 //std::vector< const LArRawChannel* > channelVector;
 LArRawChannelVector channelVector;
 LArRawChannelContainer::const_iterator it_chan_coll = channel_cont->begin(); 
 LArRawChannelContainer::const_iterator it_chan_coll_end = channel_cont->end(); 
 //log <<MSG::DEBUG<<" number of LArRawChannel collections "<< channel_cont->size() <<endreq;
 for( ; it_chan_coll!=it_chan_coll_end;++it_chan_coll) {
   m_chan++;
   channelVector.push_back(&(*it_chan_coll));
 }
 m_outfile << "Event " << m_count << " contains " << m_chan << " channels\n";
 std::cout << "Event " << m_count << " contains " << m_chan << " channels\n";
 
 mySort aSort;
 std::sort(channelVector.begin(),channelVector.end(),aSort);
 LArRawChannelVector::const_iterator vec_it=channelVector.begin();
 LArRawChannelVector::const_iterator vec_it_e=channelVector.end();
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
    //std::cout << "LArRawChannel #" << chid.get_compact();
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
    int Time=(*vec_it)->time();
    /*
    if (Time>0x1fff)
      Time=0x1fff;
    if (Time<-0x1fff)
      Time=-0x1fff;
    */
    //if (abs(Time)>24000)
    //Time=0;
    std::cout << "  l/e/p= " << layer << "/" << eta << "/" << phi << ":"// << std::endl;
	      << " E= " << (*vec_it)->energy() << " t= " << Time << " Q= " << (*vec_it)->quality() << " G=" 
	      << (*vec_it)->gain() << std::endl;
    
    m_outfile << "l/e/p= " << layer << "/" << eta << "/" << phi << ":" 
	      << " E= " << (*vec_it)->energy() << " t= " << Time << " Q= " << (*vec_it)->quality() << " G=" 
	      << (*vec_it)->gain() << std::endl;
   }
 //std::cout << "Collection #" << ++nColl << " contains " << chan_coll->size() << " elementes." << std::endl;
 std::cout << "Event " << m_count << " contains " << m_chan << " (" <<channelVector.size() <<")  channels\n";
 return StatusCode::SUCCESS;
}

StatusCode DumpLArRawChannels::finalize()
{ MsgStream log(msgSvc(), name());
 m_outfile.close(); 
  log << MSG::INFO << "======== finalize DumpLArRawChannel ========" << endreq;
  return StatusCode::SUCCESS;
}

bool DumpLArRawChannels::mySort::operator()(const LArRawChannel* a, const LArRawChannel* b)
{return a->channelID() < b->channelID();}
