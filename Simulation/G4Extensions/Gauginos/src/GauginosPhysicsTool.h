/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GAUGINOS_GauginosPhysicsTool_H
#define GAUGINOS_GauginosPhysicsTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class GauginosPhysicsTool GauginosPhysicsTool.h "G4AtlasInfrstructure/GauginosPhysicsTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Edoardo Farina
 *  @date   15-05-2015
 */
class GauginosPhysicsTool :  public G4VPhysicsConstructor, public extends<AthAlgTool, IPhysicsOptionTool>  {
public:
  /// Standard constructor
  GauginosPhysicsTool( const std::string& type , const std::string& name,
                       const IInterface* parent ) ;

  virtual ~GauginosPhysicsTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override final;
  virtual void ConstructParticle() override final;
  virtual void ConstructProcess()  override final;


  /** Implements
   */

  virtual GauginosPhysicsTool* GetPhysicsOption() override final;


protected:

  static GauginosPhysicsTool* m_physList;

  G4double m_GravitinoMass, m_GravitinoWidth, m_GravitinoLifetime, m_GravitinoCharge, m_NeutralinoPDGCode;
  G4double m_NeutralinoMass, m_NeutralinoWidth,  m_NeutralinoLifetime, m_NeutralinoCharge, m_GravitinoPDGCode;

  G4bool m_NeutralinoStable, m_NeutralinoShortlived;
  G4bool m_GravitinoStable, m_GravitinoShortlived;

};



#endif //GAUGINOS_GauginosPhysicsTool_H
