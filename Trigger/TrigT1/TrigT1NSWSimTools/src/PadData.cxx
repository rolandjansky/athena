/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1NSWSimTools/PadData.h"

std::ostream& operator<<(std::ostream& stream, NSWL1::PadData& data) {
    stream << "Pad hit at side " << ( (data.sideId())? "C":"A") // According to PadData.h
           << ", sector id "     << data.sectorId()
           << ", sector type "   << data.sectorType()
           << ", module id "     << data.moduleId()
           << ", multiplet id "  << data.multipletId()
           << ", gas gap id "    << data.gasGapId()
           << ", pad eta id "    << data.padEtaId()
           << ", pad phi id "    << data.padPhiId();

    return stream;
}

