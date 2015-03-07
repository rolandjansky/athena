/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfigSvc/L1TopoConfigSvc.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "TrigConfStorage/IStorageMgr.h"


using namespace std;

TrigConf::L1TopoConfigSvc::L1TopoConfigSvc(const string& name, ISvcLocator* pSvcLocator) : 
   base_class(name, pSvcLocator),
   m_menu(new TXC::L1TopoMenu)
{
   base_class::declareCommonProperties();
}


TrigConf::L1TopoConfigSvc::~L1TopoConfigSvc()
{}


StatusCode
TrigConf::L1TopoConfigSvc::initialize() {

   CHECK(ConfigSvcBase::initialize());

   if( !fromDB() and m_xmlFile=="NONE" ) {
      ATH_MSG_INFO("xml file set to NONE, will not load L1TopoMenu");
      return StatusCode::SUCCESS;
   }

   CHECK(initStorageMgr());
   
   m_menu->setSMK( m_dbSMKey );

   m_storageMgr->l1topoMenuLoader().setLevel((MSGTC::Level)msgLevel());
   bool loadSuccess = m_storageMgr->l1topoMenuLoader().load( *m_menu );

   CHECK(freeStorageMgr());

   //   m_menu->print();

   return loadSuccess ? StatusCode::SUCCESS : StatusCode::FAILURE;
}


StatusCode
TrigConf::L1TopoConfigSvc::start() {
   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::L1TopoConfigSvc::finalize() {
   return StatusCode::SUCCESS;
}
