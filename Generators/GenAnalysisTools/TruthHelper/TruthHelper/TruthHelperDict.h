// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_THRUTHHELPERDICT_H
#define TRUTHHELPER_THRUTHHELPERDICT_H

#include <vector>
#include "TruthHelper/GenIMCselector.h"
#include "TruthHelper/IsConversion.h"
#include "TruthHelper/IsGenInteracting.h"
#include "TruthHelper/IsGenNonInteracting.h"
#include "TruthHelper/IsGenSimulStable.h"
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsPhysicalHadron.h"
#include "TruthHelper/IsLastPhysicalHadron.h"
#include "TruthHelper/IsGenType.h"
#include "TruthHelper/IsGenerator.h"
#include "TruthHelper/IsStatus.h"
#include "TruthHelper/NCutter.h"
#include "TruthHelper/GenAll.h"

namespace TruthHelperDict {
  struct tmp {
    std::vector<TruthHelper::GenIMCselector*> m_selectors;
  };
}

#endif
