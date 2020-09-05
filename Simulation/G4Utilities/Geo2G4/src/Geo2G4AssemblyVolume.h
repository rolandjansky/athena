/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_ASSEMBLYVOLUME_H
#define GEO2G4_ASSEMBLYVOLUME_H

#include "Geo2G4AssemblyTriplet.h"
#include "G4Transform3D.hh"
#include <vector>

class G4VPhysicalVolume;

class Geo2G4AssemblyVolume
{
public:  // with description

  Geo2G4AssemblyVolume();
  Geo2G4AssemblyVolume( G4LogicalVolume* volume,
                        G4ThreeVector& translation,
                        G4RotationMatrix* rotation);
  ~Geo2G4AssemblyVolume();
  //
  // Constructors & destructor.
  // At destruction all the generated physical volumes and associated
  // rotation matrices of the imprints will be destroyed.
  //
  // The rotation matrix passed as argument can be 0 (identity) or an address
  // even of an object on the upper stack frame. During assembly imprint, a
  // new matrix is created anyway and it is kept track of it so it can be
  // automatically deleted later at the end of the application.
  // This policy is adopted since user has no control on the way the
  // rotations are combined.

  void AddPlacedVolume( G4LogicalVolume* pPlacedVolume,
                        G4ThreeVector& translation,
                        G4RotationMatrix* rotation,
                        int copyNo=0,const G4String& userComment="");
  //
  // Place the given volume 'pPlacedVolume' inside the assembly.
  //
  // The adopted approach:
  //
  // - Place it w.r.t. the assembly coordinate system.
  //   This step is applied to each of the participating volumes.
  //
  // The other possible approaches:
  //
  // - Place w.r.t. the firstly added volume.
  //   When placed the first, the virtual coordinate system becomes
  //   the coordinate system of the first one.
  //   Every next volume being added into the assembly will be placed
  //   w.r.t to the first one.
  //
  // - Place w.r.t the last placed volume.
  //   When placed the first, the virtual coordinate system becomes
  //   the coordinate system of the first one.
  //   Every next volume being added into the assembly will be placed
  //   w.r.t to the previous one.
  //
  // The rotation matrix passed as argument can be 0 (identity) or an address
  // even of an object on the upper stack frame. During assembly imprint, a
  // new matrix is created anyway and it is kept track of it so it can be
  // automatically deleted later at the end of the application.
  // This policy is adopted since user has no control on the way the
  // rotations are combined.

  void AddPlacedVolume( G4LogicalVolume* pPlacedVolume,
                        G4Transform3D&   transformation,
                        int copyNo=0,const G4String& userComment="");
  //
  // The same as previous, but takes complete 3D transformation in space
  // as its argument.

  void AddPlacedAssembly( Geo2G4AssemblyVolume* pAssembly,
                          G4Transform3D&    transformation);
  //
  // The same as previous AddPlacedVolume(), but takes an assembly volume
  // as its argument.

  void AddPlacedAssembly( Geo2G4AssemblyVolume* pAssembly,
                          G4ThreeVector& translation,
                          G4RotationMatrix* rotation);
  //
  // The same as above AddPlacedVolume(), but takes an assembly volume
  // as its argument with translation and rotation.

  void MakeImprint( G4LogicalVolume* pMotherLV,
                    G4ThreeVector& translationInMother,
                    G4RotationMatrix* pRotationInMother,
                    G4int copyNumBase = 0,
                    G4bool ITkScheme = false,
                    G4bool surfCheck = false );
  //
  // Creates instance of an assembly volume inside the given mother volume.

  void MakeImprint( G4LogicalVolume* pMotherLV,
                    G4Transform3D&   transformation,
                    G4int copyNumBase = 0,
                    G4bool ITkScheme = false,
                    G4bool surfCheck = false );
  //
  // The same as previous Imprint() method, but takes complete 3D
  // transformation in space as its argument.

  inline std::vector<G4VPhysicalVolume*>::iterator GetVolumesIterator();
  inline unsigned int TotalImprintedVolumes() const;
  //
  // Methods to access the physical volumes imprinted with the assembly.

  unsigned int GetImprintsCount() const;
  //
  // Return the number of made imprints.

  unsigned int GetInstanceCount() const;
  //
  // Return the number of existing instance of Geo2G4AssemblyVolume class.

  unsigned int GetAssemblyID()    const;
  //
  // Return instance number of this concrete object.

protected:

  void SetInstanceCount( unsigned int value );
  void SetAssemblyID( unsigned int value );

  void InstanceCountPlus();
  void InstanceCountMinus();

  void SetImprintsCount( unsigned int value );
  void ImprintsCountPlus();
  void ImprintsCountMinus();
  //
  // Internal counting mechanism, used to compute unique the names of
  // physical volumes created by MakeImprint() methods.

private:

  void MakeImprint( Geo2G4AssemblyVolume* pAssembly,
                    G4LogicalVolume*  pMotherLV,
                    G4Transform3D&    transformation,
                    G4int copyNumBase = 0,
                    G4bool ITkScheme = false,
                    G4bool surfCheck = false );
  //
  // Function for placement of the given assembly in the given mother
  // (called recursively if the assembly contains an assembly).

private:

  std::vector<Geo2G4AssemblyTriplet> m_triplets;
  std::vector<int> m_copyNumbers;
  std::vector<G4String> m_userComments;
  //
  // Participating volumes represented as a vector of
  // <logical volume, translation, rotation>.

  std::vector<G4VPhysicalVolume*> m_PVStore;
  //
  // We need to keep list of physical volumes created by MakeImprint() method
  // in order to be able to cleanup the objects when not needed anymore.
  // This requires the user to keep assembly objects in memory during the
  // whole job or during the life-time of G4Navigator, logical volume store
  // and physical volume store keep pointers to physical volumes generated by
  // the assembly volume.
  // When an assembly object is about to die it will destroy all its
  // generated physical volumes and rotation matrices as well !

  unsigned int m_imprintsCounter;
  //
  // Number of imprints of the given assembly volume.

  static unsigned int s_instanceCounter;
  //
  // Class instance counter.

  unsigned int m_assemblyID;
  //
  // Assembly object ID derived from instance counter at construction time.

};

inline
unsigned int Geo2G4AssemblyVolume::GetImprintsCount() const
{
  return m_imprintsCounter;
}

inline
void         Geo2G4AssemblyVolume::SetImprintsCount( unsigned int value )
{
  m_imprintsCounter = value;
}


inline
void         Geo2G4AssemblyVolume::ImprintsCountPlus()
{
  m_imprintsCounter++;
}

inline
void         Geo2G4AssemblyVolume::ImprintsCountMinus()
{
  m_imprintsCounter--;
}

inline
unsigned int Geo2G4AssemblyVolume::GetAssemblyID() const
{
  return m_assemblyID;
}

inline
void         Geo2G4AssemblyVolume::SetAssemblyID( unsigned int value )
{
  m_assemblyID = value;
}

inline
std::vector<G4VPhysicalVolume*>::iterator
Geo2G4AssemblyVolume::GetVolumesIterator()
{
  std::vector<G4VPhysicalVolume*>::iterator iterator = m_PVStore.begin();
  return iterator;
}

inline
unsigned int Geo2G4AssemblyVolume::TotalImprintedVolumes() const
{
  return m_PVStore.size();
}

#endif
