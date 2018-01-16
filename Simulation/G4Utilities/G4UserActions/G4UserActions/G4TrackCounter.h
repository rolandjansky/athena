/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS__G4UA_G4TRACKCOUNTER_H
#define G4USERACTIONS__G4UA_G4TRACKCOUNTER_H

// Geant4 includes
#include "G4UserEventAction.hh"
#include "G4UserTrackingAction.hh"


namespace G4UA
{

  /// @class G4TrackCounter
  /// @brief A simple action which counts tracks.
  ///
  /// This action currently implements BeginEvent and PreTracking interfaces.
  ///
  class G4TrackCounter : public G4UserEventAction,
                         public G4UserTrackingAction
  {

    public:

      /// @brief Simple struct for holding the counts
      /// Might want to use larger integral types for this...
      struct Report
      {
        /// Event counter. Might want a larger int for this
        unsigned int nEvents=0;
        /// Total number of tracks
        unsigned int nTotalTracks=0;
        /// Number of primary tracks
        unsigned int nPrimaryTracks=0;
        /// Number of secondary tracks
        unsigned int nSecondaryTracks=0;
        /// Number of tracks with kinetic E > 50 MeV
        unsigned int n50MeVTracks=0;

	void merge(const Report& rep);
      };

      /// @brief Increments event counter.
      /// I feel like there must be a better way to get this info.
      /// Hmm, the G4Run has a numberOfEvent field...
      virtual void BeginOfEventAction(const G4Event* event) override final;

      /// Increments the track counters
      virtual void PreUserTrackingAction(const G4Track* track) override final;

      /// Retrieve my counts
      const Report& getReport() const
      { return m_report; }

    private:

      /// Track counts for this thread.
      Report m_report;

  }; // class G4TrackCounter

} // namespace G4UA

#endif
