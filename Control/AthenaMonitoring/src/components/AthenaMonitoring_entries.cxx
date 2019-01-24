/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/AthenaMon.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"
#include "AthenaMonitoring/DQFilledBunchFilterTool.h"
#include "AthenaMonitoring/DQDummyFilterTool.h"
#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/DQBadLBFilterAlg.h"
#include "AthenaMonitoring/TriggerTranslatorSimple.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/ExampleMonitorAlgorithm.h"


DECLARE_COMPONENT( AthenaMon )
DECLARE_COMPONENT( AthenaMonManager )
DECLARE_COMPONENT( ManagedMonitorToolTest )
DECLARE_COMPONENT( DQAtlasReadyFilterTool )
DECLARE_COMPONENT( DQFilledBunchFilterTool )
DECLARE_COMPONENT( DQDummyFilterTool )
DECLARE_COMPONENT( DQBadLBFilterTool )
DECLARE_COMPONENT( DQBadLBFilterAlg )
DECLARE_COMPONENT( TriggerTranslatorToolSimple )
DECLARE_COMPONENT( GenericMonitoringTool )
DECLARE_COMPONENT( ExampleMonitorAlgorithm )