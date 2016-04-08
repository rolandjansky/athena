/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestPixelModuleDisablingSvc.h"

#include "StoreGate/StoreGate.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/IIncidentSvc.h"

TestPixelModuleDisablingSvc::TestPixelModuleDisablingSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  print(0),
  print_disabled(true)
{
  disabled_modules.clear();

  declareProperty("PrintDisabledModules", print_disabled = true);
  declareProperty("ModuleHashes", m_moduleList);
}

TestPixelModuleDisablingSvc::~TestPixelModuleDisablingSvc(){}

StatusCode TestPixelModuleDisablingSvc::queryInterface(const InterfaceID& riid, void** ppvIf){

  if(interfaceID() == riid){
    *ppvIf = dynamic_cast< TestPixelModuleDisablingSvc* > (this);
  }
  else if(IPixelTDAQSvc::interfaceID() == riid){
    *ppvIf = dynamic_cast< IPixelTDAQSvc* > (this);
  }
  else if(IIncidentListener::interfaceID() == riid){
    *ppvIf = dynamic_cast< IIncidentListener* > (this);
  }
  else{
    return AthService::queryInterface(riid, ppvIf);
  }

  addRef();

  return StatusCode::SUCCESS;
}

StatusCode TestPixelModuleDisablingSvc::initialize(){


  StatusCode sc = m_detStore.retrieve();
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve detector store");
    return StatusCode::FAILURE;
  }

  sc = m_detStore->retrieve( m_pixelID, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }

  /*
    IIncidentSvc* incSvc;
    sc = service( "IncidentSvc", incSvc );
    if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to get the IncidentSvc");
    return StatusCode::FAILURE;
    }
    long int pri=100;
    incSvc->addListener(this, "BeginRun",pri);    
  */

  ATH_MSG_INFO("Initializing " << name() << " and disabling " << m_moduleList.size() << " modules" );
  initModules();
  return StatusCode::SUCCESS;
}


void TestPixelModuleDisablingSvc::initModules(){
  for (auto it=m_moduleList.begin(); it!=m_moduleList.end(); ++it){
    Identifier moduleId = m_pixelID->wafer_id(*it);
    disabled_modules.insert(moduleId);
    ATH_MSG_INFO("inserting " << moduleId.getString() << endreq);
  }
}

void TestPixelModuleDisablingSvc::handle(const Incident& inc) {
  if (inc.type()=="BeginRun") {
    initModules();
  }
}


StatusCode TestPixelModuleDisablingSvc::finalize(){

  ATH_MSG_INFO("Finalizing TestPixelModuleDisablingSvc");
  if(print_disabled)
    print_disabled_modules();
  return StatusCode::SUCCESS;
}


bool TestPixelModuleDisablingSvc::tdaq_module_enabled(const Identifier& moduleId){

  ATH_MSG_DEBUG("                     -----   in  tdaq_module_enabled   -----"
		<< moduleId << " : " << moduleId.getString());
  

  ATH_MSG_DEBUG("Identifier: " << moduleId);

  if(disabled_modules.find(moduleId) != disabled_modules.end()){
    ATH_MSG_DEBUG("disabled: " << moduleId);
    return false;
  }

  ATH_MSG_DEBUG("enabled: " << moduleId);

  ATH_MSG_DEBUG("                     -----   in  tdaq_module_enabled ENABLED -----");

  return true;
}

bool TestPixelModuleDisablingSvc::tdaq_module_enabled(const IdentifierHash & elementHash){

  ATH_MSG_DEBUG(" ----------  in  tdaq_module_enabled  elementHash:" 
                << elementHash);
  Identifier moduleId = m_pixelID->wafer_id(elementHash);
  return tdaq_module_enabled(moduleId);
}

void TestPixelModuleDisablingSvc::print_disabled_modules(){

  for(std::set<Identifier>::const_iterator i
        = disabled_modules.begin();
      i != disabled_modules.end(); ++i)
    ATH_MSG_INFO(" | " << (*i).getString());


}
