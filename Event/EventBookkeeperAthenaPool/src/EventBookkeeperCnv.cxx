/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// File for class EventBookkeeperCnv
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////

#include "EventBookkeeperCnv.h"
#include "EventBookkeeperTPCnv/EventBookkeeper_p1.h"

EventBookkeeper_PERS* EventBookkeeperCnv::createPersistent(EventBookkeeper* transCont) {
  MsgStream log(msgSvc(), "EventBookkeeperConverter" );
  EventBookkeeper_PERS * persObj = m_TPConverter.createPersistent( transCont,   log );
  return persObj; 
}
    
EventBookkeeper* EventBookkeeperCnv::createTransient() {
  MsgStream log(msgSvc(), "EventBookkeeperConverter" );
  //p1_guid matches the number in EventBookkeeperTPCnv/selection.xtml and is generated with uuidgen | tr "[:lower:]" "[:upper:]"
  static const pool::Guid   p1_guid("A1550FE9-B52A-451D-A2B8-3B09E046A481");
  static const pool::Guid   p2_guid("EACBE9F1-84F1-4A51-9303-A39619FE965A");
  if( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr< EventBookkeeper_p1 > col_vect( poolReadObject< EventBookkeeper_p1 >() );
    return m_TPConverterP1.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(p2_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr< EventBookkeeper_p2 > col_vect( poolReadObject< EventBookkeeper_p2 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }
  else {
    throw std::runtime_error("Unsupported persistent version of EventBookkeeper. Please use a more recent software release.");
  }  
}

