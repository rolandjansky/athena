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
  m_callback_calls(0),
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

  //whenever the CondAttrListCollection is updated from the Database  
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", name());
  ATH_CHECK( detStore.retrieve() );
  const DataHandle<AthenaAttributeList> attrList;
  ATH_CHECK( detStore->regFcn(&IPixelRunConditionsDbSvc::IOVCallBack,
                              dynamic_cast<IPixelRunConditionsDbSvc*>(this),
                              attrList, m_key) );

  return StatusCode::SUCCESS;
}

StatusCode PixelRunConditionsDbSvc::finalize(){

  ATH_MSG_INFO("Finalizing PixelRunConditionsDbSvc");
  return StatusCode::SUCCESS;
}

StatusCode PixelRunConditionsDbSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys)){

  m_callback_calls++;

  ATH_MSG_INFO("                     -----   in  Callback   -----");
  for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
    ATH_MSG_INFO("HEEEEY! IOVCALLBACK for key " << *key << " number " << I<<"\n\t\t\t\t\t-----mtst-----\n");

  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", name());
  ATH_CHECK( detStore.retrieve() );
  //bool contains_key = detStore()->contains<AthenaAttributeList>(m_key);
  //std::cout << "contains_key? " << contains_key << std::endl;
  const AthenaAttributeList* attrlist = 0;
  ATH_CHECK( detStore->retrieve(attrlist, m_key) );

  m_connTag=(*attrlist)["connTag"].data<std::string>();
  ATH_MSG_INFO( "                     -----   conntag "<< m_connTag  <<"  -----" );

  ATH_MSG_INFO("                     -----   DONE  Callback "<<m_callback_calls<<"  -----");

  return StatusCode::SUCCESS;
}


