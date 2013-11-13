/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SvxSummaryCNV_P1_H
#define JETTAGINFOTPCNV_SvxSummaryCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetTagInfoTPCnv/SvxSummary_p1.h"
#include "JetTagInfo/SvxSummary.h"

#include "VxVertex/RecVertex.h"
#include "Particle/TrackParticle.h"

class MsgStream;

///
/// Converter header to move the sec vtx info into persistant format.
///


namespace Analysis {
  class BaseTagInfoCnv_p1;

  class SvxSummaryCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<JetTagInfoBase, SvxSummary, SvxSummary_p1>
    {
    public:
      inline SvxSummaryCnv_p1 (void)
	:m_baseTagCnv(0), m_recoVertexCnv(0), m_trackParticleCnv(0)
	{}

      virtual void persToTrans(const SvxSummary_p1 *persObj,
			       SvxSummary *transObj,
			       MsgStream &log);
      virtual void transToPers(const SvxSummary *persObj,
			       SvxSummary_p1 *transObj,
			       MsgStream &log);

    private:
      BaseTagInfoCnv_p1 *m_baseTagCnv;
      ITPConverterFor<Trk::Vertex> *m_recoVertexCnv;
      ITPConverterFor<Rec::TrackParticle> *m_trackParticleCnv;
    };
}


#endif
