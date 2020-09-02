/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_NSWCONDUTILS_H
#define MUONCONDSVC_NSWCONDUTILS_H

#include "MuonAlignmentData/CorrContainer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include <string>

namespace MuonCalib {
class NSWCondUtils {
 public:
  static void setNSWABLinesFromAscii(const std::string& filename,ALineMapContainer& writeALines, BLineMapContainer& writeBLines, const sTgcIdHelper* stgcHelper, const MmIdHelper* mmHelper);
  
 }; 
}

#endif
