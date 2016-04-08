/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4StepLimitationTool_H
#define G4StepLimitationTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class G4StepLimitationTool G4StepLimitationTool.h "G4Utilities/G4StepLimitationTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Edoardo Farina
 *  @date   03-08-2015
 */
class G4StepLimitationTool :  public G4VPhysicsConstructor, public AthAlgTool, virtual public IPhysicsOptionTool  {
public:
  /// Standard constructor
  G4StepLimitationTool( const std::string& type , const std::string& name,
                       const IInterface* parent ) ;

  virtual ~G4StepLimitationTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override final;
  virtual void ConstructParticle() override final;
  virtual void ConstructProcess()  override final;


  /** Implements
   */

  virtual G4VPhysicsConstructor* GetPhysicsOption() override final;


protected:



};



#endif //G4StepLimitationTool_H
