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

// Author: Ketevi A. Assamagan - may 2007

CSCcablingSvc::CSCcablingSvc(const std::string& name, ISvcLocator* sl)
  : ::AthService(name,sl), m_side(2), m_rod(16), m_max(32)
{

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

  ATH_MSG_DEBUG ( " in initialize()" );
  ATH_CHECK( AthService::initialize() );

  // Retrieve geometry config information from the database (RUN1, RUN2, etc...)
  IRDBAccessSvc* rdbAccess = 0;
  ATH_CHECK( service("RDBAccessSvc",rdbAccess) );

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
        } else if(configVal=="RUN2" || configVal=="RUN3" || configVal=="RUN4") {
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
  return StatusCode::SUCCESS;
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
  
  uint16_t subId = (subDetectorId == 0x6A) ? 0 : 1;
  uint16_t onlineColId = subId*this->nROD()+rodId;

  // new offline Rod range 0x10 - 0x1F Oct. 2014
  // double number of rods
  if((rodId & 0x10) == 0x10){
      onlineColId = subId*this->nROD() + rodId - 16;
  } else {
      onlineColId = subId*this->nROD() + rodId;
  }

  return onlineColId;
}

void CSCcablingSvc::hash2Rob(const unsigned int& hashid, uint32_t& robid) const {
   if(m_run1){
     switch (hashid){
        case 0:
        case 16:
        case 8:
        case 24:
           robid = 0x05;
	   break;
        case 1:
        case 17:
        case 9:
        case 25:
           robid = 0x07;
	   break;
        case 2:
        case 18:
        case 10:
        case 26:
           robid = 0x09;
	   break;
        case 3:
        case 19:
        case 11:
        case 27:
           robid = 0x11;
	   break;
        case 4:
        case 20:
        case 12:
        case 28:
           robid = 0x15;
	   break;
        case 5:
        case 21:
        case 13:
        case 29:
           robid = 0x17;
	   break;
        case 6:
        case 22:
        case 14:
        case 30:
           robid = 0x19;
	   break;
        case 7:
        case 23:
        case 15:
        case 31:
           robid = 0x21;
	   break;
        default:
           robid = 0xffff;
           break;
    }
  } else {
     switch (hashid){
        case 16:
        case 24:
           robid = 0x80;
	   break;
        case 17:
        case 25:
           robid = 0x82;
	   break;
        case 18:
        case 26:
           robid = 0x84;
	   break;
        case 19:
        case 27:
           robid = 0x86;
	   break;
        case 20:
        case 28:
           robid = 0x88;
	   break;
        case 21:
        case 29:
           robid = 0x8a;
	   break;
        case 22:
        case 30:
           robid = 0x8c;
	   break;
        case 23:
        case 31:
           robid = 0x8e;
	   break;
        case 0:
        case 8:
           robid = 0x81;
	   break;
        case 1:
        case 9:
           robid = 0x83;
	   break;
        case 2:
        case 10:
           robid = 0x85;
	   break;
        case 3:
        case 11:
           robid = 0x87;
	   break;
        case 4:
        case 12:
           robid = 0x89;
	   break;
        case 5:
        case 13:
           robid = 0x8b;
	   break;
        case 6:
        case 14:
           robid = 0x8d;
	   break;
        case 7:
        case 15:
           robid = 0x8f;
	   break;
        default:
           robid = 0xffff;
           break;
    }
  }
}



void CSCcablingSvc::hash2Rod(const unsigned int& hashid, uint32_t& rodid) const {
   if(m_run1){
     rodid = hashid & 7;
  } else {
    if (hashid<8)
      rodid = hashid;
    else if (hashid<24)
      rodid = hashid-8;
    else
      rodid = hashid-16;
    rodid |= 0x10;
  } 
}

void CSCcablingSvc::hash2SubdetectorId(const unsigned int& hashid, uint32_t& subdetectorid) const {
  if ((hashid >= 8 && hashid <= 15) || (hashid >= 24))
    subdetectorid = 0x69;
  else
    subdetectorid = 0x6a;
}

void CSCcablingSvc::hash2RobFull(const unsigned int& hashid, uint32_t& robid) const {
  uint32_t shortRobID = 0xffff;
  hash2Rob(hashid, shortRobID);
  
  uint32_t subdetectorid = 0;
  hash2SubdetectorId(hashid, subdetectorid);
  
  robid = (subdetectorid << 16) | shortRobID;
}

void CSCcablingSvc::hash2CollectionId(const unsigned int& hashid, uint16_t& collectionid) const {
  uint32_t rodid = 0xffff;
  hash2Rod(hashid, rodid);
  
  uint32_t subdetectorid = 0;
  hash2SubdetectorId(hashid, subdetectorid);
    
  collectionid = collectionId(subdetectorid, rodid);
}
