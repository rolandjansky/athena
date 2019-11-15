/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#include "./HLTPrescaleCondAlg.h"
#include "TrigConfIO/TrigDBHLTPrescalesSetLoader.h"

#include "CoolKernel/types.h"

TrigConf::HLTPrescaleCondAlg::HLTPrescaleCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode
TrigConf::HLTPrescaleCondAlg::initialize() {

   ATH_MSG_INFO("initialize");

   ATH_CHECK(m_pskFolderInputKey.initialize());

   ATH_CHECK(m_hltPrescaleSetOutputKey.initialize());

   return StatusCode::SUCCESS;
}

StatusCode
TrigConf::HLTPrescaleCondAlg::execute(const EventContext& ctx) const {

   SG::ReadCondHandle<AthenaAttributeList> readH(m_pskFolderInputKey, ctx);
   const AthenaAttributeList * pskAL{ *readH };
   if ( pskAL == nullptr ) {
      ATH_MSG_FATAL("Null pointer to the read conditions object of " << m_pskFolderInputKey.key());
      return StatusCode::FAILURE;
   }
   
   EventIDRange range;
   if (not readH.range(range)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readH.key());
      return StatusCode::FAILURE;
   }


   // get the prescale key from the cool folder 
   unsigned int hltPsk = (*pskAL)["HltPrescaleKey"].data<cool::UInt32>();


   // load the HLT psk into HLT prescales set
   TrigConf::TrigDBHLTPrescalesSetLoader psLoader(m_dbConnection);
   psLoader.setLevel(TrigConf::MSGTC::WARNING); 
   TrigConf::HLTPrescalesSet * pss = new TrigConf::HLTPrescalesSet;
   if( psLoader.loadHLTPrescales( hltPsk, *pss ) ) {
      ATH_MSG_INFO( "HLT prescales set successfully loaded from db with key " << m_psk.value() );
   } else {
      ATH_MSG_ERROR( "Failed loading HLT prescales set from db with key " << m_psk.value() );
      return StatusCode::FAILURE;
   }


   // recording HLT prescales set
   SG::WriteCondHandle<TrigConf::HLTPrescalesSet> writeCondHandle(m_hltPrescaleSetOutputKey, ctx);

   ATH_MSG_INFO("Recording HLT prescales set with range " << range);
   ATH_CHECK(writeCondHandle.record( range, pss ));

   return StatusCode::SUCCESS;
}
