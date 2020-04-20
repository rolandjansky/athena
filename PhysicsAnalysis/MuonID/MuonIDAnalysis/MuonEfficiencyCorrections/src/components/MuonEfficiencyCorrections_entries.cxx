/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
// Local include(s):
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
// MCP tester macros
#include "../MuonEfficiencyCorrections_TestAlg.h"
#include "../TestTrigSF.h"
#include "../MuonTriggerSF_TestAlg.h"
#include "../MuonCloseJetDecorationAlg.h"

DECLARE_COMPONENT( CP::MuonEfficiencyScaleFactors )

DECLARE_COMPONENT( CP::MuonTriggerScaleFactors )

DECLARE_COMPONENT( CP::MuonEfficiencyCorrections_TestAlg )
DECLARE_COMPONENT( CP::MuonCloseJetDecorationAlg )

DECLARE_COMPONENT( Trig::TestTrigSF )

DECLARE_COMPONENT( Trig::MuonTriggerSF_TestAlg )
