/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_JetProbInfoBASECNV_P1_H
#define JETTAGINFOTPCNV_JetProbInfoBASECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/JetProbInfoBase_p1.h"
#include "JetTagInfo/JetProbInfoBase.h"

class MsgStream;

namespace Analysis {

  class BaseTagInfoCnv_p1;

  class JetProbInfoBaseCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, JetProbInfoBase, JetProbInfoBase_p1>  {
  public:
    JetProbInfoBaseCnv_p1()
      : m_baseTagCnv(0)
      {}

    virtual void   persToTrans(const JetProbInfoBase_p1* persObj, JetProbInfoBase* transObj, MsgStream &log);
    virtual void   transToPers(const JetProbInfoBase* transObj, JetProbInfoBase_p1* persObj, MsgStream &log);

    BaseTagInfoCnv_p1 *m_baseTagCnv;
  };
}
#endif

