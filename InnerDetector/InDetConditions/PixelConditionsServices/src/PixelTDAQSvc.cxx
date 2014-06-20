/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelTDAQSvc.h"

#include "StoreGate/StoreGate.h"

#include "InDetIdentifier/PixelID.h"
//#include "PixelCalibAlgs/PixelConvert.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

// CoralDB
#include "CoralDB/CoralDB.h"
#include "CoralDB/Alias.h"
#include "CoralDB/Connection.h"


PixelTDAQSvc::PixelTDAQSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  m_pc("PixelCablingSvc", name),
  calback_calls(0),
  print(0),
  m_modulelistkey("/TDAQ/EnabledResources/ATLAS/PIXEL/Modules"),
  m_atrlistcol(0),
  first_lumiblock_filled(false),
  print_disabled(true),
  always_disabled_module_name("L2_B11_S1_A6_M4A")
  {

  all_modules.clear();
  disabled_modules.clear();
  disabled_modules_first_lumiblock.clear();
  disabled_modules_last_lumiblock.clear();

  declareProperty("ModuleListKey", m_modulelistkey, "Key of pixel module list.");
  declareProperty("PrintDisabledModules", print_disabled = true);
}

PixelTDAQSvc::~PixelTDAQSvc(){}

StatusCode PixelTDAQSvc::queryInterface(const InterfaceID& riid, void** ppvIf){

  if(interfaceID() == riid){
    *ppvIf = dynamic_cast< PixelTDAQSvc* > (this);
  }
  else if(IPixelTDAQSvc::interfaceID() == riid){
    *ppvIf = dynamic_cast< IPixelTDAQSvc* > (this);
  }
  else{
    return AthService::queryInterface(riid, ppvIf);
  }

  addRef();

  return StatusCode::SUCCESS;
}

StatusCode PixelTDAQSvc::initialize(){

  ATH_MSG_INFO("Initializing PixelTDAQSvc");

  StatusCode sc = m_detStore.retrieve();
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve detector store");
    return StatusCode::FAILURE;
  }
  sc = m_pc.retrieve();
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve pc");
    return StatusCode::FAILURE;
  }
  //Register callback to update the list of modules
  //whenever the CondAttrListCollection is updated from the Database
  const DataHandle<CondAttrListCollection> attrListColl;
  sc = m_detStore->regFcn(&IPixelTDAQSvc::IOVCallBack,
                          dynamic_cast<IPixelTDAQSvc*>(this),
                          attrListColl, m_modulelistkey);

  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to register callback");
    return StatusCode::FAILURE;
  }

  sc = m_detStore->retrieve( m_pixelID, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelTDAQSvc::finalize(){

  ATH_MSG_INFO("Finalizing PixelTDAQSvc");
  if(print_disabled)
    print_disabled_modules();
  return StatusCode::SUCCESS;
}

StatusCode PixelTDAQSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys)){

  calback_calls++;

  ATH_MSG_DEBUG("                     -----   in  Callback   -----");
  for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
    ATH_MSG_DEBUG("IOVCALLBACK for key " << *key << " number " << I<<"\n\t\t\t\t\t-----mtst-----\n");


  //usig TDAQ
  StatusCode sc = m_detStore->retrieve(m_atrlistcol, m_modulelistkey);
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve CondAttrListCollection");
    return StatusCode::FAILURE;
  }

  //update disabled_modules
  disabled_modules.clear();
  disabled_modules_last_lumiblock.clear();
  for(CondAttrListCollection::name_const_iterator nameIt
          = m_atrlistcol->name_begin();
      nameIt != m_atrlistcol->name_end(); ++nameIt){

    if(m_atrlistcol->chanAttrListPair(nameIt->first) ==  m_atrlistcol->end()){
      disabled_modules.insert(m_pc->getOfflineIdFromDCS(nameIt->second));

      //set disabled_modules_first_lumiblock
      if(!first_lumiblock_filled)
        disabled_modules_first_lumiblock.insert(nameIt->second);

      //and disabled_modules_last_lumiblock
      disabled_modules_last_lumiblock.insert(nameIt->second);
    }
  }
  //add always disabled module
  if(!first_lumiblock_filled){
    disabled_modules_first_lumiblock.insert(always_disabled_module_name);
    first_lumiblock_filled = true;
  }
  disabled_modules_last_lumiblock.insert(always_disabled_module_name);

  //fill all_modules
  if(all_modules.size() == 0){
    ATH_MSG_DEBUG("                                    ----- filling all_modules\n");

    for(CondAttrListCollection::name_const_iterator nameIt
          = m_atrlistcol->name_begin();
        nameIt != m_atrlistcol->name_end(); ++nameIt)
      all_modules.insert(m_pc->getOfflineIdFromDCS(nameIt->second));
  }
  //add always disabled module
  //all_modules.insert(always_disabled_module_offlineid);

  ATH_MSG_DEBUG("                     -----   DONE  Callback "<<calback_calls<<"  -----");

  return StatusCode::SUCCESS;
}

bool PixelTDAQSvc::tdaq_module_enabled(const Identifier& moduleId){

  if(print < calback_calls)
    ATH_MSG_DEBUG("                     -----   in  tdaq_module_enabled   -----"
                  << moduleId << " : " << moduleId.getString());

  //check for always disabled module
  if(moduleId == always_disabled_module_offlineid){//52903936){// module_name == "L2_B11_S1_A6_M4A"
    ATH_MSG_DEBUG("ALWAYS DISABLED MODULE " << always_disabled_module_name
                  << " offlineID: " << moduleId);
    return false;
  }

  if(print < calback_calls)
    ATH_MSG_DEBUG("                     -----   in  tdaq_module_enabled chek  -----");
  ATH_MSG_DEBUG("Identifier: " << moduleId);

  if(disabled_modules.find(moduleId) != disabled_modules.end()){
    ATH_MSG_DEBUG("disabled: " << moduleId);
    return false;
  }

  ATH_MSG_DEBUG("enabled: " << moduleId);

  if(print < calback_calls)
    ATH_MSG_DEBUG("                     -----   in  tdaq_module_enabled ENABLED -----");

  print = calback_calls;

  return true;
}

bool PixelTDAQSvc::tdaq_module_enabled(const IdentifierHash & elementHash){

  ATH_MSG_DEBUG(" ----------  in  tdaq_module_enabled  elementHash:" 
                << elementHash);
  Identifier moduleId = m_pixelID->wafer_id(elementHash);
  return tdaq_module_enabled(moduleId);
}

void PixelTDAQSvc::print_disabled_modules(){

  ATH_MSG_INFO(disabled_modules_first_lumiblock.size()
               << " disabled modules in the first processed lumi block:");
  for(std::set<std::string>::const_iterator i
        = disabled_modules_first_lumiblock.begin();
      i != disabled_modules_first_lumiblock.end(); ++i)
    ATH_MSG_INFO(" | " << *i);

  ATH_MSG_INFO(disabled_modules_last_lumiblock.size()
               << " disabled modules in the last processed lumi block:");
  for(std::set<std::string>::const_iterator i
        = disabled_modules_last_lumiblock.begin();
      i != disabled_modules_last_lumiblock.end(); ++i)
    ATH_MSG_INFO(" | " << *i);

}
