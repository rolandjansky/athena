/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/



#include "RawInfoSummaryForTagCnv.h"


RawInfoSummaryForTagCnv::RawInfoSummaryForTagCnv(ISvcLocator* svcloc)
      :     RawInfoSummaryForTagCnvBase(svcloc)
{
}


TRANS*  RawInfoSummaryForTagCnv::createTransient()
{
   static const pool::Guid p1_guid( "4CBBDF6D-4A58-42A9-9B26-534676C52F4C" );

   MsgStream mlog(msgSvc(), "RawInfoSummaryForTagCnv" );
   mlog << MSG::DEBUG << "createTransient()" << endmsg;

   if( compareClassGuid( p1_guid ) ) {
      std::unique_ptr<  RawInfoSummaryForTag_p1 > pobj( poolReadObject< RawInfoSummaryForTag_p1  >() );
      return m_TPConverter.createTransient( pobj.get(), mlog );
    }
    else
       throw std::runtime_error( "Unsupported persistent version of RawInfoSummaryForTag (unknown GUID)" );
        
    return 0;
}

PERS* RawInfoSummaryForTagCnv::createPersistent(TRANS* transObj)
{
   MsgStream mlog(msgSvc(), "RawInfoSummaryForTagCnv" );
   mlog << MSG::DEBUG << "createPersistent()" << endmsg;
   return m_TPConverter.createPersistent( transObj, mlog );   
}
