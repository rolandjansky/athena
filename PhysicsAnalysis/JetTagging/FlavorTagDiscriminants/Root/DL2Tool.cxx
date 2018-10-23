/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/DL2Tool.h"
#include "FlavorTagDiscriminants/DL2.h"

#include "lwtnn/parse_json.hh"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

#include <fstream>

namespace {
  // define a regex literal operator
  std::regex operator""_r(const char* c, size_t /* length */) {
    return std::regex(c);
  }
}

namespace FlavorTagDiscriminants {

  DL2Tool::DL2Tool(const std::string& nn_file_name):
    m_dl2(nullptr)
  {
    std::ifstream input_stream(nn_file_name);
    lwt::GraphConfig config = lwt::parse_json_graph(input_stream);

    std::vector<std::string> input_names;
    for (const auto& var: config.inputs.at(0).variables) {
      input_names.push_back(var.name);
    }

    // type and default value-finding regexes are hardcoded for now
    TypeRegexes type_regexes{
      {"(IP[23]D_|SV[12]_|rnnip_)p(b|c|u|tau)"_r, EDMType::DOUBLE},
      {"(JetFitter_|SV1_)[Nn].*"_r, EDMType::INT},
      {"(JetFitter_|SV1_).*"_r, EDMType::FLOAT}};
    StringRegexes default_flag_regexes{
      {"IP2D_.*"_r, "IP2D_isDefaults"},
      {"IP3D_.*"_r, "IP3D_isDefaults"},
      {"SV1_.*"_r, "SV1_isDefaults"},
      {"JetFitter_.*"_r, "JetFitter_isDefaults"},
      {"secondaryVtx_.*"_r, "secondaryVtx_isDefaults"},
      {"rnnip_.*"_r, "rnnip_isDefaults"}};

    std::vector<DL2InputConfig> input_config = get_input_config(
      input_names, type_regexes, default_flag_regexes);
    m_dl2.reset(new DL2(config, input_config));
  }

  DL2Tool::~DL2Tool() = default;

  void DL2Tool::decorate(const xAOD::Jet& jet) const {
    m_dl2->decorate(jet);
  }

}
