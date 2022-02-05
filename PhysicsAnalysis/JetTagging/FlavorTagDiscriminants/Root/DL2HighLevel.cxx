/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/DL2HighLevel.h"
#include "FlavorTagDiscriminants/DL2.h"

#include "PathResolver/PathResolver.h"

#include "lwtnn/parse_json.hh"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

#define BOOST_BIND_GLOBAL_PLACEHOLDERS // Needed to silence Boost pragma message
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

#include <fstream>


namespace FlavorTagDiscriminants {

  DL2HighLevel::DL2HighLevel(const std::string& nn_file_name,
                             FlipTagConfig flip_config,
                             std::map<std::string,std::string> remap_scalar,
                             TrackLinkType track_link_type):
    m_dl2(nullptr)
  {
    // get the graph
    std::string nn_path = PathResolverFindCalibFile(nn_file_name);
    if (nn_path.size() == 0) {
      throw std::runtime_error("no file found at '" + nn_file_name + "'");
    }
    std::ifstream input_stream(nn_path);
    lwt::GraphConfig config = lwt::parse_json_graph(input_stream);

    if (config.inputs.size() > 1) {
      throw std::logic_error("DL2 doesn't support multiple inputs");
    }

    auto [input_config, trk_config, options] = dataprep::createGetterConfig(
      config, flip_config, remap_scalar, track_link_type);

    m_dl2.reset(
      new DL2(
        config,                 // lwtnn config
        input_config,           // EDM input config
        trk_config,             // edm track input config
        options
        ));
  }

  DL2HighLevel::~DL2HighLevel() = default;
  DL2HighLevel::DL2HighLevel(DL2HighLevel&&) = default;

  void DL2HighLevel::decorate(const xAOD::BTagging& btag) const {
    m_dl2->decorate(btag);
  }
  void DL2HighLevel::decorate(const xAOD::Jet& jet) const {
    m_dl2->decorate(jet);
  }
  void DL2HighLevel::decorateWithDefaults(const xAOD::Jet& jet) const {
    m_dl2->decorateWithDefaults(jet);
  }

  FTagDataDependencyNames DL2HighLevel::getDataDependencyNames() const
  {
    return m_dl2->getDataDependencyNames();
  }

}
