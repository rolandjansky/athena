/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_CSCRAWDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_CSCRAWDATACONTAINERCNV_H

#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRdoContainerTPCnv.h"
#include "CscRawDataContainerCnv_p1.h"
#include "CscRawDataContainerCnv_p2.h"
#include "CscRawDataContainerCnv_p3.h"
#include "CscRawDataContainerCnv_p4.h"

/*
  custom POOL TP converter for CSC RDO
  @author Marcin Nowak
*/
typedef MuonRdoContainerTPCnv<CscRawDataContainer, CscRawDataContainer_p3, CscRawDataContainerCnv_p3 >
CscRawDataContainerCnv;

template < >
inline
CscRawDataContainer*
CscRawDataContainerCnv::createTransient()
{
   MsgStream log(msgSvc(), "CscRawDataContainerCnv" );
   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << " **** Entered createTransient() "<< endmsg;

   // the use of auto pointers ensures that the persistent object is deleted
   // using the correct persistent type pointer
   
   CscRawDataContainer *            trans_cont = 0;
   
   static pool::Guid p4_guid("F94905BF-D474-4453-B9D0-80ECB7BA4041");
   static pool::Guid p3_guid("A77330FB-BD12-4E50-829C-DADFBF556119");
   static pool::Guid p2_guid("19221A0D-4167-4A1C-BE2A-EE335D7C9D5F");
   static pool::Guid p1_guid("3586FE6B-0504-4E78-BD9F-AF839C50F931");
   static pool::Guid p0_guid("D7600810-31BC-4344-A3C6-9C59F47E5551");

   if( compareClassGuid(p4_guid) ) {
      CscRawDataContainerCnv_p4        tpConvertor_p4;       
      std::unique_ptr< CscRawDataContainer_p4 > col_vect( poolReadObject< CscRawDataContainer_p4 >() );
      trans_cont = tpConvertor_p4.createTransient( col_vect.get(), log );
   } else if( compareClassGuid(p3_guid) ) {
      std::unique_ptr< CscRawDataContainer_p3 > col_vect( poolReadObject< CscRawDataContainer_p3 >() );
      trans_cont = m_TPconverter.createTransient( col_vect.get(), log );
   } else if( compareClassGuid(p2_guid) ) {
      CscRawDataContainerCnv_p2        tpConvertor_p2;       
       
      std::unique_ptr< CscRawDataContainer_p2 > col_vect( poolReadObject< CscRawDataContainer_p2 >() );
      trans_cont = tpConvertor_p2.createTransient( col_vect.get(), log );
   } else if( compareClassGuid(p1_guid) ) {
      CscRawDataContainerCnv_p1        tpConvertor_p1;       
       
      std::unique_ptr< CscRawDataContainer_p1 > col_vect( poolReadObject< CscRawDataContainer_p1 >() );
      trans_cont = tpConvertor_p1.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // old version from before TP separation
      std::unique_ptr< COLL_vector >	col_vect( this->poolReadObject< COLL_vector >() );
      trans_cont = createTransientFrom_p0( col_vect.get(), log );
   }
   else {
      throw std::runtime_error("Unsupported persistent version of CSC RDO container");
   }
   return trans_cont;
}

#endif


