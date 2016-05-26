/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2JetContainerCnv.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainerCnv_tlp1.h"

#include "TrigCaloEventTPCnv/TrigT2JetContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainerCnv_p3.h"


static TrigT2JetContainerCnv_tlp1   TPConverter_tlp1;
static TrigT2JetContainerCnv_p3     TPConverter;

//createPersistent 
TrigT2JetContainer_PERS * TrigT2JetContainerCnv::createPersistent( TrigT2JetContainer *transObj)
{

  MsgStream mlog(messageService(), "TrigT2JetContainerConverter" );

  mlog << MSG::DEBUG << "TrigT2JetContainerCnv::createPersistent" << endreq;

  TrigT2JetContainer_PERS* p_T2JetCont = TPConverter.createPersistent( transObj, mlog );
 
  return p_T2JetCont;
 
}//end of create persistent method
 

//createTransient
TrigT2JetContainer * TrigT2JetContainerCnv::createTransient()
{
  
  MsgStream mlog(messageService(), "TrigT2JetContainerConverter" );

  mlog << MSG::DEBUG << "TrigT2JetContainerCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "3B670168-C5AA-48A1-9813-C94530980EBF" );
  static pool::Guid p3_guid( "6215BEE2-45E7-4681-9089-9BD470CDAF4D" );


  if( compareClassGuid( p3_guid ) ){
         std::auto_ptr< TrigT2JetContainer_p3 > col_vect( poolReadObject< TrigT2JetContainer_p3 >() );
         //std::cout << "Reading TTCC p3" << std::endl;
         return TPConverter.createTransient( col_vect.get(), mlog ) ;
  } else if( compareClassGuid( tlp1_guid ) ) {
         std::auto_ptr< TrigT2JetContainer_tlp1 > col_vect( poolReadObject< TrigT2JetContainer_tlp1 >() );
         //std::cout << "Reading TTC tlp1" << std::endl;
         return TPConverter_tlp1.createTransient( col_vect.get(), mlog );
  } else  throw std::runtime_error( "Unsupported persistent version of TrigT2JetContainer" );
     
}//end of create transient method
 
