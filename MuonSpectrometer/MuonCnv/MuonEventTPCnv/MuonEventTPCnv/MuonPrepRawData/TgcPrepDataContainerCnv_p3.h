/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TgcPREPDATACONTAINERCNV_P3_H
#define TgcPREPDATACONTAINERCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"


#include <iostream>

class StoreGateSvc;
class TgcIdHelper;
namespace Muon{
/** Class to handle the conversion of the transient TgcPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p3).
This replaces TgcPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the TgcPrepData.*/
class TgcPrepDataContainerCnv_p3 : public T_AthenaPoolTPCnvBase<Muon::TgcPrepDataContainer, Muon::TgcPrepDataContainer_p3>
{
public:
  TgcPrepDataContainerCnv_p3() = default;

    virtual void	persToTrans(const Muon::TgcPrepDataContainer_p3* persCont,
        Muon::TgcPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::TgcPrepDataContainer* transCont,
        Muon::TgcPrepDataContainer_p3* persCont,
        MsgStream &log) ;

    virtual Muon::TgcPrepDataContainer* createTransient(const Muon::TgcPrepDataContainer_p3* persObj, MsgStream& log);


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


