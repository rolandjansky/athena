/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DARKSECTOR_DarkSectorPhysicsTool_H
#define DARKSECTOR_DarkSectorPhysicsTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class DarkSectorPhysicsTool DarkSectorPhysicsTool.h "DarkSector/DarkSectorPhysicsTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Andrea Dell'Acqua
 *  @date   17-02-2017
 */
class DarkSectorPhysicsTool :  public G4VPhysicsConstructor, public AthAlgTool, virtual public IPhysicsOptionTool  {
public:
  /// Standard constructor
  DarkSectorPhysicsTool( const std::string& type , const std::string& name,
                         const IInterface* parent ) ;

  virtual ~DarkSectorPhysicsTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override final;
  virtual void ConstructParticle() override final;
  virtual void ConstructProcess()  override final;


  /** Implements
   */

  virtual DarkSectorPhysicsTool* GetPhysicsOption() override final;


protected:

  static DarkSectorPhysicsTool* m_physList;

  G4double DarkSectorLSPMass;

};



#endif //DARKSECTOR_DarkSectorPhysicsTool_H
