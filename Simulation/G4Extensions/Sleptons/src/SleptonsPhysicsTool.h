/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SLEPTONS_SleptonsPhysicsTool_H
#define SLEPTONS_SleptonsPhysicsTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class SleptonsPhysicsTool SleptonsPhysicsTool.h "G4AtlasInfrstructure/SleptonsPhysicsTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Edoardo Farina
 *  @date   15-05-2015
 */
class SleptonsPhysicsTool :  public G4VPhysicsConstructor, public AthAlgTool, virtual public IPhysicsOptionTool  {
public:
  /// Standard constructor
  SleptonsPhysicsTool( const std::string& type , const std::string& name,
                       const IInterface* parent ) ;

  virtual ~SleptonsPhysicsTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override final;
  virtual void ConstructParticle() override final;
  virtual void ConstructProcess() override final;



  /** Implements
   */


  virtual SleptonsPhysicsTool* GetPhysicsOption() override final;


protected:


  G4double G4SElectronMinusMass, G4SElectronMinusWidth, G4SElectronMinusCharge, G4SElectronMinusPDGCode, G4SElectronMinusLifetime;
  G4bool G4SElectronMinusShortlived,  G4SElectronMinusStable;

  G4double G4SElectronPlusMass, G4SElectronPlusWidth, G4SElectronPlusCharge, G4SElectronPlusPDGCode, G4SElectronPlusLifetime;
  G4bool G4SElectronPlusShortlived,  G4SElectronPlusStable;

  G4double G4SMuonMinusMass, G4SMuonMinusWidth, G4SMuonMinusCharge, G4SMuonMinusPDGCode, G4SMuonMinusLifetime;
  G4bool G4SMuonMinusShortlived,  G4SMuonMinusStable;

  G4double G4SMuonPlusMass, G4SMuonPlusWidth, G4SMuonPlusCharge, G4SMuonPlusPDGCode, G4SMuonPlusLifetime;
  G4bool G4SMuonPlusShortlived,  G4SMuonPlusStable;

  G4double G4STauMinusMass, G4STauMinusWidth, G4STauMinusCharge, G4STauMinusPDGCode, G4STauMinusLifetime;
  G4bool G4STauMinusShortlived,  G4STauMinusStable;

  G4double G4STauPlusMass, G4STauPlusWidth, G4STauPlusCharge, G4STauPlusPDGCode, G4STauPlusLifetime;
  G4bool G4STauPlusShortlived,  G4STauPlusStable;


  G4ParticleDefinition *theSElectronMinus;
  G4ParticleDefinition *theSElectronPlus;
  G4ParticleDefinition *theSMuonMinus;
  G4ParticleDefinition *theSMuonPlus;
  G4ParticleDefinition *theSTauMinus;
  G4ParticleDefinition *theSTauPlus;

};



#endif // SLEPTONS_SleptonsPhysicsTool_H
