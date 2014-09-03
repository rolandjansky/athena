/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelReadoutSpeedDbSvc.h"

#include "StoreGate/StoreGate.h"
#include "InDetIdentifier/PixelID.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// COOL includes
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"

PixelReadoutSpeedDbSvc::PixelReadoutSpeedDbSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  calback_calls(0),
  print(0),
  m_dataString(""),
  m_key("/PIXEL/ReadoutSpeed")
  {
  declareProperty("Key", m_key, "Key=/PIXEL/ReadoutSpeed");
}

PixelReadoutSpeedDbSvc::~PixelReadoutSpeedDbSvc(){}

StatusCode PixelReadoutSpeedDbSvc::queryInterface(const InterfaceID& riid, void** ppvIf){

  if(interfaceID() == riid){
    *ppvIf = dynamic_cast< PixelReadoutSpeedDbSvc* > (this);
  }
  else if(IPixelReadoutSpeedDbSvc::interfaceID() == riid){
    *ppvIf = dynamic_cast< IPixelReadoutSpeedDbSvc* > (this);
  }
  else{
    return AthService::queryInterface(riid, ppvIf);
  }

  addRef();

  return StatusCode::SUCCESS;
}

StatusCode PixelReadoutSpeedDbSvc::initialize(){

  ATH_MSG_INFO("Initializing PixelReadoutSpeedDbSvc");

  StatusCode sc = m_detStore.retrieve();
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve detector store");
    return StatusCode::FAILURE;
  }

//   ATH_MSG_INFO("Retrieving Pixel ReadoutSpeed");
  //   const AthenaAttributeList*    attrList     = 0;
//   sc = m_detStore->retrieve(attrList,"/PIXEL/ReadoutSpeed");
//   if (sc.isFailure()) {
//     ATH_MSG_ERROR("Could not retrieve readout speed");
//   }
//   if (0 == attrList) {
//     ATH_MSG_ERROR("Pixel readout speed ptr is 0");
//     return( StatusCode::FAILURE);
//   }
//   else {
//     ATH_MSG_INFO("Succesfully retrieved AthenaAttributeList for Pixel ReadoutSpeed");
//   }

  //  whenever the CondAttrListCollection is updated from the Database  
  const DataHandle<AthenaAttributeList> attrList;
  sc = m_detStore->regFcn(&IPixelReadoutSpeedDbSvc::IOVCallBack,
			  dynamic_cast<IPixelReadoutSpeedDbSvc*>(this),
			  attrList, m_key);

  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to register callback");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelReadoutSpeedDbSvc::finalize(){

  ATH_MSG_INFO("Finalizing PixelReadoutSpeedDbSvc");
  return StatusCode::SUCCESS;
}

StatusCode PixelReadoutSpeedDbSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys)){

  calback_calls++;

  ATH_MSG_INFO("                     -----   in  Callback   -----");
  for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
    ATH_MSG_INFO("IOVCALLBACK for key " << *key << " number " << I<<"\n\t\t\t\t\t-----mtst-----\n");

//  bool contains_key = m_detStore->contains<AthenaAttributeList>(m_key);
  const AthenaAttributeList* attrlist = 0;
  StatusCode sc = m_detStore->retrieve(attrlist, m_key);
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve AthenaAttributeList");
    return StatusCode::FAILURE;
  }

  const coral::Blob& blob=(*attrlist)["readoutspeed_per_ROD"].data<coral::Blob>();
  const char* p = static_cast<const char*>(blob.startingAddress());
  unsigned int len = blob.size();
  m_dataString.resize(len);
  for (unsigned int i = 0; i!=len; ++i) m_dataString[i] = *p++;
  
//   for (unsigned int i = 0; i!=len; ++i) {
//     std::cout << m_dataString[i] ;
//   }
  
  ATH_MSG_INFO("                     -----   DONE  Callback "<<calback_calls<<"  -----");

  return StatusCode::SUCCESS;
}


