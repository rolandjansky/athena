/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SMTrackInfoCNV_P1_H
#define JETTAGINFOTPCNV_SMTrackInfoCNV_P1_H

#include "JetTagInfoTPCnv/SMTrackInfo_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

#include "JetTagInfo/SMTrackInfo.h"

class MsgStream;

namespace Analysis {

  class SMTrackInfoCnv_p1 :
    public T_AthenaPoolTPCnvBase<SMTrackInfo, SMTrackInfo_p1>  {
    public:
    SMTrackInfoCnv_p1() {}
    virtual void   persToTrans(const SMTrackInfo_p1* persObj, SMTrackInfo* transObj, MsgStream &log);
    virtual void   transToPers(const SMTrackInfo* transObj, SMTrackInfo_p1* persObj, MsgStream &log);

    private:

    ElementLinkCnv_p1<ElementLink<MuonContainer> > m_elementLinkCnv;
  };
}
#endif

