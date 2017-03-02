/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHARGINOS_CHARGINOSPHYSICSTOOL_H
#define CHARGINOS_CHARGINOSPHYSICSTOOL_H

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


  G4double m_CharginoPlusMass, m_CharginoPlusWidth, m_CharginoPlusCharge, m_CharginoPlusPDGCode, m_CharginoPlusLifetime;
  G4bool m_CharginoPlusShortlived,  m_CharginoPlusStable;

  G4double m_CharginoMinusMass, m_CharginoMinusWidth, m_CharginoMinusCharge, m_CharginoMinusPDGCode, m_CharginoMinusLifetime;
  G4bool m_CharginoMinusShortlived,  m_CharginoMinusStable;

  G4double m_NeutralinoMass, m_NeutralinoWidth, m_NeutralinoCharge, m_NeutralinoPDGCode, m_NeutralinoLifetime;
  G4bool m_NeutralinoShortlived,  m_NeutralinoStable;


  G4ParticleDefinition *m_theCharginoMinus;
  G4ParticleDefinition *m_theCharginoPlus;
  G4ParticleDefinition *m_theNeutralino;
};



#endif // CHARGINOS_CHARGINOSPHYSICSTOOL_H
