/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_TRUTHINFOCNV_P1_H
#define JETTAGINFOTPCNV_TRUTHINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/TruthInfo_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"
#include "JetTagInfo/TruthInfo.h"

class MsgStream;

///
/// Converter header to move the truth info into persistant format.
///


namespace Analysis {
  class BaseTagInfoCnv_p1;

  class TruthInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, TruthInfo, TruthInfo_p1>
    {
    public:
      inline TruthInfoCnv_p1 (void)
	:m_baseTagCnv(0)
	{}

      virtual void persToTrans(const TruthInfo_p1 *persObj,
			       TruthInfo *transObj,
			       MsgStream &log);
      virtual void transToPers(const TruthInfo *persObj,
			       TruthInfo_p1 *transObj,
			       MsgStream &log);

    private:
      BaseTagInfoCnv_p1 *m_baseTagCnv;
    };
}


#endif
