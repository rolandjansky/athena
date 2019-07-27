/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MESONS_MESONSPHYSICSTOOL_H
#define MESONS_MESONSPHYSICSTOOL_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class MesonsPhysicsTool MesonsPhysicsTool.h
 * "G4AtlasInfrstructure/MesonsPhysicsTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Miha Muskinja
 *  @date   01-July-2019
 */
class MesonsPhysicsTool : public G4VPhysicsConstructor,
                          public extends<AthAlgTool, IPhysicsOptionTool> {
  public:
    /// Standard constructor
    MesonsPhysicsTool(const std::string &type, const std::string &name,
                      const IInterface *parent);

    virtual ~MesonsPhysicsTool(); ///< Destructor

    /// Initialize method
    virtual StatusCode initialize() override final;
    virtual void ConstructParticle() override final;
    virtual void ConstructProcess() override final;

    /** Implements */
    virtual MesonsPhysicsTool *GetPhysicsOption() override final;

  protected:
    /// a set of parameters for Meson building
    std::map<std::string, std::vector<double>> m_mesonsConfig;

    /// a set to hold the newly created mesons
    std::set<G4ParticleDefinition *> m_setOfMesons;
};

#endif // MESONS_MESONSPHYSICSTOOL_H
