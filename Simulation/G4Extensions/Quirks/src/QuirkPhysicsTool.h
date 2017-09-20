/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUIRKS_QuirksPhysicsTool_H
#define QUIRKS_QuirksPhysicsTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class QuirksPhysicsTool QuirksPhysicsTool.h "G4AtlasInfrstructure/QuirksPhysicsTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Edoardo Farina
 *  @date   15-05-2015
 */
class QuirksPhysicsTool :  public G4VPhysicsConstructor, public extends<AthAlgTool, IPhysicsOptionTool>  {
public:
  /// Standard constructor
  QuirksPhysicsTool( const std::string& type , const std::string& name,
                       const IInterface* parent ) ;

  virtual ~QuirksPhysicsTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override final;
  virtual void ConstructParticle() override final;
  virtual void ConstructProcess() override final;

  virtual QuirksPhysicsTool* GetPhysicsOption() override final;


private:
    G4double m_mass;
    G4double m_charge;
    G4int m_pdgid;
    G4double m_stringForce;
    G4double m_firstStringLength;
    G4double m_maxBoost;
    G4double m_maxMergeT;
    G4double m_maxMergeMag;
    G4int m_enableDebug;
    G4double m_debugStep;
    G4int m_numDebugSteps;


};



#endif // QUIRKS_QuirksPhysicsTool_H
