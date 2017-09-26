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
   m_ckey("CutBookkeepers","MetaDataStore")
{
}

MetadataTest::~MetadataTest()
{}

StatusCode MetadataTest::initialize() 
{
   ATH_CHECK( m_hkey.initialize() );  
   ATH_CHECK( m_ckey.initialize() );  
   return StatusCode::SUCCESS;
}

StatusCode MetadataTest::execute() 
{
   ATH_MSG_DEBUG ( "in execute()" );

   ServiceHandle<StoreGateSvc> metastore("StoreGateSvc/MetaDataStore",this->name());
   ATH_CHECK(metastore.retrieve());
 
/*
   SG::ReadHandle< MetaCont<xAOD::CutBookkeeperContainer> > cont(m_ckey);
   //const MetaCont<xAOD::CutBookkeeperContainer>* cont(*rckey);
   if (cont != nullptr) {
     ATH_MSG_INFO("BLARG cbk container size = " << cont->entries());
   } else {
     ATH_MSG_INFO("Did not retrieve MetaCont from store");
   }
*/

   SG::ReadMetaHandle<xAOD::CutBookkeeperContainer> readkey(m_hkey);
   const xAOD::CutBookkeeperContainer* cbkCont(*readkey);
   //ATH_MSG_INFO("BLARG " << metastore->dump());
   if (cbkCont != nullptr) {
     ATH_MSG_INFO("Found cbk size " << cbkCont->size());
     //for (auto it = cbkCont->begin(); it != cbkCont->end(); ++it) {
       //ATH_MSG_INFO("Name: " <<(*it)->name() << ", N=" << (*it)->nAcceptedEvents());
     //}
   } else {
     ATH_MSG_ERROR("Did not retrieve CutBookkeeperContainer from store");
   }

   return(StatusCode::SUCCESS);
}

StatusCode MetadataTest::finalize() 
{
   ATH_MSG_DEBUG ( "in m_finalize()" );
   return(StatusCode::SUCCESS);
}
