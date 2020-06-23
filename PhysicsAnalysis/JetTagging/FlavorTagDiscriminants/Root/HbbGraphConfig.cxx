/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Note: please don't include any ROOT in here (i.e. xAOD::Jet or
// anything from the EDM), it's not a stable dependency (in particular
// the TString overloads change between releases).

#include "FlavorTagDiscriminants/HbbGraphConfig.h"
#include "FlavorTagDiscriminants/HbbConstants.h"

#include "lwtnn/lightweight_network_config.hh"

#include <regex>

namespace {
  std::string formatSet(const std::set<std::string>& s) {
    std::string out("{");
    for (const std::string& var: s) {
      out.append(var);
      if (var != *s.rbegin()) out.append(", ");
    }
    out.append("}");
    return out;
  }
}

namespace FlavorTagDiscriminants {
  HbbGraphConfig getHbbGraphConfig(const lwt::GraphConfig& graph_cfg){
    namespace hk = FlavorTagDiscriminants::hbb_key;
    HbbGraphConfig k;
    k.n_subjets = 0;
    std::regex subjet_match(hk::subjet + "[0-9]+");
    std::string first_match;
    for (const auto& node: graph_cfg.inputs) {
      for (const auto& var: node.variables) {
        const std::string& name = var.name;
        k.defaults[node.name][name] = node.defaults.at(name);
      }
      if (node.name == hk::fatjet) {
        for (const auto& var: node.variables) {
          k.fatjet.insert(var.name);
        }
      } else if (std::regex_match(node.name, subjet_match)) {
        std::set<std::string> subjet_set;
        for (const auto& var: node.variables) {
          subjet_set.insert(var.name);
        }
        if (k.n_subjets == 0) {
          k.subjet = subjet_set;
          first_match = node.name;
        } else if (k.subjet != subjet_set) {
          std::string error = "mismatch in subjet keys: we expect the same"
            " variables to be read from each subjet. ";
          error.append("first subjet (" + first_match + ") "
                       + formatSet(k.subjet) + ", match "
                       + std::to_string(k.n_subjets)
                       + ", (" + node.name + ") " + formatSet(subjet_set));
          throw std::runtime_error(error);
        }
        k.n_subjets++;
      } else {
        throw std::runtime_error("no way to match '" + node.name + "'");
      }
    }
    return k;
  }
}
