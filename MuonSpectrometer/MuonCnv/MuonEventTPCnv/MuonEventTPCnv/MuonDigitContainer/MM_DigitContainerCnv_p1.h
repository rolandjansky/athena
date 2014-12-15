/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDIGITCONTAINERCNV_p1_H
#define MMDIGITCONTAINERCNV_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonDigitContainer_p1.h" // Has typedefs to all concrete types e.g. MM_DigitContainer_p1

#include <iostream>

class StoreGateSvc;
class MmIdHelper;
namespace MuonGM {class MuonDetectorManager;}

namespace Muon{
/** Class to handle the conversion of the transient MmDigitContainer into its persistent representation */
class MM_DigitContainerCnv_p1 : public T_AthenaPoolTPCnvBase<MmDigitContainer, Muon::MM_DigitContainer_p1>
{
public:
    MM_DigitContainerCnv_p1() : m_MMId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0)  {};

    virtual void	persToTrans(const Muon::MM_DigitContainer_p1* persCont,
        MmDigitContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const MmDigitContainer* transCont,
        Muon::MM_DigitContainer_p1* persCont,
        MsgStream &log) ;

    virtual MmDigitContainer* createTransient(const Muon::MM_DigitContainer_p1* persObj, MsgStream& log);


private:
    const MmIdHelper *m_MMId;
    StoreGateSvc *m_storeGate;
    const MuonGM::MuonDetectorManager* m_muonDetMgr;
    bool m_isInitialized;
    StatusCode initialize(MsgStream &log);
};
}


#endif


