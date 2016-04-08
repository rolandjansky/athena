/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHARGINOS_CharginosPhysicsTool_H
#define CHARGINOS_CharginosPhysicsTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class CharginosPhysicsTool CharginosPhysicsTool.h "G4AtlasInfrstructure/CharginosPhysicsTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Edoardo Farina
 *  @date   15-05-2015
 */
class CharginosPhysicsTool :  public G4VPhysicsConstructor, public AthAlgTool, virtual public IPhysicsOptionTool  {
public:
  /// Standard constructor
  CharginosPhysicsTool( const std::string& type , const std::string& name,
                        const IInterface* parent ) ;

  virtual ~CharginosPhysicsTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override final;
  virtual void ConstructParticle() override final;
  virtual void ConstructProcess() override final;


  /** Implements
   */

  virtual CharginosPhysicsTool* GetPhysicsOption() override final;


protected:


  G4double CharginoPlusMass, CharginoPlusWidth, CharginoPlusCharge, CharginoPlusPDGCode, CharginoPlusLifetime;
  G4bool CharginoPlusShortlived,  CharginoPlusStable;

  G4double CharginoMinusMass, CharginoMinusWidth, CharginoMinusCharge, CharginoMinusPDGCode, CharginoMinusLifetime;
  G4bool CharginoMinusShortlived,  CharginoMinusStable;

  G4double NeutralinoMass, NeutralinoWidth, NeutralinoCharge, NeutralinoPDGCode, NeutralinoLifetime;
  G4bool NeutralinoShortlived,  NeutralinoStable;


  G4ParticleDefinition *theCharginoMinus;
  G4ParticleDefinition *theCharginoPlus;
  G4ParticleDefinition *theNeutralino;
};



#endif
