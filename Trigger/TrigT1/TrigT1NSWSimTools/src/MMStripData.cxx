/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1NSWSimTools/MMStripData.h"

std::ostream& operator<<(std::ostream& stream, NSWL1::MMStripData& data) {
    stream << "MM strip hit at side " << ( (data.sideId())? "A":"C")
           << ", trigger sector "     << data.sectorId()
           << ", module "             << data.moduleId()
           << ", multiplet "          << data.multipletId()
           << ", gas gap "            << data.gasGapId();

    return stream;
}

