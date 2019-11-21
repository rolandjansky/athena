/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#include "./HLTPrescaleCondAlg.h"
#include "TrigConfIO/TrigDBHLTPrescalesSetLoader.h"
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "CoolKernel/types.h"

#include <memory>

TrigConf::HLTPrescaleCondAlg::HLTPrescaleCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   AthReentrantAlgorithm(name, pSvcLocator)
{}


TrigConf::HLTPrescalesSet *
TrigConf::HLTPrescaleCondAlg::createFromFile( const std::string & filename ) const {
   auto pss = new HLTPrescalesSet;
   // load the file into the HLT prescales set
   ATH_MSG_DEBUG( "Setting up JsonFileLoader with file " << filename );
   TrigConf::JsonFileLoader psLoader;
   psLoader.setLevel(TrigConf::MSGTC::WARNING); 
   ATH_MSG_DEBUG( "Going to load prescales" );
   if( psLoader.loadFile( filename, *pss) ) {
      ATH_MSG_INFO( "HLT prescales set successfully loaded from file " << filename );
   } else {
      ATH_MSG_ERROR( "Failed loading HLT prescales set from file " << filename );
      delete pss;
      pss = nullptr;
   }
   return pss;
}


TrigConf::HLTPrescalesSet *
TrigConf::HLTPrescaleCondAlg::createFromDB( unsigned int psk ) const {
   auto pss = new HLTPrescalesSet;
   // load the HLT psk into the HLT prescales set
   ATH_MSG_DEBUG( "Setting up TrigDBHLTPrescalesSetLoader with DB connection " << m_dbConnection.value() );
   TrigConf::TrigDBHLTPrescalesSetLoader psLoader(m_dbConnection);
   psLoader.setLevel(TrigConf::MSGTC::WARNING); 
   ATH_MSG_DEBUG( "Going to load prescales" );
   if( psLoader.loadHLTPrescales( psk, *pss ) ) {
      ATH_MSG_INFO( "HLT prescales set successfully loaded from db with key " << psk );
   } else {
      ATH_MSG_ERROR( "Failed loading HLT prescales set from db with key " << psk );
      delete pss;
      pss = nullptr;
   }
   return pss;
}



StatusCode
TrigConf::HLTPrescaleCondAlg::initialize() {

   ATH_MSG_DEBUG("HLTPrescaleCondAlg::initialize()");

   ATH_CHECK(m_pskFolderInputKey.initialize());
   if( m_configSource == "FILE" || m_configSource == "DB" ) {
      renounce( m_pskFolderInputKey ); // need to disable updates when the COOL folder changes
   }

   ATH_CHECK(m_hltPrescalesSetOutputKey.initialize());

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

   ATH_MSG_DEBUG(m_configSource);
   ATH_MSG_DEBUG(m_dbConnection);
   ATH_MSG_DEBUG(m_psk);
   ATH_MSG_DEBUG(m_filename);

   if( m_configSource == "FILE" ) {

      // index 0 indicates that the configuration is from a file, a DB
      // PSK is greater than 0
      m_pssMap[0] = createFromFile( m_filename );

   } else if( m_psk != 0 ) {

      // this is for the case where the reading from the DB was
      // configured and also when we read from COOL online and get a
      // PSK through the JobOptionsSvc
      m_pssMap[m_psk] = createFromDB( m_psk );

   }

   return StatusCode::SUCCESS;
}

StatusCode
TrigConf::HLTPrescaleCondAlg::execute(const EventContext& ctx) const {

   ATH_MSG_DEBUG("HLTPrescaleCondAlg::execute with lb " << ctx.eventID().lumi_block());

   unsigned int hltPsk = m_psk;
   EventIDRange range;

   if(m_configSource == "COOL") {
      // get prescale key and range from COOL
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
      ATH_MSG_DEBUG( "Extracted the HLT PSK " << hltPsk << " for run " << ctx.eventID().run_number()
                     << " and lb " << ctx.eventID().lumi_block() );
   } else {

      // in case of reading from DB or from FILE, the EventID range is always the full run
      EventIDBase::number_type run = ctx.eventID().run_number();
      EventIDBase start, stop;
      start.set_run_number(run);
      start.set_lumi_block(0);
      stop.set_run_number(run+1);
      stop.set_lumi_block(0);
      range = EventIDRange(start,stop);

   }


   HLTPrescalesSet * pss (nullptr);
   if( m_configSource == "FILE" ) {

      pss = m_pssMap.at(0);

   } else if ( hltPsk != 0 ) {

      auto pssi = m_pssMap.find( hltPsk );

      if( pssi == m_pssMap.end()) {

         m_pssMap[hltPsk] = createFromDB( hltPsk );

         pss = m_pssMap.at( hltPsk );
      }

   } else {

      ATH_MSG_ERROR( "Failed loading HLT prescales set (not reading from FILE and no psk known)" );
      return StatusCode::FAILURE;

   }

   // record HLT prescales set
   SG::WriteCondHandle<TrigConf::HLTPrescalesSet> writeCondHandle(m_hltPrescalesSetOutputKey, ctx);

   ATH_MSG_INFO("Recording HLT prescales set with range " << range);
   ATH_CHECK( writeCondHandle.record( range, new HLTPrescalesSet(*pss) ) );

   return StatusCode::SUCCESS;
}
