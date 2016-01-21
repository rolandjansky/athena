/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigPassBitsCollectionCnv.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCollectionCnv_p1.h"

static TrigPassBitsCollectionCnv_p1   TPconverter1;

//createPersistent 
TrigPassBitsCollection_PERS * TrigPassBitsCollectionCnv::createPersistent( TrigPassBitsCollection *transObj)
{
  MsgStream mlog(messageService(), "TrigPassBitsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigPassBitsCollectionCnv::createPersistent called" << endreq;
  
  TrigPassBitsCollection_PERS * p_cont = TPconverter1.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
TrigPassBitsCollection * TrigPassBitsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigPassBitsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigPassBitsCollectionCnv::createTransient called" << endreq;
  
  static pool::Guid p1_guid( "E6E4F396-D696-4C1C-A0C4-7766E0AF9BB5" );

  //  TrigPassBitsCollection *p_collection = 0;
  if( compareClassGuid( p1_guid ) )     
  {
         std::auto_ptr< TrigPassBitsCollection_p1 > col_vect( poolReadObject< TrigPassBitsCollection_p1 >() );
	 //         std::cout << "Reading MFC p1" << std::endl;
         return TPconverter1.createTransient( col_vect.get(), mlog ) ;
  }
  else  throw std::runtime_error( "Unsupported persistent version of TrigPassBitsCollection" );
  
}//end of create transient method

