/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "../ExtrapolateMuonToIPTool.h"
#include "../MuonAlignmentUncertTool.h"
#include "../MuonCaloTagTool.h"
#include "../MuonCandidateTool.h"
#include "../MuonCombinedDebuggerTool.h"
#include "../MuonCombinedFitTagTool.h"
#include "../MuonCombinedStacoTagTool.h"
#include "../MuonCombinedTool.h"
#include "../MuonCreatorTool.h"
#include "../MuonDressingTool.h"
#include "../MuonSegmentTagTool.h"
#include "../TrackSegmentAssociationTool.h"

using namespace MuonCombined;
using namespace Muon;
DECLARE_COMPONENT(MuonCandidateTool)
DECLARE_COMPONENT(MuonAlignmentUncertTool)
DECLARE_COMPONENT(MuonCombinedTool)
DECLARE_COMPONENT(MuonCombinedDebuggerTool)
DECLARE_COMPONENT(MuonCreatorTool)
DECLARE_COMPONENT(MuonDressingTool)
DECLARE_COMPONENT(MuonCombinedStacoTagTool)
DECLARE_COMPONENT(MuonCombinedFitTagTool)
DECLARE_COMPONENT(MuonSegmentTagTool)
DECLARE_COMPONENT(MuonCaloTagTool)
DECLARE_COMPONENT(TrackSegmentAssociationTool)
DECLARE_COMPONENT(ExtrapolateMuonToIPTool)
