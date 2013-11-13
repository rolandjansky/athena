/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SECVTXINFOCNV_P1_H
#define JETTAGINFOTPCNV_SECVTXINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/SecVtxInfo_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"
#include "JetTagInfo/SecVtxInfo.h"
#include "JetTagInfo/BaseTagInfo.h"
#include "VxVertex/RecVertex.h"

class MsgStream;

///
/// Converter header to move the sec vtx info into persistant format.
///


namespace Analysis {
  class BaseTagInfoCnv_p1;

  class SecVtxInfoCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, SecVtxInfo, SecVtxInfo_p1>
    {
    public:
      inline SecVtxInfoCnv_p1 (void)
	:m_baseTagCnv(0), m_recoVertexCnv(0)
	{}

      virtual void persToTrans(const SecVtxInfo_p1 *persObj,
			       SecVtxInfo *transObj,
			       MsgStream &log);
      virtual void transToPers(const SecVtxInfo *persObj,
			       SecVtxInfo_p1 *transObj,
			       MsgStream &log);

    private:
      BaseTagInfoCnv_p1 *m_baseTagCnv;
      ITPConverterFor<Trk::Vertex> *m_recoVertexCnv;
    };
}


#endif
