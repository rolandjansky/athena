/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTER_IMUONTESTERBRANCH_H
#define MUONTESTER_IMUONTESTERBRANCH_H

#include <StoreGate/ReadHandleKey.h>
#include <xAODBase/IParticle.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
class MuonTesterTree;

// Interface classes

/// Most basic interface class used by the MuonTester tree
class IMuonTesterBranch {
public:
    virtual ~IMuonTesterBranch() = default;
    /// The fill method checks if enough information is provided
    /// such that the branch is cleared from the information of the
    /// prevoius event and in cases of that updates did no happen befure
    /// a default value is  provided.
    ///     --> returns false if no update is called or the
    ///         dummy value has not been defined
    virtual bool fill(const EventContext& ctx) = 0;
    /// The init method checks whether the branch name has already registered to the
    /// MuonTree and tries then to add the branch to the tree
    ///     --> return false  if the branch has already been defined or
    ///         the inclusion of the branch to the tree failed
    ///         or the name of the branch is empty

    virtual bool init() = 0;
    ///     Returns the name of the branch
    virtual std::string name() const = 0;

    using DataDependency = SG::VarHandleKey*;
    /// Returns a vector of all Event data dependencies needed by the Branch to work
    virtual std::vector<DataDependency> data_dependencies() = 0;

};

// Extension of the interface in case someone wants to store information directly associated
// with the particles
// Particles are usually saved in vectors
class IAuxElementDecorationBranch : virtual public IMuonTesterBranch {
public:
    /// Reads out a generic auxillary element and add its
    /// information to the output vector.
    /// The interface takes internally care of deleting data from previous fill calls
    virtual void push_back(const SG::AuxElement* ele) = 0;
    virtual void push_back(const SG::AuxElement& ele) = 0;

    virtual void operator+=(const SG::AuxElement& ele) = 0;
    virtual void operator+=(const SG::AuxElement* ele) = 0;
    virtual ~IAuxElementDecorationBranch() = default;
};

class IParticleDecorationBranch : virtual public IMuonTesterBranch {
public:
    /// Similar to the IAuxElementDecoration branch but only accepting
    /// IParticles
    virtual void push_back(const xAOD::IParticle* p) = 0;
    virtual void push_back(const xAOD::IParticle& p) = 0;
    virtual void operator+=(const xAOD::IParticle* p) = 0;
    virtual void operator+=(const xAOD::IParticle& p) = 0;
    virtual ~IParticleDecorationBranch() = default;
};
#endif
