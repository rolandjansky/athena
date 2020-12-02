/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/DL2HighLevelTools.h"

// functions that are used internally
namespace {
  template <typename T>
  T match_first(const std::vector<std::pair<std::regex, T> >& regexes,
                const std::string var_name,
                const std::string context) {
    for (const auto& pair: regexes) {
      if (std::regex_match(var_name, pair.first)) {
        return pair.second;
      }
    }
    throw std::logic_error(
      "no regex match found for input variable '" + var_name + "' in "
      + context);
  }
}

namespace FlavorTagDiscriminants {
  // ______________________________________________________________________
  // High level configuration functions
  //
  std::vector<DL2InputConfig> get_input_config(
    const std::vector<std::string>& variable_names,
    const TypeRegexes& type_regexes,
    const StringRegexes& default_flag_regexes)
  {
    std::vector<DL2InputConfig> inputs;
    for (const auto& var: variable_names) {
      DL2InputConfig input;
      input.name = var;
      input.type = match_first(type_regexes, var, "type matching");
      input.default_flag = match_first(default_flag_regexes, var,
                                       "default matching");

      inputs.push_back(input);
    }
    return inputs;
  }
  // do some input variable magic in case someone asked
  void remap_inputs(std::vector<lwt::Input>& nn,
                    std::map<std::string, std::string>& replaced_vars,
                    std::map<std::string, double>& defaults) {
    // keep track of the new default values, and which values they
    // were moved from
    std::map<std::string, double> new_defaults;
    std::set<std::string> moved_defaults;
    for (lwt::Input& input: nn) {
      std::string nn_name = input.name;
      auto replacement_itr = replaced_vars.find(nn_name);
      if (replacement_itr != replaced_vars.end()) {
        std::string new_name = replacement_itr->second;
        input.name = new_name;
        if (defaults.count(nn_name)) {
          new_defaults[new_name] = defaults.at(nn_name);
          moved_defaults.insert(nn_name);
        }
        replaced_vars.erase(replacement_itr);
      }
    }
    for (const auto& new_default: new_defaults) {
      defaults[new_default.first] = new_default.second;
      // if something was a new default we don't want to delete it
      // below.
      moved_defaults.erase(new_default.first);
    }
    // delete anything that was moved but wasn't assigned to
    for (const auto& moved: moved_defaults) {
      defaults.erase(moved);
    }
  }
  std::vector<DL2TrackSequenceConfig> get_track_input_config(
    const std::vector<std::pair<std::string, std::vector<std::string>>>& names,
    const TypeRegexes& type_regexes,
    const SortRegexes& sort_regexes,
    const TrkSelRegexes& select_regexes) {
    std::vector<DL2TrackSequenceConfig> nodes;
    for (const auto& name_node: names) {
      DL2TrackSequenceConfig node;
      node.name = name_node.first;
      node.order = match_first(sort_regexes, name_node.first,
                               "track order matching");
      node.selection = match_first(select_regexes, name_node.first,
                                   "track selection matching");
      for (const auto& varname: name_node.second) {
        DL2TrackInputConfig input;
        input.name = varname;
        input.type = match_first(type_regexes, varname,
                                 "track type matching");
        node.inputs.push_back(input);
      }
      nodes.push_back(node);
    }
    return nodes;
  }


  // functions to rewrite input names
  std::string sub_first(const StringRegexes& res,
                        const std::string var_name) {
    for (const auto& pair: res) {
      const std::regex& re = pair.first;
      const std::string& fmt = pair.second;
      std::string new_name = std::regex_replace(
        var_name, re, fmt, std::regex_constants::format_no_copy);
      if (new_name.size() > 0) {
        return new_name;
      }
    }
    throw std::logic_error(
      "no regex match found for variable '" + var_name + "' while building "
      "negative tag b-btagger");
  }
  void rewriteFlipConfig(lwt::GraphConfig& config,
                         const StringRegexes& res){
    for (auto& node: config.inputs) {
      for (auto& var: node.variables) {
        var.name = sub_first(res, var.name);
      }
      std::map<std::string, double> new_defaults;
      for (auto& pair: node.defaults) {
        new_defaults[sub_first(res, pair.first)] = pair.second;
      }
      node.defaults = new_defaults;
    }
    std::map<std::string, lwt::OutputNodeConfig> new_outputs;
    for (auto& pair: config.outputs) {
      new_outputs[sub_first(res, pair.first)] = pair.second;
    }
    config.outputs = new_outputs;
  }

  void flipSequenceSigns(lwt::GraphConfig& config,
                         const std::regex& re) {
    for (auto& node: config.input_sequences) {
      for (auto& var: node.variables) {
        if (std::regex_match(var.name, re)) {
          var.offset *= -1.0;
          var.scale *= -1.0;
        }
      }
    }
  }

}
