/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RHADRONS_RHADRONSPHYSICSTOOL_H
#define RHADRONS_RHADRONSPHYSICSTOOL_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class RHadronsPhysicsTool RHadronsPhysicsTool.h "RHadrons/RHadronsPhysicsTool.h"
 *
 *
 *
 *  @author Edoardo Farina
 *  @date  2015-05-14
 */
class RHadronsPhysicsTool :  public G4VPhysicsConstructor, public extends<AthAlgTool, IPhysicsOptionTool>
{
public:
  /// Standard constructor
  RHadronsPhysicsTool( const std::string& type , const std::string& name,
                       const IInterface* parent ) ;

  virtual ~RHadronsPhysicsTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  virtual void ConstructParticle();
  virtual void ConstructProcess();

  /** Implements
   */

  virtual RHadronsPhysicsTool* GetPhysicsOption();

protected:

};



#endif
