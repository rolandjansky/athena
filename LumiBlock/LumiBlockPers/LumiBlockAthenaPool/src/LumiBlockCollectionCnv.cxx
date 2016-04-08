/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// File for class LumiBlockCollectionCnv
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
/////////////////////////////////////////////////////////////////

#include "LumiBlockCollectionCnv.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p1.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p2.h"


LumiBlockCollection_PERS* LumiBlockCollectionCnv::createPersistent(LumiBlockCollection* transCont) {
  MsgStream log(messageService(), "LumiBlockCollectionConverter" );
  LumiBlockCollection_PERS * persObj = m_TPConverter.createPersistent( transCont,   log );

  log << MSG::DEBUG << "Success" << endreq;
  return persObj; 
}
    
LumiBlockCollection* LumiBlockCollectionCnv::createTransient() {
  MsgStream log(messageService(), "LumiBlockCollectionConverter" );
  static pool::Guid   p1_guid("CF1F40C9-6125-4C35-87FF-DDA2C319000C");
  static pool::Guid   p2_guid("DEF9282A-F174-4382-8248-B94567CD869F");

  static  LumiBlockCollectionCnv_p1   TPConverter_p1;
  static  LumiBlockCollectionCnv_p2   TPConverter_p2;


  if( compareClassGuid(p2_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< LumiBlockCollection_p2 > col_vect( poolReadObject< LumiBlockCollection_p2 >() );
    return TPConverter_p2.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< LumiBlockCollection_p1 > col_vect( poolReadObject< LumiBlockCollection_p1 >() );
    return TPConverter_p1.createTransient( col_vect.get(), log );
  }
  else {
    throw std::runtime_error("Unsupported persistent version of LumiBlockCollection");
  }  
}
