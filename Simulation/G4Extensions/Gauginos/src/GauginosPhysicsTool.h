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

  G4double GravitinoMass, GravitinoWidth, GravitinoLifetime, GravitinoCharge, NeutralinoPDGCode;
  G4double NeutralinoMass, NeutralinoWidth,  NeutralinoLifetime, NeutralinoCharge, GravitinoPDGCode;

  G4bool NeutralinoStable, NeutralinoShortlived;
  G4bool GravitinoStable, GravitinoShortlived;

};



#endif //GAUGINOS_GauginosPhysicsTool_H
