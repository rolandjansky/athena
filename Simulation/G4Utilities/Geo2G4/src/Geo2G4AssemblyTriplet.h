/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_ASSEMBLYTRIPLET_H
#define GEO2G4_ASSEMBLYTRIPLET_H 

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

class G4LogicalVolume;
class Geo2G4AssemblyVolume;

class Geo2G4AssemblyTriplet
{
 public:  // with description

    Geo2G4AssemblyTriplet();
      // Default constructor

    Geo2G4AssemblyTriplet( G4LogicalVolume* pVolume,
                       G4ThreeVector& translation,
                       G4RotationMatrix* pRotation,
                       G4bool isReflection = false);
      // An explicit constructor for a logical volume
    
    Geo2G4AssemblyTriplet( Geo2G4AssemblyVolume* pAssembly,
                       G4ThreeVector& translation,
                       G4RotationMatrix* pRotation,
                       G4bool isReflection = false);
      // An explicit constructor for an assembly volume
    
    Geo2G4AssemblyTriplet( const Geo2G4AssemblyTriplet& second );
      // Copy constructor

    ~Geo2G4AssemblyTriplet();    
      // Destructor

    Geo2G4AssemblyTriplet& operator=( const Geo2G4AssemblyTriplet& second );
      // Assignment operator

    G4LogicalVolume* GetVolume() const;
      // Retrieve the logical volume reference

    void SetVolume( G4LogicalVolume* pVolume );
      // Update the logical volume reference

    Geo2G4AssemblyVolume* GetAssembly() const;
      // Retrieve the assembly volume reference

    void SetAssembly( Geo2G4AssemblyVolume* pAssembly );
      // Update the assembly volume reference

    G4ThreeVector GetTranslation() const;
      // Retrieve the logical volume translation

    void SetTranslation( G4ThreeVector& pVolume );
      // Update the logical volume translation

    G4RotationMatrix* GetRotation() const;
      // Retrieve the logical volume rotation

    void SetRotation( G4RotationMatrix* pVolume );
      // Update the logical volume rotation

    G4bool IsReflection() const;
      // Return true if the logical or assembly volume has reflection

 private:

    G4LogicalVolume*  fVolume;
      // A logical volume

    G4ThreeVector     fTranslation;
      // A logical volume translation

    G4RotationMatrix* fRotation;
      // A logical volume rotation

 private:

    // Member data for handling assemblies of assemblies and reflections

    Geo2G4AssemblyVolume* fAssembly;
      // An assembly volume

    G4bool            fIsReflection;
      // True if the logical or assembly volume has reflection  
};

inline
Geo2G4AssemblyTriplet::Geo2G4AssemblyTriplet()
  : fVolume( 0 ), fRotation( 0 ), fAssembly(0), fIsReflection(false)
{
  G4ThreeVector v(0.,0.,0.);
  fTranslation = v;
}

inline
Geo2G4AssemblyTriplet::Geo2G4AssemblyTriplet( G4LogicalVolume* pVolume,
                                      G4ThreeVector& translation,
                                      G4RotationMatrix* pRotation,
                                      G4bool isReflection )
  : fVolume( pVolume ), fTranslation( translation ), fRotation( pRotation ),
    fAssembly( 0 ), fIsReflection(isReflection)
{
}

inline
Geo2G4AssemblyTriplet::Geo2G4AssemblyTriplet( Geo2G4AssemblyVolume* pAssembly,
                                      G4ThreeVector& translation,
                                      G4RotationMatrix* pRotation,
                                      G4bool isReflection )
  : fVolume( 0 ), fTranslation( translation ), fRotation( pRotation ),
    fAssembly( pAssembly ), fIsReflection(isReflection) 
{
}

inline
Geo2G4AssemblyTriplet::Geo2G4AssemblyTriplet( const Geo2G4AssemblyTriplet& second )
{
  fVolume       = second.GetVolume();
  fRotation     = second.GetRotation();
  fTranslation  = second.GetTranslation();
  fAssembly     = second.GetAssembly();
  fIsReflection = second.IsReflection();
}

inline
Geo2G4AssemblyTriplet::~Geo2G4AssemblyTriplet()
{
}

inline
G4LogicalVolume* Geo2G4AssemblyTriplet::GetVolume() const
{
  return fVolume;
}

inline
void Geo2G4AssemblyTriplet::SetVolume( G4LogicalVolume* pVolume )
{
  if ( fAssembly ) 
  {
    G4Exception("Geo2G4AssemblyTriplet::SetVolume()",
                "IllegalCall", JustWarning,
                "There is an assembly already set, it will be ignored.");
  }
  fVolume = pVolume;
  fAssembly = 0;
}

inline
Geo2G4AssemblyVolume* Geo2G4AssemblyTriplet::GetAssembly() const
{
  return fAssembly;
}

inline
void Geo2G4AssemblyTriplet::SetAssembly( Geo2G4AssemblyVolume* pAssembly )
{
  if ( fVolume ) 
  {
    G4Exception("Geo2G4AssemblyTriplet::SetAssembly()",
                "IllegalCall", JustWarning,
                "There is a volume already set, it will be ignored.");
  }
  fAssembly = pAssembly;
  fVolume = 0;
}

inline
G4ThreeVector Geo2G4AssemblyTriplet::GetTranslation() const
{
  return fTranslation;
}

inline
void Geo2G4AssemblyTriplet::SetTranslation( G4ThreeVector& translation )
{
  fTranslation = translation;
}

inline
G4RotationMatrix* Geo2G4AssemblyTriplet::GetRotation() const
{
  return fRotation;
}

inline
void Geo2G4AssemblyTriplet::SetRotation( G4RotationMatrix* pRotation )
{
  fRotation = pRotation;
}

inline
G4bool Geo2G4AssemblyTriplet::IsReflection() const
{
  return fIsReflection;
}  

inline
Geo2G4AssemblyTriplet&
Geo2G4AssemblyTriplet::operator=( const Geo2G4AssemblyTriplet& second )
{
  if( this != &second )
  {
    fVolume       = second.GetVolume();
    fRotation     = second.GetRotation();
    fTranslation  = second.GetTranslation();
    fAssembly     = second.GetAssembly();
    fIsReflection = second.IsReflection();
  }
  
  return *this;
}

#endif
