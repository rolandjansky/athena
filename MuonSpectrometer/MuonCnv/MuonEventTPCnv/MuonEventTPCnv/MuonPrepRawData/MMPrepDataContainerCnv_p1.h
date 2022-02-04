/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMPREPDATACONTAINERCNV_p1_H
#define MMPREPDATACONTAINERCNV_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"


#include <iostream>

class StoreGateSvc;
class MmIdHelper;


namespace Muon{
/** Class to handle the conversion of the transient MMPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p1).
This replaces MMPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the MMPrepData.*/
class MMPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase<Muon::MMPrepDataContainer, Muon::MMPrepDataContainer_p1>
{
public:
    MMPrepDataContainerCnv_p1() = default;

    virtual void	persToTrans(const Muon::MMPrepDataContainer_p1* persCont,
        Muon::MMPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::MMPrepDataContainer* transCont,
        Muon::MMPrepDataContainer_p1* persCont,
        MsgStream &log) ;

    virtual Muon::MMPrepDataContainer* createTransient(const Muon::MMPrepDataContainer_p1* persObj, MsgStream& log);


private:
    const MuonGM::MMReadoutElement* getReadOutElement(const Identifier& id ) const;
    const MmIdHelper *m_MMId{nullptr};
    StoreGateSvc *m_storeGate{nullptr};
    ToolHandle  < Trk::IEventCnvSuperTool > m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"};
    bool m_isInitialized{false};
    StatusCode initialize(MsgStream &log);
};
}


#endif


