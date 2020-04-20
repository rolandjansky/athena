/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4MyPhysicalVolume_h
#define G4MyPhysicalVolume_h

#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4VPVParameterisation.hh"

class G4MyPhysicalVolume : public G4VPhysicalVolume {

public:

  G4MyPhysicalVolume( G4RotationMatrix *pRot, const G4ThreeVector &tlate, const G4String& pName, G4LogicalVolume* pLogical, G4VPhysicalVolume* pPhysical ) : G4VPhysicalVolume(pRot, tlate, pName, pLogical, pPhysical)
  {
    
  }
  
  ~G4MyPhysicalVolume() {}

  G4int GetCopyNo() const final
  {
    return copyNo;
  }

  void  SetCopyNo(G4int CopyNo) final 
  {
    copyNo = CopyNo;
  }
  void SetVolumeType(EVolume volumetype){ fTypeOfVolume = volumetype ;} 
 
  G4bool IsMany() const final { return true; }

  G4bool IsReplicated() const final { return true; }

  G4bool IsParameterised() const final { return true; }

  G4VPVParameterisation* GetParameterisation() const final { return nullptr; }  

  void GetReplicationData(EAxis& axis, G4int& nReplicas, G4double& width, G4double& offset, G4bool& consuming) const final
  {
    axis = (EAxis)1;
    nReplicas = 0;
    width = 0.0;
    offset = 0.0;
    consuming = true;
  }

  G4bool  IsRegularStructure() const final { return true; }

  G4int  GetRegularStructureId() const final { return 0; }

  EVolume VolumeType()const{ return fTypeOfVolume; }

  G4int copyNo;

private:

  EVolume fTypeOfVolume;

};
#endif
