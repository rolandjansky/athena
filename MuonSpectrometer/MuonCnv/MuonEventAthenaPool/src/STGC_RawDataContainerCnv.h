/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_STGC_RAWDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_STGC_RAWDATACONTAINERCNV_H

#include "MuonRdoContainerTPCnv.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainerCnv_p1.h"
#include "MuonRDO/STGC_RawDataContainer.h"

typedef  Muon::STGC_RawDataContainer_p1  STGC_RawDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::STGC_RawDataContainer, STGC_RawDataContainer_PERS >  STGC_RawDataContainerCnvBase;


class STGC_RawDataContainerCnv : 
    public STGC_RawDataContainerCnvBase 
{
        
public:
    STGC_RawDataContainerCnv(ISvcLocator* svcloc);
    virtual ~STGC_RawDataContainerCnv();
    
    virtual STGC_RawDataContainer_PERS*   createPersistent (Muon::STGC_RawDataContainer* transCont);
    virtual Muon::STGC_RawDataContainer*  createTransient ();

    // Must initialize ID helpers
    virtual StatusCode initialize();
        
private:
    Muon::STGC_RawDataContainerCnv_p1  m_TPConverter;
};



#endif


