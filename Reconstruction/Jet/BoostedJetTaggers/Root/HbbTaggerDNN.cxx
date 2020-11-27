/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// nn stuff
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/lightweight_nn_streamers.hh"
#include "lwtnn/NanReplacer.hh"

#include "BoostedJetTaggers/HbbTaggerDNN.h"

// utilities
#include "PathResolver/PathResolver.h"

// EDM stuff
#include <xAODJet/JetContainer.h>

// to parse the input file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "boost/property_tree/exceptions.hpp"

#include <fstream>
#include <regex>
#include <set>


namespace BoostedJetTaggers {
// internal class to read a jet and build an std::map of intputs that
// will be passed to lwtnn.
  struct SubstructureAccessors;

  class HbbInputBuilder
  {
  public:
    HbbInputBuilder(const std::string& config_file,
                    const lwt::GraphConfig& network_config);
    typedef std::map<std::string, std::map<std::string, double> > VMap;
    VMap get_map(const xAOD::Jet&) const;
    size_t n_subjets(const xAOD::Jet&) const;
  private:
    typedef ElementLink<xAOD::JetContainer> JetLink;
    SG::AuxElement::ConstAccessor<JetLink> m_acc_parent;
    typedef std::vector<ElementLink<xAOD::IParticleContainer> > ParticleLinks;
    SG::AuxElement::ConstAccessor<ParticleLinks> m_acc_subjets;
    double m_subjet_pt_threshold;
    std::string m_fat_jet_node_name;
    std::vector<std::string> m_subjet_node_names;
    typedef SG::AuxElement::ConstAccessor<float> FloatAcc;
    typedef SG::AuxElement::ConstAccessor<double> DoubleAcc;
    typedef SG::AuxElement::ConstAccessor<int> IntAcc;
    std::vector<std::pair<std::string, FloatAcc> > m_acc_btag_floats;
    std::vector<std::pair<std::string, DoubleAcc> > m_acc_btag_doubles;
    std::vector<std::pair<std::string, IntAcc> > m_acc_btag_ints;
    std::vector<std::string> m_float_subjet_inputs;
    std::vector<std::string> m_int_subjet_inputs;
    std::map<std::string, double> m_dummy_values;

    // grab substructure info
    std::unique_ptr<SubstructureAccessors> m_ssa;
  };

  // set of accessors for JSS input variables
  struct SubstructureAccessors {
    typedef SG::AuxElement AE;
    SubstructureAccessors();
    std::map<std::string, double> get_map(const xAOD::Jet&) const;
  private:
    float c2(const xAOD::Jet&) const;
    float d2(const xAOD::Jet&) const;
    float e3(const xAOD::Jet&) const;
    float tau21(const xAOD::Jet&) const;
    float tau32(const xAOD::Jet&) const;
    float fw20(const xAOD::Jet&) const;

    AE::ConstAccessor<float> ecf1;
    AE::ConstAccessor<float> ecf2;
    AE::ConstAccessor<float> ecf3;

    AE::ConstAccessor<float> tau1wta;
    AE::ConstAccessor<float> tau2wta;
    AE::ConstAccessor<float> tau3wta;

    AE::ConstAccessor<float> fw2;
    AE::ConstAccessor<float> fw0;

    // other accessors used by get_map
    std::vector<std::pair<std::string, AE::ConstAccessor<float> > > m_acc;
  };

}

namespace {
  // constants, default configuration files
  const std::string NN_CONFIG = (
    "BoostedJetTaggers/HbbTaggerDNN/PreliminaryConfigNovember2017.json");
}


HbbTaggerDNN::HbbTaggerDNN( const std::string& name ) :
  asg::AsgTool(name),
  m_neuralNetworkFile(""),
  m_configurationFile(NN_CONFIG),
  m_lwnn(nullptr),
  m_input_builder(nullptr),
  m_tag_threshold(1000000000.),
  m_output_value_names(),
  m_decoration_names(),
  m_decorators()
{
  declareProperty( "neuralNetworkFile",   m_neuralNetworkFile);
  declareProperty( "configurationFile",   m_configurationFile);
  declareProperty( "tagThreshold", m_tag_threshold);
  declareProperty( "decorationNames", m_decoration_names);
}

