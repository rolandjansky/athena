/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigRNNOutputContainerCnv.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_p2.h"

//createPersistent 
TrigRNNOutputContainer_PERS * TrigRNNOutputContainerCnv::createPersistent( TrigRNNOutputContainer *transObj)
{
  MsgStream mlog(msgSvc(), "TrigRNNOutputContainerConverter" );

  mlog << MSG::DEBUG << "TrigRNNOutputContainerCnv::createPersistent called" << endmsg;

  TrigRNNOutputContainer_PERS * p_Cont = m_converter.createPersistent( transObj, mlog );
 
  return p_Cont;
 
}//end of create persistent method
 

//createTransient
TrigRNNOutputContainer * TrigRNNOutputContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigRNNOutputContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigRNNOutputContainerCnv::createTransient called" << endmsg;

  static const pool::Guid tr_guid("FED72B55-6275-DE11-8F1B-000423DD5A1A");
  static const pool::Guid tlp1_guid("86A89E9D-F776-DE11-B65D-000423DD5A1A");
  static const pool::Guid p2_guid("B10FA1AF-F38F-4025-83C4-3A83A3F3AE71");

  if( compareClassGuid( p2_guid ) ){
      std::unique_ptr< TrigRNNOutputContainer_p2 > col_vect( poolReadObject< TrigRNNOutputContainer_p2 >() );
      //         std::cout << "Reading IMFC p2" << std::endl;
      return m_converter.createTransient( col_vect.get(), mlog ) ;
  } else if ( compareClassGuid( tlp1_guid ) ) {
      std::unique_ptr< TrigRNNOutputContainer_tlp1 > col_vect( poolReadObject< TrigRNNOutputContainer_tlp1 >() );
      return m_converter_tlp1.createTransient( col_vect.get(), mlog );
  } else if (compareClassGuid(tr_guid)) {
      // regular object from before the T/P separation
      return poolReadObject<TrigRNNOutputContainer>();
  } else
      throw std::runtime_error( "Unsupported persistent version of TrigRNNOutputContainer" );
   
}//end of create transient method
