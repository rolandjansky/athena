/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_ISENSITIVEDETECTORMASTERTOOL_H
#define G4ATLASINTERFACES_ISENSITIVEDETECTORMASTERTOOL_H

#include "GaudiKernel/IAlgTool.h"

/** @class ISensitiveDetectorMasterTool ISensitiveDetectorMasterTool.h "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
 *
 *  Abstract interface to service for Geant4 sensitive detector classes
 *
 *  @author Zach Marshall
 *  @date   2014-12-20
 */

class ISensitiveDetectorMasterTool : virtual public IAlgTool {
 public:
  ISensitiveDetectorMasterTool() {}
  virtual ~ISensitiveDetectorMasterTool() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_ISensitiveDetectorMasterTool( "ISensitiveDetectorMasterTool" , 1 , 0 ) ;
    return IID_ISensitiveDetectorMasterTool ;
  }

  /** Method for initializing the SDs of each tool.  Not done in the athena-level
   initialize() function, as this may need to happen once per thread in a multi-threaded
   environment. */
  virtual StatusCode initializeSDs() = 0;

  /** Begin of athena event method.  This should get called once per athena event by the
   framework, somewhere.  Eventually it may be done via a Gaudi function.  This ensures
   a simple call when we have multiple G4 events per athena event.  The function calls
   the SetupEvent() method for all the SD handles that it owns. */
  virtual StatusCode BeginOfAthenaEvent() = 0;

  /** End of athena event method.  This should get called once per athena event by the
   framework, somewhere.  Eventually it may be done via a Gaudi function.  This ensures
   a simple call when we have multiple G4 events per athena event.  The function calls
   the Gather() method for all the SD handles that it owns. */
  virtual StatusCode EndOfAthenaEvent() = 0;
};

#endif
