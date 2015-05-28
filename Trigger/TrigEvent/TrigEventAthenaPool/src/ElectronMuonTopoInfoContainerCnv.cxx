/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronMuonTopoInfoContainerCnv.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoContainerCnv_p1.h"

static ElectronMuonTopoInfoContainerCnv_p1   TPConverter;

//createPersistent 
ElectronMuonTopoInfoContainer_PERS * ElectronMuonTopoInfoContainerCnv::createPersistent( ElectronMuonTopoInfoContainer *transObj)
{
  MsgStream mlog(messageService(), "ElectronMuonTopoInfoContainerConverter" );

  mlog << MSG::DEBUG << "ElectronMuonTopoInfoContainerCnv::createPersistent called" << endreq;

  ElectronMuonTopoInfoContainer_PERS * p_emTopoCont = TPConverter.createPersistent( transObj, mlog );
 
  return p_emTopoCont;
 
}//end of create persistent method

//createTransient
ElectronMuonTopoInfoContainer * ElectronMuonTopoInfoContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "ElectronMuonTopoInfoContainerConverter" );
  
  mlog << MSG::DEBUG << "ElectronMuonTopoInfoContainerCnv::createTransient called" << endreq;

  static pool::Guid p1_guid( "0A775717-3FC9-4FF4-A18B-3F520B2D4DAC" );

 if( compareClassGuid( p1_guid ) ){
   std::auto_ptr< ElectronMuonTopoInfoContainer_p1 > col_vect( poolReadObject< ElectronMuonTopoInfoContainer_p1 >() );
   return TPConverter.createTransient( col_vect.get(), mlog ) ;
 } else { throw std::runtime_error( "Unsupported persistent version of ElectronMuonTopoInfoContainer" ); }
   
}//end of create transient method
 
