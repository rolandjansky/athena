/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_RPCPREPDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_RPCPREPDATACONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonEventTPCnv/RpcPrepDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataContainerCnv_p3.h"

class StoreGateSvc;
class RpcIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

// the latest persistent representation type of DataCollection:
typedef  Muon::RpcPrepDataContainer_p3  RpcPrepDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::RpcPrepDataContainer, RpcPrepDataContainer_PERS >  RpcPrepDataContainerCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class RpcPrepDataContainerCnv : 
    public RpcPrepDataContainerCnvBase
{
    
    friend class CnvFactory<RpcPrepDataContainerCnv>;
    
public:
    RpcPrepDataContainerCnv(ISvcLocator* svcloc);
    virtual ~RpcPrepDataContainerCnv();
    
    virtual RpcPrepDataContainer_PERS*   createPersistent (Muon::RpcPrepDataContainer* transCont);
    virtual Muon::RpcPrepDataContainer*  createTransient ();

    // Must initialize ID helpers
    virtual StatusCode initialize();
    virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter; }
        
private:
    // Not bothering with being able to read 'old' PRDs yet. Ed
    //RpcPrepDataContainerCnv_p1   m_converter_p1;
    RpcPrepDataContainerCnv_tlp1        m_TPConverter;
    Muon::RpcPrepDataContainerCnv_p3    m_converter_p3;
};

#endif

