/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/GNNTool.h"
#include "FlavorTagDiscriminants/GNN.h"

namespace FlavorTagDiscriminants {

  GNNTool::GNNTool(const std::string& name):
          asg::AsgTool(name),
          m_props()
  {
    declareProperty("nnFile", m_props.nnFile,
      "the path to the netowrk file used to run inference");
    declareProperty("flipTagConfig", m_props.flipTagConfig,
      "flip configuration used for calibration");
    declareProperty("variableRemapping", m_props.variableRemapping,
      "user-defined mapping to rename the vars stored in the NN");
    declareProperty("trackLinkType", m_props.trackLinkType,
      "access tracks as IParticleContainer or as TrackParticleContainer");
    declareProperty("defaultOutputValue", m_props.default_output_value);
  }

  GNNTool::~GNNTool() {}

  StatusCode GNNTool::initialize() {

    ATH_MSG_INFO("Initialize bTagging Tool (GNN) from: " + m_props.nnFile);

    FlipTagConfig flip_config = FlipTagConfig::STANDARD;
    if (m_props.flipTagConfig.size() > 0) {
      flip_config = flipTagConfigFromString(m_props.flipTagConfig);
    }
    TrackLinkType trackLinkType = TrackLinkType::TRACK_PARTICLE;
    if (m_props.trackLinkType.size() > 0) {
      trackLinkType = trackLinkTypeFromString(m_props.trackLinkType);
    }
    m_gnn.reset(
      new GNN(
        m_props.nnFile,
        flip_config,
        m_props.variableRemapping,
        trackLinkType,
        m_props.default_output_value));

    return StatusCode::SUCCESS;
  }

  void GNNTool::decorate(const xAOD::BTagging& btag) const {
    m_gnn->decorate(btag);
  }
  void GNNTool::decorate(const xAOD::Jet& jet) const {
    m_gnn->decorate(jet, jet);
  }
  void GNNTool::decorateWithDefaults(const xAOD::Jet& jet) const {
    m_gnn->decorateWithDefaults(jet);
  }

  void GNNTool::decorate(const xAOD::Jet& jet, const SG::AuxElement& btag) const
  {
    m_gnn->decorate(jet, btag);
  }

  // Dependencies
  std::set<std::string> GNNTool::getDecoratorKeys() const {
    return m_gnn->getDecoratorKeys();
  }
  std::set<std::string> GNNTool::getAuxInputKeys() const {
    return m_gnn->getAuxInputKeys();
  }
  std::set<std::string> GNNTool::getConstituentAuxInputKeys() const {
    return m_gnn->getConstituentAuxInputKeys();
  }

}
