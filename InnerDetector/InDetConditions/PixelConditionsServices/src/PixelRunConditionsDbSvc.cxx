/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRunConditionsDbSvc.h"
#include "StoreGate/StoreGate.h"
#include "InDetIdentifier/PixelID.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// COOL includes
#include <CoolKernel/IObjectIterator.h>
#include "CoolKernel/IObject.h"

PixelRunConditionsDbSvc::PixelRunConditionsDbSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  calback_calls(0),
  print(0),
  m_connTag(""),
  m_key("/PIXEL/SOR_Params")
  {
  declareProperty("Key", m_key, "Key=/PIXEL/SOR_Params");
}

PixelRunConditionsDbSvc::~PixelRunConditionsDbSvc(){}

StatusCode PixelRunConditionsDbSvc::queryInterface(const InterfaceID& riid, void** ppvIf){

  if(interfaceID() == riid){
    *ppvIf = dynamic_cast< PixelRunConditionsDbSvc* > (this);
  }
  else if(IPixelRunConditionsDbSvc::interfaceID() == riid){
    *ppvIf = dynamic_cast< IPixelRunConditionsDbSvc* > (this);
  }
  else{
    return AthService::queryInterface(riid, ppvIf);
  }

  addRef();

  return StatusCode::SUCCESS;
}

StatusCode PixelRunConditionsDbSvc::initialize(){

  ATH_MSG_INFO("Initializing PixelRunConditionsDbSvc");

  StatusCode sc = m_detStore.retrieve();
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve detector store");
    return StatusCode::FAILURE;
  }

  //whenever the CondAttrListCollection is updated from the Database  
  const DataHandle<AthenaAttributeList> attrList;
  sc = m_detStore->regFcn(&IPixelRunConditionsDbSvc::IOVCallBack,
			  dynamic_cast<IPixelRunConditionsDbSvc*>(this),
			  attrList, m_key);

  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to register callback");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelRunConditionsDbSvc::finalize(){

  ATH_MSG_INFO("Finalizing PixelRunConditionsDbSvc");
  return StatusCode::SUCCESS;
}

StatusCode PixelRunConditionsDbSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys)){

  calback_calls++;

  ATH_MSG_INFO("                     -----   in  Callback   -----");
  for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
    ATH_MSG_INFO("HEEEEY! IOVCALLBACK for key " << *key << " number " << I<<"\n\t\t\t\t\t-----mtst-----\n");

  bool contains_key = m_detStore->contains<AthenaAttributeList>(m_key);
  std::cout << "contains_key? " << contains_key << std::endl;
  const AthenaAttributeList* attrlist = 0;
  StatusCode sc = m_detStore->retrieve(attrlist, m_key);
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve AthenaAttributeList");
    return StatusCode::FAILURE;
  }

  m_connTag=(*attrlist)["connTag"].data<std::string>();
  ATH_MSG_INFO( "                     -----   conntag "<< m_connTag  <<"  -----" );

  ATH_MSG_INFO("                     -----   DONE  Callback "<<calback_calls<<"  -----");

  return StatusCode::SUCCESS;
}


