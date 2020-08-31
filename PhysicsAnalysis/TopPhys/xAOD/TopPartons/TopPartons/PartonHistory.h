/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

// $Id: PartonHistory.h 782353 2016-11-04 12:11:17Z scalvet $
#ifndef ANALYSISTOP_TOPPARTONS_PARTONHISORTY_H
#define ANALYSISTOP_TOPPARTONS_PARTONHISORTY_H

/**
 * @author John Morris <john.morris@cern.ch>
 * @author Silvestre Marino Romano <sromanos@cern.ch>
 * @author Samuel Calvet <scalvet@cern.ch>
 *
 * @brief PartonHistory
 *   A simple xAOD class which we can persist into a mini-xAOD
 *   The xAOD EDM is way too complex, so let's simplify it
 *   It's not like ROOT can do schema evolution......
 *
 *   In order for this object to be used by any
 *   PartonHistory factory, it contains no AuxElements
 *   Everything should be done via decoration
 *
 *   This really should be an AuxInfoBase type of xAOD object
 *   But John can't figure out how to copy these to the
 *   output mini-xAOD.....yet
 *
 * $Revision: 782353 $
 * $Date: 2016-11-04 12:11:17 +0000 (Fri, 04 Nov 2016) $
 *
 **/

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/CLASS_DEF.h"
#include <vector>

namespace xAOD {
  /// Aux Container
  class PartonHistoryAuxContainer: public AuxContainerBase {
  public:
    /// Default constructor
    PartonHistoryAuxContainer();
  }; // end Aux container

  /// Interface class
  class PartonHistory: public SG::AuxElement {
  public:
    /// Default constructor
    PartonHistory();
    /// Default desturctor
    virtual ~PartonHistory() {}

    void IniVarTtbar();
    void IniVarTzq();
    void IniVarTtbarLight();
    void IniVarTbbar();
    void IniVarWlv();
    void IniVarWtb();
    void IniVarZ();
    void IniVarTtGamma();
    void IniVarThqtautau();
  };
  typedef DataVector < xAOD::PartonHistory > PartonHistoryContainer;
}

// Dictonaries
CLASS_DEF(xAOD::PartonHistory, 135846343, 1)
CLASS_DEF(xAOD::PartonHistoryContainer, 1219079565, 1)
CLASS_DEF(xAOD::PartonHistoryAuxContainer, 1244378748, 1)

#endif
