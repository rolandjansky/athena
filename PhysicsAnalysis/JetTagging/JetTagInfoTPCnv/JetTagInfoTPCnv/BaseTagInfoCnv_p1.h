/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_BASETAGINFOCNV_P1_H
#define JETTAGINFOTPCNV_BASETAGINFOCNV_P1_H

#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfo/BaseTagInfo.h"

class MsgStream;

namespace Analysis {

  class BaseTagInfoCnv_p1 :
    public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, BaseTagInfo, BaseTagInfo_p1>  {
    public:
    BaseTagInfoCnv_p1() {}
    virtual void   persToTrans(const BaseTagInfo_p1* persObj, BaseTagInfo* transObj, MsgStream &log);
    virtual void   transToPers(const BaseTagInfo* transObj, BaseTagInfo_p1* persObj, MsgStream &log);
  };
}
#endif

