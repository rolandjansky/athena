/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SETrackInfoCNV_P1_H
#define JETTAGINFOTPCNV_SETrackInfoCNV_P1_H

#include "JetTagInfoTPCnv/SETrackInfo_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/PhotonContainer.h"

#include "JetTagInfo/SETrackInfo.h"

class MsgStream;

namespace Analysis {

  class SETrackInfoCnv_p1 :
    public T_AthenaPoolTPCnvBase<SETrackInfo, SETrackInfo_p1>  {
    public:
    SETrackInfoCnv_p1() {}
    virtual void   persToTrans(const SETrackInfo_p1* persObj, SETrackInfo* transObj, MsgStream &log);
    virtual void   transToPers(const SETrackInfo* transObj, SETrackInfo_p1* persObj, MsgStream &log);

    private:
    ElementLinkCnv_p1<ElementLink<ElectronContainer> > m_eleElementLinkCnv;
    ElementLinkCnv_p1<ElementLink<PhotonContainer> > m_phoElementLinkCnv;

  };
}
#endif

