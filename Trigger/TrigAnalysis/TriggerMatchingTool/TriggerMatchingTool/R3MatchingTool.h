/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMATCHINGTOOL_R3MATCHINGTOOL_H
#define TRIGGERMATCHINGTOOL_R3MATCHINGTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig
{
  class R3MatchingTool : public asg::AsgTool, virtual public IMatchingTool
  {
    ASG_TOOL_CLASS(R3MatchingTool, IMatchingTool)
  public:
    R3MatchingTool(const std::string &name);
    ~R3MatchingTool();

    virtual StatusCode initialize() override;

    virtual bool match(
        const std::vector<const xAOD::IParticle *> &recoObjects,
        const std::string &chain,
        double matchThreshold,
        bool rerun) const override;

    virtual bool match(
        const xAOD::IParticle &recoObject,
        const std::string &chain,
        double matchThreshold,
        bool rerun) const override;

  private:
    ToolHandle<TrigDecisionTool> m_trigDecTool;
    bool matchObjects(
        const xAOD::IParticle *reco,
        const ElementLink<xAOD::IParticleContainer> &onlineLink,
        std::map<std::pair<uint32_t, uint32_t>, bool> &cache,
        double drThreshold) const;

    // Internal functions
    /// Inherited from the interface but does nothing
    virtual MatchingImplementation *impl() const override { return nullptr; }

  }; //> end class R3MatchingTool
} // namespace Trig

#endif //> !TRIGGERMATCHINGTOOL_R3MATCHINGTOOL_H