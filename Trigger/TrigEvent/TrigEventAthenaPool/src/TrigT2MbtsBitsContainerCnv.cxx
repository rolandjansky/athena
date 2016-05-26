/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MbtsBitsContainerCnv.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainerCnv_tlp1.h" 
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainerCnv_p3.h" 

static TrigT2MbtsBitsContainerCnv_tlp1 TPConverter_tlp1;
static TrigT2MbtsBitsContainerCnv_p3   TPConverter;
static TrigT2MbtsBitsContainerCnv_p1   TP1Converter;

//createPersistent 
TrigT2MbtsBitsContainer_PERS * TrigT2MbtsBitsContainerCnv::createPersistent( TrigT2MbtsBitsContainer *transObj)
{
  MsgStream mlog(messageService(), "TrigT2MbtsBitsContainerConverter" );

  mlog << MSG::DEBUG << "TrigT2MbtsBitsContainerCnv::createPersistent" << endreq;

  TrigT2MbtsBitsContainer_PERS* persObj = TPConverter.createPersistent( transObj, mlog );
 
  return persObj;
 
}//end of create persistent method
 

//createTransient
TrigT2MbtsBitsContainer* TrigT2MbtsBitsContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigT2MbtsBitsContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigT2MbtsBitsContainerCnv::createTransient" << endreq;

  static pool::Guid p3_guid( "60BD805D-F64E-46DF-87CD-0B4443660C97" );
  static pool::Guid tlp1_guid( "82BAAC80-62FC-4E6D-9BD7-1619064FF7AC" );
  static pool::Guid p1_guid( "139D9BFE-0944-44A6-8D9E-10CEEF8B30B9" );
  static pool::Guid trans_guid( "BBB00ED0-1D5C-4C73-8785-6BF239D07816" );

 if( compareClassGuid( p3_guid ) ){

         std::auto_ptr< TrigT2MbtsBitsContainer_p3 > col_vect( poolReadObject< TrigT2MbtsBitsContainer_p3 >() );
         //         std::cout << "Reading IMFC p3" << std::endl;
         return TPConverter.createTransient( col_vect.get(), mlog ) ;

  } else if(compareClassGuid(tlp1_guid)) {

         std::auto_ptr< TrigT2MbtsBitsContainer_tlp1 > col_vect( poolReadObject< TrigT2MbtsBitsContainer_tlp1 >() );
         //  std::cout << "Reading IMFC tlp1" << std::endl;
         return TPConverter_tlp1.createTransient( col_vect.get(), mlog );

  } else if(compareClassGuid(p1_guid)) {

         std::auto_ptr< TrigT2MbtsBitsContainer_p1 > col_vect( poolReadObject< TrigT2MbtsBitsContainer_p1 >() );
         return TP1Converter.createTransient( col_vect.get(), mlog );

  } else if(compareClassGuid(trans_guid)) {

         return poolReadObject<TrigT2MbtsBitsContainer>();

  } else  {

    throw std::runtime_error( "Unsupported persistent version of TrigT2MbtsBitsContainer" );

  }
  
}//end of create transient method
 
