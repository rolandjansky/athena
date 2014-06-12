/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUEVENTDICT_H
#define PANTAUEVENTDICT_H

#include "PanTauEvent/TauSeed.h"
#include "PanTauEvent/TauSeedCollection.h"

#include "PanTauEvent/PanTauDetails.h"

namespace PanTauEventDict{
  struct tmp {
      std::vector<PanTau::TauSeed*> m_TauSeedDummyVec;
  };
}
#endif
