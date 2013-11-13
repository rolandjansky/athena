/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_IPINFOPLUSCNV_P1_H
#define JETTAGINFOTPCNV_IPINFOPLUSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/IPInfoPlus_p1.h"
#include "JetTagInfo/IPInfoPlus.h"
#include "JetTagInfo/IPInfoBase.h"

class MsgStream;

namespace Analysis {

  class IPInfoPlusCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, IPInfoPlus, IPInfoPlus_p1>  {
    public:
    IPInfoPlusCnv_p1()
      : m_baseTagCnv(0),
      m_trackInfoCnv(0)
      {}
    virtual void   persToTrans(const IPInfoPlus_p1* persObj, IPInfoPlus* transObj, MsgStream &log);
    virtual void   transToPers(const IPInfoPlus* transObj, IPInfoPlus_p1* persObj, MsgStream &log);

    private:

    BaseTagInfoCnv_p1 *m_baseTagCnv;

    //typedef T_AthenaPoolTPValVectorCnv<std::vector<IPTrackInfo>,
    //std::vector<TPObjRef>,
    //ITPConverterFor<IPTrackInfo> > IPTrackInfoVectorCnv;
    //IPTrackInfoVectorCnv m_trackInfoVectorCnv;
    ITPConverterFor<IPTrackInfo> *m_trackInfoCnv;
};

}
#endif

