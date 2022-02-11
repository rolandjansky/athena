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
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"


class MsgStream;
class StoreGateSvc;
class MdtIdHelper;

namespace Muon{
    class MdtPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase< Muon::MdtPrepDataContainer, Muon::MuonPRD_Container_p1 >

    {
    public:
        typedef Muon::MuonPRD_Container_p1 PERS; 
        typedef Muon::MdtPrepDataContainer TRANS;
        MdtPrepDataContainerCnv_p1() = default;
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual Muon::MdtPrepDataContainer* createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log);
    private:
        StatusCode initialize(MsgStream &log);

        const MuonGM::MdtReadoutElement* getReadOutElement(const Identifier& id ) const;         
        
        const MdtIdHelper *m_MdtId{nullptr};
        StoreGateSvc *m_storeGate{nullptr};
        ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"}; 
        bool m_isInitialized{false};
    };

}
#endif 

