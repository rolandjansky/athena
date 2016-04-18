/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_RpcCoinDataContainerCNV_H
#define MUONEVENTATHENAPOOL_RpcCoinDataContainerCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinDataContainerCnv_p1.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"


typedef  T_AthenaPoolCustomCnv<Muon::RpcCoinDataContainer, Muon::RpcCoinDataContainerCnv_p1::PERS >  RpcCoinDataContainerCnvBase;


/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class RpcCoinDataContainerCnv : 
    public RpcCoinDataContainerCnvBase
{
    
    friend class CnvFactory<RpcCoinDataContainerCnv>;
    
public:
    RpcCoinDataContainerCnv(ISvcLocator* svcloc);
    virtual ~RpcCoinDataContainerCnv();
    
    virtual Muon::RpcCoinDataContainerCnv_p1::PERS*   createPersistent (Muon::RpcCoinDataContainer* transCont);
    virtual Muon::RpcCoinDataContainer*  createTransient ();

    // Must initialize ID helpers
    virtual StatusCode initialize();
    // virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_converter_tlp1; }
        
private:
    Muon::RpcCoinDataContainerCnv_p1    m_converter_p1; 
};

#endif
