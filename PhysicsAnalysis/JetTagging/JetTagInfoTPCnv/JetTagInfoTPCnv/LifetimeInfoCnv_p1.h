/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_LifeTimeCNV_P1_H
#define JETTAGINFOTPCNV_LifeTimeCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/LifetimeInfo_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"
#include "JetTagInfo/LifetimeInfo.h"
#include "JetTagInfo/BaseTagInfo.h"

class MsgStream;

///
/// Converter header to move the truth info into persistant format.
///


namespace Analysis {
  class BaseTagInfoCnv_p1;

  class LifetimeInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, LifetimeInfo, LifetimeInfo_p1>
    {
    public:
      inline LifetimeInfoCnv_p1 (void)
	:m_baseTagCnv(0)
	{}

      virtual void persToTrans(const LifetimeInfo_p1 *persObj,
			       LifetimeInfo *transObj,
			       MsgStream &log);
      virtual void transToPers(const LifetimeInfo *persObj,
			       LifetimeInfo_p1 *transObj,
			       MsgStream &log);

    private:
      BaseTagInfoCnv_p1 *m_baseTagCnv;
    };
}


#endif
