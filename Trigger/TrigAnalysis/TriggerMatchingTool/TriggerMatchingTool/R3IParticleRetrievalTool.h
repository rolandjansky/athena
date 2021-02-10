/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerMatchingTool_R3IParticleRetrievalTool_H
#define TriggerMatchingTool_R3IParticleRetrievalTool_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TriggerMatchingTool/IIParticleRetrievalTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig
{
  class R3IParticleRetrievalTool final : public virtual IIParticleRetrievalTool,
                                         public asg::AsgTool
  {
    ASG_TOOL_CLASS(R3IParticleRetrievalTool, IIParticleRetrievalTool)
  public:
    /// Standard constructor
    R3IParticleRetrievalTool(const std::string &name);

    /// Destructor
    virtual ~R3IParticleRetrievalTool() override;

    /// Initialise the tool
    virtual StatusCode initialize() override;

    /**
     * @brief Retrieve the particles that caused this trigger to fire
     * @param[out] combinations All combinations of particles that could have
     * caused the trigger to fire.
     * @param chain The name of the chain.
     * @param rerun Whether to look at the chain in 'rerun' mode.
     * The provided vector will be cleared before use!
     * This tool may not work if too much trigger information has been
     * removed.
     */
    virtual StatusCode retrieveParticles(
        std::vector<std::vector<const xAOD::IParticle *>> &combinations,
        const std::string &chain,
        bool rerun = false) const override;

  private:
    // Properties
    /// The TrigDecisionTool that will be used to get the navigation
    ToolHandle<Trig::TrigDecisionTool> m_tdt{"Trig::TrigDecisionTool/TrigDecisionTool"};
  }; //> end class R3IParticleRetrievalTool
} // namespace Trig

#endif //> !TriggerMatchingTool_R3IParticleRetrievalTool_H