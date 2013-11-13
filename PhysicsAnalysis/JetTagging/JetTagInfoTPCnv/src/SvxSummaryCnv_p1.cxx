/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the SvxSummary persistent/transient classes
///

#define private public
#define protected public
#include "JetTagInfo/SvxSummary.h"
#undef protected
#undef private
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
    persObj->m_Results.assign(transObj->m_Results.begin(), transObj->m_Results.end());

    /// Now, the various objects that are contained.
    persObj->m_Svx = baseToPersistent(&m_recoVertexCnv,
				      &transObj->m_Svx,
				      msg);

    transToPersVectorObj
      (this,
       &m_trackParticleCnv,
       transObj->m_TrkInSvx,
       persObj->m_TrkInSvx,
       msg);
    transToPersVectorObj
      (this,
       &m_trackParticleCnv,
       transObj->m_TrkFromV0,
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
    transObj->m_Results.assign(persObj->m_Results.begin(), persObj->m_Results.end());

    /// Now, the various objects that are contained.
    fillTransFromPStore(&m_recoVertexCnv,
			persObj->m_Svx,
			&transObj->m_Svx,
			msg);
    
    persToTransVectorObj(this,
			 &m_trackParticleCnv,
			 persObj->m_TrkInSvx,
			 transObj->m_TrkInSvx,
			 msg);
    persToTransVectorObj(this,
			 &m_trackParticleCnv,
			 persObj->m_TrkFromV0,
			 transObj->m_TrkFromV0,
			 msg);
  }

}
