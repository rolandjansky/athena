/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSTRIPPREPDATACONTAINERCNV_P1_H
#define CSCSTRIPPREPDATACONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CscStripPrepDataContainerCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"
class PixelID;

namespace MuonGM{ class PixelDetectorManager;}

class MsgStream;
class StoreGateSvc;
class CscIdHelper;

namespace Muon{
    class CscStripPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase< Muon::CscStripPrepDataContainer, Muon::MuonPRD_Container_p1 >

    {
    public:
        typedef Muon::MuonPRD_Container_p1 PERS; 
        typedef Muon::CscStripPrepDataContainer TRANS;
        CscStripPrepDataContainerCnv_p1(): m_cscStripId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0) {}
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual Muon::CscStripPrepDataContainer* createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log);
        //private:
        StatusCode initialize(MsgStream &log);

        const CscIdHelper *m_cscStripId;
        StoreGateSvc *m_storeGate;
        const MuonGM::MuonDetectorManager* m_muonDetMgr;
        bool m_isInitialized;
    };

}
#endif 

