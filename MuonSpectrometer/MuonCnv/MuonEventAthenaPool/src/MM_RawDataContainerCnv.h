/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MM_RAWDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_MM_RAWDATACONTAINERCNV_H

#include "MuonRdoContainerTPCnv.h"
#include "MuonEventTPCnv/MuonRDO/MM_RawDataContainerCnv_p1.h"
#include "MuonEventTPCnv/MuonRDO/MM_RawDataContainerCnv_p2.h"
#include "MuonEventTPCnv/MuonRDO/MM_RawDataContainerCnv_p3.h"
#include "MuonRDO/MM_RawDataContainer.h"

typedef  Muon::MM_RawDataContainer_p3  MM_RawDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::MM_RawDataContainer, MM_RawDataContainer_PERS >  MM_RawDataContainerCnvBase;


class MM_RawDataContainerCnv : 
    public MM_RawDataContainerCnvBase 
{
        
public:
    MM_RawDataContainerCnv(ISvcLocator* svcloc);
    virtual ~MM_RawDataContainerCnv();
    
    virtual MM_RawDataContainer_PERS*   createPersistent (Muon::MM_RawDataContainer* transCont);
    virtual Muon::MM_RawDataContainer*  createTransient ();

    // Must initialize ID helpers
    virtual StatusCode initialize();
        
private:
    Muon::MM_RawDataContainerCnv_p1  m_TPConverter_p1;
    Muon::MM_RawDataContainerCnv_p2  m_TPConverter_p2;
    Muon::MM_RawDataContainerCnv_p3  m_TPConverter_p3;
};



#endif


