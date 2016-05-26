/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#ifndef MUONEVENTATHENAPOOL_TGCCOINDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_TGCCOINDATACONTAINERCNV_H

//#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonTrigCoinData/TgcCoinDataCollection.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp2.h"
#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp3.h"

class StoreGateSvc;
class TgcIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

// the latest persistent representation type of DataCollection:
typedef  Muon::TgcCoinDataContainer_tlp3  TgcCoinDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::TgcCoinDataContainer, TgcCoinDataContainer_PERS >  TgcCoinDataContainerCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class TgcCoinDataContainerCnv : 
    public TgcCoinDataContainerCnvBase 
{
    
    friend class CnvFactory<TgcCoinDataContainerCnv>;
    
public:
    TgcCoinDataContainerCnv(ISvcLocator* svcloc);
    virtual ~TgcCoinDataContainerCnv();
    
    virtual TgcCoinDataContainer_PERS*   createPersistent (Muon::TgcCoinDataContainer* transCont);
    virtual Muon::TgcCoinDataContainer*  createTransient ();

    // Must initialize ID helpers
    virtual StatusCode initialize();
    virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter_tlp3; }
        
private:
    //TgcCoinDataCollVec              m_prdCollVec;
    StoreGateSvc*                   m_storeGate;
    
    // Not bothering with being able to read 'old' PRDs yet. Ed
    //TgcCoinDataContainerCnv_p1   m_converter_p1;
    TgcCoinDataContainerCnv_tlp1    m_TPConverter_tlp1;
    TgcCoinDataContainerCnv_tlp2    m_TPConverter_tlp2;
    TgcCoinDataContainerCnv_tlp3    m_TPConverter_tlp3;
    
    //const TgcIdHelper*   m_cscId;
    //const MuonGM::MuonDetectorManager* m_muonMgr; //!< Muon Detector Manager
};

#endif
