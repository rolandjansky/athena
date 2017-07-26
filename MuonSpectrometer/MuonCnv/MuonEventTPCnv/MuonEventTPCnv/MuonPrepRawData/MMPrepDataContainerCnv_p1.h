/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMPREPDATACONTAINERCNV_p1_H
#define MMPREPDATACONTAINERCNV_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"

#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"

#include "MuonEventTPCnv/CreateTransientTemplates.h"
#include <iostream>

class StoreGateSvc;
class MmIdHelper;
namespace MuonGM {class MuonDetectorManager;}

namespace Muon{
/** Class to handle the conversion of the transient MMPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p1).
This replaces MMPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the MMPrepData.*/
class MMPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase<Muon::MMPrepDataContainer, Muon::MMPrepDataContainer_p1>
{
public:
    MMPrepDataContainerCnv_p1() : m_MMId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0)  {};

    virtual void	persToTrans(const Muon::MMPrepDataContainer_p1* persCont,
        Muon::MMPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::MMPrepDataContainer* transCont,
        Muon::MMPrepDataContainer_p1* persCont,
        MsgStream &log) ;

    virtual Muon::MMPrepDataContainer* createTransient(const Muon::MMPrepDataContainer_p1* persObj, MsgStream& log);


private:
    const MmIdHelper *m_MMId;
    StoreGateSvc *m_storeGate;
    const MuonGM::MuonDetectorManager* m_muonDetMgr;
    bool m_isInitialized;
    StatusCode initialize(MsgStream &log);
};
}


#endif


