/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/GNNTool.h"
#include "FlavorTagDiscriminants/BTagTrackIpAccessor.h"

#include "xAODBTagging/BTaggingUtilities.h"
#include "PathResolver/PathResolver.h"

#include "lwtnn/parse_json.hh"

#include <fstream>


namespace {
  const std::string jetLinkName = "jetLink";
}

namespace FlavorTagDiscriminants {

  GNNTool::GNNTool(const std::string& name):
          asg::AsgTool(name),
          m_props(),
         m_jetLink(jetLinkName)
  {
    declareProperty("nnFile", m_props.nnFile,
      "the path to the netowrk file used to run inference");
    declareProperty("flipTagConfig", m_props.flipTagConfig,
      "flip configuration used for calibration");
    declareProperty("variableRemapping", m_props.variableRemapping,
      "user-defined mapping to rename the vars stored in the NN");
    declareProperty("trackLinkType", m_props.trackLinkType,
      "access tracks as IParticleContainer or as TrackParticleContainer");
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

    std::string fullPathToOnnxFile = PathResolverFindCalibFile(m_props.nnFile);

    m_onnxUtil = std::make_unique<OnnxUtil> (fullPathToOnnxFile);
    m_onnxUtil->initialize();

    std::string gnn_config_str = m_onnxUtil->getMetaData("gnn_config");

    std::stringstream gnn_config_stream;
    gnn_config_stream << gnn_config_str;

    m_config = lwt::parse_json_graph(gnn_config_stream);

    auto [inputs, track_sequences, options] = dataprep::createGetterConfig(
        m_config, flip_config, m_props.variableRemapping, trackLinkType);

    auto [vb, vj, ds] = dataprep::createBvarGetters(inputs);
    m_varsFromBTag = vb;
    m_varsFromJet = vj;
    m_dataDependencyNames = ds;

    auto [tsb, td] = dataprep::createTrackGetters(
      track_sequences, options, jetLinkName);
    m_trackSequenceBuilders = tsb;
    m_dataDependencyNames += td;

    auto [decorators, dd] = dataprep::createDecorators(
      m_config, options);
    m_decorators = decorators;
    m_dataDependencyNames += dd;

    return StatusCode::SUCCESS;
  }

  void GNNTool::decorate(const xAOD::BTagging& btag) const {
    auto jetLink = m_jetLink(btag);
    if (!jetLink.isValid()) {
      throw std::runtime_error("invalid jetLink");
    }
    const xAOD::Jet& jet = **jetLink;
    decorate(jet, btag);
  }
  void GNNTool::decorate(const xAOD::Jet& jet) const {
    decorate(jet, jet);
  }

  void GNNTool::decorate(const xAOD::Jet& jet, const SG::AuxElement& btag) const {

    using namespace internal;

    std::map<std::string, input_pair> gnn_input;

    std::vector<float> jet_feat;
    for (const auto& getter: m_varsFromBTag) {
      jet_feat.push_back(getter(btag).second);
    }
    for (const auto& getter: m_varsFromJet) {
      jet_feat.push_back(getter(jet).second);
    }
    std::vector<int64_t> jet_feat_dim = {1, static_cast<int64_t>(jet_feat.size())};

    input_pair jet_info (jet_feat, jet_feat_dim);
    gnn_input.insert({"jet_features", jet_info});

    for (const auto& builder: m_trackSequenceBuilders) {
      std::vector<float> track_feat; // (#tracks, #feats).flatten
      int num_track_vars = static_cast<int>(builder.sequencesFromTracks.size());
      int num_tracks = 0;

      Tracks sorted_tracks = builder.tracksFromJet(jet, btag);
      Tracks flipped_tracks = builder.flipFilter(sorted_tracks, jet);

      int track_var_idx=0;
      for (const auto& seq_builder: builder.sequencesFromTracks) {
        auto double_vec = seq_builder(jet, flipped_tracks).second;

        if (track_var_idx==0){
          num_tracks = static_cast<int>(double_vec.size());
          track_feat.resize(num_tracks * num_track_vars);
        }

        // need to transpose + flatten
        for (unsigned int track_idx=0; track_idx<double_vec.size(); track_idx++){
          track_feat.at(track_idx*num_track_vars + track_var_idx) 
            = double_vec.at(track_idx);
        }
        track_var_idx++;
      }
      std::vector<int64_t> track_feat_dim = {num_tracks, num_track_vars};
      
      input_pair track_info (track_feat, track_feat_dim);
      gnn_input.insert({"track_features", track_info});
    }
    
    std::map<std::string, float> outputs;
    m_onnxUtil->runInference(gnn_input, outputs);

    for (const auto& dec: m_decorators) {
      for (const auto& node: dec.second) {
        node.second(btag) = outputs.at(node.first);
      }
    }
  } // end of decorate()

  // Dependencies
  std::set<std::string> GNNTool::getDecoratorKeys() const {
    return m_dataDependencyNames.bTagOutputs;
  }
  std::set<std::string> GNNTool::getAuxInputKeys() const {
    return m_dataDependencyNames.bTagInputs;
  }
  std::set<std::string> GNNTool::getConstituentAuxInputKeys() const {
    return m_dataDependencyNames.trackInputs;
  }

}
