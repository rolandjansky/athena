/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "McEventSelector/McEventCnv.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StorableConversions.h"
#include "McEventSelector/McAddress.h"
#include "McEventSelector/McCnvSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
// Externals

McEventCnv::McEventCnv(ISvcLocator* svcloc) :
    Converter(McCnvSvc::storageType(), EventInfo::classID(), svcloc) {
}

StatusCode
McEventCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
  McAddress *pMcAddr;
  EventType*  peT = new EventType();
  peT->add_type(EventType::IS_SIMULATION);
  peT->set_user_type("McEvent");
  if ((pMcAddr = dynamic_cast<McAddress*>(pAddr)) &&
      (pObj = SG::asStorable (new EventInfo(
					    new EventID(pMcAddr->runNumber(), 
							pMcAddr->eventNumber(),
							pMcAddr->timeStamp(),
							0,
							pMcAddr->lumiBlock(),
							0),
					    peT) )   //owned by EventInfo --> TDS 
	) )
    return StatusCode::SUCCESS;
  else  
    delete peT;
  return StatusCode::FAILURE;    
}

long
McEventCnv::storageType()     { return McCnvSvc::storageType(); }
 
const CLID&
McEventCnv::classID()  { return EventInfo::classID(); }     




