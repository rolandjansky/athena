/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/HbbTaggerDNN.h"

// utilities
#include "PathResolver/PathResolver.h"

// EDM stuff
#include <xAODJet/JetContainer.h>

// nn stuff
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/lightweight_nn_streamers.hh"

// to parse the input file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "boost/property_tree/exceptions.hpp"

#include <fstream>

// internal class
class InputMapBuilder
{
public:
  InputMapBuilder(const std::string& map_file,
                  const std::map<std::string, double>& dummies);
  typedef std::map<std::string, std::map<std::string, double> > VMap;
  VMap get_map(const xAOD::Jet&) const;
private:
  typedef ElementLink<xAOD::JetContainer> JetLink;
  SG::AuxElement::ConstAccessor<JetLink> m_acc_parent;
  typedef std::vector<ElementLink<xAOD::IParticleContainer> > ParticleLinks;
  SG::AuxElement::ConstAccessor<ParticleLinks> m_acc_subjets;
  size_t m_n_subjets;
  std::string m_jet_prefix;
  std::string m_subjet_prefix;
  typedef SG::AuxElement::ConstAccessor<float> FloatAcc;
  typedef SG::AuxElement::ConstAccessor<double> DoubleAcc;
  typedef SG::AuxElement::ConstAccessor<int> IntAcc;
  std::vector<std::pair<std::string, FloatAcc> > m_acc_btag_floats;
  std::vector<std::pair<std::string, DoubleAcc> > m_acc_btag_doubles;
  std::vector<std::pair<std::string, IntAcc> > m_acc_btag_ints;
  std::map<std::string, double> m_dummy_values;
};


HbbTaggerDNN::HbbTaggerDNN( const std::string& name ) :
  asg::AsgTool(name),
  m_configFile(""),
  m_variableMapFile(""),
  m_lwnn(nullptr),
  m_input_builder(nullptr),
  m_tag_threshold(INFINITY),
  m_output_value_name(""),
  m_decoration_name(""),
  m_decorator("")
{
  declareProperty( "ConfigFile",   m_configFile);
  declareProperty( "VariableMapFile",   m_variableMapFile);
  declareProperty( "ScoreCut", m_tag_threshold);
  declareProperty( "Decoration", m_decoration_name);
}

HbbTaggerDNN::~HbbTaggerDNN() {}

StatusCode HbbTaggerDNN::initialize(){

  // Initialize the DNN tagger tool
  ATH_MSG_INFO("Initializing HbbTaggerDNN tool");

  // read json file for DNN weights
  ATH_MSG_INFO("Using NN file: "+ m_configFile);
  std::string configPath = PathResolverFindCalibFile(m_configFile);
  ATH_MSG_INFO("NN file resolved to: " + configPath);
  lwt::GraphConfig config;
  try {
    std::ifstream input_cfg( configPath );
    config = lwt::parse_json_graph(input_cfg);
  } catch (boost::property_tree::ptree_error& err) {
    ATH_MSG_ERROR("NN file is garbage");
    return StatusCode::FAILURE;
  }
  std::map<std::string, double> dummy_inputs;
  ATH_MSG_DEBUG("Hbb inputs:");
  for (auto& input_node: config.inputs) {
    ATH_MSG_DEBUG(" input node: " << input_node.name);
    for (auto& input: input_node.variables) {
      bool dummy = false;
      if (input.scale == 0.0) {
        dummy_inputs[input.name] = input.offset;
        dummy = true;
      }
      ATH_MSG_DEBUG("  " << input << (dummy ? " <-- DUMMY": "") );
    }
  }
  if (config.outputs.size() != 1) {
    ATH_MSG_ERROR("Graph needs one output node");
    return StatusCode::FAILURE;
  }
  auto output_node_name = config.outputs.begin()->first;
  auto out_names = config.outputs.at(output_node_name).labels;
  if (out_names.size() != 1) {
    ATH_MSG_ERROR("Graph needs one output value");
    return StatusCode::FAILURE;
  }
  m_output_value_name = out_names.at(0);
  try {
    m_lwnn.reset(new lwt::LightweightGraph(config, output_node_name));
  } catch (lwt::NNConfigurationException& exc) {
    ATH_MSG_ERROR("NN configuration problem: " << exc.what());
    return StatusCode::FAILURE;
  }

  // setup the input builder
  ATH_MSG_INFO( "Using variable map: "<< m_variableMapFile );
  std::string var_map_file = PathResolverFindDataFile(m_variableMapFile);
  ATH_MSG_INFO( "Variable map resolved to: "<< var_map_file );
  try {
    m_input_builder.reset(new InputMapBuilder(var_map_file, dummy_inputs));
  } catch (boost::property_tree::ptree_error& err) {
    ATH_MSG_ERROR("Config file is garbage: " << err.what());
    return StatusCode::FAILURE;
  }

  // set up the output decorators
  if (m_decoration_name.size() == 0) {
    m_decoration_name = m_output_value_name;
  }
  m_decorator = SG::AuxElement::Decorator<float>(m_decoration_name);

  return StatusCode::SUCCESS;
}

