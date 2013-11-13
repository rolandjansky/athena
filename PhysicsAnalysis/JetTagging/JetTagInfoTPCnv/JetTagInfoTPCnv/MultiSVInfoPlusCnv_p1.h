/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_MultiSVInfoPlusCnv_P1_H
#define JETTAGINFOTPCNV_MultiSVInfoPlusCnv_P1_H


#include "JetTagInfo/MSVVtxInfo.h"
#include "JetTagInfo/MultiSVInfoPlus.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/MultiSVInfoPlus_p1.h"

class MsgStream;


namespace Analysis {
  class MultiSVInfoPlusCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, MultiSVInfoPlus, MultiSVInfoPlus_p1>
    {
    public:
      inline MultiSVInfoPlusCnv_p1 (void)
	:m_baseTagCnv(0), m_mSVVtxInfoCnv(0)
	{}

      virtual void persToTrans(const MultiSVInfoPlus_p1 *persObj,
			       MultiSVInfoPlus *transObj,
			       MsgStream &log);
      virtual void transToPers(const MultiSVInfoPlus *persObj,
			       MultiSVInfoPlus_p1 *transObj,
			       MsgStream &log);

    private:
      BaseTagInfoCnv_p1 *m_baseTagCnv;
      ITPConverterFor<MSVVtxInfo> *m_mSVVtxInfoCnv;

    };
}


#endif
