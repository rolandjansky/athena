/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the SvxSummary persistent/transient classes
///

#include "JetTagInfo/SvxSummary.h"
#include "JetTagInfoTPCnv/SvxSummaryCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include "GaudiKernel/MsgStream.h"

#include "TPHelpers.h"

namespace Analysis {

  ///
  /// Given a transient rep, save it to a persistent one.
  ///
  void SvxSummaryCnv_p1::transToPers (const SvxSummary *transObj,
				     SvxSummary_p1 *persObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    persObj->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv,
					      transObj,
					      msg);

    /// Now, our particular members.
    persObj->m_Results.assign(transObj->Results().begin(),
                              transObj->Results().end());

    /// Now, the various objects that are contained.
    persObj->m_Svx = baseToPersistent(&m_recoVertexCnv,
				      &transObj->Svx(),
				      msg);

    transToPersVectorObj
      (this,
       &m_trackParticleCnv,
       transObj->TrkInSvx(),
       persObj->m_TrkInSvx,
       msg);
    transToPersVectorObj
      (this,
       &m_trackParticleCnv,
       transObj->TrkFromV0(),
       persObj->m_TrkFromV0,
       msg);

  }

  ///
  /// Load from disk format to in-memory format.
  ///
  void SvxSummaryCnv_p1::persToTrans (const SvxSummary_p1 *persObj,
				      SvxSummary *transObj,
				      MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->Results (std::vector<double> (persObj->m_Results.begin(),
                                            persObj->m_Results.end()));

    /// Now, the various objects that are contained.
    Trk::RecVertex svx;
    fillTransFromPStore(&m_recoVertexCnv,
			persObj->m_Svx,
			&svx,
			msg);
    transObj->Svx (std::move (svx));

    std::vector<const Rec::TrackParticle*> trkInSvx;
    persToTransVectorObj(this,
			 &m_trackParticleCnv,
			 persObj->m_TrkInSvx,
			 trkInSvx,
			 msg);
    transObj->TrkInSvx (std::move (trkInSvx));

    std::vector<const Rec::TrackParticle*> trkFromV0;
    persToTransVectorObj(this,
			 &m_trackParticleCnv,
			 persObj->m_TrkFromV0,
			 trkFromV0,
			 msg);
    transObj->TrkFromV0 (std::move (trkFromV0));
  }

}
