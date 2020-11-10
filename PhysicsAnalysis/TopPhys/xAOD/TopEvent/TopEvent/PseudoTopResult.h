/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
#ifndef ANALYSISTOP_TOPEVENT_PSEUDOTOPRESULT_H
#define ANALYSISTOP_TOPEVENT_PSEUDOTOPRESULT_H

/**
 * @author Andrea Knue <aknue@cern.ch>
 **/

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "xAODCore/CLASS_DEF.h"

namespace xAOD {

  /// Interface class
  class PseudoTopResult: public SG::AuxElement {
  public:
    /// Default constructor
    PseudoTopResult() = default;

    void IniVar(bool);

  }; // class PseudoTopResult

} // namespace xAOD

// Define a ClassID for the type.
CLASS_DEF(xAOD::PseudoTopResult, 135846343, 1)

#endif // not ANALYSISTOP_TOPEVENT_PSEUDOTOPRESULT_H
