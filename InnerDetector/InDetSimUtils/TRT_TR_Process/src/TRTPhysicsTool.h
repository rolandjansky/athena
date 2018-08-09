/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTPhysicsTool_H
#define TRTPhysicsTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"

#include "G4AtlasInterfaces/IPhysicsOptionTool.h"

//to handle
#include "GaudiKernel/ToolHandle.h"

#include "G4VPhysicsConstructor.hh"


/** @class TRTPhysicsTool TRTPhysicsTool.h "TRT:TR_Process/TRTPhysicsTool.h"
 *
 *
 *
 *  @author Edoardo Farina
 *  @date  18-05-2015
 */
class TRTPhysicsTool final: public G4VPhysicsConstructor, public extends<AthAlgTool, IPhysicsOptionTool>
{
public:
  /// Standard constructor
  TRTPhysicsTool( const std::string& type , const std::string& name,
                  const IInterface* parent ) ;

  virtual ~TRTPhysicsTool(){}; ///< Destructor

  /// Initialize method
  virtual StatusCode initialize() override;
  virtual void ConstructParticle() override;
  virtual void ConstructProcess() override;

  /// IPhysicsOptionTool method; simply returns self.
  virtual G4VPhysicsConstructor* GetPhysicsOption() override;

protected:

  std::string m_xmlFile;

};

#endif
