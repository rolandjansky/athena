/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TgcPREPDATACONTAINERCNV_P2_H
#define TgcPREPDATACONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"

#include <iostream>

class StoreGateSvc;
class TgcIdHelper;


namespace Muon{
/** Class to handle the conversion of the transient TgcPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p2).
This replaces TgcPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the TgcPrepData.*/
class TgcPrepDataContainerCnv_p2 : public T_AthenaPoolTPCnvBase<Muon::TgcPrepDataContainer, Muon::TgcPrepDataContainer_p2>
{
public:
  TgcPrepDataContainerCnv_p2()  = default;

    virtual void	persToTrans(const Muon::TgcPrepDataContainer_p2* persCont,
        Muon::TgcPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::TgcPrepDataContainer* transCont,
        Muon::TgcPrepDataContainer_p2* persCont,
        MsgStream &log) ;

    virtual Muon::TgcPrepDataContainer* createTransient(const Muon::TgcPrepDataContainer_p2* persObj, MsgStream& log);


private:
    const MuonGM::TgcReadoutElement* getReadOutElement(const Identifier& id ) const;   
    const TgcIdHelper *m_TgcId{nullptr};
    StoreGateSvc *m_storeGate{nullptr};
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"}; 
    bool m_isInitialized{false};
    StatusCode initialize(MsgStream &log);
};
}


#endif


