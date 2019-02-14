/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/DumpLArDigits.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "xAODEventInfo/EventInfo.h"
//#include "testpack/compdigit.h"
#include <stdlib.h>

#include <vector>

DumpLArDigits::DumpLArDigits(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_chan(0),
    m_onlineHelper(0),
    m_emId(0)
{m_count=0;
 declareProperty("LArDigitContainerName",m_key=""); 
 declareProperty("OutputFileName",m_FileName="LArDigits.txt");
}

DumpLArDigits::~DumpLArDigits() 
{//empty
}

StatusCode DumpLArDigits::initialize()
{
  ATH_MSG_INFO ( "======== initialize DumpLArDigits ========" );
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  ATH_MSG_DEBUG ( " Found the LArOnlineID helper. " );

  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  if (!caloIdMgr) {
    ATH_MSG_ERROR ( "Unable to get instance of CaloIdManager" );
    return StatusCode::FAILURE;
  }
  m_emId=caloIdMgr->getEM_ID();
  if (!m_emId)  {
    ATH_MSG_ERROR ( "Unable to get EM_ID" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_cablingKey.initialize() );

  m_outfile.open(m_FileName.c_str(),std::ios::out);

  ATH_MSG_INFO ( "======== test-stuff initialize successfully ========" );
  return StatusCode::SUCCESS;
}


StatusCode DumpLArDigits::execute()
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

 const DataHandle < LArDigitContainer > digit_cont;
 
 if (m_key.size())
   ATH_CHECK( evtStore()->retrieve(digit_cont,m_key) );
 else
   ATH_CHECK( evtStore()->retrieve(digit_cont) );
 ATH_MSG_INFO ( "Retrieved LArDigitContainer from StoreGate! key=" << m_key );
 SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
 const LArOnOffIdMapping* cabling{*cablingHdl};
 if(!cabling){
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
     return StatusCode::FAILURE;
 }
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
     std::cout << "FEB_ID: 0x" << std::hex << febid.get_identifier32().get_compact() 
	       << " channel: " << std::dec <<  m_onlineHelper->channel(chid) 
	       << " (" << m_onlineHelper->channelInSlotMax(febid) << "/FEB), ";
     m_outfile << "FEB_ID: 0x" << std::hex << febid.get_compact() 
	       << " channel: " << std::dec <<  m_onlineHelper->channel(chid) 
	       << " (" << m_onlineHelper->channelInSlotMax(febid) << "/FEB), ";
     if (!cabling->isOnlineConnected(chid))
       {std::cout << "disconnected: \n";
       m_outfile << "disconnected: \n";
       continue;
       }
     const Identifier id=cabling->cnvToIdentifier(chid);
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
{
  m_outfile.close(); 
  ATH_MSG_INFO ( "======== finalize DumpLArDigit ========" );
  return StatusCode::SUCCESS;
}

bool DumpLArDigits::mySort::operator()(const LArDigit* a, const LArDigit* b)
{return a->channelID() < b->channelID();}
