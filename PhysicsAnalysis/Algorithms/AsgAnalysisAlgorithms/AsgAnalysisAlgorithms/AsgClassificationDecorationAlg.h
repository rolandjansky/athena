/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak <tadej@cern.ch>

#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_CLASSIFICATION_DECORATION_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_CLASSIFICATION_DECORATION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODBase/IParticle.h>
#include <xAODBase/IParticleContainer.h>

#include <AsgAnalysisInterfaces/IClassificationTool.h>


namespace CP
{

/// \brief an algorithm for decorating classification using a tool based
/// on the \ref CP::IClassificationTool
class AsgClassificationDecorationAlg final : public EL::AnaAlgorithm
{
  /// \brief the standard constructor
public:
  AsgClassificationDecorationAlg(const std::string& name, 
                                 ISvcLocator* pSvcLocator);


public:
  virtual StatusCode initialize() override;

public:
  virtual StatusCode execute() override;


  /// \brief truth classifier tool handle
private:
    ToolHandle<IClassificationTool> m_tool;

  /// \brief the systematics list we run
private:
  CP::SysListHandle m_systematicsList {this};

  /// \brief particles container handle
private:
  CP::SysCopyHandle<xAOD::IParticleContainer> m_particlesHandle {
    this, "particles", "", "the container to use"};

  /// \brief the decoration for the truth classification
private:
  std::string m_classificationDecoration {};

  /// \brief the accessor for \ref m_truthClassificationDecoration
private:
  std::unique_ptr<const SG::AuxElement::Accessor<unsigned int> > m_classificationAccessor {};
};

} // namespace CP

#endif
