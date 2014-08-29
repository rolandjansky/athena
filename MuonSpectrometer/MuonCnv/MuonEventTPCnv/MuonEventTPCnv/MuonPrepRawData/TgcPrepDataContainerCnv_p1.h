/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCPREPDATACONTAINERCNV_P1_H
#define TGCPREPDATACONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TgcPrepDataContainerCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"

namespace MuonGM{ class MuonDetectorManager;}

class MsgStream;
class StoreGateSvc;
class TgcIdHelper;

namespace Muon{
    class TgcPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase< Muon::TgcPrepDataContainer, Muon::MuonPRD_Container_p1 >

    {
    public:
        typedef Muon::MuonPRD_Container_p1 PERS; 
        typedef Muon::TgcPrepDataContainer TRANS;
        TgcPrepDataContainerCnv_p1(): m_TgcId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0) {}
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual Muon::TgcPrepDataContainer* createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log);
    private:
        StatusCode initialize(MsgStream &log);

        const TgcIdHelper *m_TgcId;
        StoreGateSvc *m_storeGate;
        const MuonGM::MuonDetectorManager* m_muonDetMgr;
        bool m_isInitialized;
    };

}
#endif 

