/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4UA_ATHENASTACKINGACTION_H
#define G4ATLASALG_G4UA_ATHENASTACKINGACTION_H

#include "G4UserStackingAction.hh"

class PrimaryParticleInformation;

namespace G4UA
{

  /// @class AthenaStackingAction
  /// @brief Standard ATLAS stacking action functionality.
  /// Mostly taken from the old AthenaStackingAction implementation, but
  /// adapted for the new user action design for multi-threading.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class AthenaStackingAction : public G4UserStackingAction
  {

    public:

      /// Configuration option struct for AthenaStackingAction.
      struct Config
      {
        /// Flag to toggle killing neutrinos at tracking stage
        bool killAllNeutrinos;
        /// Photon energy cut
        double photonEnergyCut;
        /// Is this an ISF job
        bool isISFJob;
      };

      /// Constructor with configuration
      AthenaStackingAction(const Config& config);

      /// @brief Classify a new track.
      /// Result can be fUrgent, fWaiting, fPostpone, or fKill.
      virtual G4ClassificationOfNewTrack
      ClassifyNewTrack(const G4Track* track) override final;

    private:

      /// @brief Identify track as a neutrino.
      /// It might be useful to move this kind of functionality
      /// into some standalong helper function(s).
      bool isNeutrino(const G4Track*) const;

      /// @brief Identify track as a photon.
      bool isGamma(const G4Track*) const;
      /// @brief obtain the PrimaryParticleInformation from the current G4Track
      PrimaryParticleInformation* getPrimaryParticleInformation(const G4Track *track) const;

      /// My configuration options
      Config m_config;

  }; // class AthenaStackingAction

} // namespace G4UA

#endif
