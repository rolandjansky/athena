/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MetadataTest.cxx
 * @brief Implementation of class MetadataTest
 */
 
#include "MetadataTest.h"

//#include "GaudiKernel/MsgStream.h"
#include "StoreGate/ReadMetaHandle.h"

#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "ByteStreamData/ByteStreamMetadata.h"

#include "StoreGate/StoreGateSvc.h"


//___________________________________________________________________________
MetadataTest::MetadataTest(const std::string& name, ISvcLocator* pSvcLocator) : 
   AthAlgorithm(name, pSvcLocator),
   m_hkey("CutBookkeepers","MetaDataStore"),
   m_hinckey("IncompleteCutBookkeepers","MetaDataStore"),
   m_eihkey("StreamAOD","MetaDataStore"),
   m_bshkey("ByteStreamMetadata","MetaDataStore"),
   m_esidone(false),
   m_inputstream("StreamAOD")
{
   declareProperty("InputStream",
                   m_inputstream = "StreamAOD",
                   "Key to read EventStreamInfo.");
}

MetadataTest::~MetadataTest()
{}

StatusCode MetadataTest::start() 
{
  // Get proper dbkey.
   m_eihkey = SG::ReadMetaHandleKey<EventStreamInfo>(m_inputstream,"MetaDataStore");
   ATH_CHECK( m_hkey.initialize() );  
   ATH_CHECK( m_hinckey.initialize() );  
   ATH_CHECK( m_eihkey.initialize() );
   ATH_CHECK( m_bshkey.initialize() );
   m_esidone = false;
   return StatusCode::SUCCESS;
}

StatusCode MetadataTest::execute() 
{
   ATH_MSG_DEBUG ( "in execute()" );
   if (!m_esidone) {
     ATH_MSG_INFO("== CUTBOOKKEEPERCONTAINER CHECKS ==");
     // create handle, get pointer to object
     SG::ReadMetaHandle<xAOD::CutBookkeeperContainer> readhand(m_hkey,this->getContext());
     const xAOD::CutBookkeeperContainer* cbkCont(*readhand);
     if (cbkCont != nullptr) {
       ATH_MSG_INFO("Found cbk size " << cbkCont->size());
       for (auto it = cbkCont->begin(); it != cbkCont->end(); ++it) {
         ATH_MSG_INFO("Name: " <<(*it)->name() << ", N=" << (*it)->nAcceptedEvents());
       }
     } else {
       ATH_MSG_ERROR("Did not retrieve CutBookkeeperContainer from store");
     }
     // create handle, get pointer to object
     SG::ReadMetaHandle<xAOD::CutBookkeeperContainer> increadhand(m_hinckey,this->getContext());
     const xAOD::CutBookkeeperContainer* inccbkCont(*increadhand);
     if (inccbkCont != nullptr) {
       ATH_MSG_INFO("Found inc cbk size " << inccbkCont->size());
       for (auto it = inccbkCont->begin(); it != inccbkCont->end(); ++it) {
         ATH_MSG_INFO("Name: " <<(*it)->name() << ", N=" << (*it)->nAcceptedEvents());
       }
     } else {
       ATH_MSG_ERROR("Did not retrieve inc CutBookkeeperContainer from store");
     }

     ATH_MSG_INFO("== EVENTSTREAMINFO CHECKS ==");
     // create handle, get pointer to object
     SG::ReadMetaHandle<EventStreamInfo> eikey(m_eihkey,this->getContext());
     const EventStreamInfo* esi(*eikey);
     if (esi!=nullptr) {
       ATH_MSG_INFO(" esi has n events = " << esi->getNumberOfEvents());
     } else {
       ATH_MSG_ERROR("Did not retrieve EventStreamInfo " << m_eihkey.objKey());
     }

     ATH_MSG_INFO("== BYTESTREAMMETADATACONTAINER CHECKS ==");
     // create handle, get pointer to object
     SG::ReadMetaHandle<ByteStreamMetadataContainer> bskey(m_bshkey,this->getContext());
     const ByteStreamMetadataContainer* bsmc(*bskey);
     if (bsmc!=nullptr) {
       ATH_MSG_INFO("ByteStreamMetadataContainer size " << bsmc->size());
       for (auto it = bsmc->begin(); it != bsmc->end(); ++it) {
         ATH_MSG_INFO("Run="<<(*it)->getRunNumber()<<" E="<<(*it)->getBeamEnergy()<<" s="<<(*it)->getStream());
       } 
     } else {
       ATH_MSG_ERROR("Did not find ByteStreamMetadataContainer");
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
