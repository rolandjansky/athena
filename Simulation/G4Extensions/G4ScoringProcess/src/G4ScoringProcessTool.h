/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ScoringProcessTool_H
#define G4ScoringProcessTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class G4ScoringProcessTool G4ScoringProcessTool.h 
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Edoardo Farina
 *  @date   20-10-2015
 */
class G4ScoringProcessTool :  public G4VPhysicsConstructor, public AthAlgTool, virtual public IPhysicsOptionTool  {
public:
  /// Standard constructor
  G4ScoringProcessTool( const std::string& type , const std::string& name,
                       const IInterface* parent ) ;

  virtual ~G4ScoringProcessTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override final;
  virtual void ConstructParticle() override final;
  virtual void ConstructProcess() override final;



  /** Implements
   */


  virtual G4ScoringProcessTool* GetPhysicsOption() override final;


protected:


};



#endif // G4ScoringProcessTool_H
