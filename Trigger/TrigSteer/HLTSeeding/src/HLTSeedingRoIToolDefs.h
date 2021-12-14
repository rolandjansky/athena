/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTSEEDING_HLTSEEDINGROITOOLDEFS_H
#define HLTSEEDING_HLTSEEDINGROITOOLDEFS_H

#include "xAODTrigger/eFexEMRoIContainer.h"
#include "xAODTrigger/eFexTauRoIContainer.h"
#include "xAODTrigger/jFexTauRoIContainer.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"

template<typename T_RoI, typename T_RoIContainer, const char* N_RoIContainer, const char* N_ThresholdType>
class RoIThresholdsTool;
template<typename T_RoI, typename T_RoIContainer, auto F_RoIWordGetter, const char* N_RoIContainer, const char* N_ThresholdType>
class RoIsUnpackingToolPhase1;


/**
 * @namespace HLTSeedingRoIToolDefs
 * Collection of template parameter values for the RoIThresholdsTool and RoIsUnpackingToolPhase1 class templates
 */
namespace HLTSeedingRoIToolDefs {
  namespace eFexEM {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<xAOD::eFexEMRoI, xAOD::eFexEMRoIContainer, &xAOD::eFexEMRoI::word0, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<xAOD::eFexEMRoI, xAOD::eFexEMRoIContainer, ContainerName, ThresholdType>;
  }
  namespace eFexTau {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<xAOD::eFexTauRoI, xAOD::eFexTauRoIContainer, &xAOD::eFexTauRoI::word0, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<xAOD::eFexTauRoI, xAOD::eFexTauRoIContainer, ContainerName, ThresholdType>;
  }
  namespace jFexTau {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<xAOD::jFexTauRoI, xAOD::jFexTauRoIContainer, &xAOD::jFexTauRoI::tobWord, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<xAOD::jFexTauRoI, xAOD::jFexTauRoIContainer, ContainerName, ThresholdType>;
  }
  namespace cTau {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<xAOD::eFexTauRoI, xAOD::eFexTauRoIContainer, &xAOD::eFexTauRoI::word0, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<xAOD::eFexTauRoI, xAOD::eFexTauRoIContainer, ContainerName, ThresholdType>;
  }
  namespace jFexSRJet {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<xAOD::jFexSRJetRoI, xAOD::jFexSRJetRoIContainer, &xAOD::jFexSRJetRoI::tobWord, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<xAOD::jFexSRJetRoI, xAOD::jFexSRJetRoIContainer, ContainerName, ThresholdType>;
  }
  namespace jFexLRJet {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<xAOD::jFexLRJetRoI, xAOD::jFexLRJetRoIContainer, &xAOD::jFexLRJetRoI::tobWord, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<xAOD::jFexLRJetRoI, xAOD::jFexLRJetRoIContainer, ContainerName, ThresholdType>;
  }
  namespace gFexSRJet {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<xAOD::gFexJetRoI, xAOD::gFexJetRoIContainer, &xAOD::gFexJetRoI::word, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<xAOD::gFexJetRoI, xAOD::gFexJetRoIContainer, ContainerName, ThresholdType>;
  }
  namespace gFexLRJet {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<xAOD::gFexJetRoI, xAOD::gFexJetRoIContainer, &xAOD::gFexJetRoI::word, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<xAOD::gFexJetRoI, xAOD::gFexJetRoIContainer, ContainerName, ThresholdType>;
  }
  namespace Muon {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<xAOD::MuonRoI, xAOD::MuonRoIContainer, &xAOD::MuonRoI::roiWord, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<xAOD::MuonRoI, xAOD::MuonRoIContainer, ContainerName, ThresholdType>;
  }
}

#endif //> !HLTSEEDING_HLTSEEDINGROITOOLDEFS_H
