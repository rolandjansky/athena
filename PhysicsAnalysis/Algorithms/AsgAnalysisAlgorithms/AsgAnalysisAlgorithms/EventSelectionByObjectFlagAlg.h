/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Miha Muskinja

#ifndef ASG_ANALYSIS_ALGORITHMS__EVENT_SELECTION_BY_OBJECT_FLAG_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__EVENT_SELECTION_BY_OBJECT_FLAG_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <xAODBase/IParticleContainer.h>

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

    /// \brief the particle collection we run on
  private:
    SysReadHandle<xAOD::IParticleContainer> m_particleHandle{
        this, "particles", "", "the particle collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection{this, "preselection", "",
                                       "the preselection to apply"};

    /// \brief counter for passed events
  private:
    long long m_passed = 0;

    /// \brief counter for total events
  private:
    long long m_total = 0;
};

} // namespace CP

#endif
