/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/DumpLArRawChannels.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "xAODEventInfo/EventInfo.h"
//#include "testpack/compdigit.h"
#include <stdlib.h>

#include <vector>

DumpLArRawChannels::DumpLArRawChannels(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_chan(0),
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
{
  ATH_MSG_INFO ( "======== initialize DumpLArRawChanels ========" );

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  ATH_MSG_DEBUG ( " Found the LArOnlineID helper. " );

  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  if (!caloIdMgr) {
    ATH_MSG_ERROR ( "Unable to get instance of CaloIdManager" );
    return StatusCode::FAILURE;
  }
  m_emId=caloIdMgr->getEM_ID();
  if (!m_emId) {
    ATH_MSG_ERROR ( "Unable to get EM_ID" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK( toolSvc()->retrieveTool("LArCablingService",m_larCablingSvc) );

  m_outfile.open(m_FileName.c_str(),std::ios::out);

  ATH_MSG_INFO ( "======== test-stuff initialize successfully ========" );
  return StatusCode::SUCCESS;
}


StatusCode DumpLArRawChannels::execute()
{
 m_count++; 
 ATH_MSG_INFO ( "======== executing event "<< m_count << " ========" );
 const DataHandle<xAOD::EventInfo> thisEventInfo;
 StatusCode sc=evtStore()->retrieve(thisEventInfo);
 if (sc!=StatusCode::SUCCESS)
   ATH_MSG_WARNING ( "No EventInfo object found!" );
 else
   {
   std::cout << "*** Event #" << std::dec << thisEventInfo->runNumber() << "/" << thisEventInfo->eventNumber() << std::endl;
   }

 const DataHandle < LArRawChannelContainer > channel_cont;
 
 if (m_key.size())
   ATH_CHECK( evtStore()->retrieve(channel_cont,m_key) );
 else
   ATH_CHECK( evtStore()->retrieve(channel_cont) );
 ATH_MSG_INFO ( "Retrieved LArRawChannelContainer from StoreGate! key=" << m_key );
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
{
  m_outfile.close(); 
  ATH_MSG_INFO ( "======== finalize DumpLArRawChannel ========" );
  return StatusCode::SUCCESS;
}

bool DumpLArRawChannels::mySort::operator()(const LArRawChannel* a, const LArRawChannel* b)
{return a->channelID() < b->channelID();}
