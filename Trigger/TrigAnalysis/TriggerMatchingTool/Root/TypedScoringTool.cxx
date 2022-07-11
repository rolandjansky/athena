/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include "TriggerMatchingTool/TypedScoringTool.h"
#include "xAODBase/IParticle.h"

#include <sstream>

namespace Trig {
    TypedScoringTool::TypedScoringTool(const std::string &name) :
        asg::AsgTool(name)
    {}

    StatusCode TypedScoringTool::initialize()
    {
        if (!m_defaultTool.empty())
            ATH_CHECK(m_defaultTool.retrieve());
        if (m_typedTools.size() != m_toolTypes.size())
        {
            ATH_MSG_ERROR("Number of tools does match the number types!");
            return StatusCode::FAILURE;
        }
        ATH_CHECK(m_typedTools.retrieve());
        for (std::size_t idx = 0; idx < m_typedTools.size(); ++idx)
            m_tools[static_cast<xAOD::Type::ObjectType>(m_toolTypes[idx])] = m_typedTools[idx].operator->();
        return StatusCode::SUCCESS;
    }

    float TypedScoringTool::score(
            const xAOD::IParticle &online,
            const xAOD::IParticle &offline) const
    {
        const Trig::IMatchScoringTool *tool = getTool(online.type());
        if (!tool)
        {
            // Convert the enum to a string
            std::ostringstream oss;
            oss << online.type();
            throw std::out_of_range(oss.str());
        }
        return tool->score(online, offline);
    }

    const Trig::IMatchScoringTool *TypedScoringTool::getTool(xAOD::Type::ObjectType type) const
    {
        auto itr = m_tools.find(type);
        if (itr != m_tools.end())
            return itr->second;
        if (m_defaultTool.empty())
            return nullptr;
        return m_defaultTool.operator->();
    }
}
