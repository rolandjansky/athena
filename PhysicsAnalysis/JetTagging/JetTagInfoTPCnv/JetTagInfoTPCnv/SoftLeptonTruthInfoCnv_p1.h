/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SOFTLEPTONTRUTHINFOCNV_P1_H
#define JETTAGINFOTPCNV_SOFTLEPTONTRUTHINFOCNV_P1_H

#include "JetEvent/JetTagInfoBase.h"
#include "JetTagInfo/SoftLeptonTruthInfo.h"
#include "JetTagInfoTPCnv/SoftLeptonTruthInfo_p1.h"
#include "SLTrueInfoCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

namespace Analysis {

  class BaseTagInfoCnv_p1;

  class SoftLeptonTruthInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, SoftLeptonTruthInfo, SoftLeptonTruthInfo_p1> {

    public:
    
    inline SoftLeptonTruthInfoCnv_p1()
      : m_baseTagCnv(0), m_sltCnv(0)
      { }

    virtual void persToTrans(const SoftLeptonTruthInfo_p1 *persObj,
			     SoftLeptonTruthInfo *transObj,
			     MsgStream &log);
    virtual void transToPers(const SoftLeptonTruthInfo *persObj,
			     SoftLeptonTruthInfo_p1 *transObj,
			     MsgStream &log);

    private:
    BaseTagInfoCnv_p1 *m_baseTagCnv;
    ITPConverterFor<SLTrueInfo> *m_sltCnv;
  };
}


#endif

