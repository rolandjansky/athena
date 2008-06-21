/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "CSCcabling/CSCcablingSvc.h"
#include <cassert>

// Author: Ketevi A. Assamagan - may 2007

CSCcablingSvc::CSCcablingSvc(const std::string& name, ISvcLocator* sl)
  : Service(name,sl) {

  m_side = 2;
  m_rod  = 8;
  m_max  = m_side * m_rod;
}

CSCcablingSvc::~CSCcablingSvc() {}

StatusCode CSCcablingSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
 
  if( IID_ICSCcablingSvc.versionMatch(riid) ) { 
      *ppvIF = (CSCcablingSvc*)this; 
  } else { 
    return Service::queryInterface(riid, ppvIF); 
  }

  return StatusCode::SUCCESS;
} 

StatusCode CSCcablingSvc::initialize() { 

  m_log = new MsgStream(msgSvc(), name());
  *m_log << MSG::DEBUG << " in initialize()" << endreq;
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;

  StatusCode sc;

  sc = Service::initialize();

  if ( sc.isFailure() ) {
    *m_log << MSG::ERROR << "Service::initialise() failed" << endreq;
    return sc;
  }

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

  return check;
}

/** check that we have the correct online ROD id */
bool CSCcablingSvc::is_rodId (const uint16_t rodId ) const {

  bool check = rodId == 0x5  || rodId == 0x7  || rodId == 0x9  || rodId == 0x11 ||
               rodId == 0x15 || rodId == 0x17 || rodId == 0x19 || rodId == 0x21;
  return check;
}

/** check that we have the correct offline ROD id */
bool CSCcablingSvc::is_offlineRodId (const uint16_t rodId ) const {

  bool check = rodId == 0x0  || rodId == 0x1  || rodId == 0x2  || rodId == 0x3 ||
               rodId == 0x4  || rodId == 0x5  || rodId == 0x6  || rodId == 0x7;
  return check;
}

/** calculate the collection Identifier */
uint16_t CSCcablingSvc::collectionId(const uint16_t subDetectorId, const uint16_t rodId) const {
  
  assert ( subDetectorId == 0x6A || subDetectorId == 0x69 );
  assert ( this->is_offlineRodId(rodId) );

  uint16_t subId = (subDetectorId == 0x6A) ? 0 : 1;
  uint16_t onlineColId = subId*this->nROD()+rodId;

  assert ( onlineColId < 16 );
  
  return onlineColId;
}


