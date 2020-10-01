/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcConditions/ZdcCablingSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include <cstdlib>
#include "GeoModelInterfaces/IGeoModelSvc.h"

//
//____________________________________________________________________
static const InterfaceID IID_ZdcCablingSvc("ZdcCablingSvc", 1, 0);  
const InterfaceID&  ZdcCablingSvc::interfaceID() 
{ 
  return IID_ZdcCablingSvc;
}

//
//_____________________________________________________________________________
ZdcCablingSvc::ZdcCablingSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_cablingService(0),
  m_detStore("DetectorStore", name)
{
  declareProperty("CablingType", m_cablingType = 0, "Cabling type: 0 - simulation, 1 - 2009");
}

//
//_____________________________________________________________________________
StatusCode 
ZdcCablingSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( ZdcCablingSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ZdcCablingSvc*>(this);
  } 
  else {
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//
//_____________________________________________________________________________
StatusCode
ZdcCablingSvc::initialize()
{
  MsgStream log(msgSvc(),name());
  log <<MSG::DEBUG <<"In initialize() " << endmsg;

  //=== initialize base class
  if(Service::initialize().isFailure()){
    log << MSG::ERROR << "Unable to initialize base class" << endmsg;
  }

  //=== set properties
  if(setProperties().isFailure()){
    log << MSG::ERROR << "Unable to set properties" << endmsg;
  }
  
  //=== 
  if(m_detStore.retrieve().isFailure()){
    log << MSG::ERROR << "Unable to retrieve " << m_detStore << endmsg;
    return StatusCode::FAILURE;
  }
  //=== retrieve all helpers from detector store

  /*
  const ZdcLVL1_ID* zdcL1ID(0);
  if(m_detStore->retrieve(caloID).isFailure()){
    log << MSG::ERROR << "Unable to retrieve ZdcLVL1_ID helper from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  */

  //=== Initialize ZdcCablingService singleton
  m_cablingService = ZdcCablingService::getInstance();
  if(!m_cablingService){
    log << MSG::ERROR << "Cannot get instance of ZdcCablingService"<< endmsg ;
    return StatusCode::FAILURE ;
  }

  IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    log << MSG::ERROR << "Could not locate GeoModelSvc" << endmsg;
  } else {
    // check the DetDescr version
    std::string atlasVersion = geoModel->atlasVersion();
    int geo  = atlasVersion.compare(0,9,"ATLAS-GEO");
    int ibl  = atlasVersion.compare(0,9,"ATLAS-IBL");
    int slhc = atlasVersion.compare(0,10,"ATLAS-SLHC");
    int comm = atlasVersion.compare(0,10,"ATLAS-Comm");

    if (geo == 0 || ibl == 0 || slhc == 0) {
      log << MSG::INFO << "New ATLAS geometry detected: " << atlasVersion << endmsg;
      m_cablingType = 2;
    } else if (comm == 0) {
      log << MSG::INFO << "ATLAS Commissioning geometry detected: " << atlasVersion << endmsg;
      m_cablingType = 2;
    } else {
      log << MSG::INFO << "Old ATLAS geometry detected: " << atlasVersion << endmsg;
      // in this case change cabling type only if it is not set via jobOptions
      if (m_cablingType < -1 || m_cablingType > 3)
        m_cablingType = 0;
      else 
        log << MSG::INFO << "Using cabling type from jobOptions " << endmsg;
    }
  }

  MSG::Level logLevel = log.level();
  if (logLevel <= MSG::VERBOSE) {

    // checking all HWIDs
    std::cout << "===============================" << std::endl;

    // should do something similar for zdc
    /*    
    for (int ros=1; ros<5; ++ros) {
      for (int drawer=0; drawer<64; ++drawer) {
        for (int channel=0; channel<48; ++channel) {
        
          HWIdentifier hwid1 = tileHWID->channel_id(ros,drawer,channel);
          std::cout << " hw1=" << tileHWID->to_string(hwid1,-1);
          Identifier id1 = m_cablingService->h2s_pmt_id(hwid1);
          std::cout << " id1=" << tileID->to_string(id1,-1);
          HWIdentifier hwid2 = m_cablingService->s2h_channel_id(id1);
          if (hwid2 != hwid1) std::cout << " hw1!=hw2=" << tileHWID->to_string(hwid2,-1);
          Identifier id2 = m_cablingService->h2s_pmt_id(hwid2);
          if (id2 != id1) std::cout << " id1!=id2=" << tileID->to_string(id2,-1);
          std::cout << std::endl;
        }
      }
    }
    */
    
    // check all ID's
    /*
    std::cout << "===============================" << std::endl;
    
    for (int side=-1; side<2; side+=2) {
      for (int module=0; module<64; ++module) {
        for (int tower=13; tower<16; tower+=2) {
        
          Identifier id1 = tileID->pmt_id(3, side, module, tower, 3, 0);
          std::cout << " id1=" << tileID->to_string(id1,-1);
          HWIdentifier hw1 = m_cablingService->s2h_channel_id(id1);
          std::cout << " hw1=" << tileHWID->to_string(hw1,-1);
          Identifier id2 = m_cablingService->h2s_pmt_id(hw1);
          if (id2 != id1) std::cout << " id1!=id2=" << tileID->to_string(id2,-1);
          HWIdentifier hw2 = m_cablingService->s2h_channel_id(id2);
          if (hw2 != hw1) std::cout << " hwid1!=hwid2=" << tileHWID->to_string(hw2,-1);
          std::cout << std::endl;
        }
      }
    }
    */

  }
  
  return StatusCode::SUCCESS;
}

//
//_____________________________________________________________________________
StatusCode
ZdcCablingSvc::finalize()
{
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "finalize called, deleting ZdcCablingService instance" << endmsg;
  m_cablingService->deleteInstance();
  return StatusCode::SUCCESS;
}
