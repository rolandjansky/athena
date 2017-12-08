/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IDETECTORGEOMETRYSVC_H
#define G4ATLASINTERFACES_IDETECTORGEOMETRYSVC_H

#include "GaudiKernel/IService.h"

#include <vector>
#include <string>

/** @class IDetectorGeometrySvc IDetectorGeometrySvc.h "G4AtlasInterfaces/IDetectorGeometrySvc.h"
 *  
 *  Abstract interface to service for Geant4 detector geometry classes
 * 
 *  @author ADA
 *  @date   2015-01-16
 */
 
class G4VUserDetectorConstruction;

class IDetectorGeometrySvc : virtual public IService {
 public:
  IDetectorGeometrySvc() {}
  virtual ~IDetectorGeometrySvc() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IDetectorGeometrySvc( "IDetectorGeometrySvc" , 1 , 0 ) ;
    return IID_IDetectorGeometrySvc ; 
  }

  /// Setup the magnetic field managers for detector volumes
  virtual StatusCode initializeFields() = 0;
  
  virtual G4VUserDetectorConstruction* GetDetectorConstruction() = 0;
  
  virtual void ActivateParallelWorlds() = 0;

  virtual std::vector<std::string>& GetParallelWorldNames() = 0;

};

#endif
