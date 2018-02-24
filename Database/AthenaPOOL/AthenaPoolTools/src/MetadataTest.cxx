/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MetadataTest.cxx
 * @brief Implementation of class MetadataTest
 */
 
#include "MetadataTest.h"

//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/ReadMetaHandle.h"

#include "xAODCutFlow/CutBookkeeperContainer.h"

#include "StoreGate/StoreGateSvc.h"


//___________________________________________________________________________
MetadataTest::MetadataTest(const std::string& name, ISvcLocator* pSvcLocator) : 
   AthAlgorithm(name, pSvcLocator),
   m_hkey("CutBookkeepers","MetaDataStore"),
   m_eihkey("StreamAOD","MetaDataStore"),
   m_eihkey2("DataStream","MetaDataStore"),
   m_esidone(false)
{
}

MetadataTest::~MetadataTest()
{}

StatusCode MetadataTest::start() 
{
  // Get proper dbkey.
   ATH_CHECK( m_hkey.initialize() );  
   ATH_CHECK( m_eihkey.initialize() );
   ATH_CHECK( m_eihkey2.initialize() );
   m_esidone = false;
   return StatusCode::SUCCESS;
}

StatusCode MetadataTest::execute() 
{
   ATH_MSG_DEBUG ( "in execute()" );

   const DataHeader* thisDH = nullptr;
   ATH_CHECK( evtStore()->retrieve(thisDH, "EventSelector") );
   std::string sid = thisDH->begin()->getToken()->dbID().toString();
   if (!m_esidone) {
     //m_hkey.setDbKey(sid);
     SG::ReadMetaHandleKey<xAOD::CutBookkeeperContainer> readkey(m_hkey.key(),sid);
     ATH_CHECK(readkey.initialize());
     SG::ReadMetaHandle<xAOD::CutBookkeeperContainer> readhand(readkey,this->getContext());
     const xAOD::CutBookkeeperContainer* cbkCont(*readhand);
     if (cbkCont != nullptr) {
       ATH_MSG_INFO("Found cbk size " << cbkCont->size());
       for (auto it = cbkCont->begin(); it != cbkCont->end(); ++it) {
         ATH_MSG_INFO("Name: " <<(*it)->name() << ", N=" << (*it)->nAcceptedEvents());
       }
     } else {
       ATH_MSG_ERROR("Did not retrieve CutBookkeeperContainer from store");
     }

     m_eihkey.setDbKey(sid);
     SG::ReadMetaHandle<EventStreamInfo> eikey(m_eihkey,this->getContext());
     const EventStreamInfo* esi(*eikey);
     if (esi!=nullptr) {
       ATH_MSG_INFO(" esi has n events = " << esi->getNumberOfEvents());
     } else {
       ATH_MSG_ERROR("Did not retrieve EventStreamInfo " << m_eihkey.objKey());
     }
     m_eihkey2.setDbKey(sid);
     SG::ReadMetaHandle<EventStreamInfo> eikey2(m_eihkey2,this->getContext());
     const EventStreamInfo* esi2(*eikey2);
     if (esi2!=nullptr) {
       ATH_MSG_INFO(" esi has n events = " << esi2->getNumberOfEvents());
     } else {
       ATH_MSG_ERROR("Did not retrieve EventStreamInfo " << m_eihkey2.objKey());
     }
     m_esidone=true;
   }

   return(StatusCode::SUCCESS);
}

StatusCode MetadataTest::finalize() 
{
   ATH_MSG_DEBUG ( "in m_finalize()" );
   return(StatusCode::SUCCESS);
}
