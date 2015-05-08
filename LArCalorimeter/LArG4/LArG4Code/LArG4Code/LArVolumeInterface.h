/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArVolumeInterface.h
// 09-Apr-2003 Bill Seligman

// This is an interface for handling volumes inside LArG4.  The idea
// is that each detector (or sub-detector) must return a
// logical-volume envelope, and the parent detector places it within a
// physical volume.

#ifndef __LArVolumeInterface_h__
#define __LArVolumeInterface_h__

#include "G4LogicalVolume.hh"

class LArVolumeInterface {

public:

  virtual ~LArVolumeInterface() {};

  // Get a logical-volume envelope around the sub-element of the ATLAS
  // detector.
  virtual G4LogicalVolume* GetEnvelope() = 0;

protected:

  // The constructor is protected to make sure no one creates an
  // explicit LArVolumeInterface object.
  LArVolumeInterface() {};

};

#endif // __LArVolumeInterface_h__
