/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the SVInfoPlus persistent/transient classes
///

#define private public
#define protected public
#include "JetTagInfo/SVInfoPlus.h"
#include "JetTagInfo/SVTrackInfo.h"
#undef protected
#undef private
#include "JetTagInfoTPCnv/SVInfoPlusCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include "GaudiKernel/MsgStream.h"

#include <algorithm>
using std::transform;
using std::back_inserter;

namespace Analysis {


  ///
  /// Helper function to get the EL out of the se vector.
  ///
  const ElementLink<Rec::TrackParticleContainer> extractEL(const SVTrackInfo &item)
  {
    return item.m_track;
  }

  ///
  /// Given a transient rep, save it to a persistent one.
  ///
  void SVInfoPlusCnv_p1::transToPers (const SVInfoPlus *transObj,
				     SVInfoPlus_p1 *persObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    persObj->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv,
					      transObj,
					      msg);

    /// Now, our particular members.
    persObj->m_NGTinJet = transObj->m_NGTinJet;
    persObj->m_NGTinSvx = transObj->m_NGTinSvx;
    persObj->m_N2Tpair = transObj->m_N2Tpair;
    persObj->m_masssvx = transObj->m_masssvx;
    persObj->m_efracsvx = transObj->m_efracsvx;
    persObj->m_normdist = transObj->m_normdist;

    persObj->m_recsvx = toPersistent (&m_recoVertexCnv,
				      &transObj->m_recsvx,
				      msg);

    ///
    /// This is a little funny. The SVTrackInfo's are really just
    /// element links. So to save space, we will store them as such
    /// rather than trying to save them as a sub-object. So the first
    /// thing we have to do is convert them to a vector of element links.
    ///

    ElementLinkVector<Rec::TrackParticleContainer> movedVector;
    transform(transObj->m_trackinfo.begin(), transObj->m_trackinfo.end(),
	      back_inserter(movedVector), &extractEL);

    m_trackVecCnv.transToPers (&movedVector, &persObj->m_trackinfo, msg);
  }

  ///
  /// Helper class to turn an element link into a SVTrackInfo object.
  ///
  SVTrackInfo generateSVT (const ElementLink<Rec::TrackParticleContainer> &item)
  {
    SVTrackInfo result;
    result.m_track = item;
    return result;
  }

  ///
  /// Load from disk format to in-memory format.
  ///
  void SVInfoPlusCnv_p1::persToTrans (const SVInfoPlus_p1 *persObj,
				      SVInfoPlus *transObj,
				      MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->m_NGTinJet = persObj->m_NGTinJet;
    transObj->m_NGTinSvx = persObj->m_NGTinSvx;
    transObj->m_N2Tpair = persObj->m_N2Tpair;
    transObj->m_masssvx = persObj->m_masssvx;
    transObj->m_efracsvx = persObj->m_efracsvx;
    transObj->m_normdist = persObj->m_normdist;

    fillTransFromPStore (&m_recoVertexCnv,
			 persObj->m_recsvx,
			 &transObj->m_recsvx,
			 msg);

    ///
    /// This is a little funny. The SVTrackInfo's are really just
    /// element links. So to save space, we will store them as such
    /// rather than trying to save them as a sub-object. So in order
    /// to use these guys we have to extract them first, and then
    /// place them inside the objects next.
    ///

    ElementLinkVector<Rec::TrackParticleContainer> movedVector;
    m_trackVecCnv.persToTrans(&persObj->m_trackinfo, &movedVector, msg);

    transform(movedVector.begin(), movedVector.end(),
	      back_inserter(transObj->m_trackinfo),
	      &generateSVT);
  }

}
