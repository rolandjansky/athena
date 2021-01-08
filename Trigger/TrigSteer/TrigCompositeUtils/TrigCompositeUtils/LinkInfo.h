/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOMPOSITEUTILS_LINKINFO_H
#define TRIGCOMPOSITEUTILS_LINKINFO_H

#include "xAODTrigger/TrigComposite.h"
#include "AthLinks/ElementLink.h"
#include "AsgMessaging/StatusCode.h"

namespace TrigCompositeUtils {
  /**
   * @brief Additional information returned by the TrigerDecisionTool's feature retrieval, contained within the LinkInfo.
   **/
  enum ActiveState {
    UNSET, //!< Default property of state. Indicates that the creator of the LinkInfo did not supply this information
    ACTIVE, //!< The link was still active for one-or-more of the HLT Chains requested in the TDT
    INACTIVE //!< The link was inactive for all of the HLT Chains requested in the TDT. I.e. the object was rejected by these chains.
  };

  /**
   * @brief Helper to keep a Decision object, ElementLink and ActiveState (with respect to some requested ChainGroup) linked together (for convenience)
   **/
  template<typename T>
  struct LinkInfo {
    LinkInfo()
      : source{0} {}
    LinkInfo(const Decision* s, const ElementLink<T>& l, ActiveState as = ActiveState::UNSET)
      : source{s}, link{l}, state{as} {}

    bool isValid() const {
      return source && link.isValid();
    }
    /**
     * @brief helper conversion to make it usable with CHECK macro expecting StatusCode
     */
    operator StatusCode () {
      return (isValid() ? StatusCode::SUCCESS : StatusCode::FAILURE);
    }

    const Decision* source;
    ElementLink<T> link;
    ActiveState state;
  };
} //> end namespace TrigCompositeUtils

#endif //> !TRIGCOMPOSITEUTILS_LINKINFO_H