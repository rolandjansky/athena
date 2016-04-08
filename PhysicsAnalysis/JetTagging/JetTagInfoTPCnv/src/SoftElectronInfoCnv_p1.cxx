/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code to fluf and compress the ip info plus data.

#include "JetTagInfo/SoftElectronInfo.h"
#include "JetTagInfoTPCnv/SoftElectronInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include <vector>
using std::vector;

namespace Analysis {

  ///
  /// Save data to the persistant format
  ///
  void SoftElectronInfoCnv_p1::transToPers(const SoftElectronInfo* pa, SoftElectronInfo_p1* pb, MsgStream & msg) {
    pb->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);

    pb->m_vectorOfTrackProb.assign(pa->vectorTrackProb().begin(),
                                   pa->vectorTrackProb().end());
    pb->m_nTrackProb = pa->nTrackProb();

    size_t sz = pa->numTrackInfo();
    pb->m_trackinfo.clear();
    pb->m_trackinfo.reserve(sz);
    for (size_t i = 0; i < sz; i++) {
      pb->m_trackinfo.push_back(toPersistent(&m_seTrackInfoCnv,
                                             &pa->getTrackInfo(i), msg));
    }
  }

  ///
  /// Fluf the data up again.
  ///
  void SoftElectronInfoCnv_p1::persToTrans(const SoftElectronInfo_p1* pa, SoftElectronInfo* pb, MsgStream & msg)
  {
    // Clear vector
    *pb = SoftElectronInfo();

    fillTransFromPStore (&m_baseTagCnv, pa->m_BaseTagInfo, pb, msg);

    pb->setTrackProb (std::vector<double> (pa->m_vectorOfTrackProb.begin(),
                                           pa->m_vectorOfTrackProb.end()));
    pb->setNTrackProb (pa->m_nTrackProb);

    for (const TPObjRef& pinfo : pa->m_trackinfo) {
      SETrackInfo info;
      fillTransFromPStore(&m_seTrackInfoCnv, pinfo, &info, msg);
      pb->addTrackInfo (info);
    }

  }

}
