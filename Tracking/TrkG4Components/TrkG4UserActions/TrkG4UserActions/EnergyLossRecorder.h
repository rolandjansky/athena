/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossRecorder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef EnergyLossRecorder_H
#define EnergyLossRecorder_H

#include <string>
#include <vector>

#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"

/** @class EnergyLossRecorder

    This class records the Energy Loss and other properties of primary particles
    (e.g. Muons) at the Geant4 step level when passing virtual cylinder surfaces
    (while actually using the full simulation geometry).

    Comparison can be done to files produced with the TrkExEngine based
    TrkExUnitTests/ExtrapolationEngineTest output file.

    @author Wolfgang.Lukas@cern.ch, Andreas.Salzburger@cern.ch
*/

namespace Trk {
  class IPositionMomentumWriter;
}

namespace G4UA{

  class EnergyLossRecorder: public G4UserRunAction, public G4UserEventAction,  public G4UserSteppingAction
  {

  public:

    struct Config
    {
      Trk::IPositionMomentumWriter *pmWriter=nullptr;
    };

    EnergyLossRecorder(const Config& config);
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void UserSteppingAction(const G4Step*) override;
  private:
    Config m_config;
    unsigned int                             m_entries;
  }; // class EnergyLossRecorder


} // namespace G4UA


#endif
