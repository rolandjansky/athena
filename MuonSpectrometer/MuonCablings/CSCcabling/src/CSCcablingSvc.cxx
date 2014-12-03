/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "CSCcabling/CSCcablingSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include <cassert>

// Author: Ketevi A. Assamagan - may 2007

CSCcablingSvc::CSCcablingSvc(const std::string& name, ISvcLocator* sl)
  : ::AthService(name,sl), m_side(2), m_rod(16), m_max(32),
    m_log(0), m_debug(false), m_verbose(false) {

  declareProperty("Run1Cabling", m_run1 = false);
}

CSCcablingSvc::~CSCcablingSvc() {}

StatusCode CSCcablingSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
 
  if( IID_ICSCcablingSvc.versionMatch(riid) ) { 
      *ppvIF = (CSCcablingSvc*)this; 
  } else { 
    return ::AthService::queryInterface(riid, ppvIF); 
  }

  return StatusCode::SUCCESS;
} 

StatusCode CSCcablingSvc::initialize() { 

  m_log = new MsgStream(msgSvc(), name());
  *m_log << MSG::INFO << " in initialize() with name " << this->name() << endreq;
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;

  if(!::AthService::initialize().isSuccess()){
    *m_log << MSG::ERROR << "AthService::initialise() failed" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve geometry config information from the database (RUN1, RUN2, etc...)
  IRDBAccessSvc* rdbAccess = 0;
  StatusCode result = service("RDBAccessSvc",rdbAccess);

  if(result.isFailure()) {
     ATH_MSG_ERROR("Unable to get RDBAccessSvc");
     return result;
  }

  if(!rdbAccess->connect()) {
     ATH_MSG_ERROR("Unable to connect to the Geometry DB");
     return StatusCode::FAILURE;
  }

  const IGeoModelSvc* geoModel = 0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not locate GeoModelSvc"  );
  } else {
      // check the DetDescr version
     std::string atlasVersion = geoModel->atlasVersion();

     IRDBRecordset_ptr atlasCommonRec = rdbAccess->getRecordsetPtr("AtlasCommon",atlasVersion,"ATLAS");
     if(atlasCommonRec->size()==0) {
        m_run1 = true;
     } else {
        std::string configVal = (*atlasCommonRec)[0]->getString("CONFIG");
        if(configVal=="RUN1"){
          m_run1 = true;
        } else if(configVal=="RUN2") {
          m_run1 = false;
        } else {
          ATH_MSG_FATAL("Unexpected value for geometry config read from the database: " << configVal);
          return StatusCode::FAILURE;
        }
     }
  }
  m_rod  = (m_run1 ? 8 : 16);
  m_max  = m_side * m_rod;

  return StatusCode::SUCCESS;
}

StatusCode CSCcablingSvc::finalize() { 
  StatusCode sc = StatusCode::SUCCESS;
  delete m_log;
  return sc;
}

/** map offline ROD identifier to online ID */
bool CSCcablingSvc::onlineId(const uint16_t subDetectorID, const uint16_t offlineID, uint32_t& rodId) const {

   bool check = subDetectorID == 0x6A || subDetectorID == 0x69;
   uint16_t id = 0xFFFF;
   check = check && this->onlineId(offlineID, id);
   rodId = (subDetectorID << 16) | id;
   return check;
}

bool CSCcablingSvc::onlineId(const uint16_t offlineID, uint16_t& rodId) const {

   bool check = true;
   switch ( offlineID ) {
     case 0x0 : 
	rodId = 0x0005;
	break;
     case 0x1 : 
	rodId = 0x0007;
	break;
     case 0x2 : 
	rodId = 0x0009;
	break;
     case 0x3 : 
	rodId = 0x0011;
	break;
     case 0x4 : 
	rodId = 0x0015; // slot 13 reserved
	break;
     case 0x5 : 
	rodId = 0x0017;
	break;
     case 0x6 : 
	rodId = 0x0019;
	break;
     case 0x7 : 
	rodId = 0x0021;
	break;
     default : 
	check = false;
	rodId = 0xFFFF;
   } 

   if((offlineID & 0x1F) & 0x10){
     check = true;
     // uses Rod Ids in 0x10 - 0x1F 
     rodId = 0x80 | (0xF & offlineID);
   }
   return check;
}

/** reverse map of online ID into offline ROD ID */
bool CSCcablingSvc::offlineId(const uint32_t onlineIdentifier, uint16_t& rodId ) const {
  bool check = true;
  uint16_t id = onlineIdentifier & 0xFFFF;
  switch ( id ) {
    case 0x0005 : 
	rodId = 0x0;
	break;
    case 0x0007 : 
	rodId = 0x1;
	break;
    case 0x0009 : 
	rodId = 0x2;
	break;
    case 0x0011 : 
	rodId = 0x3;
	break;
    case 0x0015 : 
	rodId = 0x4; // slot 13 reserved
	break;
    case 0x0017 : 
	rodId = 0x5;	
	break;
    case 0x0019 : 
	rodId = 0x6;
	break;
    case 0x0021 : 
	rodId = 0x7;
	break;
    case 0x000A : 
	rodId = 0x0; // cosmic test simulation
	break;
    case 0x000C : 
	rodId = 0x1; // cosmic test simulation
	break; 
    default : 
	check = false;
	rodId = 0xFFFF;
  }
  // new online Rod 0x80 - 0x8F range Oct. 2014
  if(!check && (id & 0x70) == 0){
     check = true;
     if( id & 1 ) {
        rodId = (((id & 0xf) >> 1) | 0x10);
     } else {
        rodId = (((id & 0xf) >> 1) | 0x18);
     }
  }

  return check;
}

/** check that we have the correct online ROD id */
bool CSCcablingSvc::is_rodId (const uint16_t rodId ) const {

  bool check = rodId == 0x5  || rodId == 0x7  || rodId == 0x9  || rodId == 0x11 ||
               rodId == 0x15 || rodId == 0x17 || rodId == 0x19 || rodId == 0x21;

  // new online Rod 0x80 - 0x8F range Oct. 2014
  if((rodId & 0x70) == 0) check = true;

  return check;
}

/** check that we have the correct offline ROD id */
bool CSCcablingSvc::is_offlineRodId (const uint16_t rodId ) const {

  bool check = rodId == 0x0  || rodId == 0x1  || rodId == 0x2  || rodId == 0x3 ||
               rodId == 0x4  || rodId == 0x5  || rodId == 0x6  || rodId == 0x7;

  // new offline Rod range 0x10 - 0x1F Oct. 2014
  if((0x10 & rodId)==0x10) check = true;
  return check;
}

/** calculate the collection Identifier */
uint16_t CSCcablingSvc::collectionId(const uint16_t subDetectorId, const uint16_t rodId) const {
  
  assert ( subDetectorId == 0x6A || subDetectorId == 0x69 );
  assert ( this->is_offlineRodId(rodId) );

  uint16_t subId = (subDetectorId == 0x6A) ? 0 : 1;
  uint16_t onlineColId = subId*this->nROD()+rodId;

  // new offline Rod range 0x10 - 0x1F Oct. 2014
  // double number of rods
  if((rodId & 0x10) == 0x10){
      onlineColId = subId*this->nROD() + rodId - 16;
  } else {
      onlineColId = subId*this->nROD() + rodId;
  }

  //assert ( onlineColId < 16 );
  
  return onlineColId;
}
