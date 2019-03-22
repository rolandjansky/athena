/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#include <GaudiKernel/DeclareFactoryEntries.h>

#include <AsgAnalysisAlgorithms/AsgPtEtaSelectionTool.h>
#include <AsgAnalysisAlgorithms/AsgEventScaleFactorAlg.h>
#include <AsgAnalysisAlgorithms/AsgFlagSelectionTool.h>
#include <AsgAnalysisAlgorithms/AsgLeptonTrackSelectionAlg.h>
#include <AsgAnalysisAlgorithms/AsgSelectionAlg.h>
#include <AsgAnalysisAlgorithms/AsgViewFromSelectionAlg.h>
#include <AsgAnalysisAlgorithms/AsgxAODNTupleMakerAlg.h>
#include <AsgAnalysisAlgorithms/EventFlagSelectionAlg.h>
#include <AsgAnalysisAlgorithms/IsolationCloseByCorrectionAlg.h>
#include <AsgAnalysisAlgorithms/KinematicHistAlg.h>
#include <AsgAnalysisAlgorithms/ObjectCutFlowHistAlg.h>
#include <AsgAnalysisAlgorithms/OverlapRemovalAlg.h>
#include <AsgAnalysisAlgorithms/PileupReweightingAlg.h>
#include <AsgAnalysisAlgorithms/SysListLoaderAlg.h>

DECLARE_NAMESPACE_TOOL_FACTORY (CP, AsgPtEtaSelectionTool)
DECLARE_NAMESPACE_TOOL_FACTORY (CP, AsgFlagSelectionTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgEventScaleFactorAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgLeptonTrackSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgViewFromSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, AsgxAODNTupleMakerAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, EventFlagSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, IsolationCloseByCorrectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, KinematicHistAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, ObjectCutFlowHistAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, OverlapRemovalAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, PileupReweightingAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, SysListLoaderAlg)

DECLARE_FACTORY_ENTRIES(AsgAnalysisAlgorithms) {
  DECLARE_NAMESPACE_ALGTOOL (CP, AsgPtEtaSelectionTool)
  DECLARE_NAMESPACE_ALGTOOL (CP, AsgFlagSelectionTool)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgEventScaleFactorAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgLeptonTrackSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgViewFromSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, AsgxAODNTupleMakerAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, EventFlagSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, IsolationCloseByCorrectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, KinematicHistAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, ObjectCutFlowHistAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, OverlapRemovalAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, PileupReweightingAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, SysListLoaderAlg)
}
