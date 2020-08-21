/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "FlavorTagDiscriminants/HbbTag.h"
#include "FlavorTagDiscriminants/HbbConstants.h"
#include "FlavorTagDiscriminants/HbbGraphConfig.h"

// EDM includes
#include "xAODJet/Jet.h"

// atlas utilities
#include "PathResolver/PathResolver.h"

// external libraries
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"

// c++ core
#include <set>
#include <fstream>

namespace {

  template <typename T>
  class BTagPairGetter
  {
  public:
    BTagPairGetter(const std::string& key);
    std::pair<std::string, double> operator()(const xAOD::Jet& jet);
  private:
    std::string m_key;
    SG::AuxElement::ConstAccessor<T> m_getter;
  };

  using Pg = std::function<std::pair<std::string, double>(const xAOD::Jet&)>;
  Pg makePairGetter(const std::string& key);

  void requireOverwrite(std::map<std::string, double>& target,
                        const std::pair<std::string, double>& value);


}

namespace FlavorTagDiscriminants {

  HbbTag::HbbTag(const HbbTagConfig& config):
    m_parent_link("Parent"),
    m_subjet_link_getter(config.subjet_link_name),
    m_n_subjets(0),
    m_min_subjet_pt(config.min_subjet_pt)
  {
    namespace fs = boost::filesystem;
    // setup NN
    fs::path nn_path = config.input_file_path;
    if (!fs::exists(nn_path)) {
      nn_path = PathResolverFindCalibFile(nn_path.string());
      if (nn_path.empty()) {
        throw std::runtime_error(
          "no file found at '" + config.input_file_path.string() + "'");
      }
    }
    std::ifstream input_stream(nn_path.string());
    lwt::GraphConfig graph_cfg = lwt::parse_json_graph(input_stream);
    m_graph.reset(new lwt::LightweightGraph(graph_cfg));

    // setup large-R jet getters and defaults
    HbbGraphConfig keys = getHbbGraphConfig(graph_cfg);
    // add the getters
    for (const std::string& key: keys.fatjet) {
      m_fat_jet_getters.push_back(makePairGetter(key));
    }
    for (const std::string& key: keys.subjet) {
      m_subjet_getters.push_back(makePairGetter(key));
    }
    m_defaults = keys.defaults;
    m_n_subjets = keys.n_subjets;

    // setup outputs
    for (const auto& output: graph_cfg.outputs) {
      const std::string& node_name = output.first;
      const lwt::OutputNodeConfig& node = output.second;
      NodeWriter node_writer;
      for (const std::string& varname: node.labels) {
        std::string write_name = node_name + "_" + varname;
        node_writer.emplace_back(varname, write_name);
      }
      m_outputs.emplace_back(node_name, node_writer);
    }
  }
  HbbTag::~HbbTag() {}

  void HbbTag::decorate(const xAOD::Jet& jet) const {
    namespace hk = hbb_key;
    std::map<std::string, std::map<std::string, double>> inputs = m_defaults;
    for (const auto& getter: m_fat_jet_getters) {
      requireOverwrite(inputs.at(hk::fatjet),getter(jet));
    }

    std::vector<const xAOD::IParticle*> subjets;
    const xAOD::Jet* parent = *m_parent_link(jet);
    if (!parent) throw std::runtime_error("can't resolve parent jet");
    for (const auto& link: m_subjet_link_getter(*parent)) {
      const xAOD::IParticle* subjet = *link;
      if (!subjet) throw std::runtime_error("can't resolve subjet link");
      if (subjet->pt() >= m_min_subjet_pt) {
        subjets.push_back(subjet);
      }
    }
    std::sort(subjets.begin(), subjets.end(),
              [](auto* a, auto* b) { return a->pt() > b->pt(); });

    size_t n_jets = std::min(subjets.size(), m_n_subjets);
    for (size_t jet_n = 0; jet_n < n_jets; jet_n++) {
      const auto* subjet = dynamic_cast<const xAOD::Jet*>(subjets.at(jet_n));
      if (!subjet) throw std::runtime_error("IParticle is not a Jet");
      std::string subjet_name = hk::subjet + std::to_string(jet_n);
      for (const auto& getter: m_subjet_getters) {
        requireOverwrite(inputs.at(subjet_name),getter(*subjet));
      }
    }

    // calculate and write
    for (const auto& node: m_outputs) {
      const auto& result = m_graph->compute(inputs, {}, node.first);
      for (const auto& var_writer: node.second) {
        var_writer.second(jet) = result.at(var_writer.first);
      }
    }

  }

}


namespace {
  // implemenation
  template <typename T>
  BTagPairGetter<T>::BTagPairGetter(const std::string& key):
    m_key(key), m_getter(key)
  {
  }
  template <typename T>
  std::pair<std::string, double>
  BTagPairGetter<T>::operator()(const xAOD::Jet& jet) {
    const xAOD::BTagging* btag = jet.btagging();
    if (!btag) throw std::runtime_error("can't find btagging object");
    return {m_key, m_getter(*btag)};
  }


  Pg makePairGetter(const std::string& key) {
    namespace hk = FlavorTagDiscriminants::hbb_key;
    if (key == hk::pt) {
      return [](const xAOD::Jet& j) -> std::pair<std::string, double> {
        return {hk::pt, j.pt()};
      };
    } else if (key == hk::eta) {
      return [](const xAOD::Jet& j) -> std::pair<std::string, double> {
        return {hk::eta, j.eta()};
      };
    } else {
      // for now we assume everything we read from b-tagging is a double,
      // this is only true for DL1 scores.
      return BTagPairGetter<double>(key);
    }
  }

  void requireOverwrite(std::map<std::string, double>& target,
                        const std::pair<std::string, double>& value) {
    const auto itr = target.find(value.first);
    if (itr == target.end()) {
      throw std::logic_error("can't fine a default value for " + value.first);
    }
    itr->second = value.second;
  }

}
