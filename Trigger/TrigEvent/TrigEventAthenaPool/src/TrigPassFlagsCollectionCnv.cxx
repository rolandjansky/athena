/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigPassFlagsCollectionCnv.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCollectionCnv_p1.h"

static TrigPassFlagsCollectionCnv_p1   TPconverter1;

//createPersistent 
TrigPassFlagsCollection_PERS * TrigPassFlagsCollectionCnv::createPersistent( TrigPassFlagsCollection *transObj)
{
  MsgStream mlog(messageService(), "TrigPassFlagsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigPassFlagsCollectionCnv::createPersistent called" << endreq;
  
  TrigPassFlagsCollection_PERS * p_cont = TPconverter1.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
TrigPassFlagsCollection * TrigPassFlagsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigPassFlagsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigPassFlagsCollectionCnv::createTransient called" << endreq;
  
  static pool::Guid p1_guid( "7DE670AF-A256-4E8E-BC44-1CA30810F294" );

  //  TrigPassFlagsCollection *p_collection = 0;
  if( compareClassGuid( p1_guid ) )     
  {
         std::auto_ptr< TrigPassFlagsCollection_p1 > col_vect( poolReadObject< TrigPassFlagsCollection_p1 >() );
	 //         std::cout << "Reading MFC p1" << std::endl;
         return TPconverter1.createTransient( col_vect.get(), mlog ) ;
  }
  else  throw std::runtime_error( "Unsupported persistent version of TrigPassFlagsCollection" );
  
}//end of create transient method

