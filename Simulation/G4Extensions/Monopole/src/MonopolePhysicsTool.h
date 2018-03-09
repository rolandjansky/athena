/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONOPOLE_MonopolePhysicsTool_H
#define MONOPOLE_MonopolePhysicsTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class MonopolePhysicsTool MonopolePhysicsTool.h "Monopole/MonopolePhysicsTool.h"
 *
 *
 *
 *  @author Edoardo Farina
 *  @date  14-05-2015
 */
class MonopolePhysicsTool :  public G4VPhysicsConstructor, public extends<AthAlgTool, IPhysicsOptionTool>  {
public:
  /// Standard constructor
  MonopolePhysicsTool( const std::string& type , const std::string& name,
                       const IInterface* parent ) ;

  virtual ~MonopolePhysicsTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override final;
  virtual void ConstructParticle() override final;
  virtual void ConstructProcess() override final;



  /** Implements
   */

  virtual MonopolePhysicsTool* GetPhysicsOption() override final;


protected:





};
#endif //MONOPOLE_MonopolePhysicsTool_H
