/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCOINDATACONTAINERCNV_P4_H
#define TGCCOINDATACONTAINERCNV_P4_H

//-----------------------------------------------------------------------------
//
// file:   TgcCoinDataContainerCnv_p4.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonEventTPCnv/MuonTrigCoinData/MuonCoinDataContainer_p1.h"

namespace MuonGM{ class MuonDetectorManager;}

class MsgStream;
class StoreGateSvc;
class TgcIdHelper;

namespace Muon{
    class TgcCoinDataContainerCnv_p4 : public T_AthenaPoolTPCnvBase< Muon::TgcCoinDataContainer, Muon::MuonCoinDataContainer_p1 >

    {
    public:
        typedef Muon::MuonCoinDataContainer_p1 PERS; 
        typedef Muon::TgcCoinDataContainer TRANS;
        TgcCoinDataContainerCnv_p4(): m_TgcId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0) {}
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual Muon::TgcCoinDataContainer* createTransient(const Muon::MuonCoinDataContainer_p1* persObj, MsgStream& log);
    private:
        StatusCode initialize(MsgStream &log);

        const TgcIdHelper *m_TgcId;
        StoreGateSvc *m_storeGate;
        const MuonGM::MuonDetectorManager* m_muonDetMgr;
        bool m_isInitialized;
    };

}
#endif 

