/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#include "./L1MenuCondAlg.h"
#include "TrigConfIO/JsonFileLoader.h"

TrigConf::L1MenuCondAlg::L1MenuCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode
TrigConf::L1MenuCondAlg::initialize() {

   ATH_MSG_INFO("initialize");
   ATH_CHECK(m_l1MenuOutputKey.initialize());

   if( m_inputType == "none" ) {
      renounce(m_l1MenuOutputKey);
      ATH_MSG_INFO("Configured to not provide an L1 trigger menu"); 
   } else if ( m_inputType == "file" ) {
      ATH_MSG_INFO("Configured to provide an L1 trigger menu from " << m_fileName); 
   }
   return StatusCode::SUCCESS;
}

StatusCode
TrigConf::L1MenuCondAlg::execute(const EventContext& ctx) const {

   if( m_inputType == "none" ) {
      return StatusCode::SUCCESS; 
   }

   SG::WriteCondHandle<TrigConf::L1Menu> writeCondHandle(m_l1MenuOutputKey, ctx);

   if(writeCondHandle.isValid()) {
      ATH_MSG_DEBUG("CondHandle " << writeCondHandle.fullKey() << " is already valid.");
      return StatusCode::SUCCESS; 
   } else {
      ATH_MSG_DEBUG("CondHandle " << writeCondHandle.fullKey() << " not valid. Generate new one.");
   }

   // load the json file into TrigConf::L1Menu
   TrigConf::JsonFileLoader fileLoader;
   fileLoader.setLevel(TrigConf::MSGTC::WARNING); 
   TrigConf::L1Menu * l1menu = new TrigConf::L1Menu;
   if( m_inputType == "file" ) {
      if( fileLoader.loadFile( m_fileName, *l1menu ) ) {
         ATH_MSG_INFO( "Loaded L1 menu file " << m_fileName );
      } else {
         ATH_MSG_ERROR( "Failed loading L1 menu file " << m_fileName.value() );
         return StatusCode::FAILURE;
      }
   }

   // create EventIDRange for the full run
   EventIDBase::number_type run = ctx.eventID().run_number();
   EventIDBase start, stop;
   start.set_run_number(run);
   start.set_lumi_block(0);
   stop.set_run_number(run+1);
   stop.set_lumi_block(0);
   EventIDRange range(start,stop);

   ATH_MSG_INFO("Recording L1 menu with range " << range);
   ATH_CHECK(writeCondHandle.record( range, l1menu ));

   return StatusCode::SUCCESS;
}
