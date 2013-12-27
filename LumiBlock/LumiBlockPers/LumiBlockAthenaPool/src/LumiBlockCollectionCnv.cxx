/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// File for class LumiBlockCollectionCnv
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
/////////////////////////////////////////////////////////////////

#include "LumiBlockCollectionCnv.h"

LumiBlockCollection_PERS* LumiBlockCollectionCnv::createPersistent(LumiBlockCollection* transCont) {
  MsgStream log(messageService(), "LumiBlockCollectionConverter" );
  LumiBlockCollection_PERS * persObj = m_TPConverter.createPersistent( transCont,   log );

  log << MSG::DEBUG << "Success" << endreq;
  return persObj; 
}
    
LumiBlockCollection* LumiBlockCollectionCnv::createTransient() {
  MsgStream log(messageService(), "LumiBlockCollectionConverter" );
  static pool::Guid   p1_guid("CF1F40C9-6125-4C35-87FF-DDA2C319000C");
  if( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< LumiBlockCollection_p1 > col_vect( poolReadObject< LumiBlockCollection_p1 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }
  else {
    throw std::runtime_error("Unsupported persistent version of LumiBlockCollection");
  }  
}
