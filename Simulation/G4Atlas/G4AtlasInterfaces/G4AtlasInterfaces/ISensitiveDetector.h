/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_ISENSITIVEDETECTOR_H
#define G4ATLASINTERFACES_ISENSITIVEDETECTOR_H

#include "GaudiKernel/IAlgTool.h"

/** @class ISensitiveDetector ISensitiveDetector.h "G4AtlasInterfaces/ISensitiveDetector.h"
 *
 *  Abstract interface to service for Geant4 sensitive detector classes
 *
 *  @author Zach Marshall
 *  @date   2014-12-20
 */

class G4VSensitiveDetector;

class ISensitiveDetector : virtual public IAlgTool {
 public:
  ISensitiveDetector() {}
  virtual ~ISensitiveDetector() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_ISensitiveDetector( "ISensitiveDetector" , 1 , 0 ) ;
    return IID_ISensitiveDetector;
  }

  /** Method to initialize the SD itself - this is our hook for initialization in G4,
   since in a multi-threaded setup the SDs are local and they might need to be
   initialized in each thread.  Saves us using the AlgTool's initialize() for this. */
  virtual StatusCode initializeSD() = 0;

  /** Method to be called at the end of the *athena* event.  This is where the
   hit collection should be uploaded into StoreGate.  The G4VSensitiveDetector already
   provides an end of G4 event method.  In old-style simulation, there is only one G4
   event per athena event.  In ISF, there could be many. */
  virtual StatusCode Gather() = 0;

  /** Method to be called at the beginning of the *athena* event.  This is where the
   hit collections should be set up, if need be.  G4VSensitiveDetector has an
   Initialize() method for the beginning of the G4 event as well. */
  virtual StatusCode SetupEvent() = 0;

 protected:
  /** Method to make the actual SD itself, which will be owned by the tool.  Must be
   implemented in all concrete base classes. */
  virtual G4VSensitiveDetector* makeSD() = 0;
};

#endif
