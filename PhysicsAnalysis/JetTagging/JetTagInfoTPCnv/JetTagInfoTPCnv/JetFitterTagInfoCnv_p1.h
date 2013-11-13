/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_JetFitterTagInfoCNV_P1_H
#define JETTAGINFOTPCNV_JetFitterTagInfoCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/JetFitterTagInfo_p1.h"
#include "JetTagInfo/JetFitterTagInfo.h"

class MsgStream;

namespace Analysis {

  class BaseTagInfoCnv_p1;

  class JetFitterTagInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, JetFitterTagInfo, JetFitterTagInfo_p1>  {
  public:
    JetFitterTagInfoCnv_p1()
      : m_baseTagCnv(0)
      {}

    virtual void   persToTrans(const JetFitterTagInfo_p1* persObj, JetFitterTagInfo* transObj, MsgStream &log);
    virtual void   transToPers(const JetFitterTagInfo* transObj, JetFitterTagInfo_p1* persObj, MsgStream &log);

    BaseTagInfoCnv_p1 *m_baseTagCnv;
  };
}
#endif

