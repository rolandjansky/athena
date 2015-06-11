/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossRecorder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef EnergyLossRecorder_H
#define EnergyLossRecorder_H

#include "FadsActions/UserAction.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <vector>


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

class EnergyLossRecorder: public FADS::UserAction {

  public:
    /** Standard FADS UserAction */
    EnergyLossRecorder(std::string s);

    /** All G4 interface methods */
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void SteppingAction(const G4Step*);

  private:
     ToolHandle<Trk::IPositionMomentumWriter> m_pmWriter;
     unsigned int                             m_entries;
     bool                                     m_retrieved;

    
       
};

#endif

