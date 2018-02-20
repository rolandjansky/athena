/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_ADDPHYSICSDECAYTOOL_H
#define G4ATLASTOOLS_ADDPHYSICSDECAYTOOL_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

#include <string>
#include <vector>

/** @class AddPhysicsDecayTool AddPhysicsDecayTool.h "G4AtlasInfrstructure/AddPhysicsDecayTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Andrea Dell'Acqua
 *  @date   2014-10-01
 */
class AddPhysicsDecayTool : public G4VPhysicsConstructor, public extends<AthAlgTool, IPhysicsOptionTool>
{
public:
  /// Standard constructor
  AddPhysicsDecayTool( const std::string& type , const std::string& name,
                       const IInterface* parent );

  virtual ~AddPhysicsDecayTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override;

  virtual void ConstructProcess() override;
  virtual AddPhysicsDecayTool* GetPhysicsOption() override;

  virtual void ConstructParticle() override;

protected:
  std::string m_ParticleName;
  double m_BR;
  std::string m_Daughters;
  std::vector<std::string> m_Daughters_vec;

};

#endif //G4ATLASTOOLS_ADDPHYSICSDECAYTOOL_H
