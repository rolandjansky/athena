/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the SVInfoPlus persistent/transient classes
///

#include "JetTagInfo/SVInfoPlus.h"
#include "JetTagInfo/SVTrackInfo.h"
#include "JetTagInfoTPCnv/SVInfoPlusCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include "GaudiKernel/MsgStream.h"

#include <algorithm>
using std::transform;
using std::back_inserter;

namespace Analysis {


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
    persObj->m_NGTinJet = transObj->getNGTrackInJet();
    persObj->m_NGTinSvx = transObj->getNGTrackInSvx();
    persObj->m_N2Tpair = transObj->getN2T();
    persObj->m_masssvx = transObj->getMass();
    persObj->m_efracsvx = transObj->getEnergyFraction();
    persObj->m_normdist = transObj->getNormDist();

    persObj->m_recsvx = toPersistent (&m_recoVertexCnv,
				      &transObj->getRecSvx(),
				      msg);

    ///
    /// This is a little funny. The SVTrackInfo's are really just
    /// element links. So to save space, we will store them as such
    /// rather than trying to save them as a sub-object. So the first
    /// thing we have to do is convert them to a vector of element links.
    ///

    ElementLinkVector<Rec::TrackParticleContainer> movedVector;
    size_t sz = transObj->numTrackInfo();
    for (size_t i = 0; i < sz; i++)
      movedVector.push_back (transObj->getTrackInfo(i).trackLink());

    m_trackVecCnv.transToPers (&movedVector, &persObj->m_trackinfo, msg);
  }

  ///
  /// Load from disk format to in-memory format.
  ///
  void SVInfoPlusCnv_p1::persToTrans (const SVInfoPlus_p1 *persObj,
				      SVInfoPlus *transObj,
				      MsgStream &msg)
  {
    // Clear contained vector.
    *transObj = SVInfoPlus();

    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->setNGTrackInJet (persObj->m_NGTinJet);
    transObj->setNGTrackInSvx (persObj->m_NGTinSvx);
    transObj->setN2T (persObj->m_N2Tpair);
    transObj->setMass (persObj->m_masssvx);
    transObj->setEnergyFraction (persObj->m_efracsvx);
    transObj->setNormDist (persObj->m_normdist);

    Trk::RecVertex svx;
    fillTransFromPStore (&m_recoVertexCnv,
			 persObj->m_recsvx,
			 &svx,
			 msg);
    transObj->setRecSvx (std::move (svx));

    ///
    /// This is a little funny. The SVTrackInfo's are really just
    /// element links. So to save space, we will store them as such
    /// rather than trying to save them as a sub-object. So in order
    /// to use these guys we have to extract them first, and then
    /// place them inside the objects next.
    ///

    ElementLinkVector<Rec::TrackParticleContainer> movedVector;
    m_trackVecCnv.persToTrans(&persObj->m_trackinfo, &movedVector, msg);
    size_t sz = movedVector.size();
    for (size_t i = 0; i < sz; i++)
      transObj->addTrackInfo (SVTrackInfo (movedVector[i]));
  }

}
