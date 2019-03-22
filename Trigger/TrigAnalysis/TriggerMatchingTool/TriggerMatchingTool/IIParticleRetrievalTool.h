/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerMatchingTool_IIParticleRetrievalTool_H
#define TriggerMatchingTool_IIParticleRetrievalTool_H

// Framework includes
#include "AsgTools/IAsgTool.h"
#include "xAODBase/IParticle.h"

#include <vector>

namespace Trig {
  class IIParticleRetrievalTool : public virtual asg::IAsgTool {
    ASG_TOOL_INTERFACE( IIParticleRetrievalTool )
    public:
      /**
       * @brief Retrieve the particles that caused this trigger to fire
       * @param[out] combinations All combinations of particles that could have
       * caused the trigger to fire.
       * @param chain The name of the chain.
       * @param rerun Whether to look at the chain in 'rerun' mode.
       */
      virtual StatusCode retrieveParticles(
          std::vector<std::vector<const xAOD::IParticle*>>& combinations,
          const std::string& chain,
          bool rerun = false) = 0;
  }; //> end class IIParticleRetrievalTool
} //> end namespace Trig

#endif //> !TriggerMatchingTool_IIParticleRetrievalTool_H
