/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/ 

#include "./L1PrescaleCondAlg.h"
#include "TrigConfIO/TrigDBL1PrescalesSetLoader.h"
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "CoolKernel/types.h"

#include <memory>

TrigConf::L1PrescaleCondAlg::L1PrescaleCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   AthReentrantAlgorithm(name, pSvcLocator)
{}


std::shared_ptr<TrigConf::L1PrescalesSet>
TrigConf::L1PrescaleCondAlg::createFromFile( const std::string & filename ) const {
   auto pss = std::make_shared<L1PrescalesSet>();
   // load the file into the L1 prescales set
   ATH_MSG_DEBUG( "Setting up JsonFileLoader with file " << filename );
   TrigConf::JsonFileLoader psLoader;
   psLoader.setLevel(TrigConf::MSGTC::WARNING); 
   ATH_MSG_DEBUG( "Going to load prescales" );
   if( psLoader.loadFile( filename, *pss) ) {
      ATH_MSG_INFO( "L1 prescales set successfully loaded from file " << filename );
   } else {
      ATH_MSG_WARNING( "Failed loading L1 prescales set from file " << filename ); // will be made an error later
      pss = nullptr;
   }
   return pss;
}


std::shared_ptr<TrigConf::L1PrescalesSet>
TrigConf::L1PrescaleCondAlg::createFromDB( unsigned int psk, bool isRun3 ) const {
   if( ! isRun3 ) {
      ATH_MSG_WARNING( "Currently it is not possible to load run 2 prescale sets from the database. Will not load L1 psk " << psk ); 
      return nullptr;
   }
   auto pss = std::make_shared<L1PrescalesSet>();
   // load the L1 psk into the L1 prescales set
   ATH_MSG_DEBUG( "Setting up TrigDBL1PrescalesSetLoader with DB connection " << m_dbConnection.value() );
   TrigConf::TrigDBL1PrescalesSetLoader psLoader(m_dbConnection);
   psLoader.setLevel(TrigConf::MSGTC::WARNING); 
   ATH_MSG_DEBUG( "Going to load prescales" );
   try {
      psLoader.loadL1Prescales( psk, *pss );
   }
   catch(std::exception & e) {
      ATH_MSG_WARNING( "Failed loading L1 prescales set from db with key " << psk ); // will be made an error later
      ATH_MSG_WARNING( e.what() );
      pss = nullptr;
   }
   return pss;
}



StatusCode
TrigConf::L1PrescaleCondAlg::initialize() {

   ATH_MSG_DEBUG("L1PrescaleCondAlg::initialize()");

   ATH_CHECK(m_pskFolderInputKey.initialize());
   if( m_configSource == "FILE" || m_configSource == "DB" ) {
      renounce( m_pskFolderInputKey ); // need to disable updates when the COOL folder changes
   }

   ATH_CHECK(m_l1PrescalesSetOutputKey.initialize());

   if( m_configSource == "COOL" && m_dbConnection == "JOSVC" ) {
      if( auto joSvc = serviceLocator()->service<TrigConf::IJobOptionsSvc>( "JobOptionsSvc" ) ) {
         if( joSvc->l1PrescaleKey()>0 ) {
            m_psk = joSvc->l1PrescaleKey();
            m_dbConnection = joSvc->server();
            ATH_MSG_INFO("Set psk to " << m_psk <<  " and db connection to " << m_dbConnection );
         }
      } else {
         ATH_MSG_DEBUG("Did not locate TrigConf::IJobOptionsSvc");
      }
   }

   ATH_MSG_INFO(m_configSource);
   ATH_MSG_INFO(m_dbConnection);
   ATH_MSG_INFO(m_psk);
   ATH_MSG_INFO(m_filename);

   if( m_configSource == "FILE" ) {

      // index 0 indicates that the configuration is from a file, a DB
      // PSK is greater than 0
      m_pssMap[0] = createFromFile( m_filename );

   } else if( m_psk != 0u ) {

      // this is for the case where the reading from the DB was
      // configured and also when we read from COOL online and get a
      // PSK through the JobOptionsSvc
      m_pssMap[m_psk] = createFromDB( m_psk, true );

   }

   return StatusCode::SUCCESS;
}

StatusCode
TrigConf::L1PrescaleCondAlg::execute(const EventContext& ctx) const {

   ATH_MSG_DEBUG("L1PrescaleCondAlg::execute with lb " << ctx.eventID().lumi_block());

   unsigned int l1Psk = m_psk;
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
      l1Psk = (*pskAL)["Lvl1PrescaleConfigurationKey"].data<cool::UInt32>();
      ATH_MSG_INFO( "Extracted the L1 PSK " << l1Psk << " for run " << ctx.eventID().run_number()
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

   std::shared_ptr<L1PrescalesSet> pss;

   if( m_configSource == "FILE" ) {

      pss = m_pssMap.at(0);

   } else if ( l1Psk != 0 ) {

      auto pssi = m_pssMap.find( l1Psk );

      if( pssi == m_pssMap.end()) {

         bool isRun3 = range.start().run_number()>350000;

         pss = m_pssMap[l1Psk] = createFromDB( l1Psk, isRun3 );

      } else {
      
         pss = pssi->second;

      }

   } else {

      ATH_MSG_ERROR( "Failed loading L1 prescales set (not reading from FILE and no psk known)" );
      return StatusCode::FAILURE;

   }

   // record L1 prescales set
   SG::WriteCondHandle<TrigConf::L1PrescalesSet> writeCondHandle(m_l1PrescalesSetOutputKey, ctx);

   if( pss == nullptr ) {
      ATH_MSG_INFO("Recording empty L1 prescales set with range " << range);
      ATH_CHECK( writeCondHandle.record( range, new L1PrescalesSet ) );
   } else {
      ATH_MSG_INFO("Recording L1 prescales set with range " << range);
      ATH_CHECK( writeCondHandle.record( range, new L1PrescalesSet(*pss) ) );
   }

   return StatusCode::SUCCESS;
}
