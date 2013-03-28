/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_OfflineOnlineIdConversion_h
#define MuonCalib_OfflineOnlineIdConversion_h

#include "MuonCalibIdentifier/MuonFixedId.h"
#include <string>

namespace MuonCalib {

MuonFixedId OnlineToOffline(const std::string online_id);

//std::string OfflineToOonline(const MuonFixedId &id, char ml_sep='/');


}

#endif
