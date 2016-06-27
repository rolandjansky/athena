/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaStackingAction_H
#define AthenaStackingAction_H

//#include "FadsActions/ApplicationStackingAction.h"
#include "G4AtlasTools/UserActionBase.h"

/// Current stacking action implementation
class AthenaStackingAction:public UserActionBase {
public:
        AthenaStackingAction(const std::string& type, const std::string& name, const IInterface* parent);
        ~AthenaStackingAction(){}

        virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack) override;

	virtual StatusCode queryInterface(const InterfaceID&, void**) override;


        inline void KillAllNeutrinos(const bool a){ p_killAllNeutrinos=a; }
        inline bool KillAllNeutrinos() const { return p_killAllNeutrinos; }

        inline void KillLowEPhotons(const double a){ p_stackEnergyCut=a; }
        inline double KillLowEPhotons() const { return p_stackEnergyCut; }

private:

	bool p_killAllNeutrinos;
	double p_stackEnergyCut;
};

#endif



#ifndef G4ATLASALG_G4UA_ATHENASTACKINGACTION_H
#define G4ATLASALG_G4UA_ATHENASTACKINGACTION_H

#include "G4AtlasInterfaces/IStackingAction.h"

namespace G4UA
{

  /// @class AthenaStackingAction
  /// @brief Standard ATLAS stacking action functionality.
  /// Mostly taken from the old AthenaStackingAction implementation, but
  /// adapted for the new user action design for multi-threading.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class AthenaStackingAction : public IStackingAction
  {

    public:

      /// Configuration option struct for AthenaStackingAction.
      struct Config
      {
        /// Flag to toggle killing neutrinos at tracking stage
        bool killAllNeutrinos;
        /// Photon energy cut
        double photonEnergyCut;
      };

      /// Constructor with configuration
      AthenaStackingAction(const Config& config);

      /// @brief Classify a new track.
      /// Result can be fUrgent, fWaiting, fPostpone, or fKill.
      virtual G4ClassificationOfNewTrack
      classifyNewTrack(const G4Track* track) override;

      /// @brief Called when starting the next priority queue.
      /// The waiting stack gets moved into the urgent stack.
      virtual void newStage() override;

      /// @brief Invoked by stack manager at new event.
      /// This method is possibly redundant so we could maybe remove it.
      virtual void prepareNewEvent() override;

    private:

      /// @brief Identify track as a neutrino.
      /// It might be useful to move this kind of functionality
      /// into some standalong helper function(s).
      bool isNeutrino(const G4Track*) const;
      /// @brief Identify track as a photon.
      bool isGamma(const G4Track*) const;

      /// My configuration options
      Config m_config;

  }; // class AthenaStackingAction

} // namespace G4UA

#endif
