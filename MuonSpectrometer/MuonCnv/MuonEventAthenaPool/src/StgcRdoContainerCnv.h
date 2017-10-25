/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_STGC_RAWDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_STGC_RAWDATACONTAINERCNV_H

/*
  custom POOL TP converter for TGC RDO
  @author Marcin Nowak
  CERN, February 2006
*/


#include "MuonRdoContainerTPCnv.h"
#include "sTGC_RawDataContainerCnv_p1.h"
#include "MuonEventAthenaPool/sTGC_RawDataContainer_p1.h"
#include "MuonRDO/sTGC_RawDataContainer.h"

typedef MuonRdoContainerTPCnv<sTGC_RawDataContainer, sTGC_RawDataContainer_p1, sTGC_RawDataContainerCnv_p1 >
sTGC_RawDataContainerCnv;


template < >
inline
sTGC_RawDataContainer*
sTGC_RawDataContainerCnv::createTransient()
{
   sTGC_RawDataContainer *trans_cont = 0;
   static pool::Guid	p1_guid("3786AB67-8A7A-4DA5-B178-3FE9CB3A6FD2");

   if( compareClassGuid(p1_guid) ) {
      std::unique_ptr< sTGC_RawDataContainer_p1 >  col_vect( this->poolReadObject<sTGC_RawDataContainer_p1>() );
      sTGC_RawDataContainerCnv_p1 cnv;
      trans_cont =  cnv.createTransient( col_vect.get(), log );
   } else {
      throw std::runtime_error("Unsupported persistent version of sTGC RDO container");
   }
   return trans_cont;
}


#endif


