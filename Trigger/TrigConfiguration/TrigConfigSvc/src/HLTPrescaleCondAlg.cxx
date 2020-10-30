/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


std::shared_ptr<TrigConf::HLTPrescalesSet>
TrigConf::HLTPrescaleCondAlg::createFromFile( const std::string & filename ) const {
   auto pss = std::make_shared<HLTPrescalesSet>();
   // load the file into the HLT prescales set
   ATH_MSG_DEBUG( "Setting up JsonFileLoader with file " << filename );
   TrigConf::JsonFileLoader psLoader;
   psLoader.setLevel(TrigConf::MSGTC::WARNING); 
   ATH_MSG_DEBUG( "Going to load prescales" );
   if( psLoader.loadFile( filename, *pss) ) {
      ATH_MSG_INFO( "HLT prescales set successfully loaded from file " << filename );
   } else {
      pss = nullptr;
   }
   return pss;
}


std::shared_ptr<TrigConf::HLTPrescalesSet>
TrigConf::HLTPrescaleCondAlg::createFromDB( unsigned int psk, bool isRun3 ) const {
   if( ! isRun3 ) {
      ATH_MSG_WARNING( "Currently it is not possible to load run 2 prescale sets from the database. Will not load HLT psk " << psk ); 
      return nullptr;
   }
   auto pss = std::make_shared<HLTPrescalesSet>();
   // load the HLT psk into the HLT prescales set
   ATH_MSG_DEBUG( "Setting up TrigDBHLTPrescalesSetLoader with DB connection " << m_dbConnection.value() );
   TrigConf::TrigDBHLTPrescalesSetLoader psLoader(m_dbConnection);
   psLoader.setLevel(TrigConf::MSGTC::WARNING); 
   ATH_MSG_DEBUG( "Going to load prescales" );
   try {
      psLoader.loadHLTPrescales( psk, *pss );
   }
   catch(std::exception & e) {
      ATH_MSG_WARNING( "Failed loading HLT prescales set from db with key " << psk ); // will be made an error later
      ATH_MSG_WARNING( e.what() );
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

   if( m_dbConnection == "JOSVC" ) {
      if( m_configSource == "COOL" || m_configSource == "DB" ) {
         if( auto joSvc = serviceLocator()->service<TrigConf::IJobOptionsSvc>( "JobOptionsSvc" ) ) {
            if( joSvc->hltPrescaleKey()>0 ) {
               m_psk = joSvc->hltPrescaleKey();
               m_dbConnection = joSvc->server();
               ATH_MSG_INFO("Set psk to " << m_psk <<  " and db connection to " << m_dbConnection );
            }
         } else {
            ATH_MSG_DEBUG("Did not locate TrigConf::IJobOptionsSvc");
         }
      }
   }

   ATH_MSG_INFO(m_configSource);
   ATH_MSG_INFO(m_dbConnection);
   ATH_MSG_INFO(m_psk);
   ATH_MSG_INFO(m_filename);

   if( m_configSource == "FILE" ) {

      // index 0 indicates that the configuration is from a file, a DB
      // PSK is greater than 0
      std::shared_ptr<HLTPrescalesSet> pss = createFromFile( m_filename );
      if( pss == nullptr ) {
         ATH_MSG_ERROR( "Failed loading HLT prescales set from the file " << m_filename );
         return StatusCode::FAILURE;
      }
      m_pssMap[0] = pss;

   } else if( m_psk != 0u ) {

      // this is for the case where the reading from the DB was
      // configured and also when we read from COOL online and get a
      // PSK through the JobOptionsSvc
      std::shared_ptr<HLTPrescalesSet> pss = createFromDB( m_psk, true );
      if( pss == nullptr ) {
         ATH_MSG_ERROR( "Failed loading HLT prescales set " << m_psk << " from the database" );
         return StatusCode::FAILURE;
      }
      m_pssMap[m_psk] = pss;
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
      }
      if (not readH.range(range)) {
         ATH_MSG_FATAL("Failed to retrieve validity range for " << readH.key());
         return StatusCode::FAILURE;
      } else {
         ATH_MSG_DEBUG("Read handle has range " << range);
      }
      // get the prescale key from the cool folder 
      hltPsk = (*pskAL)["HltPrescaleKey"].data<cool::UInt32>();
      ATH_MSG_INFO( "Extracted the HLT PSK " << hltPsk << " for run " << ctx.eventID().run_number()
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

   std::shared_ptr<HLTPrescalesSet> pss;

   if( m_configSource == "FILE" ) {

      pss = m_pssMap.at(0);

   } else if ( hltPsk != 0 ) {

      auto pssi = m_pssMap.find( hltPsk );

      if( pssi == m_pssMap.end()) {
         
         bool isRun3 = range.start().run_number()>350000;

         pss = m_pssMap[hltPsk] = createFromDB( hltPsk, isRun3 );

         if( pss == nullptr ) {
            ATH_MSG_ERROR( "Failed loading HLT prescales set from the database" );
            return StatusCode::FAILURE;
         }

      } else {
      
         pss = pssi->second;

      }

   } else {

      ATH_MSG_ERROR( "Failed loading HLT prescales set (not reading from FILE and no psk known)" );
      return StatusCode::FAILURE;

   }

   // record HLT prescales set
   SG::WriteCondHandle<TrigConf::HLTPrescalesSet> writeCondHandle(m_hltPrescalesSetOutputKey, ctx);

   if( pss == nullptr ) {
      ATH_MSG_INFO("Recording empty HLT prescales set with range " << range);
      ATH_CHECK( writeCondHandle.record( range, new HLTPrescalesSet ) );
   } else {
      ATH_MSG_INFO("Recording HLT prescales set with range " << range << " (key = " << hltPsk << ")");
      ATH_CHECK( writeCondHandle.record( range, new HLTPrescalesSet(*pss) ) );
   }

   return StatusCode::SUCCESS;
}
