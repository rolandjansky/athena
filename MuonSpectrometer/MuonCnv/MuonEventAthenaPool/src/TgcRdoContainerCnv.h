/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_TGCRDOCONTAINERCNV_H
#define MUONEVENTATHENAPOOL_TGCRDOCONTAINERCNV_H

/*
  custom POOL TP converter for TGC RDO
  @author Marcin Nowak
  CERN, February 2006
*/


#include "MuonRdoContainerTPCnv.h"
#include "TgcRdoContainerCnv_p1.h"
#include "TgcRdoContainerCnv_p2.h"
#include "TgcRdoContainerCnv_p3.h"
#include "MuonEventAthenaPool/TgcRdoContainer_p1.h"
#include "MuonEventAthenaPool/TgcRdoContainer_p2.h"
#include "MuonEventAthenaPool/TgcRdoContainer_p3.h"
#include "MuonRDO/TgcRdoContainer.h"


typedef MuonRdoContainerTPCnv<TgcRdoContainer, TgcRdoContainer_p3, TgcRdoContainerCnv_p3 >
TgcRdoContainerCnv;


template < >
inline
TgcRdoContainer*
TgcRdoContainerCnv::createTransient()
{
   MsgStream log(msgSvc(), "TgcRdoContainerCnv" );
   log << MSG::DEBUG << " **** Entered createTransient() "
       << endmsg;

   // the use of auto pointers ensures that the persistent object is deleted
   // using the correct persistent type pointer

   TgcRdoContainer *trans_cont = 0;
   static pool::Guid	p3_guid("E7D45D90-CB92-4A7D-B5FE-2791CE34FFEE");
   static pool::Guid	p2_guid("3DA250DA-321C-4DD3-996A-BB0E67A6034D");
   static pool::Guid	p1_guid("BF9D17EA-AC87-4243-9126-8FC86DDCDAA3");
   static pool::Guid	p0_guid("FBF8D72D-A6B9-4689-8E02-BB0F435BF2F7");

   if( compareClassGuid(p3_guid) ) {
      std::unique_ptr< TgcRdoContainer_p3 >  col_vect( this->poolReadObject<TgcRdoContainer_p3>() );
      trans_cont =  m_TPconverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p2_guid) ) {
      std::unique_ptr< TgcRdoContainer_p2 >  col_vect( this->poolReadObject<TgcRdoContainer_p2>() );
      TgcRdoContainerCnv_p2 cnv;
      trans_cont =  cnv.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p1_guid) ) {
      std::unique_ptr< TgcRdoContainer_p1 >  col_vect( this->poolReadObject<TgcRdoContainer_p1>() );
      TgcRdoContainerCnv_p1 cnv;
      trans_cont =  cnv.createTransient( col_vect.get(), log );
   }
   else if(compareClassGuid(p0_guid) ) {
      // old version from before TP separation
      std::unique_ptr< COLL_vector >	col_vect(  this->poolReadObject< COLL_vector >() );
      trans_cont =  createTransientFrom_p0( col_vect.get(), log );
   }
   else {
      throw std::runtime_error("Unsupported persistent version of TGC RDO container");
   }
   return trans_cont;
}


#endif


