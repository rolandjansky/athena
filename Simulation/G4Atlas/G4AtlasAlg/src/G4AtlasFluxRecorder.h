/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasFluxRecorder_H
#define G4AtlasFluxRecorder_H

#include "G4AtlasInterfaces/IFluxRecorder.h"

class G4AtlasFluxRecorder: public IFluxRecorder {

public:
  /// @name Methods related to flux recording
  /// @{
  /// Initialize flux recording
  void InitializeFluxRecording() override final;
  /// Record fluxes from current event
  void RecordFlux(const G4Event*) override final;
  /// Dump flux information to text files
  void WriteFluxInformation() override final;
  /// @}

};

#endif
