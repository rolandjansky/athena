/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//simple includes to force the CLASS_DEF to be encountered during compile
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetContainer.h"
#if !defined(SIMULATIONBASE) and !defined(GENERATIONBASE)
#include "xAODJet/JetTrigAuxContainer.h"
#endif // not SIMULATIONBASE or GENERATIONBASE
