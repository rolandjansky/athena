/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#include "./HLTMenuCondAlg.h"
#include "TrigConfIO/JsonFileLoader.h"

TrigConf::HLTMenuCondAlg::HLTMenuCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode
TrigConf::HLTMenuCondAlg::initialize() {

   ATH_MSG_INFO("initialize");
   ATH_CHECK(m_hltMenuOutputKey.initialize());

   if( m_inputType == "none" ) {
      renounce(m_hltMenuOutputKey);
      ATH_MSG_INFO("Configured to not provide an HLT trigger menu"); 
   } else if ( m_inputType == "file" ) {
      ATH_MSG_INFO("Configured to provide an HLT trigger menu from " << m_fileName); 
   }
   return StatusCode::SUCCESS;
}

StatusCode
TrigConf::HLTMenuCondAlg::execute(const EventContext& ctx) const {

   if( m_inputType == "none" ) {
      return StatusCode::SUCCESS; 
   }

   SG::WriteCondHandle<TrigConf::HLTMenu> writeCondHandle(m_hltMenuOutputKey, ctx);

   if(writeCondHandle.isValid()) {
      ATH_MSG_DEBUG("CondHandle " << writeCondHandle.fullKey() << " is already valid.");
      return StatusCode::SUCCESS; 
   } else {
      ATH_MSG_DEBUG("CondHandle " << writeCondHandle.fullKey() << " not valid. Generate new one.");
   }

   // load the json file into TrigConf::HLTMenu
   TrigConf::JsonFileLoader fileLoader;
   fileLoader.setLevel(TrigConf::MSGTC::WARNING); 
   TrigConf::HLTMenu * hltmenu = new TrigConf::HLTMenu;
   if( m_inputType == "file" ) {
      if( fileLoader.loadFile( m_fileName, *hltmenu ).isSuccess() ) {
         ATH_MSG_INFO( "Loaded HLT menu file " << m_fileName );
      } else {
         ATH_MSG_WARNING( "Failed loading HLT menu file " << m_fileName.value() );
         // as we are in a transition phase were not all jobs have a
         // json file yet but should not be aborted, will ignore this
         // problem for now
         // return StatusCode::FAILURE;
         return StatusCode::SUCCESS;
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

   ATH_MSG_INFO("Recording HLT menu with range " << range);
   ATH_CHECK(writeCondHandle.record( range, hltmenu ));

   return StatusCode::SUCCESS;
}
