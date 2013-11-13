/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SVInfoPlusCNV_P1_H
#define JETTAGINFOTPCNV_SVInfoPlusCNV_P1_H

#define private public
#define protected public
#include "VxVertex/RecVertex.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#undef protected
#undef private

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/SVInfoPlus_p1.h"
#include "JetTagInfo/SVInfoPlus.h"

class MsgStream;

///
/// Converter header to move the sec vtx info into persistant format.
///


namespace Analysis {
  class SVInfoPlusCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, SVInfoPlus, SVInfoPlus_p1>
    {
    public:
      inline SVInfoPlusCnv_p1 (void)
	:m_baseTagCnv(0), m_recoVertexCnv(0)
	{}

      virtual void persToTrans(const SVInfoPlus_p1 *persObj,
			       SVInfoPlus *transObj,
			       MsgStream &log);
      virtual void transToPers(const SVInfoPlus *persObj,
			       SVInfoPlus_p1 *transObj,
			       MsgStream &log);

    private:
      BaseTagInfoCnv_p1 *m_baseTagCnv;
      ITPConverterFor<Trk::Vertex> *m_recoVertexCnv;
      ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > m_trackVecCnv;
    };
}


#endif
