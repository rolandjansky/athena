/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXTRA_PARTICLES__EXTRA_PARTICLES_PHYSICS_TOOL_H
#define EXTRA_PARTICLES__EXTRA_PARTICLES_PHYSICS_TOOL_H 1

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class ExtraParticlesPhysicsTool ExtraParticlesPhysicsTool.h
 * "ExtraParticles/ExtraParticlesPhysicsTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Miha Muskinja
 *  @date   August-2019
 */
class ExtraParticlesPhysicsTool
    : public G4VPhysicsConstructor,
      public extends<AthAlgTool, IPhysicsOptionTool> {
  public:
    /// Standard constructor
    ExtraParticlesPhysicsTool(const std::string &type, const std::string &name,
                              const IInterface *parent);

    virtual ~ExtraParticlesPhysicsTool(); ///< Destructor

    /// Initialize method
    virtual StatusCode initialize() override final;
    virtual void ConstructParticle() override final;
    virtual void ConstructProcess() override final;

    /** Implements */
    virtual ExtraParticlesPhysicsTool *GetPhysicsOption() override final;

  protected:
    /// a set of parameters for extra particle building
    std::map<std::string, std::vector<double>> m_extraParticlesConfig;

    /// a set to hold the newly created extra particles
    std::set<G4ParticleDefinition *> m_extraParticles;
};

#endif // EXTRA_PARTICLES__EXTRA_PARTICLES_PHYSICS_TOOL_H
