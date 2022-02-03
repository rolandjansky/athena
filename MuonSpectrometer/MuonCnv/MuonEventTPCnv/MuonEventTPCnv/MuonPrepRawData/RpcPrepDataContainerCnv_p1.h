/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCPREPDATACONTAINERCNV_P1_H
#define RPCPREPDATACONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   RpcPrepDataContainerCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"

class MsgStream;
class StoreGateSvc;
class RpcIdHelper;

namespace Muon{
    class RpcPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase< Muon::RpcPrepDataContainer, Muon::MuonPRD_Container_p1 >

    {
    public:
        typedef Muon::MuonPRD_Container_p1 PERS; 
        typedef Muon::RpcPrepDataContainer TRANS;
        RpcPrepDataContainerCnv_p1() = default;
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual Muon::RpcPrepDataContainer* createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log);
    private:
        StatusCode initialize(MsgStream &log);

        const MuonGM::RpcReadoutElement* getReadOutElement(const Identifier& id ) const;   
        const RpcIdHelper *m_RpcId{nullptr};
        StoreGateSvc *m_storeGate{nullptr};
        ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"}; 
        bool m_isInitialized{false};
    };

}
#endif 

