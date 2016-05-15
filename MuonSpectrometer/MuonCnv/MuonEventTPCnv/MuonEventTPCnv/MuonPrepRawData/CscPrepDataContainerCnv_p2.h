/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscPREPDATACONTAINERCNV_P2_H
#define CscPREPDATACONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/CscPrepDataContainer.h"

#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"


#include <iostream>

class StoreGateSvc;
class CscIdHelper;
namespace MuonGM {class MuonDetectorManager;}

namespace Muon{
/** Class to handle the conversion of the transient CscPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p2).
This replaces CscPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the CscPrepData.*/
class CscPrepDataContainerCnv_p2 : public T_AthenaPoolTPCnvBase<Muon::CscPrepDataContainer, Muon::CscPrepDataContainer_p2>
{
public:
    CscPrepDataContainerCnv_p2() : m_CscId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0)  {};

    virtual void	persToTrans(const Muon::CscPrepDataContainer_p2* persCont,
        Muon::CscPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::CscPrepDataContainer* transCont,
        Muon::CscPrepDataContainer_p2* persCont,
        MsgStream &log) ;

    virtual Muon::CscPrepDataContainer* createTransient(const Muon::CscPrepDataContainer_p2* persObj, MsgStream& log);


    //private:
    const CscIdHelper *m_CscId;
    StoreGateSvc *m_storeGate;
    const MuonGM::MuonDetectorManager* m_muonDetMgr;
    bool m_isInitialized;
    StatusCode initialize(MsgStream &log);
};
}


#endif


