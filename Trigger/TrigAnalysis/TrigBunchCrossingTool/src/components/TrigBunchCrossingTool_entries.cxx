/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBunchCrossingTool/StaticBunchCrossingTool.h"
#include "TrigBunchCrossingTool/xAODBunchCrossingTool.h"
#include "TrigBunchCrossingTool/TrigConfBunchCrossingTool.h"

#include "../MCBunchCrossingTool.h"
#include "../LHCBunchCrossingTool.h"

DECLARE_COMPONENT( Trig::StaticBunchCrossingTool )
DECLARE_COMPONENT( Trig::xAODBunchCrossingTool )

DECLARE_COMPONENT( Trig::TrigConfBunchCrossingTool )
DECLARE_COMPONENT( Trig::MCBunchCrossingTool )
DECLARE_COMPONENT( Trig::LHCBunchCrossingTool )
