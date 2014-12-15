/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPREPDATACONTAINERCNV_P2_H
#define MDTPREPDATACONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/MdtPrepDataContainer.h"

#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"


#include <iostream>

class StoreGateSvc;
class MdtIdHelper;
namespace MuonGM {class MuonDetectorManager;}

namespace Muon{
/** Class to handle the conversion of the transient MdtPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p2).
This replaces MdtPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the MdtPrepData.*/
class MdtPrepDataContainerCnv_p2 : public T_AthenaPoolTPCnvBase<Muon::MdtPrepDataContainer, Muon::MdtPrepDataContainer_p2>
{
public:
    MdtPrepDataContainerCnv_p2() : m_MdtId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0)  {};

    virtual void	persToTrans(const Muon::MdtPrepDataContainer_p2* persCont,
        Muon::MdtPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::MdtPrepDataContainer* transCont,
        Muon::MdtPrepDataContainer_p2* persCont,
        MsgStream &log) ;

    virtual Muon::MdtPrepDataContainer* createTransient(const Muon::MdtPrepDataContainer_p2* persObj, MsgStream& log);


private:
    const MdtIdHelper *m_MdtId;
    StoreGateSvc *m_storeGate;
    const MuonGM::MuonDetectorManager* m_muonDetMgr;
    bool m_isInitialized;
    StatusCode initialize(MsgStream &log);
};
}


#endif


