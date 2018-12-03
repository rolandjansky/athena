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
class SleptonsPhysicsTool :  public G4VPhysicsConstructor, public extends<AthAlgTool, IPhysicsOptionTool>  {
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


  G4double m_G4SElectronLMinusMass, m_G4SElectronLMinusWidth, m_G4SElectronLMinusCharge, m_G4SElectronLMinusPDGCode, m_G4SElectronLMinusLifetime;
  G4bool m_G4SElectronLMinusShortlived,  m_G4SElectronLMinusStable;

  G4double m_G4SElectronLPlusMass, m_G4SElectronLPlusWidth, m_G4SElectronLPlusCharge, m_G4SElectronLPlusPDGCode, m_G4SElectronLPlusLifetime;
  G4bool m_G4SElectronLPlusShortlived,  m_G4SElectronLPlusStable;

  G4double m_G4SMuonLMinusMass, m_G4SMuonLMinusWidth, m_G4SMuonLMinusCharge, m_G4SMuonLMinusPDGCode, m_G4SMuonLMinusLifetime;
  G4bool m_G4SMuonLMinusShortlived,  m_G4SMuonLMinusStable;

  G4double m_G4SMuonLPlusMass, m_G4SMuonLPlusWidth, m_G4SMuonLPlusCharge, m_G4SMuonLPlusPDGCode, m_G4SMuonLPlusLifetime;
  G4bool m_G4SMuonLPlusShortlived,  m_G4SMuonLPlusStable;

  G4double m_G4STau1MinusMass, m_G4STau1MinusWidth, m_G4STau1MinusCharge, m_G4STau1MinusPDGCode, m_G4STau1MinusLifetime;
  G4bool m_G4STau1MinusShortlived,  m_G4STau1MinusStable;

  G4double m_G4STau1PlusMass, m_G4STau1PlusWidth, m_G4STau1PlusCharge, m_G4STau1PlusPDGCode, m_G4STau1PlusLifetime;
  G4bool m_G4STau1PlusShortlived,  m_G4STau1PlusStable;



  G4double m_G4SElectronRMinusMass, m_G4SElectronRMinusWidth, m_G4SElectronRMinusCharge, m_G4SElectronRMinusPDGCode, m_G4SElectronRMinusLifetime;
  G4bool m_G4SElectronRMinusShortlived,  m_G4SElectronRMinusStable;

  G4double m_G4SElectronRPlusMass, m_G4SElectronRPlusWidth, m_G4SElectronRPlusCharge, m_G4SElectronRPlusPDGCode, m_G4SElectronRPlusLifetime;
  G4bool m_G4SElectronRPlusShortlived,  m_G4SElectronRPlusStable;

  G4double m_G4SMuonRMinusMass, m_G4SMuonRMinusWidth, m_G4SMuonRMinusCharge, m_G4SMuonRMinusPDGCode, m_G4SMuonRMinusLifetime;
  G4bool m_G4SMuonRMinusShortlived,  m_G4SMuonRMinusStable;

  G4double m_G4SMuonRPlusMass, m_G4SMuonRPlusWidth, m_G4SMuonRPlusCharge, m_G4SMuonRPlusPDGCode, m_G4SMuonRPlusLifetime;
  G4bool m_G4SMuonRPlusShortlived,  m_G4SMuonRPlusStable;

  G4double m_G4STau2MinusMass, m_G4STau2MinusWidth, m_G4STau2MinusCharge, m_G4STau2MinusPDGCode, m_G4STau2MinusLifetime;
  G4bool m_G4STau2MinusShortlived,  m_G4STau2MinusStable;

  G4double m_G4STau2PlusMass, m_G4STau2PlusWidth, m_G4STau2PlusCharge, m_G4STau2PlusPDGCode, m_G4STau2PlusLifetime;
  G4bool m_G4STau2PlusShortlived,  m_G4STau2PlusStable;


  G4ParticleDefinition *m_theSElectronLMinus;
  G4ParticleDefinition *m_theSElectronLPlus;
  G4ParticleDefinition *m_theSMuonLMinus;
  G4ParticleDefinition *m_theSMuonLPlus;
  G4ParticleDefinition *m_theSTau1Minus;
  G4ParticleDefinition *m_theSTau1Plus;

  G4ParticleDefinition *m_theSElectronRMinus;
  G4ParticleDefinition *m_theSElectronRPlus;
  G4ParticleDefinition *m_theSMuonRMinus;
  G4ParticleDefinition *m_theSMuonRPlus;
  G4ParticleDefinition *m_theSTau2Minus;
  G4ParticleDefinition *m_theSTau2Plus;

};



#endif // SLEPTONS_SleptonsPhysicsTool_H
