/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_TGCPREPDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_TGCPREPDATACONTAINERCNV_H

//#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonEventTPCnv/TgcPrepDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepDataContainerCnv_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepDataContainerCnv_p3.h"

class StoreGateSvc;
class TgcIdHelper;

// the latest persistent representation type of DataCollection:
typedef  Muon::TgcPrepDataContainer_p3  TgcPrepDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::TgcPrepDataContainer, TgcPrepDataContainer_PERS >  TgcPrepDataContainerCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class TgcPrepDataContainerCnv : 
    public TgcPrepDataContainerCnvBase 
{
    
    friend class CnvFactory<TgcPrepDataContainerCnv>;
    
public:
    TgcPrepDataContainerCnv(ISvcLocator* svcloc);
    virtual ~TgcPrepDataContainerCnv();
    
    virtual TgcPrepDataContainer_PERS*   createPersistent (Muon::TgcPrepDataContainer* transCont);
    virtual Muon::TgcPrepDataContainer*  createTransient ();

    // Must initialize ID helpers
    virtual StatusCode initialize();
    virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter; }
        
private:
    // Not bothering with being able to read 'old' PRDs yet. Ed
    Muon::TgcPrepDataContainerCnv_p3    m_converter_p3;
    Muon::TgcPrepDataContainerCnv_p2    m_converter_p2;
    TgcPrepDataContainerCnv_tlp1  m_TPConverter;
};

#endif
