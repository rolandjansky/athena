/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include "TriggerMatchingTool/DRScoringTool.h"
#include "FourMomUtils/xAODP4Helpers.h"

namespace Trig {
    DRScoringTool::DRScoringTool(const std::string &name) :
        asg::AsgTool(name)
    {}

    StatusCode DRScoringTool::initialize()
    {
        return StatusCode::SUCCESS;
    }

    float DRScoringTool::score(
            const xAOD::IParticle &online,
            const xAOD::IParticle &offline) const
    {
        return xAOD::P4Helpers::deltaR(online, offline, m_useRapidity.value());
    }
} //> end namespace Trig
