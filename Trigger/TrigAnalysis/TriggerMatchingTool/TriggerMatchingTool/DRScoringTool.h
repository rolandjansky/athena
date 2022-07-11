/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGGERMATCHINGTOOL_DRSCORINGTOOL_H
#define TRIGGERMATCHINGTOOL_DRSCORINGTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "TriggerMatchingTool/IMatchScoringTool.h"

namespace Trig {
    /// Score pairs of particles based on their deltaR
    class DRScoringTool : public asg::AsgTool, virtual public IMatchScoringTool
    {
    ASG_TOOL_CLASS(DRScoringTool, IMatchScoringTool)
    public:
        DRScoringTool(const std::string &name);
        virtual ~DRScoringTool() override = default;

        virtual StatusCode initialize() override;
        virtual float score(
                const xAOD::IParticle &online,
                const xAOD::IParticle &offline) const override;
    private:
        Gaudi::Property<bool> m_useRapidity{
            this, "UseRapidity", false, "Whether to use rapidity or eta for the calculation"};
    }; //> end class DRScoringTool
} //> end namespace Trig

#endif //> !TRIGGERMATCHINGTOOL_DRSCORINGTOOL_H
