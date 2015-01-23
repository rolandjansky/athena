/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_ISENSITIVEDETECTORSVC_H
#define G4ATLASINTERFACES_ISENSITIVEDETECTORSVC_H

#include "GaudiKernel/IService.h"

/** @class ISensitiveDetectorSvc ISensitiveDetectorSvc.h "G4AtlasInterfaces/ISensitiveDetectorSvc.h"
 *  
 *  Abstract interface to service for Geant4 sensitive detector classes
 * 
 *  @author Zach Marshall
 *  @date   2014-12-20
 */

class ISensitiveDetectorSvc : virtual public IService {
 public:
  ISensitiveDetectorSvc() {}
  virtual ~ISensitiveDetectorSvc() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_ISensitiveDetectorSvc( "ISensitiveDetectorSvc" , 1 , 0 ) ;
    return IID_ISensitiveDetectorSvc ;
  }

  /** End of athena event method.  This should get called once per athena event by the
   framework, somewhere.  Eventually it may be done via a Gaudi function.  This ensures
   a simple call when we have multiple G4 events per athena event.  The function calls
   the gather() method for all the SD handles that it owns. */
  virtual void EndOfAthenaEvent() = 0;
};

#endif
