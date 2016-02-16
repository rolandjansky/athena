/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4AssemblyVolume.h"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4AffineTransform.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ReflectionFactory.hh"

#include <sstream>

unsigned int Geo2G4AssemblyVolume::fsInstanceCounter = 0;

// Default constructor
//
Geo2G4AssemblyVolume::Geo2G4AssemblyVolume()
: fAssemblyID( 0 )
{
  InstanceCountPlus();
  SetAssemblyID( GetInstanceCount() );
  SetImprintsCount( 0 );
}

// Destructor
//
Geo2G4AssemblyVolume::~Geo2G4AssemblyVolume()
{
  unsigned int howmany = fTriplets.size();
  if( howmany != 0 )
  {
    for( unsigned int i = 0; i < howmany; i++ )
    {
      G4RotationMatrix* pRotToClean = fTriplets[i].GetRotation();
      if( pRotToClean != 0 )
      {
        delete pRotToClean;
      }
    }
  }
  fTriplets.clear();
  
  howmany = fPVStore.size();
  if( howmany != 0 )
  {
    for( unsigned int j = 0; j < howmany; j++ )
    {
      G4RotationMatrix* pRotToClean = fPVStore[j]->GetRotation();
      if( pRotToClean != 0 )
      {
        delete pRotToClean;
      }
      delete fPVStore[j];
    }
  }
  fPVStore.clear();
  InstanceCountMinus();
}

// Add and place the given volume according to the specified
// translation and rotation.
//
// The rotation matrix passed in can be 0 = identity or an address even of an
// object on the upper stack frame. During assembly imprint, it creates anyway
// a new matrix and keeps track of it so it can delete it later at destruction
// time.
// This policy has been adopted since user has no control on the way the
// rotations are combined.
//
void Geo2G4AssemblyVolume::AddPlacedVolume( G4LogicalVolume*  pVolume,
                                        G4ThreeVector&    translation,
                                        G4RotationMatrix* pRotation,
                                        int copyNo, G4String userC) 
{
  G4RotationMatrix*  toStore  = new G4RotationMatrix;
  
  if( pRotation != 0 )  { *toStore = *pRotation; }
  
  Geo2G4AssemblyTriplet toAdd( pVolume, translation, toStore );
  fTriplets.push_back( toAdd );

  copyNumbers.push_back( copyNo );
  userComments.push_back( userC );
}

// Add and place the given volume according to the specified transformation
//
void Geo2G4AssemblyVolume::AddPlacedVolume( G4LogicalVolume*  pVolume,
                                        G4Transform3D&    transformation,
                                        int copyNo, G4String userC) 
{
  // Decompose transformation
  G4Scale3D     scale;
  G4Rotate3D    rotation;
  G4Translate3D translation;
  transformation.getDecomposition(scale, rotation, translation);

  G4ThreeVector      v = translation.getTranslation();
  G4RotationMatrix*  r = new G4RotationMatrix;
                    *r = rotation.getRotation();
  
  G4bool isReflection = false;
  if (scale(0,0)*scale(1,1)*scale(2,2) < 0.)  { isReflection = true; }

  Geo2G4AssemblyTriplet toAdd( pVolume, v, r, isReflection );
  fTriplets.push_back( toAdd );

  copyNumbers.push_back( copyNo );
  userComments.push_back( userC );
}

// Add and place the given assembly volume according to the specified
// translation and rotation.
//
void Geo2G4AssemblyVolume::AddPlacedAssembly( Geo2G4AssemblyVolume* pAssembly,
                                          G4ThreeVector&    translation,
                                          G4RotationMatrix* pRotation )
{
  G4RotationMatrix*  toStore  = new G4RotationMatrix;
  
  if( pRotation != 0 )  { *toStore = *pRotation; }
  
  Geo2G4AssemblyTriplet toAdd( pAssembly, translation, toStore );
  fTriplets.push_back( toAdd );
}

