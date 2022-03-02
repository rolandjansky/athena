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
#include "xAODTrigger/TrigComposite.h"
#include "AthenaKernel/ClassID_traits.h"
#include <variant>
#include <optional>

template<typename T_RoI, typename T_RoIContainer, const char* N_RoIContainer, const char* N_ThresholdType>
class RoIThresholdsTool;
template<typename T_RoI, typename T_RoIContainer, auto F_RoIWordGetter, const char* N_RoIContainer, const char* N_ThresholdType>
class RoIsUnpackingToolPhase1;


/**
 * @namespace HLTSeedingRoIToolDefs
 * Collection of template parameter values for the RoIThresholdsTool and RoIsUnpackingToolPhase1 class templates
 *
 * Also provides utility functions to access the RoI objects and their basic properties with a single interface
 */
namespace HLTSeedingRoIToolDefs {
  /** @defgroup Types The type definitions
   *  @{
   */
  namespace eFexEM {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using T_RoI = xAOD::eFexEMRoI;
    using T_RoIContainer = xAOD::eFexEMRoIContainer;
    constexpr auto F_RoIWordGetter = &xAOD::eFexEMRoI::word0;
    constexpr auto F_TobEtGetter = &xAOD::eFexEMRoI::etTOB;
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<T_RoI, T_RoIContainer, F_RoIWordGetter, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<T_RoI, T_RoIContainer, ContainerName, ThresholdType>;
  }
  namespace eFexTau {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using T_RoI = xAOD::eFexTauRoI;
    using T_RoIContainer = xAOD::eFexTauRoIContainer;
    constexpr auto F_RoIWordGetter = &xAOD::eFexTauRoI::word0;
    constexpr auto F_TobEtGetter = &xAOD::eFexTauRoI::etTOB;
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<T_RoI, T_RoIContainer, F_RoIWordGetter, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<T_RoI, T_RoIContainer, ContainerName, ThresholdType>;
  }
  namespace jFexTau {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using T_RoI = xAOD::jFexTauRoI;
    using T_RoIContainer = xAOD::jFexTauRoIContainer;
    constexpr auto F_RoIWordGetter = &xAOD::jFexTauRoI::tobWord;
    constexpr auto F_TobEtGetter = &xAOD::jFexTauRoI::tobEt;
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<T_RoI, T_RoIContainer, F_RoIWordGetter, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<T_RoI, T_RoIContainer, ContainerName, ThresholdType>;
  }
  namespace cTau {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using T_RoI = xAOD::eFexTauRoI;
    using T_RoIContainer = xAOD::eFexTauRoIContainer;
    constexpr auto F_RoIWordGetter = &xAOD::eFexTauRoI::word0;
    constexpr auto F_TobEtGetter = &xAOD::eFexTauRoI::etTOB;
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<T_RoI, T_RoIContainer, F_RoIWordGetter, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<T_RoI, T_RoIContainer, ContainerName, ThresholdType>;
  }
  namespace jFexSRJet {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using T_RoI = xAOD::jFexSRJetRoI;
    using T_RoIContainer = xAOD::jFexSRJetRoIContainer;
    constexpr auto F_RoIWordGetter = &xAOD::jFexSRJetRoI::tobWord;
    constexpr auto F_TobEtGetter = &xAOD::jFexSRJetRoI::tobEt;
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<T_RoI, T_RoIContainer, F_RoIWordGetter, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<T_RoI, T_RoIContainer, ContainerName, ThresholdType>;
  }
  namespace jFexLRJet {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using T_RoI = xAOD::jFexLRJetRoI;
    using T_RoIContainer = xAOD::jFexLRJetRoIContainer;
    constexpr auto F_RoIWordGetter = &xAOD::jFexLRJetRoI::tobWord;
    constexpr auto F_TobEtGetter = &xAOD::jFexLRJetRoI::tobEt;
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<T_RoI, T_RoIContainer, F_RoIWordGetter, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<T_RoI, T_RoIContainer, ContainerName, ThresholdType>;
  }
  namespace gFexSRJet {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using T_RoI = xAOD::gFexJetRoI;
    using T_RoIContainer = xAOD::gFexJetRoIContainer;
    constexpr auto F_RoIWordGetter = &xAOD::gFexJetRoI::word;
    constexpr auto F_TobEtGetter = &xAOD::gFexJetRoI::tobEt;
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<T_RoI, T_RoIContainer, F_RoIWordGetter, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<T_RoI, T_RoIContainer, ContainerName, ThresholdType>;
  }
  namespace gFexLRJet {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using T_RoI = xAOD::gFexJetRoI;
    using T_RoIContainer = xAOD::gFexJetRoIContainer;
    constexpr auto F_RoIWordGetter = &xAOD::gFexJetRoI::word;
    constexpr auto F_TobEtGetter = &xAOD::gFexJetRoI::tobEt;
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<T_RoI, T_RoIContainer, F_RoIWordGetter, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<T_RoI, T_RoIContainer, ContainerName, ThresholdType>;
  }
  namespace Muon {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using T_RoI = xAOD::MuonRoI;
    using T_RoIContainer = xAOD::MuonRoIContainer;
    constexpr auto F_RoIWordGetter = &xAOD::MuonRoI::roiWord;
    constexpr auto F_TobEtGetter = &xAOD::MuonRoI::thrValue; // There is no uint pt / et getter for muons
    using UnpackingBaseClass = RoIsUnpackingToolPhase1<T_RoI, T_RoIContainer, F_RoIWordGetter, ContainerName, ThresholdType>;
    using ThresholdBaseClass = RoIThresholdsTool<T_RoI, T_RoIContainer, ContainerName, ThresholdType>;
  }
  /** @} */ // end of Types

