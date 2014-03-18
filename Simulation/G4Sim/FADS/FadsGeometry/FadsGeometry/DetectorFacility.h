/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorFacility_H
#define DetectorFacility_H

#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"

#include "FadsMaterial/MaterialManager.h"
#include "FadsGeometry/DetectorFacilityBase.h"

#include <string>
#include <vector>

namespace FADS {

  class DetectorFacility;
  typedef std::vector<DetectorFacility *> detectorVector;
  typedef std::vector<DetectorFacility *>::const_iterator detectorIterator;

  class DetectorFacility: public DetectorFacilityBase {
  protected:
    Envelope envelope;
    detectorVector detectors;
    DetectorFacility* theParent;
    virtual void BuildGeometry()=0;
    virtual void BuildDetectors();
    void PositionInParent();
    bool isWorld;
  public:
    DetectorFacility(std::string n);
    void SetAsWorld();
    void SetAsParallelWorld(std::string worldName="defaultParallelWorld");
    virtual Envelope& Build();
    inline Envelope& GetEnvelope();
    inline G4Material* GetMaterial(std::string matname);
    inline bool IsBuilt();
    inline void SetParent(DetectorFacility *d);
    inline void AddDetector(DetectorFacility *d);
    inline bool IsWorld();
    void MoveTo(G4ThreeVector& newPos);
    void RotateX(double angle);
    void RotateY(double angle);
    void RotateZ(double angle);
    void SetInitialTransformation(G4Transform3D&);
    inline void SetDetectorEnvelope(G4LogicalVolume *v);

    virtual void UseDescription(std::string) {}
  };

  void DetectorFacility::SetDetectorEnvelope(G4LogicalVolume *v)
  {
    envelope.theEnvelope=v;
  }

  bool DetectorFacility::IsWorld()
  {
    return isWorld;
  }

  bool DetectorFacility::IsBuilt()
  {
    return (envelope.theEnvelope!=0);
  }
  void DetectorFacility::AddDetector(DetectorFacility *d)
  {
    d->SetParent(this);
    detectors.push_back(d);
    if (IsBuilt()) d->Build();
  }
  void DetectorFacility::SetParent(DetectorFacility *d)
  {
    theParent=d;
  }

  Envelope& DetectorFacility::GetEnvelope()
  {
    return envelope;
  }

  G4Material* DetectorFacility::GetMaterial(std::string matname)
  {
    MaterialManager* mm=MaterialManager::GetMaterialManager();
    return mm->GetMaterial(matname);
  }

} // end namespace

#endif  // DetectorFacility_H
