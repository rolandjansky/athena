/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// nn stuff
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/lightweight_nn_streamers.hh"
#include "lwtnn/NanReplacer.hh"

#include "BoostedJetTaggers/MassDecoXbbTagger.h"

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


namespace MassDecoXbb {
// internal class to read a jet and build an std::map of intputs that
// will be passed to lwtnn.
  class MassDecoXbbInputBuilder
  {
  public:
    MassDecoXbbInputBuilder(const std::string& config_file,
                    const std::map<std::string, double>& offsets);
    typedef std::map<std::string, std::map<std::string, double> > VMap;
    VMap get_map(const xAOD::Jet&) const;
    size_t n_subjets(const xAOD::Jet&) const;
  private:
    typedef ElementLink<xAOD::JetContainer> JetLink;
    SG::AuxElement::ConstAccessor<JetLink> m_acc_parent;
    typedef std::vector<ElementLink<xAOD::IParticleContainer> > ParticleLinks;
    SG::AuxElement::ConstAccessor<ParticleLinks> m_acc_subjets;
    double m_subjet_pt_threshold;
    std::string m_node_name;
    typedef SG::AuxElement::ConstAccessor<float> FloatAcc;
    typedef SG::AuxElement::ConstAccessor<double> DoubleAcc;
    typedef SG::AuxElement::ConstAccessor<int> IntAcc;
    std::vector<std::pair<std::string, FloatAcc> > m_acc_btag_floats;
    std::vector<std::pair<std::string, DoubleAcc> > m_acc_btag_doubles;
    std::vector<std::pair<std::string, IntAcc> > m_acc_btag_ints;
    std::vector<std::string> m_float_subjet_inputs;
    std::vector<std::string> m_int_subjet_inputs;
    std::map<std::string, double> m_dummy_values;
    std::map<std::string, double> m_offsets;
  };

}

namespace {
  // constants, default configuration files
  const std::string NN_CONFIG = (
    "BoostedJetTaggers/MassDecoXbbTagger/test_config.json");
}


MassDecoXbbTagger::MassDecoXbbTagger( const std::string& name ) :
  asg::AsgTool(name),
  m_neuralNetworkFile(""),
  m_configurationFile(NN_CONFIG),
  m_lwnn(nullptr),
  m_input_builder(nullptr),
  m_tag_threshold(1000000000.),
  m_output_value_names(),
  m_decoration_names(),
  m_decorators(),
  m_offsets()
{
  declareProperty( "neuralNetworkFile",   m_neuralNetworkFile);
  declareProperty( "configurationFile",   m_configurationFile);
  declareProperty( "tagThreshold", m_tag_threshold);
  declareProperty( "decorationNames", m_decoration_names);
}

MassDecoXbbTagger::~MassDecoXbbTagger() {}

StatusCode MassDecoXbbTagger::initialize(){

  using namespace MassDecoXbb;

  // Initialize the DNN tagger tool
  ATH_MSG_INFO("Initializing MassDecoXbbTagger tool");

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
  ATH_MSG_DEBUG("Mass decorrelated Xbb inputs:");
  for (auto& input_node: config.inputs) {
    ATH_MSG_DEBUG(" input node: " << input_node.name);
    for (auto& input: input_node.variables) {
      ATH_MSG_DEBUG("  " << input);
      m_offsets[input.name] = -input.offset;
    }
  }
  for (const auto& node: config.inputs) {
    m_var_cleaners.emplace_back(
      node.name, std::make_unique<lwt::NanReplacer>(m_offsets, lwt::rep::all));
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
    m_input_builder.reset(new MassDecoXbbInputBuilder(config_file, m_offsets));
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
        //return StatusCode::FAILURE;
      }
      dec_name = m_decoration_names.at(name);
    }
    ATH_MSG_DEBUG("Adding output " + dec_name);
    m_decorators.emplace_back(SG::AuxElement::Decorator<float>(dec_name));
  }

  return StatusCode::SUCCESS;
}

int MassDecoXbbTagger::keep(const xAOD::Jet& jet) const {
  return getScore(jet) > m_tag_threshold;
}


std::map<std::string, double> MassDecoXbbTagger::getScores(const xAOD::Jet& jet)
  const {

  using namespace MassDecoXbb;

  // build the jet properties into a map
  MassDecoXbbInputBuilder::VMap inputs = m_input_builder->get_map(jet);

  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("Mass decorrelated Xbb inputs:");
    for (auto& input_node: inputs) {
      ATH_MSG_VERBOSE(" input node: " << input_node.first);
      for (auto& input: input_node.second) {
        ATH_MSG_VERBOSE("  " << input.first << ": " << input.second);
      }
    }
  }

  // if we have any NaN or infinite values, replace them with defaults
  MassDecoXbbInputBuilder::VMap cleaned;
  for (const auto& cleaner: m_var_cleaners) {
    cleaned.emplace(
      cleaner.first, cleaner.second->replace(inputs.at(cleaner.first)));
  }
    
  auto nn_output = m_lwnn->compute(cleaned);
  ATH_MSG_VERBOSE("Mass decorrelated Xbb QCD score " << nn_output.at(m_output_value_names.at(0)));
  ATH_MSG_VERBOSE("Mass decorrelated Xbb Higgs score " << nn_output.at(m_output_value_names.at(1)));
  ATH_MSG_VERBOSE("Mass decorrelated Xbb Top score " << nn_output.at(m_output_value_names.at(2)));
  return nn_output;
}

