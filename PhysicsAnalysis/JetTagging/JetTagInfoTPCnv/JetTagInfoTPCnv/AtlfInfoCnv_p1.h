/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_ATLINFOCNV_P1_H
#define JETTAGINFOTPCNV_ATLINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/AtlfInfo_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"
#include "JetTagInfo/AtlfInfo.h"

class MsgStream;

///
/// Converter header to move the truth info into persistant format.
///


namespace Analysis {

  class BaseTagInfoCnv_p1;

  class AtlfInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, AtlfInfo, AtlfInfo_p1>
    {
    public:
      inline AtlfInfoCnv_p1 (void)
	: m_baseTagCnv(0)
	{}

      virtual void persToTrans(const AtlfInfo_p1 *persObj,
			       AtlfInfo *transObj,
			       MsgStream &log);
      virtual void transToPers(const AtlfInfo *persObj,
			       AtlfInfo_p1 *transObj,
			       MsgStream &log);

    private:
      BaseTagInfoCnv_p1 *m_baseTagCnv;

    };
}


#endif
