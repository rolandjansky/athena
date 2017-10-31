/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_STGC_RAWDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_STGC_RAWDATACONTAINERCNV_H

#include "MuonRdoContainerTPCnv.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainerCnv_p1.h"
#include "MuonRDO/STGC_RawDataContainer.h"

typedef MuonRdoContainerTPCnv<Muon::STGC_RawDataContainer, Muon::STGC_RawDataContainer_p1, Muon::STGC_RawDataContainerCnv_p1 >
STGC_RawDataContainerCnv;


template < >
inline
Muon::STGC_RawDataContainer*
STGC_RawDataContainerCnv::createTransient()
{
  using namespace Muon;
  MsgStream log(msgSvc(), "STGC_RawDataContainerCnv" );
 
  STGC_RawDataContainer *transCont = 0;
  static pool::Guid	p1_guid("93035F54-0FA9-4A56-98E0-A808DD23C089");

  if( compareClassGuid(p1_guid) ) {
    std::unique_ptr< STGC_RawDataContainer_p1 >  cont( this->poolReadObject<STGC_RawDataContainer_p1>() );
    STGC_RawDataContainerCnv_p1 cnv;
    const STGC_RawDataContainer_p1* constCont = cont.get();
    transCont =  cnv.createTransient( constCont, log );
    // virtual Muon::STGC_RawDataContainer* createTransient(const Muon::STGC_RawDataContainer_p1* persObj, MsgStream& log) override final;
    
  } else {
    throw std::runtime_error("Unsupported persistent version of STGC Raw Data (RDO) container");
  }
  return transCont;
}


#endif


