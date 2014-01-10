/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// File for class SkimDecisionCnv
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////

#include "SkimDecisionCnv.h"

SkimDecision_PERS* SkimDecisionCnv::createPersistent(SkimDecision* transCont) {
  MsgStream log(messageService(), "SkimDecisionConverter" );
  SkimDecision_PERS * persObj = m_TPConverter.createPersistent( transCont,   log );

  log << MSG::DEBUG << "Success" << endreq;
  return persObj; 
}
    
SkimDecision* SkimDecisionCnv::createTransient() {
  MsgStream log(messageService(), "SkimDecisionConverter" );
  //p1_guid matches the number in EventBookkeeperTPCnv/selection.xtml and is generated with uuidgen | tr "[:lower:]" "[:upper:]"
  static pool::Guid   p1_guid("61CEDFF0-46DD-42BD-B43A-12F850D3752E"); 
  if( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< SkimDecision_p1 > col_vect( poolReadObject< SkimDecision_p1 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }
  else {
    throw std::runtime_error("Unsupported persistent version of SkimDecision");
  }  
}

