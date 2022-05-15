/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Miha Muskinja

#ifndef ASG_ANALYSIS_ALGORITHMS__PMG_HF_PRODUCTION_FRACTION_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__PMG_HF_PRODUCTION_FRACTION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <PMGAnalysisInterfaces/ISysTruthWeightTool.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <SystematicsHandles/SysWriteDecorHandle.h>
#include <xAODEventInfo/EventInfo.h>

namespace CP
{
/// \brief an algorithm for calling \ref ISysTruthWeightTool

class SysTruthWeightAlg final : public EL::AnaAlgorithm
{
    /// \brief the standard constructor
public:
    SysTruthWeightAlg(const std::string& name, ISvcLocator* pSvcLocator);

public:
    StatusCode initialize() override;

public:
    StatusCode execute() override;

    /// \brief the truth particle container to use for the calculation
private:
    std::string m_truthParticleContainer;

    /// \brief the tool
private:
    ToolHandle<PMGTools::ISysTruthWeightTool> m_sysTruthWeightTool;

    /// \brief the systematics list we run
private:
    SysListHandle m_systematicsList{this};

    /// \brief the event collection we run on
private:
    SysReadHandle<xAOD::EventInfo> m_eventInfoHandle{
        this, "eventInfo", "EventInfo", "the event info object to run on"};

    /// \brief the decoration for the truth weights
private:
    SysWriteDecorHandle<float> m_decoration{
        this, "decoration", "", "the decoration for the truth weights"};
};
}  // namespace CP

#endif
