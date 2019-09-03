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

  G4int GetCopyNo() const
  {
    return copyNo;
  }

  void  SetCopyNo(G4int CopyNo)
  {
    copyNo = CopyNo;
  }
  
  G4bool IsMany() const {return true;}
  G4bool IsReplicated() const {return true;}
  G4bool IsParameterised() const {return true;}
  G4VPVParameterisation* GetParameterisation() const {return NULL;}  
  void GetReplicationData(EAxis& axis, G4int& nReplicas, G4double& width, G4double& offset, G4bool& consuming) const 
{
  axis = (EAxis)1;
  nReplicas = 0;
  width = 0.0;
  offset = 0.0;
  consuming = true;
}
  G4bool  IsRegularStructure() const {return true;}
  G4int  GetRegularStructureId() const {return 0;}

  G4int copyNo;

};
#endif
