/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   METComponent
 *
 * @brief Helper class to build up MET values
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_METCOMPONENT_H
#define TRIGEFMISSINGET_METCOMPONENT_H

#include "xAODTrigMissingET/TrigMissingET.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include <TLorentzVector.h>
#include <ostream>

namespace HLT { namespace MET {
  /**
   * @brief Helper struct to build up MET values before moving them into the EDM
   *
   * The reason for doing this is that the EDM doesn't allow += operators etc
   * which makes interacting with it rather painful and (maybe?) slow.
   */
  class METComponent {
  public:
    METComponent() {}
    /// Initialize from an xAOD::TrigMissingET object
    METComponent(const xAOD::TrigMissingET& met);
    /// Initialize from a *component* of an xAOD::TrigMissingET object
    METComponent(std::size_t idx, const xAOD::TrigMissingET& met);
    /// Momentum components
    /// x momentum
    float mpx{0.};
    /// y momentum
    float mpy{0.};
    /// z momentum
    float mpz{0.};
    /// The actual met
    float met() const;
    /// The magnitude of the missing 3-vector
    float magnitude() const;
    /// The direction
    float phi() const;
    /// The (pseudo) eta
    float eta() const;

    /// Also store the sumE
    float sumE{0.};
    /// And the sumEt
    float sumEt{0.};
    /// The status flag
    int status{0};

    /// Add two of these things together
    friend METComponent operator+(const METComponent& lhs, const METComponent& rhs);
    /// Add one to us
    METComponent& operator+=(const METComponent& other);
    /// Add a four momentum. Use the convention used in xAOD::MissingET
    /// where adding particle *subtracts* its four momentum. So 'adding' a
    /// particle to the MET works as you'd expect.
    METComponent& operator+=(const TLorentzVector& otherP4);
    /// Add a signed object. This uses the same convention as above (so a
    /// negative energy object increases the object
    METComponent& operator+=(const SignedKinematics& kin);

    /// Fill the main component of the MET with this
    void fillMET(xAOD::TrigMissingET& met) const;
    /// Fill a component of the MET with this
    void fillMETComponent(std::size_t idx, xAOD::TrigMissingET& met) const;
  }; //> end class METComponent

  std::ostream& operator<<(std::ostream& os, const METComponent& component);

} } //> end namespace HLT::MET
#endif //> !TRIGEFMISSINGET_METCOMPONENT_H
