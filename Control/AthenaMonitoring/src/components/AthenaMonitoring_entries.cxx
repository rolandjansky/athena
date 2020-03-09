/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/AthenaMon.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"
#include "AthenaMonitoring/DQFilledBunchFilterTool.h"
#include "AthenaMonitoring/DQEventFlagFilterTool.h"
#include "AthenaMonitoring/DQDummyFilterTool.h"
#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/DQBadLBFilterAlg.h"
#include "AthenaMonitoring/TriggerTranslatorSimple.h"
#include "AthenaMonitoring/ExampleMonitorAlgorithm.h"
#include "../ForceIDConditionsAlg.h"
#include "../ForceMSConditionsAlg.h"


DECLARE_COMPONENT( AthenaMon )
DECLARE_COMPONENT( AthenaMonManager )
DECLARE_COMPONENT( ManagedMonitorToolTest )
DECLARE_COMPONENT( DQAtlasReadyFilterTool )
DECLARE_COMPONENT( DQFilledBunchFilterTool )
DECLARE_COMPONENT( DQEventFlagFilterTool )
DECLARE_COMPONENT( DQDummyFilterTool )
DECLARE_COMPONENT( DQBadLBFilterTool )
DECLARE_COMPONENT( DQBadLBFilterAlg )
DECLARE_COMPONENT( TriggerTranslatorToolSimple )
DECLARE_COMPONENT( ExampleMonitorAlgorithm )
DECLARE_COMPONENT( ForceIDConditionsAlg )
DECLARE_COMPONENT( ForceMSConditionsAlg )