HbbTaggerDNN::~HbbTaggerDNN() {}

StatusCode HbbTaggerDNN::initialize(){

  using namespace BoostedJetTaggers;

  ATH_MSG_INFO("Initializing HbbTaggerDNN tool");

  // read json file for DNN weights
  ATH_MSG_INFO("Using NN file: "+ m_neuralNetworkFile);
  std::string nn_path = PathResolverFindCalibFile(m_neuralNetworkFile);
  ATH_MSG_INFO("NN file resolved to: " + nn_path);
  lwt::GraphConfig config;
  try {
    std::ifstream input_cfg( nn_path );
    config = lwt::parse_json_graph(input_cfg);
  } catch (boost::property_tree::ptree_error& err) {
    ATH_MSG_ERROR("NN file is garbage");
    return StatusCode::FAILURE;
  }

  // setup inputs
  ATH_MSG_DEBUG("Hbb inputs:");
  for (auto& input_node: config.inputs) {
    ATH_MSG_DEBUG(" input node: " << input_node.name);
    for (auto& input: input_node.variables) {
      ATH_MSG_DEBUG("  " << input);
    }
  }
  for (const auto& node: config.inputs) {
    m_var_cleaners.emplace_back(
      node.name, std::make_unique<lwt::NanReplacer>(node.defaults, lwt::rep::all));
  }

  // setup outputs
  if (config.outputs.size() != 1) {
    ATH_MSG_ERROR("Graph needs one output node");
    return StatusCode::FAILURE;
  }
  auto output_node_name = config.outputs.begin()->first;
  auto out_names = config.outputs.at(output_node_name).labels;
  m_output_value_names = out_names;

  // build the network
  try {
    m_lwnn.reset(new lwt::LightweightGraph(config, output_node_name));
  } catch (lwt::NNConfigurationException& exc) {
    ATH_MSG_ERROR("NN configuration problem: " << exc.what());
    return StatusCode::FAILURE;
  }

  // setup the input builder
  ATH_MSG_INFO( "Using config file: "<< m_configurationFile );
  std::string config_file = PathResolverFindDataFile(m_configurationFile);
  ATH_MSG_INFO( "Config file resolved to: "<< config_file );
  try {
    m_input_builder.reset(new HbbInputBuilder(config_file, config));
  } catch (boost::property_tree::ptree_error& err) {
    ATH_MSG_ERROR("Config file is garbage: " << err.what());
    return StatusCode::FAILURE;
  }

  // set up the output decorators
  //
  // if there aren't any decorators given in the constructor we look
  // in the config file
  if (m_decoration_names.size() > 0) {
    ATH_MSG_INFO("Output names set in tool setup");
  } else {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(config_file, pt);
    if (pt.count("outputs") > 0) {
      for (const auto& pair: pt.get_child("outputs.decoration_map") ) {
        m_decoration_names[pair.first] = pair.second.data();
      }
      ATH_MSG_INFO("Output names read from config file");
    } else {
      ATH_MSG_INFO("Output names read from NN file ");
    }
  }
  // now build the decoration vectors
  for (const std::string& name: out_names) {
    std::string dec_name = name;
    // if we've given custom decoration names, insist that this output
    // has one
    if (m_decoration_names.size() > 0) {
      if (!m_decoration_names.count(name)) {
        ATH_MSG_ERROR("NN output " + name + " has no decoration name");
        return StatusCode::FAILURE;
      }
      dec_name = m_decoration_names.at(name);
    }
    ATH_MSG_DEBUG("Adding output " + dec_name);
    m_decorators.emplace_back(SG::AuxElement::Decorator<float>(dec_name));
  }

  return StatusCode::SUCCESS;
}

int HbbTaggerDNN::keep(const xAOD::Jet& jet) const {
  return getScore(jet) > m_tag_threshold;
}

