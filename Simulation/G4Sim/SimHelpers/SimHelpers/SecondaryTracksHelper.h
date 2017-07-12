/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SecondaryTracksHelper_H
#define SecondaryTracksHelper_H


#include "G4TrackingManager.hh"
#include <vector>

/// @brief Helper class for retrieving information on track secondaries created
/// in the last step.
///
/// The design and usage of this class is currently in flux. It may be dropped
/// completely or updated along with sim infrastructure updates for
/// multi-threading.
///
class SecondaryTracksHelper {
public:
  /// Default constructor retrieves the tracking manager
  /// from the global G4EventManager.
  SecondaryTracksHelper();
  /// Constructor with tracking manager argument.
  SecondaryTracksHelper(const G4TrackingManager *);
  /// Redundant setter for the tracking manager;
  /// only here now for backwards compatibility.
  void SetTrackingManager(const G4TrackingManager *);
  const G4TrackingManager* GetTrackingManager();
  /// Calculate the number of new secondaries and update the current count.
  int NrOfNewSecondaries();
  inline void ResetNrOfSecondaries() { m_currentNrOfSecondaries=0; }
  inline int CurrentNrOfSecondaries() const {
    return m_theTrackingManager->GimmeSecondaries()->size();
  }

  std::vector<G4Track*> GetSecondaries(int) const;
private:
  const G4TrackingManager* m_theTrackingManager;
  int m_currentNrOfSecondaries;
};

#endif
