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

DECLARE_COMPONENT (CP::AsgFlagSelectionTool)
DECLARE_COMPONENT (CP::AsgPtEtaSelectionTool)
DECLARE_COMPONENT (CP::AsgCutBookkeeperAlg)
DECLARE_COMPONENT (CP::AsgCutBookkeeperAlg)
DECLARE_COMPONENT (CP::AsgEventScaleFactorAlg)
DECLARE_COMPONENT (CP::AsgLeptonTrackSelectionAlg)
DECLARE_COMPONENT (CP::AsgOriginalObjectLinkAlg)
DECLARE_COMPONENT (CP::AsgSelectionAlg)
DECLARE_COMPONENT (CP::AsgViewFromSelectionAlg)
DECLARE_COMPONENT (CP::AsgxAODNTupleMakerAlg)
DECLARE_COMPONENT (CP::EventFlagSelectionAlg)
DECLARE_COMPONENT (CP::EventSelectionByObjectFlagAlg)
DECLARE_COMPONENT (CP::IsolationCloseByCorrectionAlg)
DECLARE_COMPONENT (CP::KinematicHistAlg)
DECLARE_COMPONENT (CP::ObjectCutFlowHistAlg)
DECLARE_COMPONENT (CP::OverlapRemovalAlg)
DECLARE_COMPONENT (CP::PileupReweightingAlg)
DECLARE_COMPONENT (CP::PMGTruthWeightAlg)
DECLARE_COMPONENT (CP::SysListDumperAlg)
DECLARE_COMPONENT (CP::SysListLoaderAlg)
DECLARE_COMPONENT (CP::TreeFillerAlg)
DECLARE_COMPONENT (CP::TreeMakerAlg)
