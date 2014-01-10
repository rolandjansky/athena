/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// File for class SkimDecisionCollectionCnv
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////

#include "SkimDecisionCollectionCnv.h"

SkimDecisionCollection_PERS* SkimDecisionCollectionCnv::createPersistent(SkimDecisionCollection* transCont) {
  MsgStream log(messageService(), "SkimDecisionCollectionConverter" );
  SkimDecisionCollection_PERS * persObj = m_TPConverter.createPersistent( transCont,   log );

  log << MSG::DEBUG << "Success" << endreq;
  return persObj; 
}
    
SkimDecisionCollection* SkimDecisionCollectionCnv::createTransient() {
  MsgStream log(messageService(), "SkimDecisionCollectionConverter" );
  //p1_guid matches the number in EventBookkeeperTPCnv/selection.xtml and is generated with uuidgen | tr "[:lower:]" "[:upper:]"
  static pool::Guid   p1_guid("5705559E-F062-4F15-A220-78A8542EFBF3");
  if( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< SkimDecisionCollection_p1 > col_vect( poolReadObject< SkimDecisionCollection_p1 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }
  else {
    throw std::runtime_error("Unsupported persistent version of SkimDecisionCollection");
  }  
}