// Add and place the given assembly volume according to the specified 
// transformation
//
void Geo2G4AssemblyVolume::AddPlacedAssembly( Geo2G4AssemblyVolume* pAssembly,
                                          G4Transform3D&    transformation )
{
  // Decompose transformation
  //
  G4Scale3D     scale;
  G4Rotate3D    rotation;
  G4Translate3D translation;
  transformation.getDecomposition(scale, rotation, translation);

  G4ThreeVector      v = translation.getTranslation();
  G4RotationMatrix*  r = new G4RotationMatrix;
                    *r = rotation.getRotation();
  
  G4bool isReflection = false;
  if (scale(0,0)*scale(1,1)*scale(2,2) < 0.)  { isReflection = true; }
  
  Geo2G4AssemblyTriplet toAdd( pAssembly, v, r, isReflection );
  fTriplets.push_back( toAdd );
}

// Create an instance of an assembly volume inside of the specified
// mother volume. This works analogically to making stamp imprints.
// This method makes use of the Geant4 affine transformation class.
// The algorithm is defined as follows:
//  
// Having rotation matrix Rm and translation vector Tm to be applied
// inside the mother and rotation matrix Ra and translation vector Ta
// to be applied inside the assembly itself for each of the participating
// volumes the resulting transformation is
//  
// Tfinal = Ta * Tm
//  
// where Ta and Tm are constructed as
//  
//        -1                                     -1
// Ta = Ra  * Ta           and            Tm = Rm  * Tm
//  
// which in words means that we create first the affine transformations
// by inverse rotation matrices and translations for mother and assembly.
// The resulting final transformation to be applied to each of the
// participating volumes is their product.
//
// IMPORTANT NOTE!
// The order of multiplication is reversed when comparing to CLHEP 3D
// transformation matrix(G4Transform3D class).
//  
// The rotation matrix passed in can be 0 = identity or an address even of an
// object on the upper stack frame. During assembly imprint, it creates anyway
// a new matrix and keeps track of it so it can delete it later at destruction
// time.
// This policy has been adopted since user has no control on the way the
// rotations are combined.
// 
// If the assembly volume contains assembly (a'), the function is called
// recursively with composed transformation:
//
// Tanew =  Ta * Ta'
//
void Geo2G4AssemblyVolume::MakeImprint( Geo2G4AssemblyVolume* pAssembly,
                                    G4LogicalVolume*  pMotherLV,
                                    G4Transform3D&    transformation,
                                    G4int copyNumBase,
                                    G4bool ITkScheme,
                                    G4bool surfCheck )
{
  unsigned int  numberOfDaughters;
  
  if( copyNumBase == 0 )
  {
    numberOfDaughters = pMotherLV->GetNoDaughters();
  }
  else
  {
    numberOfDaughters = copyNumBase;
  }
  // We start from the first available index
  //
  numberOfDaughters++;

  ImprintsCountPlus();
  std::vector<Geo2G4AssemblyTriplet> triplets = pAssembly->fTriplets;
  for( unsigned int   i = 0; i < triplets.size(); i++ )
  {
    G4Transform3D Ta( *(triplets[i].GetRotation()),
                      triplets[i].GetTranslation() );
    if ( triplets[i].IsReflection() )  { Ta = Ta * G4ReflectZ3D(); }

    G4Transform3D Tfinal = transformation * Ta;
    if ( triplets[i].GetVolume() )
    {
      // Generate the unique name for the next PV instance
      // The name has format:
      //
      // av_WWW_impr_XXX_YYY_ZZZ
      // where the fields mean:
      // WWW - assembly volume instance number
      // XXX - assembly volume imprint number
      // YYY - the name of a log. volume we want to make a placement of
      // ZZZ - the log. volume index inside the assembly volume
      //


      std::stringstream pvName;
      pvName << "av_"
             << GetAssemblyID()
             << "_impr_"
             << GetImprintsCount()
             << "_"
             << triplets[i].GetVolume()->GetName().c_str()
             << "_pv_"
             << i;
    if (i<userComments.size() && !(userComments[i].empty())) pvName<<"_"<<userComments[i];
	pvName<<std::ends;
      // Generate a new physical volume instance inside a mother
      // (as we allow 3D transformation use G4ReflectionFactory to 
      //  take into account eventual reflection)
      //
      int ccn=numberOfDaughters + i;
      if (i<copyNumbers.size() && copyNumbers[i]) {
         if(ITkScheme) ccn=copyNumbers[i];
         else ccn=copyNumbers[i]+copyNumBase;
      }

      G4PhysicalVolumesPair pvPlaced
        = G4ReflectionFactory::Instance()->Place( Tfinal,
                                                  pvName.str().c_str(),
                                                  triplets[i].GetVolume(),
                                                  pMotherLV,
                                                  false,
                                                  ccn,
                                                  surfCheck );


      // Register the physical volume created by us so we can delete it later
      //
      fPVStore.push_back( pvPlaced.first );
      if ( pvPlaced.second )  { fPVStore.push_back( pvPlaced.second ); }
    }
    else if ( triplets[i].GetAssembly() )
    {
      // Place volumes in this assembly with composed transformation
      //
      if(ITkScheme) triplets[i].GetAssembly()->MakeImprint( triplets[i].GetAssembly(), pMotherLV,
                                                            Tfinal, i*100+copyNumBase, ITkScheme, surfCheck ); 
      else MakeImprint( triplets[i].GetAssembly(), pMotherLV,
                        Tfinal, i*100+copyNumBase, ITkScheme, surfCheck ); 
    }
    else
    {
      G4Exception("Geo2G4AssemblyVolume::MakeImprint(..)",
                  "NotApplicable", FatalException,
                  "Triplet has no volume and no assembly");
    }  
  }  
}    

