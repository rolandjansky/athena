/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_IISOLATIONLOWPTPLVTOOL_H
#define ISOLATIONSELECTION_IISOLATIONLOWPTPLVTOOL_H

#include <xAODBase/IParticle.h>
#include <AsgTools/IAsgTool.h>

namespace CP {
  class IIsolationLowPtPLVTool: public virtual asg::IAsgTool {
    ASG_TOOL_INTERFACE (CP::IIsolationLowPtPLVTool)
    public:
    virtual ~IIsolationLowPtPLVTool() = default;
    ///   This method adds the lowPT PLV score as decoration 
    ///   to the lepton. The method needs to be called before
    ///   the call of the IsolationTool if you're running on 
    ///   derivations with a cache older than <blah> and using the
    ///   new refined PLV working points provided by the IFF
    virtual StatusCode augmentPLV(const xAOD::IParticle& Particle) = 0; 
  };
}
#endif
