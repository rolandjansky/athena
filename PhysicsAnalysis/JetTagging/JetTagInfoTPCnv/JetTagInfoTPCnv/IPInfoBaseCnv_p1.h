/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_IPINFOBASECNV_P1_H
#define JETTAGINFOTPCNV_IPINFOBASECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/IPInfoBase_p1.h"
#include "JetTagInfo/IPInfoBase.h"

class MsgStream;

namespace Analysis {

  class BaseTagInfoCnv_p1;

  class IPInfoBaseCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, IPInfoBase, IPInfoBase_p1>  {
  public:
    IPInfoBaseCnv_p1()
      : m_baseTagCnv(0)
      {}

    virtual void   persToTrans(const IPInfoBase_p1* persObj, IPInfoBase* transObj, MsgStream &log);
    virtual void   transToPers(const IPInfoBase* transObj, IPInfoBase_p1* persObj, MsgStream &log);

    BaseTagInfoCnv_p1 *m_baseTagCnv;
  };
}
#endif

