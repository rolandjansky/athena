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


  G4double m_G4SElectronMinusMass, m_G4SElectronMinusWidth, m_G4SElectronMinusCharge, m_G4SElectronMinusPDGCode, m_G4SElectronMinusLifetime;
  G4bool m_G4SElectronMinusShortlived,  m_G4SElectronMinusStable;

  G4double m_G4SElectronPlusMass, m_G4SElectronPlusWidth, m_G4SElectronPlusCharge, m_G4SElectronPlusPDGCode, m_G4SElectronPlusLifetime;
  G4bool m_G4SElectronPlusShortlived,  m_G4SElectronPlusStable;

  G4double m_G4SMuonMinusMass, m_G4SMuonMinusWidth, m_G4SMuonMinusCharge, m_G4SMuonMinusPDGCode, m_G4SMuonMinusLifetime;
  G4bool m_G4SMuonMinusShortlived,  m_G4SMuonMinusStable;

  G4double m_G4SMuonPlusMass, m_G4SMuonPlusWidth, m_G4SMuonPlusCharge, m_G4SMuonPlusPDGCode, m_G4SMuonPlusLifetime;
  G4bool m_G4SMuonPlusShortlived,  m_G4SMuonPlusStable;

  G4double m_G4STauMinusMass, m_G4STauMinusWidth, m_G4STauMinusCharge, m_G4STauMinusPDGCode, m_G4STauMinusLifetime;
  G4bool m_G4STauMinusShortlived,  m_G4STauMinusStable;

  G4double m_G4STauPlusMass, m_G4STauPlusWidth, m_G4STauPlusCharge, m_G4STauPlusPDGCode, m_G4STauPlusLifetime;
  G4bool m_G4STauPlusShortlived,  m_G4STauPlusStable;


  G4ParticleDefinition *m_theSElectronMinus;
  G4ParticleDefinition *m_theSElectronPlus;
  G4ParticleDefinition *m_theSMuonMinus;
  G4ParticleDefinition *m_theSMuonPlus;
  G4ParticleDefinition *m_theSTauMinus;
  G4ParticleDefinition *m_theSTauPlus;

};



#endif // SLEPTONS_SleptonsPhysicsTool_H
