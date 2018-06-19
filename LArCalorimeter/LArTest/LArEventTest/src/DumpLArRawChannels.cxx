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
 //log <<MSG::DEBUG<<" number of LArRawChannel collections "<< channel_cont->size() <<endmsg;
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
    std::cout << "Channel: " << m_onlineHelper->channel_name(chid);
    m_outfile << "Channel: " << m_onlineHelper->channel_name(chid);

    if (!m_larCablingSvc->isOnlineConnected(chid)) {
      std::cout << " disconnected" << std::endl;
      m_outfile << " disconnected" << std::endl;
      continue;
    }
    const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);

    if(!m_emId->is_lar_em(id)) {
      eta=m_emId->eta(id); 
      phi=m_emId->phi(id);
      layer=m_emId->sampling(id);      
      if (m_emId->is_em_endcap(id)) {
	std::cout << " Endcap l/e/p= " << layer << "/" << eta << "/" << phi << ":";
	m_outfile << " Endcap l/e/p= " << layer << "/" << eta << "/" << phi << ":";
      }
      else {
	std::cout << " Barrel l/e/p= " << layer << "/" << eta << "/" << phi << ":";
	m_outfile << " Barrel l/e/p= " << layer << "/" << eta << "/" << phi << ":";
      }
    }
    else {
      std::cout << " (is not EM)";
      m_outfile << " (is not EM)";
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
    std::cout << " E= " << (*vec_it)->energy() << " t= " << Time << " Q= " << (*vec_it)->quality() << " P=0x" 
	      << std::hex << (*vec_it)->provenance() << std::dec << " G=" << (*vec_it)->gain() << std::endl;
    
    m_outfile << " E= " << (*vec_it)->energy() << " t= " << Time << " Q= " << (*vec_it)->quality() << " P=0x"
	      << std::hex << (*vec_it)->provenance() << std::dec << " G=" << (*vec_it)->gain() << std::endl;

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
