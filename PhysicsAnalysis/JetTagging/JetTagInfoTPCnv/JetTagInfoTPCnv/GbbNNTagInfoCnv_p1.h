/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_GbbNNTagInfoCNV_P1_H
#define JETTAGINFOTPCNV_GbbNNTagInfoCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/GbbNNTagInfo_p1.h"
#include "JetTagInfo/GbbNNTagInfo.h"

class MsgStream;

namespace Analysis {

  class BaseTagInfoCnv_p1;

  class GbbNNTagInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, GbbNNTagInfo, GbbNNTagInfo_p1>  {
  public:
    GbbNNTagInfoCnv_p1()
      : m_baseTagCnv(0)
      {}

    virtual void   persToTrans(const GbbNNTagInfo_p1* persObj, GbbNNTagInfo* transObj, MsgStream &log);
    virtual void   transToPers(const GbbNNTagInfo* transObj, GbbNNTagInfo_p1* persObj, MsgStream &log);

    BaseTagInfoCnv_p1 *m_baseTagCnv;
  };
}
#endif

