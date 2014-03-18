/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorFacilityBase_H
#define DetectorFacilityBase_H

#include <string>
#include <vector>
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"

namespace FADS {

  struct Envelope {
    G4LogicalVolume *theEnvelope;
    G4ThreeVector thePosition;
    G4RotationMatrix *theRotation;
    G4VPhysicalVolume *thePositionedVolume;
  };

  class DetectorFacilityBase {
  protected:
    std::string name;
    DetectorFacilityBase(std::string n);
  public:
    virtual ~DetectorFacilityBase() {};
    virtual void SetAsWorld()=0;
    virtual Envelope& Build()=0;
    std::string GetName() {return name;};
    virtual DetectorFacilityBase* construct(std::string /*n*/) {return 0;};

  };

} // end namespace

#endif // DetectorFacilityBase
