#include "FlavorTagDiscriminants/DL2.h"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

// functions that are used internally
namespace {
  template <typename T>
  T match_first(const std::vector<std::pair<std::regex, T> >& regexes,
                const std::string var_name) {
    for (const auto& pair: regexes) {
      if (std::regex_match(var_name, pair.first)) {
        return pair.second;
      }
    }
    throw std::logic_error("no match found for " + var_name);
  }
}

namespace FlavorTagDiscriminants {

  // DL2
  //
  // TODO: make this work with more input nodes
  DL2::DL2(const lwt::GraphConfig& graph_config,
           const std::vector<DL2InputConfig>& inputs):
    m_input_node_name(graph_config.inputs.at(0).name),
    m_graph(new lwt::LightweightGraph(graph_config)),
    m_variable_cleaner(new lwt::NanReplacer(
                         graph_config.inputs.at(0).defaults))
  {
    // set up inputs
    if (graph_config.inputs.size() != 1) {
      throw std::logic_error("We don't currently support graphs with "
                             "more than one input");
    }
    for (const auto& input: inputs) {
      auto filler = get_filler(input.name, input.type, input.default_flag);
      m_getters.push_back(filler);
    }
    // can add other getters here
    m_getters.push_back([](const xAOD::Jet& j) -> Variable {
        return {"pt", j.pt()};});
    m_getters.push_back([](const xAOD::Jet& j) -> Variable {
        return {"abs_eta", std::abs(j.eta())};});

    for (const auto& out_node: graph_config.outputs) {
      std::string node_name = out_node.first;
      OutNode node;
      for (const std::string& element: out_node.second.labels) {
        std::string name = node_name + "_" + element;
        node.emplace_back(element, OutputDecorator(name));
      }
      m_decorators[node_name] = node;
    }
  }
  void DL2::decorate(const xAOD::Jet& jet) const {
    std::vector<Variable> vvec;
    for (const auto& getter: m_getters) {
      vvec.push_back(getter(jet));
    }
    std::map<std::string, double> variables(vvec.begin(), vvec.end());
    auto cleaned = m_variable_cleaner->replace(variables);

    // Note, you can hack in more variables to `cleaned` here.

    // put the cleaned inputs into the node structure
    std::map<std::string, std::map<std::string, double> > nodes{ {
        m_input_node_name, cleaned} };

    // save out things
    for (const auto& dec: m_decorators) {
      // the second argument to compute(...) is for sequences, we
      // don't currently have any.
      auto out_vals = m_graph->compute(nodes, {}, dec.first);
      for (const auto& node: dec.second) {
        node.second(jet) = out_vals.at(node.first);
      }
    }
  }

  std::vector<DL2InputConfig> get_input_config(
    const std::vector<std::string>& variable_names,
    const TypeRegexes& type_regexes,
    const StringRegexes& default_flag_regexes)
  {
    std::vector<DL2InputConfig> inputs;
    for (const auto& var: variable_names) {
      DL2InputConfig input;
      input.name = var;
      input.type = match_first(type_regexes, var);
      input.default_flag = match_first(default_flag_regexes, var);
      inputs.push_back(input);
    }
    return inputs;
  }

  std::function<std::pair<std::string, double>(const xAOD::Jet&)>
  get_filler(std::string name, EDMType type, std::string default_flag) {
    switch (type) {
    case EDMType::INT: return BVarGetter<int>(name, default_flag);
    case EDMType::FLOAT: return BVarGetter<float>(name, default_flag);
    case EDMType::DOUBLE: return BVarGetter<double>(name, default_flag);
    default: {
      throw std::logic_error("Unknown EDM type");
    }
    }
  }

}
