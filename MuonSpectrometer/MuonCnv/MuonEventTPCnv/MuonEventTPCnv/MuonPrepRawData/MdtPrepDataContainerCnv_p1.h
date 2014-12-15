/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPREPDATACONTAINERCNV_P1_H
#define MDTPREPDATACONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   MdtPrepDataContainerCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"
class PixelID;

namespace MuonGM{ class MuonDetectorManager;}

class MsgStream;
class StoreGateSvc;
class MdtIdHelper;

namespace Muon{
    class MdtPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase< Muon::MdtPrepDataContainer, Muon::MuonPRD_Container_p1 >

    {
    public:
        typedef Muon::MuonPRD_Container_p1 PERS; 
        typedef Muon::MdtPrepDataContainer TRANS;
        MdtPrepDataContainerCnv_p1(): m_MdtId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0) {}
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual Muon::MdtPrepDataContainer* createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log);
    private:
        StatusCode initialize(MsgStream &log);

        const MdtIdHelper *m_MdtId;
        StoreGateSvc *m_storeGate;
        const MuonGM::MuonDetectorManager* m_muonDetMgr;
        bool m_isInitialized;
    };

}
#endif 

