/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak <tadej@cern.ch>

#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_PRIORITY_DECORATION_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_PRIORITY_DECORATION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SelectionHelpers/SysReadSelectionHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <xAODBase/IParticleContainer.h>


namespace CP
{

/// \brief an algorithm for decorating priorities
class AsgPriorityDecorationAlg final : public EL::AnaAlgorithm
{
  /// \brief the standard constructor
public:
  AsgPriorityDecorationAlg(const std::string& name, 
                           ISvcLocator* pSvcLocator);


public:
  virtual StatusCode initialize() override;

public:
  virtual StatusCode execute() override;


  /// \brief the systematics list we run
private:
  SysListHandle m_systematicsList {this};

  /// \brief the preselection we apply to our input
private:
  SysReadSelectionHandleArray m_preselections {
    this, "preselections", {}, "the preselections to apply with the highest priority first"};

  /// \brief particles container handle
private:
  SysReadHandle<xAOD::IParticleContainer> m_particlesHandle {
    this, "particles", "", "the container to use"};

  /// \brief the values of the priorities
private:
  std::vector<int> m_priorities {};

  /// \brief the decoration for the priority
private:
  std::string m_priorityDecoration {};

  /// \brief the accessor for \ref m_priorityDecoration
private:
  std::unique_ptr<const SG::AuxElement::Decorator<char> > m_priorityDecorator {};
};

} // namespace CP

#endif
