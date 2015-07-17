/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConnCoolDbSvc.h"

#include "StoreGate/StoreGate.h"

#include "InDetIdentifier/PixelID.h"
//#include "PixelCalibAlgs/PixelConvert.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// CoralDB
#include "CoralDB/CoralDB.h"
#include "CoralDB/Alias.h"
#include "CoralDB/Connection.h"

//Registration SVC
#include "RegistrationServices/IIOVRegistrationSvc.h"

//#include "PixelCabling/PixelCablingData.h"

PixelConnCoolDbSvc::PixelConnCoolDbSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_regSvc(0),
  m_detStore("DetectorStore", name)
  {
}

PixelConnCoolDbSvc::~PixelConnCoolDbSvc(){}

StatusCode PixelConnCoolDbSvc::queryInterface(const InterfaceID& riid, void** ppvIf){

  if(interfaceID() == riid){
    *ppvIf = dynamic_cast< PixelConnCoolDbSvc* > (this);
  }
  else if(IPixelConnCoolDbSvc::interfaceID() == riid){
    *ppvIf = dynamic_cast< IPixelConnCoolDbSvc* > (this);
  }
  else{
    return AthService::queryInterface(riid, ppvIf);
  }

  addRef();

  return StatusCode::SUCCESS;
}

StatusCode PixelConnCoolDbSvc::initialize(){

  ATH_MSG_INFO("Initializing PixelConnCoolDbSvc");

  StatusCode sc = m_detStore.retrieve();
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve detector store");
    return StatusCode::FAILURE;
  }

  sc = service("IOVRegistrationSvc", m_regSvc);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Unable to find IOVRegistrationSvc");
    return StatusCode::FAILURE;
  }  
  else {
    ATH_MSG_INFO("Found IOVRegistrationSvc ");
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelConnCoolDbSvc::finalize(){

  StatusCode sc = StatusCode::SUCCESS;
  
  unsigned int runNumber = 0;
  sc = m_regSvc->registerIOV("AthenaAttributeList","/PIXEL/PixelCablingData","CablingData",runNumber,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT);
  
  //sc = m_regSvc->registerIOV("AthenaAttributeList", "/PIXEL/PixelCablingData","");
  
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not register in IOV DB for AthenaAttributeList");
    return( StatusCode::FAILURE);
  }
  ATH_MSG_INFO("registered PixelCablingData AthenaAttributeList");

  ATH_MSG_INFO("Finalizing PixelConnCoolDbSvc");
  return StatusCode::SUCCESS;
}


StatusCode PixelConnCoolDbSvc::retrieveData(std::string& dataString){
  ATH_MSG_INFO("Retrieving PixelCablingData");
  
  const AthenaAttributeList*    attrList     = 0;
  StatusCode sc = m_detStore->retrieve(attrList,"/PIXEL/PixelCablingData");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not retrieve PixelCablingData");
  }
  if (0 == attrList) {
    ATH_MSG_ERROR("PixelCablingData ptr is 0");
    return( StatusCode::FAILURE);
  }
  else {
    ATH_MSG_INFO("Succesfully retrieved AthenaAttributeList for PixelCablingData");
  }

  const coral::Blob& blob=(*attrList)["PixelCablingData"].data<coral::Blob>();
  const char* p = static_cast<const char*>(blob.startingAddress());
  unsigned int len = blob.size();
  dataString.resize(len);
  for (unsigned int i = 0; i!=len; ++i) dataString[i] = *p++;
  
  std::cout << "try dataString per char " << std::endl;
  for (unsigned int i = 0; i!=len; ++i) {
    std::cout << dataString[i] ;
  }
  
  std::cout << "" << std::endl;

  std::cout << "try full dataString " << std::endl;
  std::cout << dataString << std::endl;


  return StatusCode::SUCCESS;
}

StatusCode PixelConnCoolDbSvc::record(std::string& dataString){

  ATH_MSG_INFO("Recording PixelCablingData");
  
  coral::AttributeListSpecification* Spec = new coral::AttributeListSpecification();
  Spec->extend("PixelCablingData","blob");
  AthenaAttributeList* List=new AthenaAttributeList(*Spec);

  // special construction to set blob type
  coral::Blob& blob=(*List)["PixelCablingData"].data<coral::Blob>();
  unsigned int len = dataString.size();
  std::cout << "len " << dataString.size() << std::endl;
  blob.resize(len);
  char* p=static_cast<char*>(blob.startingAddress());
  for (unsigned int i=0;i<len;++i) *p++ = dataString[i];

  StatusCode sc = m_detStore->record(List, "/PIXEL/PixelCablingData");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not record PixelCablingData");
    return( StatusCode::FAILURE);
  }
  
  return StatusCode::SUCCESS;
}


