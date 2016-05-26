// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCALOEVENT_VERSIONS_CALOTOWERAUXCONTAINER_V1_H
#define XAODCALOEVENT_VERSIONS_CALOTOWERAUXCONTAINER_V1_H

#include "xAODCore/AuxContainerBase.h"

#include <vector>

namespace xAOD {
  
  class CaloTowerAuxContainer_v1 : public AuxContainerBase
  {
  public:

    /// @brief Default constructor
    CaloTowerAuxContainer_v1();
    /// @brief Destructor
    ~CaloTowerAuxContainer_v1();

  private:

    /// @brief Data payload
    ///
    /// List of tower energies. This list is sorted in the sense that the index of a given float in this container
    /// is uniquely related to the tower location.
    std::vector<float> towerE;
    //    std::vector<short> towerEtaIdx;
    //    std::vector<short> towerPhiIdx;

  };
}

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CaloTowerAuxContainer_v1, xAOD::AuxContainerBase );

/// @class xAOD::CaloTowerAuxContainer_v1
///
/// @brief Payload container for @c xAOD::CaloTowerContainer_v1.

#endif
