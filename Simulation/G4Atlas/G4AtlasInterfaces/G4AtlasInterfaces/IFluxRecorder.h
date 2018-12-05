/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFluxRecorder_H
#define IFluxRecorder_H

class G4Event;

class IFluxRecorder {
// simple interface to implement flux recording strategies
public:
  virtual ~IFluxRecorder() {}
  /// @name Methods related to flux recording
  /// @{
  /// Initialize flux recording
  virtual void InitializeFluxRecording() = 0;
  /// Record fluxes from current event
  virtual void RecordFlux(const G4Event*) = 0;
  /// Dump flux information to text files
  virtual void WriteFluxInformation() = 0;
  /// @}

};

#endif
