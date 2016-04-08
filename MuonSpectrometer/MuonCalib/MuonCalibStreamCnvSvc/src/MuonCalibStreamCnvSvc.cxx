/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files
#include "MuonCalibStreamCnvSvc/MuonCalibStreamCnvSvc.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IOpaqueAddress.h"

#include "StoreGate/StoreGate.h" 

#include "AthenaKernel/IClassIDSvc.h"

//External definitions
long MuonCalibStream_StorageType=0x43;

/// Standard constructor
MuonCalibStreamCnvSvc::MuonCalibStreamCnvSvc(const std::string& name, ISvcLocator* svc):
  ConversionSvc( name, svc, MuonCalibStream_StorageType)//, m_calibEvent(NULL) 
{
  m_initCnvs.push_back("Muon::MdtPrepDataContainer");
  declareProperty("InitCnvs", m_initCnvs ); 
}

/// Standard Destructor
MuonCalibStreamCnvSvc::~MuonCalibStreamCnvSvc() {
}

/// Initialize the service.
StatusCode MuonCalibStreamCnvSvc::initialize() {

  StatusCode sc = ConversionSvc::initialize();
  MsgStream log(messageService(), name());
  if(sc != StatusCode::SUCCESS ) {
    log<<MSG::ERROR << " Cant initialize base class " <<endreq;
    return sc ;
  } else {
    log<<MSG::INFO << " initializing MuonCalibStreamCnvSvc " <<endreq;
  } 

  IClassIDSvc* clidSvc ; 
  sc = service("ClassIDSvc",clidSvc ); 
  if(sc != StatusCode::SUCCESS ) {
    log<<MSG::ERROR << " Cant get ClassIDSvc " <<endreq;
    return sc ;
  }

  // Initialize the converters
  std::vector<std::string>::const_iterator it   = m_initCnvs.begin(); 
  std::vector<std::string>::const_iterator it_e = m_initCnvs.end(); 
  for(; it!=it_e; ++it) {
    CLID id ;
    log<<MSG::DEBUG << " Accessing Converter for   " <<*it<<endreq;
    sc = clidSvc->getIDOfTypeName(*it, id); 
    log<<MSG::DEBUG << " Converter id   " <<id<<endreq;

    if(sc != StatusCode::SUCCESS ) {
      log<<MSG::WARNING << " Can not get CLID for  " <<*it<<endreq;
    } else {
      IConverter* cnv = converter( id ) ;
      if(! cnv) {
	log<<MSG::WARNING << " Can not get converter for  " <<*it<<endreq;
      } else {
	log<<MSG::DEBUG << " converter   " <<cnv->objType()<<" for objects"<<*it<<endreq;
      } 
    }
  } 
  return StatusCode::SUCCESS;
}

/// Query interface
StatusCode MuonCalibStreamCnvSvc::queryInterface(const InterfaceID& riid,
                                                void** ppvInterface) {
  //if ( IMuonCalibStreamEventAccess::interfaceID().versionMatch(riid) ) {
  //  *ppvInterface = (IMuonCalibStreamEventAccess*)this;
  //}
 // else  {
   return ConversionSvc::queryInterface(riid, ppvInterface);
  //}

  //addRef();
  //return StatusCode::SUCCESS;
}

/// Update state of the service
StatusCode MuonCalibStreamCnvSvc::updateServiceState(IOpaqueAddress* pAddress) {

  MsgStream log(messageService(), name());
  if ( 0 != pAddress ) {
    GenericAddress* pAddr = dynamic_cast<GenericAddress*>(pAddress);
    if ( 0 != pAddr ) {
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}
