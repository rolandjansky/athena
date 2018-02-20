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

    G4LogicalVolume*  m_volume;
      // A logical volume

    G4ThreeVector     m_translation;
      // A logical volume translation

    G4RotationMatrix* m_rotation;
      // A logical volume rotation

 private:

    // Member data for handling assemblies of assemblies and reflections

    Geo2G4AssemblyVolume* m_assembly;
      // An assembly volume

    G4bool            m_isReflection;
      // True if the logical or assembly volume has reflection  
};

inline
Geo2G4AssemblyTriplet::Geo2G4AssemblyTriplet()
  : m_volume( 0 ), m_rotation( 0 ), m_assembly(0), m_isReflection(false)
{
  G4ThreeVector v(0.,0.,0.);
  m_translation = v;
}

inline
Geo2G4AssemblyTriplet::Geo2G4AssemblyTriplet( G4LogicalVolume* pVolume,
                                      G4ThreeVector& translation,
                                      G4RotationMatrix* pRotation,
                                      G4bool isReflection )
  : m_volume( pVolume ), m_translation( translation ), m_rotation( pRotation ),
    m_assembly( 0 ), m_isReflection(isReflection)
{
}

inline
Geo2G4AssemblyTriplet::Geo2G4AssemblyTriplet( Geo2G4AssemblyVolume* pAssembly,
                                      G4ThreeVector& translation,
                                      G4RotationMatrix* pRotation,
                                      G4bool isReflection )
  : m_volume( 0 ), m_translation( translation ), m_rotation( pRotation ),
    m_assembly( pAssembly ), m_isReflection(isReflection) 
{
}

inline
Geo2G4AssemblyTriplet::Geo2G4AssemblyTriplet( const Geo2G4AssemblyTriplet& second )
{
  m_volume       = second.GetVolume();
  m_rotation     = second.GetRotation();
  m_translation  = second.GetTranslation();
  m_assembly     = second.GetAssembly();
  m_isReflection = second.IsReflection();
}

inline
Geo2G4AssemblyTriplet::~Geo2G4AssemblyTriplet()
{
}

inline
G4LogicalVolume* Geo2G4AssemblyTriplet::GetVolume() const
{
  return m_volume;
}

inline
void Geo2G4AssemblyTriplet::SetVolume( G4LogicalVolume* pVolume )
{
  if ( m_assembly ) 
  {
    G4Exception("Geo2G4AssemblyTriplet::SetVolume()",
                "IllegalCall", JustWarning,
                "There is an assembly already set, it will be ignored.");
  }
  m_volume = pVolume;
  m_assembly = 0;
}

inline
Geo2G4AssemblyVolume* Geo2G4AssemblyTriplet::GetAssembly() const
{
  return m_assembly;
}

inline
void Geo2G4AssemblyTriplet::SetAssembly( Geo2G4AssemblyVolume* pAssembly )
{
  if ( m_volume ) 
  {
    G4Exception("Geo2G4AssemblyTriplet::SetAssembly()",
                "IllegalCall", JustWarning,
                "There is a volume already set, it will be ignored.");
  }
  m_assembly = pAssembly;
  m_volume = 0;
}

inline
G4ThreeVector Geo2G4AssemblyTriplet::GetTranslation() const
{
  return m_translation;
}

inline
void Geo2G4AssemblyTriplet::SetTranslation( G4ThreeVector& translation )
{
  m_translation = translation;
}

inline
G4RotationMatrix* Geo2G4AssemblyTriplet::GetRotation() const
{
  return m_rotation;
}

inline
void Geo2G4AssemblyTriplet::SetRotation( G4RotationMatrix* pRotation )
{
  m_rotation = pRotation;
}

inline
G4bool Geo2G4AssemblyTriplet::IsReflection() const
{
  return m_isReflection;
}  

inline
Geo2G4AssemblyTriplet&
Geo2G4AssemblyTriplet::operator=( const Geo2G4AssemblyTriplet& second )
{
  if( this != &second )
  {
    m_volume       = second.GetVolume();
    m_rotation     = second.GetRotation();
    m_translation  = second.GetTranslation();
    m_assembly     = second.GetAssembly();
    m_isReflection = second.IsReflection();
  }
  
  return *this;
}

#endif
