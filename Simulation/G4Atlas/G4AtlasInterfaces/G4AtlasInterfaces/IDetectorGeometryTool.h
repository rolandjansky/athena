/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IDETECTORGEOMETRYTOOL_H
#define G4ATLASINTERFACES_IDETECTORGEOMETRYTOOL_H

#include "GaudiKernel/IAlgTool.h"

/** @class IDetectorGeometryTool IDetectorGeometryTool.h "G4AtlasInterfaces/IDetectorGeometryTool.h"
 *  
 *  Abstract interface to service for Geant4 detector geometry classes
 * 
 *  @author ADA
 *  @date   2015-01-18
 */

class IDetectorGeometryTool : virtual public IAlgTool {
 public:
  IDetectorGeometryTool() {}
  virtual ~IDetectorGeometryTool() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IDetectorGeometryTool( "IDetectorGeometryTool" , 1 , 0 ) ;
    return IID_IDetectorGeometryTool ; 
  }

  virtual void Build() = 0;
};

#endif
