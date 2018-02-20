/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IDETECTORGEOMETRYTOOL_H
#define G4ATLASINTERFACES_IDETECTORGEOMETRYTOOL_H

#include "GaudiKernel/IAlgTool.h"

// Geant4
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

struct Envelope 
{
	Envelope():theEnvelope(0),thePositionedVolume(0),theRotation(0) {;}
	bool IsBuilt() {return theEnvelope!=0;}
	G4LogicalVolume* theEnvelope;
	G4VPhysicalVolume* thePositionedVolume;
	G4RotationMatrix* theRotation;
	G4ThreeVector thePosition;
};

/** @class IDetectorGeometryTool IDetectorGeometryTool.h "G4AtlasInterfaces/IDetectorGeometryTool.h"
 *  
 *  Abstract interface to service for Geant4 detector geometry classes
 * 
 *  @author ADA
 *  @date   2015-01-18
 */

class IDetectorGeometryTool : virtual public IAlgTool {
 public:
  virtual ~IDetectorGeometryTool() {}
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IDetectorGeometryTool, 1, 0);

  virtual void Build() = 0;
  
  virtual void BuildGeometry() = 0;
  
  virtual void PositionInParent() = 0;
  
  virtual void BuildSubDetectors() =0;
  
  virtual void SetEnvelope() = 0;
  
  virtual void ResetEnvelope() = 0;
  
  virtual void SetAsWorld() = 0;
  
  virtual std::string GetDetectorName() const = 0;
  
  virtual void SetDetectorName(std::string) = 0;
  
  virtual void SetParent(IDetectorGeometryTool*) = 0;
  
  virtual Envelope& GetEnvelope() = 0;
  
  virtual G4VPhysicalVolume* GetWorldVolume() = 0;
  
};

#endif
