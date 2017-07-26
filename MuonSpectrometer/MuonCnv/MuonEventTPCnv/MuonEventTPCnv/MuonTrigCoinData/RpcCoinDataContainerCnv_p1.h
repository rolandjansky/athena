/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#ifndef RpcCOINDATACONTAINERCNV_P1_H
#define RpcCOINDATACONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   RpcCoinDataContainerCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "RpcCoinData_p1.h"
#include "MuonEventTPCnv/CreateTransientTemplates.h"

namespace MuonGM{ class MuonDetectorManager;}

class MsgStream;
class StoreGateSvc;
class RpcIdHelper;

namespace Muon{
    class RpcCoinDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase< Muon::RpcCoinDataContainer, Muon::MuonPRD_Container_p2<RpcCoinData_p1> >

    {
    public:
        typedef Muon::MuonPRD_Container_p2<RpcCoinData_p1> PERS; 
        typedef Muon::RpcCoinDataContainer TRANS;
        RpcCoinDataContainerCnv_p1(): m_RpcId(0), m_muonDetMgr(0), m_isInitialized(0) {}
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual TRANS* createTransient(const PERS* persObj, MsgStream& log);
    private:
        StatusCode initialize(MsgStream &log);

        const RpcIdHelper *m_RpcId;
        // StoreGateSvc *m_storeGate;
        const MuonGM::MuonDetectorManager* m_muonDetMgr;
        bool m_isInitialized;
    };

}
#endif 

