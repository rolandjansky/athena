///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// FourMomUtilsDict.h
// Header for dict. 'bootstrapping'
// Author  : Sebastien Binet
///////////////////////////////////////////////////////////////////
#ifndef FOURMOMUTILS_FOURMOMUTILSDICT_H
#define FOURMOMUTILS_FOURMOMUTILSDICT_H

#include "FourMomUtils/xAODP4Helpers.h"
#include "FourMomUtils/xAODHelpers.h"

// AthAnalysisBase/ManaCore doesn't currently include these
#ifndef XAOD_ANALYSIS

#include "FourMomUtils/P4Helpers.h"
#include "FourMomUtils/P4Sorters.h"
#include "FourMomUtils/Thrust.h"
#include "FourMomUtils/FoxWolfram.h"
#include "FourMomUtils/JetBroadening.h"
#include "FourMomUtils/JetMasses.h"

#endif

#endif // FOURMOMUTILS_FOURMOMUTILSDICT_H
