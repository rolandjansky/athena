/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCPREPDATACONTAINERCNV_P1_H
#define CSCPREPDATACONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CscPrepDataContainerCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"

namespace MuonGM{ class MuonDetectorManager;}

class MsgStream;
class StoreGateSvc;
class CscIdHelper;

namespace Muon{
    class CscPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase< Muon::CscPrepDataContainer, Muon::MuonPRD_Container_p1 >

    {
    public:
        typedef Muon::MuonPRD_Container_p1 PERS; 
        typedef Muon::CscPrepDataContainer TRANS;
        CscPrepDataContainerCnv_p1(): m_cscId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0) {}
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual Muon::CscPrepDataContainer* createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log);
        //private:
        StatusCode initialize(MsgStream &log);

        const CscIdHelper *m_cscId;
        StoreGateSvc *m_storeGate;
        const MuonGM::MuonDetectorManager* m_muonDetMgr;
        bool m_isInitialized;
    };

}
#endif 

