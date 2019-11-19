/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#include "./HLTPrescaleCondAlg.h"
#include "TrigConfIO/TrigDBHLTPrescalesSetLoader.h"
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "CoolKernel/types.h"

TrigConf::HLTPrescaleCondAlg::HLTPrescaleCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode
TrigConf::HLTPrescaleCondAlg::initialize() {

   ATH_MSG_DEBUG("HLTPrescaleCondAlg::initialize()");

   ATH_CHECK(m_pskFolderInputKey.initialize());

   ATH_CHECK(m_hltPrescaleSetOutputKey.initialize());

   if( m_configSource == "COOL" && m_dbConnection == "JOSVC" ) {
      if( auto joSvc = serviceLocator()->service<TrigConf::IJobOptionsSvc>( "JobOptionsSvc" ) ) {
         if( joSvc->hltPrescaleKey()>0 ) {
            m_psk = joSvc->hltPrescaleKey();
            m_dbConnection = joSvc->server();
            ATH_MSG_DEBUG("Set psk to " << m_psk <<  " and db connection to " << m_dbConnection );
         }
      } else {
         ATH_MSG_DEBUG("Did not locate TrigConf::IJobOptionsSvc");
      }
   }

   ATH_MSG_DEBUG("Source    " << m_configSource);
   ATH_MSG_DEBUG("TriggerDB " << m_dbConnection);
   ATH_MSG_DEBUG("HLTPsk    " << m_psk);
   ATH_MSG_DEBUG("Filename  " << m_filename.value());

   return StatusCode::SUCCESS;
}

StatusCode
TrigConf::HLTPrescaleCondAlg::execute(const EventContext& ctx) const {

   ATH_MSG_DEBUG("HLTPrescaleCondAlg::execute");

   unsigned int hltPsk = m_psk;
   EventIDRange range;

   if(m_configSource == "COOL") {
      SG::ReadCondHandle<AthenaAttributeList> readH(m_pskFolderInputKey, ctx);
      const AthenaAttributeList * pskAL{ *readH };
      if ( pskAL == nullptr ) {
         ATH_MSG_FATAL("Null pointer to the read conditions object of " << m_pskFolderInputKey.key());
         return StatusCode::FAILURE;
      } else {
         ATH_MSG_DEBUG("Retrieved the AthenaAttributeList");
      }

      if (not readH.range(range)) {
         ATH_MSG_FATAL("Failed to retrieve validity range for " << readH.key());
         return StatusCode::FAILURE;
      } else {
         ATH_MSG_DEBUG("Retrieved the current IOV of the readHandle");
      }

      // get the prescale key from the cool folder 
      hltPsk = (*pskAL)["HltPrescaleKey"].data<cool::UInt32>();
      ATH_MSG_DEBUG( "Extracted the HLT PSK " << hltPsk << " for run " << ctx.eventID().run_number() << " and lb " << ctx.eventID().lumi_block() );
   } else {
      EventIDBase::number_type run = ctx.eventID().run_number();
      EventIDBase start, stop;
      start.set_run_number(run);
      start.set_lumi_block(0);
      stop.set_run_number(run+1);
      stop.set_lumi_block(0);
      range = EventIDRange(start,stop);
   }

   
   TrigConf::HLTPrescalesSet * pss = new TrigConf::HLTPrescalesSet;

   if( m_configSource == "FILE" ) {

      // load the file into the HLT prescales set
      ATH_MSG_DEBUG( "Setting up JsonFileLoader with file " << m_filename.value() );
      TrigConf::JsonFileLoader psLoader;
      psLoader.setLevel(TrigConf::MSGTC::WARNING); 
      ATH_MSG_DEBUG( "Going to load prescales" );
      if( psLoader.loadFile( m_filename, *pss) ) {
         ATH_MSG_INFO( "HLT prescales set successfully loaded from file " << m_filename.value() );
      } else {
         ATH_MSG_ERROR( "Failed loading HLT prescales set from file " << m_filename.value() );
         return StatusCode::FAILURE;
      }

   } else if( hltPsk != 0 ) {

      // load the HLT psk into HLT prescales set
      ATH_MSG_DEBUG( "Setting up TrigDBHLTPrescalesSetLoader with DB connection " << m_dbConnection.value() );
      TrigConf::TrigDBHLTPrescalesSetLoader psLoader(m_dbConnection);
      psLoader.setLevel(TrigConf::MSGTC::WARNING); 
      ATH_MSG_DEBUG( "Going to load prescales" );
      if( psLoader.loadHLTPrescales( hltPsk, *pss ) ) {
         ATH_MSG_INFO( "HLT prescales set successfully loaded from db with key " << m_psk.value() );
      } else {
         ATH_MSG_ERROR( "Failed loading HLT prescales set from db with key " << m_psk.value() );
         return StatusCode::FAILURE;
      }

   } else {

      ATH_MSG_ERROR( "Failed loading HLT prescales set (not reading from FILE and no psk known)" );
      return StatusCode::FAILURE;

   }

   // recording HLT prescales set
   SG::WriteCondHandle<TrigConf::HLTPrescalesSet> writeCondHandle(m_hltPrescaleSetOutputKey, ctx);

   ATH_MSG_INFO("Recording HLT prescales set with range " << range);
   ATH_CHECK(writeCondHandle.record( range, pss ));

   return StatusCode::SUCCESS;
}