std::map<std::string, double> HbbTaggerDNN::getScores(const xAOD::Jet& jet)
  const {

  using namespace BoostedJetTaggers;

  // build the jet properties into a map
  HbbInputBuilder::VMap inputs = m_input_builder->get_map(jet);

  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("Hbb inputs:");
    for (auto& input_node: inputs) {
      ATH_MSG_VERBOSE(" input node: " << input_node.first);
      for (auto& input: input_node.second) {
        ATH_MSG_VERBOSE("  " << input.first << ": " << input.second);
      }
    }
  }

  // if we have any NaN or infinite values, replace them with defaults
  HbbInputBuilder::VMap cleaned;
  for (const auto& cleaner: m_var_cleaners) {
    cleaned.emplace(
      cleaner.first, cleaner.second->replace(inputs.at(cleaner.first)));
  }

  auto nn_output = m_lwnn->compute(cleaned);
  return nn_output;
}

double HbbTaggerDNN::getScore(const xAOD::Jet& jet) const {
  if (m_output_value_names.size() > 1) {
    throw std::logic_error(
      "asked for a single tagger score from a multi-class tagger");
  }
  auto nn_output = getScores(jet);
  ATH_MSG_VERBOSE("Hbb score " << nn_output.at(m_output_value_names.at(0)));
  return nn_output.at(m_output_value_names.at(0));
}

void HbbTaggerDNN::decorate(const xAOD::Jet& jet) const {
  std::map<std::string, double> scores = getScores(jet);
  size_t dec_num = 0;
  for (const auto& dec: m_decorators) {
    dec(jet) = scores.at(m_output_value_names.at(dec_num));
    dec_num++;
  }
}
void HbbTaggerDNN::decorateSecond(const xAOD::Jet& ref,
                                  const xAOD::Jet& target) const {
  std::map<std::string, double> scores = getScores(ref);
  size_t dec_num = 0;
  for (const auto& dec: m_decorators) {
    dec(target) = scores.at(m_output_value_names.at(dec_num));
    dec_num++;
  }
}

std::set<std::string> HbbTaggerDNN::decorationNames() const {
  std::set<std::string> out;
  for (const auto& pair: m_decoration_names) {
    out.insert(pair.second);
  }
  return out;
}

size_t HbbTaggerDNN::n_subjets(const xAOD::Jet& jet) const {
  return m_input_builder->n_subjets(jet);
}

namespace {

  // Some things can't be accessed by name from the EDM. This includes
  // things which are part of a lorentz vector and derived things like
  // deltaR. Since we build the list things to read from the EDM from
  // the NN configuration we have to specify which ones are skipped.
  static const std::set<std::string> NON_STRING_ACCESSOR{
    "pt", "eta", "mass", "deta", "dphi", "dr"
  };

  // For things that we do access from the EDM, we need to know the
  // type. We use regexes to assign these.
  typedef std::vector<std::pair<std::regex, std::string> > TypeRegexes;
  std::string get_var_type(const TypeRegexes& type_regexes,
                           const std::string& var_name) {
    for (const auto& type_regex: type_regexes) {
      if (std::regex_search(var_name, type_regex.first)) {
        return type_regex.second;
      }
    }
    throw std::logic_error(
      "can't find a type match for variable " + var_name);
  }

}

namespace BoostedJetTaggers {

  ////////////////////////////////////////
  /// Input map builder implementation ///
  ////////////////////////////////////////

