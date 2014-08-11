/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_EGAMMAPHYSVALHISTUTILITIES_H
#define EGAMMAPHYSVALMONITORING_EGAMMAPHYSVALHISTUTILITIES_H

#include "xAODEgamma/Electron.h"

namespace Egamma{

uint8_t RetrieveHitInfo(const xAOD::Electron& el, xAOD::SummaryType info, uint8_t defaultval = -10);

}

#endif
