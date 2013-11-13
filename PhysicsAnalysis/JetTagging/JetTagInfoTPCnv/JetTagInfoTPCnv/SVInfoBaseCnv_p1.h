/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SVINFOBASECNV_P1_H
#define JETTAGINFOTPCNV_SVINFOBASECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/SVInfoBase_p1.h"
#include "JetTagInfo/SVInfoBase.h"

class MsgStream;

namespace Analysis {

  class BaseTagInfoCnv_p1;

  class SVInfoBaseCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, SVInfoBase, SVInfoBase_p1>  {
  public:
    SVInfoBaseCnv_p1()
      : m_baseTagCnv(0)
      {}

    virtual void   persToTrans(const SVInfoBase_p1* persObj, SVInfoBase* transObj, MsgStream &log);
    virtual void   transToPers(const SVInfoBase* transObj, SVInfoBase_p1* persObj, MsgStream &log);

    BaseTagInfoCnv_p1 *m_baseTagCnv;
  };
}
#endif

