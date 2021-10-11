/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_TGCL1RDOCONTAINERCNV_H
#define MUONEVENTATHENAPOOL_TGCL1RDOCONTAINERCNV_H

/*
  custom POOL TP converter for TGC L1 trigger RDO
  @author Toshi Sumida
  CERN, May 2020
*/


#include "MuonRdoContainerTPCnv.h"
#include "TgcL1RdoContainerCnv_p1.h"
#include "MuonEventAthenaPool/TgcL1RdoContainer_p1.h"
#include "MuonRDO/TgcL1RdoContainer.h"


typedef MuonRdoContainerTPCnv<TgcL1RdoContainer, TgcL1RdoContainer_p1, TgcL1RdoContainerCnv_p1 >
TgcL1RdoContainerCnv;


template < >
inline
TgcL1RdoContainer*
TgcL1RdoContainerCnv::createTransient()
{
   MsgStream log(msgSvc(), "TgcL1RdoContainerCnv" );
   log << MSG::DEBUG << " **** Entered createTransient() "
       << endmsg;

   // the use of auto pointers ensures that the persistent object is deleted
   // using the correct persistent type pointer

   TgcL1RdoContainer *trans_cont = 0;
   static pool::Guid	p1_guid("799C629C-ECA9-4963-9C84-A4AD7D775B24");

   if( compareClassGuid(p1_guid) ) {
      std::unique_ptr< TgcL1RdoContainer_p1 >  col_vect( this->poolReadObject<TgcL1RdoContainer_p1>() );
      trans_cont =  m_TPconverter.createTransient( col_vect.get(), log );
   }
   else {
      throw std::runtime_error("Unsupported persistent version of TGC RDO container");
   }
   return trans_cont;
}


#endif


