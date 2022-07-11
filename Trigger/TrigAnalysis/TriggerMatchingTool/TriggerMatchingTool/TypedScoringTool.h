/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGGERMATCHINGTOOL_TYPEDSCORINGTOOL_H
#define TRIGGERMATCHINGTOOL_TYPEDSCORINGTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "TriggerMatchingTool/IMatchScoringTool.h"
#include "xAODBase/ObjectType.h"

namespace Trig
{
    /**
     * @brief Score particles of different types in different ways
     */
    class TypedScoringTool : public asg::AsgTool, virtual public IMatchScoringTool
    {
    ASG_TOOL_CLASS(TypedScoringTool, IMatchScoringTool)
    public:
        TypedScoringTool(const std::string &name);
        virtual ~TypedScoringTool() override = default;

        virtual StatusCode initialize() override;
        virtual float score(
                const xAOD::IParticle &online,
                const xAOD::IParticle &offline) const override;
    private:
        ToolHandle<Trig::IMatchScoringTool> m_defaultTool{
            this, "DefaultScoringTool", "", "The tool to use if the type-specifc one isn't set"};
        ToolHandleArray<Trig::IMatchScoringTool> m_typedTools{
            this, "TypedScoringTools", {}, "Tools for specific types (parallel with the ToolTypes property)"};
        Gaudi::Property<std::vector<int>> m_toolTypes{
            this, "ToolTypes", {}, "The xAOD types addressed by the tools"};
        std::map<xAOD::Type::ObjectType, const Trig::IMatchScoringTool *> m_tools;

        const Trig::IMatchScoringTool *getTool(xAOD::Type::ObjectType type) const;
    }; //> end class TypedScoringTool
} //> end namespace Trig

#endif //> !TRIGGERMATCHINGTOOL_TYPEDSCORINGTOOL_H
