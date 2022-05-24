/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronMuonTopoInfoContainerCnv.h"

//createPersistent 
ElectronMuonTopoInfoContainer_PERS * ElectronMuonTopoInfoContainerCnv::createPersistent( ElectronMuonTopoInfoContainer *transObj)
{
  MsgStream mlog(msgSvc(), "ElectronMuonTopoInfoContainerConverter" );

  mlog << MSG::DEBUG << "ElectronMuonTopoInfoContainerCnv::createPersistent called" << endmsg;

  ElectronMuonTopoInfoContainer_PERS * p_emTopoCont = m_converter.createPersistent( transObj, mlog );
 
  return p_emTopoCont;
 
}//end of create persistent method

//createTransient
ElectronMuonTopoInfoContainer * ElectronMuonTopoInfoContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "ElectronMuonTopoInfoContainerConverter" );
  
  mlog << MSG::DEBUG << "ElectronMuonTopoInfoContainerCnv::createTransient called" << endmsg;

  static const pool::Guid p1_guid( "0A775717-3FC9-4FF4-A18B-3F520B2D4DAC" );

 if( compareClassGuid( p1_guid ) ){
   std::unique_ptr< ElectronMuonTopoInfoContainer_p1 > col_vect( poolReadObject< ElectronMuonTopoInfoContainer_p1 >() );
   return m_converter.createTransient( col_vect.get(), mlog ) ;
 } else { throw std::runtime_error( "Unsupported persistent version of ElectronMuonTopoInfoContainer" ); }
   
}//end of create transient method
 
