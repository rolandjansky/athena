/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4_EXTRA_PROCESSESS__G4_EM_PROCESSES_PHYSICS_TOOL_H
#define G4_EXTRA_PROCESSESS__G4_EM_PROCESSES_PHYSICS_TOOL_H 1

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class G4EMProcessesPhysicsTool G4EMProcessesPhysicsTool.h
 * "G4ExtraProcesses/G4EMProcessesPhysicsTool.h"
 *
 *  Tool for the concrete implementation of a Physics List selection class
 *
 *  @author Miha Muskinja
 *  @date   August-2020
 */
class G4EMProcessesPhysicsTool
    : public G4VPhysicsConstructor,
      public extends<AthAlgTool, IPhysicsOptionTool>
{
public:
    /// Standard constructor
    G4EMProcessesPhysicsTool(const std::string &type, const std::string &name,
                             const IInterface *parent);

    virtual ~G4EMProcessesPhysicsTool(){}; ///< Destructor

    /// Initialize method
    virtual StatusCode initialize() override final;
    virtual void ConstructParticle() override final;
    virtual void ConstructProcess() override final;

    /** Implements */
    virtual G4EMProcessesPhysicsTool *GetPhysicsOption() override final;

protected:
    /// list of particles to apply the em processes to
    std::vector<int> m_particleList;
};

#endif // G4_EXTRA_PROCESSESS__G4_EM_PROCESSES_PHYSICS_TOOL_H
