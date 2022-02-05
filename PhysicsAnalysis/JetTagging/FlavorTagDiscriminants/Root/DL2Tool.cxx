/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/DL2Tool.h"
#include "FlavorTagDiscriminants/DL2HighLevel.h"

namespace FlavorTagDiscriminants {

  DL2Tool::DL2Tool(const std::string& name):
    asg::AsgTool(name),
    m_props(),
    m_dl2(nullptr)
  {
    declareProperty("nnFile", m_props.nnFile);
    declareProperty("flipTagConfig", m_props.flipTagConfig);
    declareProperty("variableRemapping", m_props.variableRemapping);
    declareProperty("trackLinkType", m_props.trackLinkType);
  }
  DL2Tool::~DL2Tool() {}

  StatusCode DL2Tool::initialize() {
    ATH_MSG_INFO("Initialize DL2 from: " + m_props.nnFile);
    FlipTagConfig flipConfig = FlipTagConfig::STANDARD;
    if (m_props.flipTagConfig.size() > 0) {
      flipConfig = flipTagConfigFromString(m_props.flipTagConfig);
    }
    TrackLinkType trackLinkType = TrackLinkType::TRACK_PARTICLE;
    if (m_props.trackLinkType.size() > 0) {
      trackLinkType = trackLinkTypeFromString(m_props.trackLinkType);
    }
    m_dl2.reset(
      new DL2HighLevel(
        m_props.nnFile,
        flipConfig,
        m_props.variableRemapping,
        trackLinkType
        )
      );
    return StatusCode::SUCCESS;
  }

  void DL2Tool::decorate(const xAOD::BTagging& btag) const {
    ATH_MSG_DEBUG("Decorating btagging object from: " + m_props.nnFile);
    m_dl2->decorate(btag);
    ATH_MSG_VERBOSE("Decorated btagging object");
  }
  void DL2Tool::decorate(const xAOD::Jet& jet) const {
    ATH_MSG_DEBUG("Decorating jet from: " + m_props.nnFile);
    m_dl2->decorate(jet);
    ATH_MSG_VERBOSE("Decorated jet");
  }
  void DL2Tool::decorateWithDefaults(const xAOD::Jet& jet) const {
    ATH_MSG_DEBUG("Decorating jet with defaults from: " + m_props.nnFile);
    m_dl2->decorateWithDefaults(jet);
    ATH_MSG_VERBOSE("Decorated jet with defaults");
  }

  std::set<std::string> DL2Tool::getDecoratorKeys() const {
    return m_dl2->getDataDependencyNames().bTagOutputs;
  }
  std::set<std::string> DL2Tool::getAuxInputKeys() const {
    return m_dl2->getDataDependencyNames().bTagInputs;
  }
  std::set<std::string> DL2Tool::getConstituentAuxInputKeys() const {
    return m_dl2->getDataDependencyNames().trackInputs;
  }

}
