/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SoftElectronInfoCNV_P1_H
#define JETTAGINFOTPCNV_SoftElectronInfoCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/SoftElectronInfo_p1.h"
#include "JetTagInfo/SoftElectronInfo.h"
#include "JetTagInfo/SETrackInfo.h"

class MsgStream;

namespace Analysis {

  class BaseTagInfoCnv_p1;

  class SoftElectronInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, SoftElectronInfo, SoftElectronInfo_p1>  {
  public:
    SoftElectronInfoCnv_p1()
      : m_baseTagCnv(0),
      m_seTrackInfoCnv(0)
      {}

    virtual void   persToTrans(const SoftElectronInfo_p1* persObj, SoftElectronInfo* transObj, MsgStream &log);
    virtual void   transToPers(const SoftElectronInfo* transObj, SoftElectronInfo_p1* persObj, MsgStream &log);

    BaseTagInfoCnv_p1 *m_baseTagCnv;
    ITPConverterFor<SETrackInfo> *m_seTrackInfoCnv;
};

}
#endif