void Geo2G4AssemblyVolume::MakeImprint( G4LogicalVolume*  pMotherLV,
                                    G4ThreeVector&    translationInMother,
                                    G4RotationMatrix* pRotationInMother,
                                    G4int copyNumBase,
                                    G4bool ITkScheme,
                                    G4bool surfCheck )
{
  // If needed user can specify explicitely the base count from which to start
  // off for the generation of phys. vol. copy numbers.
  // The old behaviour is preserved when copyNumBase == 0, e.g. the generated
  // copy numbers start from the count equal to current number of daughter
  // volumes before an imprint is made

  // Compose transformation
  //
  if( pRotationInMother == 0 )
  {
    // Make it by default an indentity matrix
    //
    pRotationInMother =
      const_cast<G4RotationMatrix*>( &G4RotationMatrix::IDENTITY );
  }

  G4Transform3D transform( *pRotationInMother,
                            translationInMother );
  MakeImprint(this, pMotherLV, transform, copyNumBase, ITkScheme, surfCheck);
}

void Geo2G4AssemblyVolume::MakeImprint( G4LogicalVolume*  pMotherLV,
                                    G4Transform3D&    transformation,
                                    G4int copyNumBase,
                                    G4bool ITkScheme,
                                    G4bool surfCheck )
{
  // If needed user can specify explicitely the base count from which to start
  // off for the generation of phys. vol. copy numbers.
  // The old behaviour is preserved when copyNumBase == 0, e.g. the generated
  // copy numbers start from the count equal to current number of daughter
  // volumes before a imprint is made

  MakeImprint(this, pMotherLV, transformation, copyNumBase, ITkScheme, surfCheck);
}

unsigned int Geo2G4AssemblyVolume::GetInstanceCount() const
{
  return Geo2G4AssemblyVolume::fsInstanceCounter;
}

void         Geo2G4AssemblyVolume::SetInstanceCount( unsigned int value )
{
  Geo2G4AssemblyVolume::fsInstanceCounter = value;
}

void         Geo2G4AssemblyVolume::InstanceCountPlus()
{
  Geo2G4AssemblyVolume::fsInstanceCounter++;
}

void         Geo2G4AssemblyVolume::InstanceCountMinus()
{
  Geo2G4AssemblyVolume::fsInstanceCounter--;
}
