/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "JetTagInfo/SVTrackInfo.h"
#include "JetTagInfo/MSVVtxInfo.h"
#include "JetTagInfoTPCnv/MSVVtxInfoCnv_p2.h"

#include "GaudiKernel/MsgStream.h"

#include <algorithm>
using std::transform;
using std::back_inserter;

namespace Analysis {


  ///
  /// Given a transient rep, save it to a persistent one.
  ///
  void MSVVtxInfoCnv_p2::transToPers (const MSVVtxInfo *transObj,
				     MSVVtxInfo_p2 *persObj,
				     MsgStream &msg)
  {

    persObj->m_masssvx = transObj->getMass();
    persObj->m_ptsvx = transObj->getPt();
    persObj->m_etasvx = transObj->getEta();
    persObj->m_phisvx = transObj->getPhi();
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
  void MSVVtxInfoCnv_p2::persToTrans (const MSVVtxInfo_p2 *persObj,
				      MSVVtxInfo *transObj,
				      MsgStream &msg)
  {
    // Clear contained vector.
    *transObj = MSVVtxInfo();

    transObj->setMass (persObj->m_masssvx);
    transObj->setPt (persObj->m_ptsvx);
    transObj->setEta (persObj->m_etasvx);
    transObj->setPhi (persObj->m_phisvx);
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
