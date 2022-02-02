/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RpcPREPDATACONTAINERCNV_P3_H
#define RpcPREPDATACONTAINERCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"


#include <iostream>

class StoreGateSvc;
class RpcIdHelper;

namespace Muon{
/** Class to handle the conversion of the transient RpcPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p3).
This replaces RpcPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the RpcPrepData.*/
class RpcPrepDataContainerCnv_p3 : public T_AthenaPoolTPCnvBase<Muon::RpcPrepDataContainer, Muon::RpcPrepDataContainer_p3>
{
public:
    RpcPrepDataContainerCnv_p3() = default;

    virtual void	persToTrans(const Muon::RpcPrepDataContainer_p3* persCont,
        Muon::RpcPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::RpcPrepDataContainer* transCont,
        Muon::RpcPrepDataContainer_p3* persCont,
        MsgStream &log) ;

    virtual Muon::RpcPrepDataContainer* createTransient(const Muon::RpcPrepDataContainer_p3* persObj, MsgStream& log);


private:
    const MuonGM::RpcReadoutElement* getReadOutElement(const Identifier& id ) const;   
    const RpcIdHelper *m_RpcId{nullptr};
    StoreGateSvc *m_storeGate{nullptr};
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"}; 
    bool m_isInitialized{false};
    StatusCode initialize(MsgStream &log);
};
}


#endif