  HbbInputBuilder::HbbInputBuilder(
    const std::string& input_file,
    const lwt::GraphConfig& network_config):
    m_acc_parent("Parent"),
    m_acc_subjets("catKittyCat"),
    m_ssa(nullptr)
  {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(input_file, pt);

    m_fat_jet_node_name = pt.get<std::string>("fatjet.node_name");

    auto sjets = pt.get<std::string>("subjet.collection");
    m_subjet_pt_threshold = pt.get<double>("subjet.pt_threshold");
    m_acc_subjets = SG::AuxElement::ConstAccessor<ParticleLinks>(sjets);

    // build regexes to figure out variable types
    std::vector<std::pair<std::regex, std::string> > type_regexes;
    for (auto& regex_spec: pt.get_child("types.regexes")) {
      std::string regex_str = regex_spec.second.get<std::string>("regex");
      std::string type = regex_spec.second.get<std::string>("type");
      std::regex regex(regex_str);
      type_regexes.emplace_back(regex, type);
    }
    // build input accessors
    std::set<std::string> valid_subjet_nodes;
    for (auto& subjet_node_name: pt.get_child("subjet.node_names")) {
      valid_subjet_nodes.insert(subjet_node_name.second.data());
    }
    std::set<std::string> float_subjet_inputs;
    std::set<std::string> int_subjet_inputs;
    for (const lwt::InputNodeConfig& node_config: network_config.inputs) {
      if (node_config.name == m_fat_jet_node_name) {
        m_ssa.reset(new SubstructureAccessors);
      } else if (valid_subjet_nodes.count(node_config.name)) {
        for (const lwt::Input input: node_config.variables) {
          if (NON_STRING_ACCESSOR.count(input.name)) {
            continue;
            // todo: other jet variables
          }
          std::string type = get_var_type(type_regexes, input.name);
          if (type == "double") {
            float_subjet_inputs.insert(input.name);
            m_acc_btag_doubles.emplace_back(input.name, input.name);
          } else if (type == "int") {
            m_acc_btag_ints.emplace_back(input.name, input.name);
            int_subjet_inputs.insert(input.name);
          } else if (type == "float") {
            m_acc_btag_floats.emplace_back(input.name, input.name);
            float_subjet_inputs.insert(input.name);
          } else {
            throw std::logic_error("don't know how to access " + input.name);
          }
        }
        m_subjet_node_names.push_back(node_config.name);
      } else {
        throw std::logic_error(
          "not sure how to find node " + node_config.name);
      }
    }

    // In cases where inputs aren't defined, we need to fill them with
    // dummy values. Keep track of all the values that need defaults
    // here.
    m_float_subjet_inputs.insert(
      m_float_subjet_inputs.end(),
      float_subjet_inputs.begin(), float_subjet_inputs.end());
    m_float_subjet_inputs.insert(
      m_float_subjet_inputs.end(),
      NON_STRING_ACCESSOR.begin(), NON_STRING_ACCESSOR.end());
    m_int_subjet_inputs.insert(
      m_int_subjet_inputs.end(),
      int_subjet_inputs.begin(), int_subjet_inputs.end());
  }

  HbbInputBuilder::VMap HbbInputBuilder::get_map(
    const xAOD::Jet& jet) const {

    // inputs dict
    std::map<std::string, std::map<std::string, double> > inputs;

    // fat jet inputs
    if (m_ssa) inputs[m_fat_jet_node_name] = m_ssa->get_map(jet);

    // get subjets
    const xAOD::Jet* parent_jet = *m_acc_parent(jet);
    if (!parent_jet) throw std::logic_error("no valid parent");
    auto subjet_links = m_acc_subjets(*parent_jet);
    std::vector<const xAOD::Jet*> subjets;
    for (const auto& el: subjet_links) {
      const auto* jet = dynamic_cast<const xAOD::Jet*>(*el);
      if (!jet) throw std::logic_error("subjet is invalid");
      subjets.push_back(jet);
    }
    auto pt_sort = [](auto& sj1, auto& sj2 ){
      return sj1->pt() > sj2->pt();
    };
    std::sort(subjets.begin(), subjets.end(), pt_sort);
    size_t n_subjets = m_subjet_node_names.size();
    for (size_t subjet_n = 0; subjet_n < n_subjets; subjet_n++) {
      bool have_jet = subjet_n < subjets.size();
      std::map<std::string, double> subjet_inputs;
      if (have_jet && subjets.at(subjet_n)->pt() > m_subjet_pt_threshold) {
        const xAOD::Jet* subjet = subjets.at(subjet_n);
        subjet_inputs["pt"] = subjet->pt();
        subjet_inputs["eta"] = subjet->eta();
        subjet_inputs["deta"] = subjet->eta() - jet.eta();
        subjet_inputs["dphi"] = subjet->p4().DeltaPhi(jet.p4());
        subjet_inputs["dr"] = subjet->p4().DeltaR(jet.p4());
        const auto* btag = subjet->btagging();
        for (const auto& pair: m_acc_btag_floats) {
          subjet_inputs[pair.first] = pair.second(*btag);
        }
        for (const auto& pair: m_acc_btag_doubles) {
          subjet_inputs[pair.first] = pair.second(*btag);
        }
        for (const auto& pair: m_acc_btag_ints) {
          subjet_inputs[pair.first] = pair.second(*btag);
        }
      } else {
        for (const std::string& input_name: m_float_subjet_inputs) {
          subjet_inputs[input_name] = NAN;
        }
        for (const std::string& input_name: m_int_subjet_inputs) {
          subjet_inputs[input_name] = -1;
        }
      }
      inputs[m_subjet_node_names.at(subjet_n)] = subjet_inputs;
    }

    return inputs;
  }

