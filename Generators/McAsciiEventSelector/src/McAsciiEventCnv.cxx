///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiEventCnv.cxx 
// Implementation file for class McAsciiEventCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StorableConversions.h"

// EventInfo includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"

// Package includes
#include "McAsciiEventCnv.h"
#include "McAsciiAddress.h"
#include "McAsciiCnvSvc.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

McAsciiEventCnv::McAsciiEventCnv( ISvcLocator* svcLoc ) :
  Converter( McAsciiCnvSvc::storageType(),
	     EventInfo::classID(),
	     svcLoc )
{}

// Destructor
///////////////
McAsciiEventCnv::~McAsciiEventCnv()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

long
McAsciiEventCnv::storageType() 
{ 
  return McAsciiCnvSvc::storageType();
}
 
const CLID&
McAsciiEventCnv::classID()
{ 
  return EventInfo::classID(); 
}

StatusCode
McAsciiEventCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) 
{
  McAsciiAddress *pMcAddr;
  EventType*  peT = new EventType();
  peT->add_type(EventType::IS_SIMULATION);
  peT->set_user_type("McEvent");
  if ((pMcAddr = dynamic_cast<McAsciiAddress*>(pAddr)) &&
      (pObj = SG::asStorable (new EventInfo(
					    new EventID(pMcAddr->runNumber(), 
							pMcAddr->eventNumber(),
							pMcAddr->timeStamp()), 
					    peT) )   //owned by EventInfo --> TDS 
       ) ) {
    return StatusCode::SUCCESS;
  } else {
    return StatusCode::FAILURE;
  }
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

