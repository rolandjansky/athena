/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef __ELECTRONEFFICIENCYCORRECTION_CREATEDUMMYEL__
#define __ELECTRONEFFICIENCYCORRECTION_CREATEDUMMYEL__

#include <vector>
#include <xAODEgamma/ElectronContainer.h>
// Infrastructure include(s):
#ifdef ROOTCORE
#include "AsgMessaging/StatusCode.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

StatusCode
getElectrons(const std::vector<std::pair<double, double>>& pt_eta,
             int runNumber,
             xAOD::TStore& store);
#endif