  size_t HbbInputBuilder::n_subjets(const xAOD::Jet& jet) const {
    const xAOD::Jet* parent_jet = *m_acc_parent(jet);
    if (!parent_jet) throw std::logic_error("no valid parent");
    auto subjet_links = m_acc_subjets(*parent_jet);
    return subjet_links.size();
  }


  // definitions for substructure accessors
  SubstructureAccessors::SubstructureAccessors():
    ecf1("ECF1"), ecf2("ECF2"), ecf3("ECF3"),
    tau1wta("Tau1_wta"), tau2wta("Tau2_wta"), tau3wta("Tau3_wta"),
    fw2("FoxWolfram2"), fw0("FoxWolfram0")
  {
    for (const std::string& name: {"Split12", "Split23",
          "Qw", "PlanarFlow", "Angularity", "Aplanarity", "ZCut12", "KtDR"}) {
      m_acc.emplace_back(std::make_pair(name, name));
    }
  }
  std::map<std::string, double>
  SubstructureAccessors::get_map(const xAOD::Jet& j) const {
    std::map<std::string, double> map {
      {"pt", j.pt()}, {"eta", j.eta()}, {"mass", j.m()},
      {"C2", c2(j)},
      {"D2", d2(j)},
      {"e3", e3(j)},
      {"Tau21_wta", tau21(j)},
      {"Tau32_wta", tau32(j)},
      {"FoxWolfram20", fw20(j)},
    };
    for (const auto& pair: m_acc) {
      map[pair.first] = pair.second(j);
    }
    return map;
  }

  float SubstructureAccessors::c2(const xAOD::Jet& j) const {
    // ECF functions return zero if there N < numConstituents,
    // need this check to prevent div by zero
    //
    if (j.numConstituents() < 2) return NAN;
    return ecf3(j) * ecf1(j) / pow(ecf2(j), 2.0);
  }
  float SubstructureAccessors::d2(const xAOD::Jet& j) const {
    // See comment for C2
    //
    if (j.numConstituents() < 2) return NAN;
    return ecf3(j) * pow(ecf1(j), 3.0) / pow(ecf2(j), 3.0);
  }
  float SubstructureAccessors::e3(const xAOD::Jet& j) const {
    // ECF1 should always be nonzero
    //
    return ecf3(j)/pow(ecf1(j),3.0);
  }
  float SubstructureAccessors::tau21(const xAOD::Jet& j) const {
    // Tau variables return zero when N < numConstituents. Here we
    // should be safe since we should always have one constituent.
    //
    return tau2wta(j) / tau1wta(j);
  }
  float SubstructureAccessors::tau32(const xAOD::Jet& j) const {
    // Tau variables return zero when N < numConstituents, need this
    // check to prevent div by zero
    //
    if (j.numConstituents() < 2) return NAN;
    // Can also be zero under special circumstances with two constituents
    // Just explicitly check for that
    return tau2wta(j)!=0.?tau3wta(j) / tau2wta(j):0.;
  }
  float SubstructureAccessors::fw20(const xAOD::Jet& j) const {
    // Fox Wolfram variables aren't defined for numConstituents < 1,
    // but we also have the problem of FW0 being zero if
    // numConstituents is equal to 1, see here:
    //
    // https://gitlab.cern.ch/atlas/athena/blob/21.2/Reconstruction/Jet/JetSubStructureUtils/Root/FoxWolfram.cxx
    //
    if (j.numConstituents() < 2) return NAN;
    return fw2(j) / fw0(j);
  }


}
