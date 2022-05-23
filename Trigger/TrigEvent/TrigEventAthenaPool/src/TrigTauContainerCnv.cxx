/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauContainerCnv.h"

//createPersistent 
TrigTauContainer_PERS * TrigTauContainerCnv::createPersistent( TrigTauContainer *transObj)
{
  MsgStream mlog(msgSvc(), "TrigTauContainerConverter" );

  mlog << MSG::DEBUG << "TrigTauContainerCnv::createPersistent called" << endmsg;

  TrigTauContainer_PERS * p_TauCont = m_converter.createPersistent( transObj, mlog );
 
  return p_TauCont;
 
}//end of create persistent method

//createTransient
TrigTauContainer * TrigTauContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigTauContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigTauContainerCnv::createTransient called" << endmsg;

  static const pool::Guid p3_guid( "32FE95F3-F85E-481E-9DD6-7BBA08ABDBD7" );
  static const pool::Guid tlp1_guid( "1551CECC-52C7-4B5B-876C-27005A8DCCC8" );

 if( compareClassGuid( p3_guid ) ){
         std::unique_ptr< TrigTauContainer_p3 > col_vect( poolReadObject< TrigTauContainer_p3 >() );
         //         std::cout << "Reading IMFC p3" << std::endl;
         return m_converter.createTransient( col_vect.get(), mlog ) ;
  } else if( compareClassGuid( tlp1_guid ) ) {
        std::unique_ptr< TrigTauContainer_tlp1 > col_vect( poolReadObject< TrigTauContainer_tlp1 >() );
        return m_converter_tlp1.createTransient( col_vect.get(), mlog );
  } else { throw std::runtime_error( "Unsupported persistent version of TrigTauContainer" ); }
   
}//end of create transient method
 
