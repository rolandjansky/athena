/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTgcPREPDATACONTAINERCNV_p1_H
#define sTgcPREPDATACONTAINERCNV_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/sTgcPrepDataContainer.h"

#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"


#include <iostream>

class StoreGateSvc;
class sTgcIdHelper;
namespace MuonGM {class MuonDetectorManager;}

namespace Muon{
/** Class to handle the conversion of the transient sTgcPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p1).
This replaces sTgcPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the sTgcPrepData.*/
class sTgcPrepDataContainerCnv_p1 : public T_AthenaPoolTPCnvBase<Muon::sTgcPrepDataContainer, Muon::sTgcPrepDataContainer_p1>
{
public:
    sTgcPrepDataContainerCnv_p1() : m_sTgcId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0)  {};

    virtual void	persToTrans(const Muon::sTgcPrepDataContainer_p1* persCont,
        Muon::sTgcPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::sTgcPrepDataContainer* transCont,
        Muon::sTgcPrepDataContainer_p1* persCont,
        MsgStream &log) ;

    virtual Muon::sTgcPrepDataContainer* createTransient(const Muon::sTgcPrepDataContainer_p1* persObj, MsgStream& log);


private:
    const sTgcIdHelper *m_sTgcId;
    StoreGateSvc *m_storeGate;
    const MuonGM::MuonDetectorManager* m_muonDetMgr;
    bool m_isInitialized;
    StatusCode initialize(MsgStream &log);
};
}


#endif


