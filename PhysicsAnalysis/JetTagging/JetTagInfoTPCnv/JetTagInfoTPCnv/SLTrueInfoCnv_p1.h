/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SoftLeptonTRUTHINFOCNV_P1_H
#define JETTAGINFOTPCNV_SoftLeptonTRUTHINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/SLTrueInfo_p1.h"
#include "JetTagInfo/SLTrueInfo.h"

namespace Analysis {

  class SLTrueInfoCnv_p1
    : public T_AthenaPoolTPCnvBase<SLTrueInfo, SLTrueInfo_p1>
    {
    public:
      virtual void persToTrans(const SLTrueInfo_p1 *persObj,
			       SLTrueInfo *transObj,
			       MsgStream &log);
      virtual void transToPers(const SLTrueInfo *persObj,
			       SLTrueInfo_p1 *transObj,
			       MsgStream &log);

    };

}

#endif
