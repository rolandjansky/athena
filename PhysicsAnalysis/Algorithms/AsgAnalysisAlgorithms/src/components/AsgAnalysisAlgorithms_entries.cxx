/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#include <GaudiKernel/DeclareFactoryEntries.h>

#include <AsgAnalysisAlgorithms/AsgFlagSelectionTool.h>
#include <AsgAnalysisAlgorithms/AsgPtEtaSelectionTool.h>
#include <AsgAnalysisAlgorithms/AsgCutBookkeeperAlg.h>
#include <AsgAnalysisAlgorithms/AsgCutBookkeeperAlg.h>
#include <AsgAnalysisAlgorithms/AsgEventScaleFactorAlg.h>
#include <AsgAnalysisAlgorithms/AsgLeptonTrackSelectionAlg.h>
#include <AsgAnalysisAlgorithms/AsgOriginalObjectLinkAlg.h>
#include <AsgAnalysisAlgorithms/AsgSelectionAlg.h>
#include <AsgAnalysisAlgorithms/AsgViewFromSelectionAlg.h>
#include <AsgAnalysisAlgorithms/AsgxAODNTupleMakerAlg.h>
#include <AsgAnalysisAlgorithms/EventFlagSelectionAlg.h>
#include <AsgAnalysisAlgorithms/EventSelectionByObjectFlagAlg.h>
#include <AsgAnalysisAlgorithms/IsolationCloseByCorrectionAlg.h>
#include <AsgAnalysisAlgorithms/KinematicHistAlg.h>
#include <AsgAnalysisAlgorithms/ObjectCutFlowHistAlg.h>
#include <AsgAnalysisAlgorithms/OverlapRemovalAlg.h>
#include <AsgAnalysisAlgorithms/PileupReweightingAlg.h>
#include <AsgAnalysisAlgorithms/PMGTruthWeightAlg.h>
#include <AsgAnalysisAlgorithms/SysListDumperAlg.h>
#include <AsgAnalysisAlgorithms/SysListLoaderAlg.h>
#include <AsgAnalysisAlgorithms/TreeFillerAlg.h>
#include <AsgAnalysisAlgorithms/TreeMakerAlg.h>

DECLARE_NAMESPACE_TOOL_FACTORY (CP, AsgFlagSelectionTool)
DECLARE_NAMESPACE_TOOL_FACTORY (CP, AsgPtEtaSelectionTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgCutBookkeeperAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgCutBookkeeperAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgEventScaleFactorAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgLeptonTrackSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgOriginalObjectLinkAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgViewFromSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgxAODNTupleMakerAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, EventFlagSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, EventSelectionByObjectFlagAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, IsolationCloseByCorrectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, KinematicHistAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, ObjectCutFlowHistAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, OverlapRemovalAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, PileupReweightingAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, PMGTruthWeightAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, SysListDumperAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, SysListLoaderAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, TreeFillerAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, TreeMakerAlg)

DECLARE_FACTORY_ENTRIES(AsgAnalysisAlgorithms) {
  DECLARE_NAMESPACE_ALGTOOL (CP, AsgFlagSelectionTool)
  DECLARE_NAMESPACE_ALGTOOL (CP, AsgPtEtaSelectionTool)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgCutBookkeeperAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgEventScaleFactorAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgLeptonTrackSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgOriginalObjectLinkAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgViewFromSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgxAODNTupleMakerAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, EventFlagSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, EventSelectionByObjectFlagAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, IsolationCloseByCorrectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, KinematicHistAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, ObjectCutFlowHistAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, OverlapRemovalAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, PileupReweightingAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, PMGTruthWeightAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, SysListDumperAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, SysListLoaderAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, TreeFillerAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, TreeMakerAlg)
}
