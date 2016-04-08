/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LucidPhysicsTool_H
#define LucidPhysicsTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"


#include "G4AtlasInterfaces/IPhysicsOptionTool.h"

//to handle
#include "GaudiKernel/ToolHandle.h"

#include "G4VPhysicsConstructor.hh"

/** @class LucidPhysicsTool LucidPhysicsTool.h "G4AtlasInfrstructure/LucidPhysicsTool.h"
 *
 *  Tool for the concrete implementation of a Physics Tool class
 *
 *  @author Edoardo Farina
 *  @date   18-05-2015
 */
class LucidPhysicsTool :  public G4VPhysicsConstructor, public AthAlgTool, virtual public IPhysicsOptionTool  {
public:
  /// Standard constructor
  LucidPhysicsTool( const std::string& type , const std::string& name,
                    const IInterface* parent ) ;

  virtual ~LucidPhysicsTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize() override final;
  virtual void ConstructParticle() override final;
  virtual void ConstructProcess() override final;


  /** Implements
   */
  virtual G4VPhysicsConstructor* GetPhysicsOption() override final;


protected:




};



#endif
