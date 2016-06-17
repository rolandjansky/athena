/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TightMuonElseNoFieldManager_H
#define TightMuonElseNoFieldManager_H

/** @class TightMuonElseNoFieldManager TightMuonElseNoFieldManager.h
 *
 *  @brief G4FieldManager that sets tight stepping for muons; disables
 *   magnetic field for other particles
 *
 *  @author Zach Marshall
 *  @date   2016-06-01
 */

#include "G4FieldManager.hh"
class G4Track;
class G4Field;
class G4ChordFinder;
class G4MagneticField;

class TightMuonElseNoFieldManager : public G4FieldManager
{
 public:
  /// Constructor
  TightMuonElseNoFieldManager(G4Field *detectorField=0, G4ChordFinder *pChordFinder=0, G4bool b=true);

  /// Simple constructor
  TightMuonElseNoFieldManager(G4MagneticField *detectorMagneticField);

  /// Simple destructor
  virtual ~TightMuonElseNoFieldManager() {}

  /// The one interesting method
  virtual void ConfigureForTrack(const G4Track *) override final;

 private:
  /** Parameters of the stepper.  This is to cache the global ones; the muon ones will be hard-coded */
  G4Field * m_globalField;

};

#endif