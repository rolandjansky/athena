/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// File for class EventBookkeeperCollectionCnv
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////

#include "EventBookkeeperCollectionCnv.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollection_p1.h"


EventBookkeeperCollection_PERS* EventBookkeeperCollectionCnv::createPersistent(EventBookkeeperCollection* transCont) {
  MsgStream log(messageService(), "EventBookkeeperCollectionConverter" );
  EventBookkeeperCollection_PERS * persObj = m_TPConverter.createPersistent( transCont,   log );
  return persObj; 
}
    
EventBookkeeperCollection* EventBookkeeperCollectionCnv::createTransient() {
  MsgStream log(messageService(), "EventBookkeeperCollectionConverter" );
  //p1_guid matches the number in EventBookkeeperTPCnv/selection.xtml and is generated with uuidgen | tr "[:lower:]" "[:upper:]"
  static pool::Guid   p1_guid("461506EA-7376-448C-A4AF-640654D313C3");
  static pool::Guid   p2_guid("4CB34AE0-ECE5-404B-8CB7-B2E20F509DBE");
  if( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< EventBookkeeperCollection_p1 > col_vect( poolReadObject< EventBookkeeperCollection_p1 >() );
    return m_TPConverterP1.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(p2_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< EventBookkeeperCollection_p2 > col_vect( poolReadObject< EventBookkeeperCollection_p2 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }
  else {
    throw std::runtime_error("Unsupported persistent version of EventBookkeeperCollection");
  }  
}

