/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"


class MsgStream;
class StoreGateSvc;
class TgcIdHelper;

namespace Muon{
    class TgcPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase< Muon::TgcPrepDataContainer, Muon::MuonPRD_Container_p1 >

    {
    public:
        typedef Muon::MuonPRD_Container_p1 PERS; 
        typedef Muon::TgcPrepDataContainer TRANS;
        TgcPrepDataContainerCnv_p1() = default;
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual Muon::TgcPrepDataContainer* createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log);
    private:
        StatusCode initialize(MsgStream &log);

        const MuonGM::TgcReadoutElement* getReadOutElement(const Identifier& id ) const;   
        const TgcIdHelper *m_TgcId{nullptr};
        StoreGateSvc *m_storeGate{nullptr};
        ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"}; 
        bool m_isInitialized{false};
    
   
    };

}
#endif 