double MassDecoXbbTagger::getScore(const xAOD::Jet& jet) const {
  if (m_output_value_names.size() > 1) {
    ATH_MSG_DEBUG("asked for the first tagger score from a multi-class tagger");
  }
  auto nn_output = getScores(jet);
  return nn_output.at(m_output_value_names.at(0));
}

void MassDecoXbbTagger::decorate(const xAOD::Jet& jet) const {
  std::map<std::string, double> scores = getScores(jet);
  size_t dec_num = 0;
  for (const auto& dec: m_decorators) {
    dec(jet) = scores.at(m_output_value_names.at(dec_num));
    dec_num++;
  }
}
void MassDecoXbbTagger::decorateSecond(const xAOD::Jet& ref,
                                  const xAOD::Jet& target) const {
  std::map<std::string, double> scores = getScores(ref);
  size_t dec_num = 0;
  for (const auto& dec: m_decorators) {
    dec(target) = scores.at(m_output_value_names.at(dec_num));
    dec_num++;
  }
}

std::set<std::string> MassDecoXbbTagger::decorationNames() const {
  std::set<std::string> out;
  for (const auto& pair: m_decoration_names) {
    out.insert(pair.second);
  }
  return out;
}

size_t MassDecoXbbTagger::n_subjets(const xAOD::Jet& jet) const {
  return m_input_builder->n_subjets(jet);
}

namespace {

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

namespace MassDecoXbb {

  ////////////////////////////////////////
  /// Input map builder implementation ///
  ////////////////////////////////////////

  MassDecoXbbInputBuilder::MassDecoXbbInputBuilder(
    const std::string& input_file,
    const std::map<std::string, double>& offsets):
    m_acc_parent("Parent"),
    m_acc_subjets("catKittyCat"),
    m_offsets(offsets)
  {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(input_file, pt);

    m_node_name = pt.get<std::string>("node_name");

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

    std::vector<std::string> subjet_var;
    for (auto& var: pt.get_child("subjet.variables")) {
       subjet_var.emplace_back(var.second.data());
    }

    // build input accessors
    std::set<std::string> float_subjet_inputs;
    std::set<std::string> int_subjet_inputs;
    for (const std::string& var_name: subjet_var)  { 
          std::string type = get_var_type(type_regexes, var_name);
          if (type == "double") {
            m_acc_btag_doubles.emplace_back(var_name, var_name);
            float_subjet_inputs.insert(var_name);
          } else if (type == "int") {
            m_acc_btag_ints.emplace_back(var_name, var_name);
            int_subjet_inputs.insert(var_name);
          } else if (type == "float") {
            m_acc_btag_floats.emplace_back(var_name, var_name);
            float_subjet_inputs.insert(var_name);
          } else {
            throw std::logic_error("don't know how to access " + var_name);
          }
      } 

    // In cases where inputs aren't defined, we need to fill them with
    // dummy values. Keep track of all the values that need defaults
    // here.
    m_float_subjet_inputs.insert(
      m_float_subjet_inputs.end(),
      float_subjet_inputs.begin(), float_subjet_inputs.end());
    m_int_subjet_inputs.insert(
      m_int_subjet_inputs.end(),
      int_subjet_inputs.begin(), int_subjet_inputs.end());
  }

  MassDecoXbbInputBuilder::VMap MassDecoXbbInputBuilder::get_map(
    const xAOD::Jet& jet) const {

    // inputs dict
    std::map<std::string, std::map<std::string, double> > node_input;
    std::map<std::string, double> inputs;

    // fat jet inputs
    inputs["pt"] = jet.pt()/1000.0;  // The pt of fat jet is in GeV in training
    inputs["eta"] = jet.eta();

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
    for (size_t subjet_n = 0; subjet_n < 3; subjet_n++) {
      bool have_jet = subjet_n < subjets.size();
      std::map<std::string, double> subjet_inputs;
      std::string order = "_" + std::to_string(subjet_n+1);
      if (have_jet && subjets.at(subjet_n)->pt() > m_subjet_pt_threshold) {
        const xAOD::Jet* subjet = subjets.at(subjet_n);
        const auto* btag = subjet->btagging();
        for (const auto& pair: m_acc_btag_floats) {
          inputs[pair.first + order] = pair.second(*btag);
        }
        for (const auto& pair: m_acc_btag_doubles) {
          inputs[pair.first + order] = pair.second(*btag);
        }
        for (const auto& pair: m_acc_btag_ints) {
          inputs[pair.first + order] = pair.second(*btag);
        }
      } else {
        for (const std::string& input_name: m_float_subjet_inputs) {
          inputs[input_name + order] = m_offsets.at(input_name + order);
        }
        for (const std::string& input_name: m_int_subjet_inputs) {
          inputs[input_name + order] = m_offsets.at(input_name + order);
        }
      }
    }
    
    node_input[m_node_name] = inputs;

    return node_input;
  }

  size_t MassDecoXbbInputBuilder::n_subjets(const xAOD::Jet& jet) const {
    const xAOD::Jet* parent_jet = *m_acc_parent(jet);
    if (!parent_jet) throw std::logic_error("no valid parent");
    auto subjet_links = m_acc_subjets(*parent_jet);
    return subjet_links.size();
  }


}
