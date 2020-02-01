/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Miha Muskinja

#ifndef ASG_ANALYSIS_ALGORITHMS__EVENT_SELECTION_BY_OBJECT_FLAG_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__EVENT_SELECTION_BY_OBJECT_FLAG_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysDecorationHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <xAODBase/IParticleContainer.h>
#include <xAODEventInfo/EventInfo.h>

namespace CP {
/// \brief an algorithm for selecting events based on object flags
/// (e.g. bad muon veto or b-jet veto)
class EventSelectionByObjectFlagAlg final : public EL::AnaAlgorithm {
  public:
    EventSelectionByObjectFlagAlg(const std::string &name,
                                   ISvcLocator *svcLoc = nullptr);

  public:
    StatusCode initialize() override;

  public:
    StatusCode execute() override;

  public:
    StatusCode finalize() override;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList{this};

    /// \brief the event info we run on
  private:
    SysCopyHandle<xAOD::EventInfo> m_eventInfoHandle {
      this, "eventInfo", "EventInfo", "the event info object to run on"};

    /// \brief the particle collection we run on
  private:
    SysReadHandle<xAOD::IParticleContainer> m_particleHandle{
        this, "particles", "", "the particle collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection{this, "preselection", "", "the preselection to apply"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_veto{this, "veto", "", "selection upon which events are vetoed"};

    /// \brief the decoration for writing the scale factor
  private:
    SysDecorationHandle<char> m_eventDecisionOutputDecoration {
      this, "eventDecisionOutputDecoration", "", "the decoration for the event decision"};

    /// \brief counter for passed events
  private:
    long long m_passed = 0;

    /// \brief counter for total events
  private:
    long long m_total = 0;
};

} // namespace CP

#endif
