/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_EVENT_SCALE_FACTOR_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_EVENT_SCALE_FACTOR_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SelectionHelpers/ISelectionAccessor.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODBase/IParticleContainer.h>
#include <xAODEventInfo/EventInfo.h>

namespace CP
{
  /// \brief an algorithm for calculating per-event scale factors

  class AsgEventScaleFactorAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    AsgEventScaleFactorAlg (const std::string& name, 
                            ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;


    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the event info we run on (empty by default)
  private:
    SysCopyHandle<xAOD::EventInfo> m_eventInfoHandle {
      this, "eventInfo", "EventInfo", "the event info object to run on"};

    /// \brief the jet collection we run on
  private:
    SysCopyHandle<xAOD::IParticleContainer> m_particleHandle {
      this, "particles", "", "the particle collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the decoration for the efficiency
  private:
    std::string m_efficiency;

    /// \brief the accessor for \ref m_efficiency
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<float> > m_efficiencyAccessor;
  };
}

#endif
