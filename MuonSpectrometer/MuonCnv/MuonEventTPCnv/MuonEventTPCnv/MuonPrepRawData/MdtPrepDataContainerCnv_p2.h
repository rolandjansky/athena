/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPREPDATACONTAINERCNV_P2_H
#define MDTPREPDATACONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"


#include <iostream>

class StoreGateSvc;
class MdtIdHelper;

namespace Muon{
/** Class to handle the conversion of the transient MdtPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p2).
This replaces MdtPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the MdtPrepData.*/
class MdtPrepDataContainerCnv_p2 : public T_AthenaPoolTPCnvBase<Muon::MdtPrepDataContainer, Muon::MdtPrepDataContainer_p2>
{
public:
    MdtPrepDataContainerCnv_p2() = default;

    virtual void	persToTrans(const Muon::MdtPrepDataContainer_p2* persCont,
        Muon::MdtPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::MdtPrepDataContainer* transCont,
        Muon::MdtPrepDataContainer_p2* persCont,
        MsgStream &log) ;

    virtual Muon::MdtPrepDataContainer* createTransient(const Muon::MdtPrepDataContainer_p2* persObj, MsgStream& log);


private:
    const MuonGM::MdtReadoutElement* getReadOutElement(const Identifier& id ) const;          
        
    const MdtIdHelper *m_MdtId{nullptr};
    StoreGateSvc *m_storeGate{nullptr};
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"}; 
    bool m_isInitialized{false};
    StatusCode initialize(MsgStream &log);
};
}


#endif


