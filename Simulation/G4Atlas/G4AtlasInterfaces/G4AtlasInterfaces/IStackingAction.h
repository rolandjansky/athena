/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_ISTACKINGACTION_H
#define G4ATLASINTERFACES__G4UA_ISTACKINGACTION_H

// Geant4 includes
#include "G4ClassificationOfNewTrack.hh"

// Forward declarations
class G4Track;

namespace G4UA
{

  /// @class IStackingAction
  /// @brief Simple interface class for an ATLAS stacking action.
  /// Controls the stacks (i.e. priority queues) of G4Track objects.
  /// Follows the methodology of the G4UserStackingAction:
  /// http://www-geant4.kek.jp/lxr/source/event/include/G4UserStackingAction.hh
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  /// 
  class IStackingAction
  {
    public:
      /// Virtual destructor
      virtual ~IStackingAction(){};

      /// @brief Assigns a classification to a new track.
      /// Result can be fUrgent, fWaiting, fPostpone, or fKill.
      virtual G4ClassificationOfNewTrack classifyNewTrack(const G4Track* track) = 0;

      /// @brief Called when starting the next priority queue.
      /// The waiting stack gets moved into the urgent stack.
      virtual void newStage() = 0;

      /// @brief Invoked by stack manager at new event.
      /// This method is possibly redundant so we could maybe remove it.
      virtual void prepareNewEvent() = 0;
  };

} // namespace G4UA

#endif
