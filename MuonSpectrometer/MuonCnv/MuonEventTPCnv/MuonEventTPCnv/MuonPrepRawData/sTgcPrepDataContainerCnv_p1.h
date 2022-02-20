/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTgcPREPDATACONTAINERCNV_p1_H
#define sTgcPREPDATACONTAINERCNV_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"


#include <iostream>

class StoreGateSvc;
class sTgcIdHelper;


namespace Muon{
/** Class to handle the conversion of the transient sTgcPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p1).
This replaces sTgcPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the sTgcPrepData.*/
class sTgcPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase<Muon::sTgcPrepDataContainer, Muon::sTgcPrepDataContainer_p1>
{
public:
    sTgcPrepDataContainerCnv_p1() = default;

    virtual void	persToTrans(const Muon::sTgcPrepDataContainer_p1* persCont,
        Muon::sTgcPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::sTgcPrepDataContainer* transCont,
        Muon::sTgcPrepDataContainer_p1* persCont,
        MsgStream &log) ;

    virtual Muon::sTgcPrepDataContainer* createTransient(const Muon::sTgcPrepDataContainer_p1* persObj, MsgStream& log);


private:
    const MuonGM::sTgcReadoutElement* getReadOutElement(const Identifier& id ) const;
    const sTgcIdHelper *m_sTgcId{nullptr};
    StoreGateSvc *m_storeGate{nullptr};
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"}; 
    bool m_isInitialized{false};
    StatusCode initialize(MsgStream &log);
};
}


#endif


