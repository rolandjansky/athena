/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#define private public
#define protected public
#include "JetTagInfo/SVTrackInfo.h"
#include "JetTagInfo/MSVVtxInfo.h"
#undef protected
#undef private
#include "JetTagInfoTPCnv/MSVVtxInfoCnv_p2.h"

#include "GaudiKernel/MsgStream.h"

#include <algorithm>
using std::transform;
using std::back_inserter;

namespace Analysis {


  ///
  /// Helper function to get the EL out of the se vector.
  ///
  const ElementLink<Rec::TrackParticleContainer> extractELForMSV2(const SVTrackInfo &item)
  {
    return item.m_track;
  }

  ///
  /// Given a transient rep, save it to a persistent one.
  ///
  void MSVVtxInfoCnv_p2::transToPers (const MSVVtxInfo *transObj,
				     MSVVtxInfo_p2 *persObj,
				     MsgStream &msg)
  {

    persObj->m_masssvx = transObj->m_masssvx;
    persObj->m_ptsvx = transObj->m_ptsvx;
    persObj->m_etasvx = transObj->m_etasvx;
    persObj->m_phisvx = transObj->m_phisvx;
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
	      back_inserter(movedVector), &extractELForMSV2);

    m_trackVecCnv.transToPers (&movedVector, &persObj->m_trackinfo, msg);
  }

  ///
  /// Helper class to turn an element link into a SVTrackInfo object.
  ///
  SVTrackInfo generateSVTForMSV2 (const ElementLink<Rec::TrackParticleContainer> &item)
  {
    SVTrackInfo result;
    result.m_track = item;
    return result;
  }

  ///
  /// Load from disk format to in-memory format.
  ///
  void MSVVtxInfoCnv_p2::persToTrans (const MSVVtxInfo_p2 *persObj,
				      MSVVtxInfo *transObj,
				      MsgStream &msg)
  {
 

    transObj->m_masssvx = persObj->m_masssvx;
    transObj->m_ptsvx = persObj->m_ptsvx;
    transObj->m_etasvx = persObj->m_etasvx;
    transObj->m_phisvx = persObj->m_phisvx;
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
	      &generateSVTForMSV2);
  }

}
