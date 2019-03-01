/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMONTOOLS_LARTRIGSTREAMMACHING
#define LARMONTOOLS_LARTRIGSTREAMMACHING

#include <vector>
#include <string>

#include "xAODEventInfo/EventInfo.h"

namespace LArMon {

//Returns a vector with the indices of the streamsFromJobO that match an entry in streamsInEvent. For streams that are not part of streamsFromJobO
//streamsFromJobO.size() is added to the return vector
std::vector<unsigned> trigStreamMatching(const std::vector<std::string>& streamsFromJobO, const std::vector<xAOD::EventInfo::StreamTag>& streamInEvent);

}
#endif
