/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscPREPDATACONTAINERCNV_P2_H
#define CscPREPDATACONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"


#include <iostream>

class StoreGateSvc;
class CscIdHelper;


namespace Muon{
/** Class to handle the conversion of the transient CscPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p2).
This replaces CscPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the CscPrepData.*/
class CscPrepDataContainerCnv_p2 : public T_AthenaPoolTPCnvBase<Muon::CscPrepDataContainer, Muon::CscPrepDataContainer_p2>
{
public:
    CscPrepDataContainerCnv_p2() =default;

    virtual void	persToTrans(const Muon::CscPrepDataContainer_p2* persCont,
        Muon::CscPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::CscPrepDataContainer* transCont,
        Muon::CscPrepDataContainer_p2* persCont,
        MsgStream &log) ;

    virtual Muon::CscPrepDataContainer* createTransient(const Muon::CscPrepDataContainer_p2* persObj, MsgStream& log);


private:
    const MuonGM::CscReadoutElement* getReadOutElement(const Identifier& id ) const;
    const CscIdHelper *m_CscId{nullptr};
    StoreGateSvc *m_storeGate{nullptr};
    ToolHandle  < Trk::IEventCnvSuperTool >  m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"}; 
    bool m_isInitialized{false};

    StatusCode initialize(MsgStream &log);
};
}


#endif


