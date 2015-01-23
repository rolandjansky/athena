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

class ISensitiveDetector : virtual public IAlgTool {
 public:
  ISensitiveDetector() {}
  virtual ~ISensitiveDetector() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_ISensitiveDetector( "ISensitiveDetector" , 1 , 0 ) ;
    return IID_ISensitiveDetector;
  }

  /** Method to be called at the end of the *athena* event.  This is where the
   hit collection should be uploaded into StoreGate.  The G4VSensitiveDetector already
   provides an end of G4 event method.  In old-style simulation, there is only one G4
   event per athena event.  In ISF, there could be many. */ 
  virtual void Gather() = 0;
};

#endif
