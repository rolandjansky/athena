/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Translation for the IPTrackInfoCnv object.
///

#include "JetTagInfo/IPTrackInfo.h"
#include "JetTagInfoTPCnv/IPTrackInfoCnv_p1.h"

namespace Analysis {

  void IPTrackInfoCnv_p1::transToPers(const IPTrackInfo* pa, IPTrackInfo_p1* pb, MsgStream & msg) {
    m_elementLinkCnv.transToPers(&pa->trackLink(), &pb->m_track, msg);
    pb->m_isFromV0 = pa->isFromV0();
    pb->m_valD0wrtPV = pa->d0Value();
    pb->m_sigD0wrtPV = pa->d0Significance();
    pb->m_valZ0wrtPV = pa->z0Value();
    pb->m_sigZ0wrtPV = pa->z0Significance();
    pb->m_trackWeight2D = pa->trackWeight2D();
    pb->m_trackWeight3D = pa->trackWeight3D();
    pb->m_trackProbJP = pa->trackProbJP();
    pb->m_trackProbJPneg = pa->trackProbJPneg();

    pb->m_trackGrade_int = pa->trackGrade().gradeNumber();
    pb->m_trackGrade_string = pa->trackGrade().gradeString();
  }

  void IPTrackInfoCnv_p1::persToTrans(const IPTrackInfo_p1* pa, IPTrackInfo* pb, MsgStream & msg)
  {
    ElementLink<Rec::TrackParticleContainer> trackLink;
    m_elementLinkCnv.persToTrans(&pa->m_track, &trackLink, msg);

    *pb = IPTrackInfo (trackLink,
                       Analysis::TrackGrade (pa->m_trackGrade_int,
                                             pa->m_trackGrade_string),
                       pa->m_isFromV0,
                       pa->m_valD0wrtPV,
                       pa->m_sigD0wrtPV,
                       pa->m_valZ0wrtPV,
                       pa->m_sigZ0wrtPV);

    pb->setTrackWeight2D (pa->m_trackWeight2D);
    pb->setTrackWeight3D (pa->m_trackWeight3D);
    pb->setTrackProbJP (pa->m_trackProbJP);
    pb->setTrackProbJPneg (pa->m_trackProbJPneg);
  }

}
