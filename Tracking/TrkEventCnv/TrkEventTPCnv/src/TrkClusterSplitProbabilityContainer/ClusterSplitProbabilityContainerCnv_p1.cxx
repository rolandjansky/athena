/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/TrkClusterSplitProbabilityContainer/ClusterSplitProbabilityContainerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"

namespace Trk {
void ClusterSplitProbabilityContainerCnv_p1::transToPers(const Trk::ClusterSplitProbabilityContainer* transObj,
                                                         Trk::ClusterSplitProbabilityContainer_p1* persObj,
                                                         [[maybe_unused]] MsgStream& log) {
  persObj->reserve(transObj->m_splitProbMap.size());
  for(const std::pair<const Trk::ClusterSplitProbabilityContainer::ClusterIdentifier,
                      Trk::ClusterSplitProbabilityContainer::ProbabilityInfo> &elm : transObj->m_splitProbMap) {
     persObj->emplace_back(elm.first,elm.second.splitProbability1(),elm.second.splitProbability2(),elm.second.isSplit());
  }
}

void ClusterSplitProbabilityContainerCnv_p1::persToTrans(const Trk::ClusterSplitProbabilityContainer_p1* persObj,
                                                         Trk::ClusterSplitProbabilityContainer* transObj,
                                                         MsgStream& log) {

  for(const ClusterSplitProbability_p1 &elm : *persObj) {
     std::pair<std::unordered_map<Trk::ClusterSplitProbabilityContainer::ClusterIdentifier, Trk::ClusterSplitProbabilityContainer::ProbabilityInfo>::iterator, bool>
        ret = transObj->m_splitProbMap.insert( std::make_pair(elm.m_id,Trk::ClusterSplitProbabilityContainer::ProbabilityInfo(elm.m_splitPrpbability[0],
                                                                                                                              elm.m_splitPrpbability[1])));
     if (!ret.second) {
        log << MSG::FATAL << "Failed to insert cluster split probability info for " << elm.m_id << endmsg;
     }
     else {
        ret.first->second.setSplit(elm.m_isSplit);
     }
  }
}
}
