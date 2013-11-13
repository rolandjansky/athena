/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SoftMuonInfoCNV_P1_H
#define JETTAGINFOTPCNV_SoftMuonInfoCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/SoftMuonInfo_p1.h"
#include "JetTagInfo/SoftMuonInfo.h"
#include "JetTagInfo/SMTrackInfo.h"

class MsgStream;

namespace Analysis {

  class BaseTagInfoCnv_p1;

  class SoftMuonInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, SoftMuonInfo, SoftMuonInfo_p1>  {
  public:
    SoftMuonInfoCnv_p1()
      : m_baseTagCnv(0),
      m_smTrackInfoCnv(0)
      {}

    virtual void   persToTrans(const SoftMuonInfo_p1* persObj, SoftMuonInfo* transObj, MsgStream &log);
    virtual void   transToPers(const SoftMuonInfo* transObj, SoftMuonInfo_p1* persObj, MsgStream &log);

    BaseTagInfoCnv_p1 *m_baseTagCnv;
    ITPConverterFor<SMTrackInfo> *m_smTrackInfoCnv;
};

}
#endif

