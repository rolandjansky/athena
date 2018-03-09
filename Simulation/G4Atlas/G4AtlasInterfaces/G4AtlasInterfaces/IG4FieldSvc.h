/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasInterfaces_IG4FieldSvc_H
#define G4AtlasInterfaces_IG4FieldSvc_H

// Include files
// from Gaudi
#include "GaudiKernel/IService.h"

class G4MagneticField;

/** @class IG4FieldSvc IG4FieldSvc.h "G4AtlasInterfaces/IG4FieldSvc.h"
 *
 *  Abstract interface to a field service.
 *
 *  @author ADA
 *  @date   2015-11-17
 */

class IG4FieldSvc : virtual public IService
{
 public:

  IG4FieldSvc() {;}
  virtual ~IG4FieldSvc() {;}

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IG4FieldSvc, 1, 0);

  virtual G4MagneticField* getField( ) = 0;

};

#endif
