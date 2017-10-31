/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_STGC_RAWDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_STGC_RAWDATACONTAINERCNV_H

#include "MuonRdoContainerTPCnv.h"
#include "MuonEventTPCnv/MuonRDO/StgcRawDataContainerCnv_p1.h"
#include "MuonRDO/sTGC_RawDataContainer.h"

typedef MuonRdoContainerTPCnv<Muon::sTGC_RawDataContainer, Muon::StgcRawDataContainer_p1, Muon::StgcRawDataContainerCnv_p1 >
sTGC_RawDataContainerCnv;


template < >
inline
Muon::sTGC_RawDataContainer*
sTGC_RawDataContainerCnv::createTransient()
{
  using namespace Muon;
  MsgStream log(msgSvc(), "sTGC_RawDataContainerCnv" );
 
  sTGC_RawDataContainer *transCont = 0;
  static pool::Guid	p1_guid("93035F54-0FA9-4A56-98E0-A808DD23C089");

  if( compareClassGuid(p1_guid) ) {
    std::unique_ptr< StgcRawDataContainer_p1 >  cont( this->poolReadObject<StgcRawDataContainer_p1>() );
    StgcRawDataContainerCnv_p1 cnv;
    const StgcRawDataContainer_p1* constCont = cont.get();
    transCont =  cnv.createTransient( constCont, log );
    // virtual Muon::sTGC_RawDataContainer* createTransient(const Muon::StgcRawDataContainer_p1* persObj, MsgStream& log) override final;
    
  } else {
    throw std::runtime_error("Unsupported persistent version of sTGC Raw Data (RDO) container");
  }
  return transCont;
}


#endif