  /** @defgroup Utils Utility functions to access the different RoI objects with a single interface
   *  @{
   */

  /// std::variant of const ptr to RoI types. Note identical types are only entered once in the template (e.g. no cTau which is same type as eFexTau)
  using AnyRoIPointer = std::variant<const eFexEM::T_RoI*, const eFexTau::T_RoI*, const jFexTau::T_RoI*, const jFexSRJet::T_RoI*, const jFexLRJet::T_RoI*, const gFexSRJet::T_RoI*, const Muon::T_RoI*>;
  /// std::variant of RoI containers. Has to correspond directly to AnyRoIPointer types.
  using AnyRoIContainer = std::variant<eFexEM::T_RoIContainer, eFexTau::T_RoIContainer, jFexTau::T_RoIContainer, jFexSRJet::T_RoIContainer, jFexLRJet::T_RoIContainer, gFexSRJet::T_RoIContainer, Muon::T_RoIContainer>;

  /**
   * Recursively try each type from AnyRoIPointer variant to retrieve an object from a TrigComposite link
   *
   * See https://stackoverflow.com/a/57643845
   */
  template <std::size_t I = 0>
  inline std::optional<AnyRoIPointer> roiFromLink(const xAOD::TrigComposite& tc, const std::string& linkName) {
    if constexpr (I < std::variant_size_v<AnyRoIPointer>) {
      using T_RoIContainer = std::variant_alternative_t<I,AnyRoIContainer>;
      using T_RoI_constptr = std::variant_alternative_t<I,AnyRoIPointer>;
      using T_RoI = std::remove_const_t<std::remove_pointer_t<T_RoI_constptr>>;
      if (tc.hasObjectLink(linkName,ClassID_traits<T_RoIContainer>::ID())) {
        return AnyRoIPointer{tc.object<T_RoI>(linkName)};
      }
      return roiFromLink<I+1>(tc,linkName);
    }
    return std::nullopt;
  }

  constexpr float roiPhi(const AnyRoIPointer& roi) {
    return std::visit([](auto arg) -> float {return arg->phi();}, roi);
  }

  constexpr float roiEta(const AnyRoIPointer& roi) {
    return std::visit([](auto arg) -> float {return arg->eta();}, roi);
  }

  constexpr unsigned int roiWord(const AnyRoIPointer& roi) {
    return std::visit([](auto arg) -> float {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, const eFexEM::T_RoI*>) {
        return (arg->*eFexEM::F_RoIWordGetter)();
      }
      if constexpr (std::is_same_v<T, const eFexTau::T_RoI*>) {
        return (arg->*eFexTau::F_RoIWordGetter)();
      }
      if constexpr (std::is_same_v<T, const jFexTau::T_RoI*>) {
        return (arg->*jFexTau::F_RoIWordGetter)();
      }
      if constexpr (std::is_same_v<T, const jFexSRJet::T_RoI*>) {
        return (arg->*jFexSRJet::F_RoIWordGetter)();
      }
      if constexpr (std::is_same_v<T, const jFexLRJet::T_RoI*>) {
        return (arg->*jFexLRJet::F_RoIWordGetter)();
      }
      if constexpr (std::is_same_v<T, const gFexSRJet::T_RoI*>) {
        return (arg->*gFexSRJet::F_RoIWordGetter)();
      }
      if constexpr (std::is_same_v<T, const Muon::T_RoI*>) {
        return (arg->*Muon::F_RoIWordGetter)();
      }
      return 0;
    }, roi);
  }

  constexpr unsigned int roiTobEt(const AnyRoIPointer& roi) {
    return std::visit([](auto arg) -> float {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, const eFexEM::T_RoI*>) {
        return (arg->*eFexEM::F_TobEtGetter)();
      }
      if constexpr (std::is_same_v<T, const eFexTau::T_RoI*>) {
        return (arg->*eFexTau::F_TobEtGetter)();
      }
      if constexpr (std::is_same_v<T, const jFexTau::T_RoI*>) {
        return (arg->*jFexTau::F_TobEtGetter)();
      }
      if constexpr (std::is_same_v<T, const jFexSRJet::T_RoI*>) {
        return (arg->*jFexSRJet::F_TobEtGetter)();
      }
      if constexpr (std::is_same_v<T, const jFexLRJet::T_RoI*>) {
        return (arg->*jFexLRJet::F_TobEtGetter)();
      }
      if constexpr (std::is_same_v<T, const gFexSRJet::T_RoI*>) {
        return (arg->*gFexSRJet::F_TobEtGetter)();
      }
      if constexpr (std::is_same_v<T, const Muon::T_RoI*>) {
        return (arg->*Muon::F_TobEtGetter)();
      }
      return 0;
    }, roi);
  }

  /** @} */ // end of Utils
}

#endif //> !HLTSEEDING_HLTSEEDINGROITOOLDEFS_H
