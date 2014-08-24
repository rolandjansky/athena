/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/RobRequestInfo.h"

void HLT::RobRequestInfo::addRequestScheduledRobIDs(const std::vector<uint32_t>& roblist) {
   m_requestScheduledRobIDs.resize(m_requestScheduledRobIDs.size()+roblist.size());
   std::copy_backward(roblist.begin(),roblist.end(), m_requestScheduledRobIDs.end());

}

void HLT::RobRequestInfo::addRequestScheduledRobIDs(uint32_t rob) {
   m_requestScheduledRobIDs.push_back(rob);
}
