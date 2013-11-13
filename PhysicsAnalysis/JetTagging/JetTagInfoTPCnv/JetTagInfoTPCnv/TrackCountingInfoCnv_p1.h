/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_TRACKCOUNTINGINFOCNV_P1_H
#define JETTAGINFOTPCNV_TRACKCOUNTINGINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/TrackCountingInfo_p1.h"
#include "JetTagInfo/TrackCountingInfo.h"

class MsgStream;

namespace Analysis {

  class BaseTagInfoCnv_p1;

  class TrackCountingInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, TrackCountingInfo, TrackCountingInfo_p1>  {
  public:
    TrackCountingInfoCnv_p1()
      : m_baseTagCnv(0)
      {}

    virtual void   persToTrans(const TrackCountingInfo_p1* persObj, 
			       TrackCountingInfo* transObj, 
			       MsgStream &log);
    virtual void   transToPers(const TrackCountingInfo* transObj, 
			       TrackCountingInfo_p1* persObj, 
			       MsgStream &log);

    BaseTagInfoCnv_p1 *m_baseTagCnv;
  };
}
#endif