StatusCode HbbTaggerDNN::finalize(){
  return StatusCode::SUCCESS;
}

int HbbTaggerDNN::keep(const xAOD::Jet& jet) const {
  return getScore(jet) > m_tag_threshold;
}


double HbbTaggerDNN::getScore(const xAOD::Jet& jet) const {

  // build the jet properties into a map
  InputMapBuilder::VMap inputs = m_input_builder->get_map(jet);
  if (!inputs.count("subjet2_mask")) return -INFINITY;
  auto nn_output = m_lwnn->compute(inputs);
  return nn_output.at(m_output_value_name);
}

void HbbTaggerDNN::decorate(const xAOD::Jet& jet) const {
  m_decorator(jet) = getScore(jet);
}

// Input map builder implementation
//
InputMapBuilder::InputMapBuilder(
  const std::string& input_file,
  const std::map<std::string,double>& dummy_inputs):
  m_acc_parent("Parent"),
  m_acc_subjets("catKittyCat"),
  m_dummy_values(dummy_inputs)
{
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(input_file, pt);
  auto sjets = pt.get<std::string>("subjet.collection");
  m_acc_subjets = SG::AuxElement::ConstAccessor<ParticleLinks>(sjets);
  m_n_subjets = pt.get<int>("subjet.number");
  m_jet_prefix = pt.get<std::string>("jet.prefix");
  m_subjet_prefix = pt.get<std::string>("subjet.prefix");
  for (auto& var: pt.get_child("btag.floats")) {
    std::string name = var.second.data();
    m_acc_btag_floats.emplace_back(name, name);
  }
  for (auto& var: pt.get_child("btag.doubles")) {
    std::string name = var.second.data();
    m_acc_btag_doubles.emplace_back(name, name);
  }
  for (auto& var: pt.get_child("btag.ints")) {
    std::string name = var.second.data();
    m_acc_btag_ints.emplace_back(name, name);
  }
}

InputMapBuilder::VMap InputMapBuilder::get_map(const xAOD::Jet& jet) const {
  std::map<std::string, double> nn_inputs;
  nn_inputs[m_jet_prefix + "pt"] = jet.pt();
  nn_inputs[m_jet_prefix + "eta"] = jet.eta();
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
  size_t n_subjets = std::min(subjets.size(), m_n_subjets);
  for (size_t subjet_n = 0; subjet_n < n_subjets; subjet_n++) {
    std::string prefix = m_subjet_prefix + std::to_string(subjet_n + 1) + "_";
    const xAOD::Jet* subjet = subjets.at(subjet_n);
    nn_inputs[prefix + "pt"] = subjet->pt();
    nn_inputs[prefix + "eta"] = subjet->eta();
    const auto& btag = subjet->btagging();
    for (const auto& pair: m_acc_btag_floats) {
      nn_inputs[prefix + pair.first] = pair.second(*btag);
    }
    for (const auto& pair: m_acc_btag_doubles) {
      nn_inputs[prefix + pair.first] = pair.second(*btag);
    }
    for (const auto& pair: m_acc_btag_ints) {
      nn_inputs[prefix + pair.first] = pair.second(*btag);
    }
    nn_inputs[prefix + "mask"] = 1;
  }
  for (const auto dummy: m_dummy_values) {
    if (nn_inputs.count(dummy.first)) {
      throw std::logic_error(dummy.first + " was redefined");
    }
    nn_inputs[dummy.first] = dummy.second;
  }
  return {{"input_node", nn_inputs}};
}

