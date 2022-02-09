/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/ 

#include "./BunchGroupCondAlg.h"
#include "CoolKernel/types.h"
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfIO/TrigDBL1BunchGroupSetLoader.h"

using TrigConf::L1BunchGroupSet;

TrigConf::BunchGroupCondAlg::BunchGroupCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   AthReentrantAlgorithm(name, pSvcLocator)
{}


StatusCode
TrigConf::BunchGroupCondAlg::initialize() {

   ATH_MSG_DEBUG("BunchGroupCondAlg::initialize()");

   ATH_CHECK(m_bgkFolderInputKey.initialize());
   if( m_configSource == "FILE" || m_configSource == "DB" ) {
      renounce( m_bgkFolderInputKey ); // need to disable updates when the COOL folder changes
   }

   ATH_CHECK(m_l1BunchGroupSetOutputKey.initialize());

   ATH_MSG_INFO(m_configSource);
   ATH_MSG_INFO(m_dbConnection);
   ATH_MSG_INFO(m_bgk);
   ATH_MSG_INFO(m_filename);

   if( m_configSource == "FILE" ) {

      // index 0 indicates that the configuration is from a file, a DB
      // BGK is greater than 0
      m_BgsMap.insert(std::make_pair(0u, createFromFile(m_filename)));

   } else if( m_bgk != 0u ) {

      // this is for the case where the reading from the DB was
      // configured and also when we read from COOL online and get a
      // BGK through the JobOptionsSvc
      m_BgsMap.insert(std::make_pair(m_bgk, createFromDB(m_bgk)));

   }

   return StatusCode::SUCCESS;
}

std::shared_ptr<TrigConf::L1BunchGroupSet>
TrigConf::BunchGroupCondAlg::createFromFile( const std::string & filename ) const {
   auto bgs = std::make_shared<L1BunchGroupSet>();
   // load the file
   ATH_MSG_DEBUG( "Setting up JsonFileLoader with file " << filename );
   TrigConf::JsonFileLoader psLoader;
   psLoader.setLevel(TrigConf::MSGTC::WARNING); 
   if( psLoader.loadFile( filename, *bgs) ) {
      bgs->setBGSK(m_bgk);
      ATH_MSG_INFO( "L1 BunchGroup set successfully loaded from file " << filename );
   } else {
      ATH_MSG_WARNING( "Failed loading L1 BunchGroup set from file " << filename ); // will be made an error later
      bgs = nullptr;
   }
   return bgs;
}


std::shared_ptr<TrigConf::L1BunchGroupSet>
TrigConf::BunchGroupCondAlg::createFromDB( unsigned int bgk ) const {
   auto bgs = std::make_shared<L1BunchGroupSet>();
   ATH_MSG_DEBUG( "Setting up TrigDBL1BunchGroupSetLoader with DB connection " << m_dbConnection.value() );
   TrigConf::TrigDBL1BunchGroupSetLoader bgLoader(m_dbConnection);
   bgLoader.setLevel(TrigConf::MSGTC::WARNING); 
   try {
      bgLoader.loadBunchGroupSet( bgk, *bgs );
   }
   catch(std::exception & e) {
      ATH_MSG_WARNING( "Failed loading L1BunchGroup set from db with key " << bgk ); // will be made an error later
      ATH_MSG_WARNING( e.what() );
      bgs = nullptr;
   }
   return bgs;
}


StatusCode
TrigConf::BunchGroupCondAlg::execute(const EventContext& ctx) const {

   ATH_MSG_DEBUG("BunchGroupCondAlg::execute with lb " << ctx.eventID().lumi_block());

   SG::WriteCondHandle<TrigConf::L1BunchGroupSet> writeCondHandle(m_l1BunchGroupSetOutputKey, ctx);
   if (writeCondHandle.isValid()) {
     return StatusCode::SUCCESS;
   }

   unsigned int l1Ggk = m_bgk;
   EventIDRange range;

   if(m_configSource == "COOL") {

      SG::ReadCondHandle<AthenaAttributeList> readH(m_bgkFolderInputKey, ctx);
      ATH_CHECK(readH.isValid());
      const AthenaAttributeList * bgkAL{ *readH };
      if ( bgkAL == nullptr ) {
         ATH_MSG_FATAL("Null pointer to the read conditions object of " << m_bgkFolderInputKey.key());
         return StatusCode::FAILURE;
      }
      if (not readH.range(range)) {
         ATH_MSG_FATAL("Failed to retrieve validity range for " << readH.key());
         return StatusCode::FAILURE;
      } else {
         ATH_MSG_DEBUG("Read handle has range " << range);
      }
      
      l1Ggk = (*bgkAL)["Lvl1BunchGroupConfigurationKey"].data<cool::UInt32>();
      ATH_MSG_INFO( "Extracted the L1 bgk " << l1Ggk << " for run " << ctx.eventID().run_number()
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

   std::shared_ptr<const L1BunchGroupSet> bgs;

   if( m_configSource == "FILE" ) {

      bgs = m_BgsMap.at(0);

   } else if ( l1Ggk != 0 ) {

      auto bgsi = m_BgsMap.find( l1Ggk );

      if( bgsi == m_BgsMap.end()) {

         const auto p = m_BgsMap.insert(std::make_pair( l1Ggk, createFromDB(l1Ggk) ));
         bgs = p.first->second;

      } else {
      
         bgs = bgsi->second;

      }

   } else {

      ATH_MSG_ERROR( "Failed loading L1 BunchGroup set (not reading from FILE and no bgk known)" );
      return StatusCode::FAILURE;

   }

   if( bgs == nullptr ) {
      ATH_MSG_INFO("Recording empty L1BunchGroupSet set with range " << range);
      ATH_CHECK( writeCondHandle.record( range, new L1BunchGroupSet ) );
   } else {
      ATH_MSG_INFO("Recording L1BunchGroupSet set with range " << range << " (key = " << bgs->bgsk() << ")");
      ATH_CHECK( writeCondHandle.record( range, new L1BunchGroupSet(*bgs) ) );
   }

   return StatusCode::SUCCESS;
}



