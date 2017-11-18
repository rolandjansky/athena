/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/HbbTaggerDNN.h"

#include "PathResolver/PathResolver.h"

// nn stuff
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "boost/property_tree/exceptions.hpp"

// to parse the input file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <fstream>

// internal class
class InputMapBuilder
{
public:
  InputMapBuilder(const std::string& map_file);
  typedef std::map<std::string, std::map<std::string, double> > VMap;
  VMap get_map(const xAOD::Jet&);
private:
  typedef std::vector<ElementLink<xAOD::IParticleContainer> > ParticleLinks;
  SG::AuxElement::ConstAccessor<ParticleLinks> m_acc_subjets;
  int m_n_subjets;
  std::string m_subjet_prefix;
  typedef SG::AuxElement::ConstAccessor<float> FloatAcc;
  typedef SG::AuxElement::ConstAccessor<double> DoubleAcc;
  typedef SG::AuxElement::ConstAccessor<int> IntAcc;
  std::vector<std::pair<std::string, FloatAcc> > m_acc_btag_floats;
  std::vector<std::pair<std::string, DoubleAcc> > m_acc_btag_doubles;
  std::vector<std::pair<std::string, IntAcc> > m_acc_btag_ints;
};


HbbTaggerDNN::HbbTaggerDNN( const std::string& name ) :
  asg::AsgTool(name),
  m_configFile(""),
  m_variableMapFile(""),
  m_lwnn(nullptr),
  m_input_builder(nullptr),
  m_tag_threshold(INFINITY),
  m_output_node_name(""),
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

  // setup the input builder
  ATH_MSG_INFO( "Using variable map: "<< m_variableMapFile );
  std::string var_map_file = PathResolverFindDataFile(m_variableMapFile);
  ATH_MSG_INFO( "Variable map resolved to: "<< var_map_file );
  try {
    m_input_builder.reset(new InputMapBuilder(var_map_file));
  } catch (boost::property_tree::ptree_error& err) {
    ATH_MSG_ERROR("Config file is garbage");
    return StatusCode::FAILURE;
  }

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
  if (config.outputs.size() != 1) {
    ATH_MSG_ERROR("Graph needs one output node");
    return StatusCode::FAILURE;
  }
  m_output_node_name = config.outputs.begin()->first;
  auto out_names = config.outputs.at(m_output_node_name).labels;
  if (out_names.size() != 1) {
    ATH_MSG_ERROR("Graph needs one output value");
    return StatusCode::FAILURE;
  }
  m_output_value_name = out_names.at(0);
  try {
    m_lwnn.reset(new lwt::LightweightGraph(config));
  } catch (lwt::NNConfigurationException& exc) {
    ATH_MSG_ERROR("NN configuration problem: " << exc.what());
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

    // // create input dictionary map<string,double> for argument to lwtnn
    // std::map<std::string,double> DNN_inputValues = getJetProperties(jet);

    // // evaluate the network
    // lwt::ValueMap discriminant = m_lwnn->compute(DNN_inputValues);

    // // obtain the output associated with the single output node
    double DNNscore(-666.);
    // DNNscore = discriminant[m_kerasConfigOutputName];

    return DNNscore;
}

void HbbTaggerDNN::decorate(const xAOD::Jet& jet) const {
  m_decorator(jet) = getScore(jet);
}

// Input map builder implementation
//
InputMapBuilder::InputMapBuilder(const std::string& input_file):
  m_acc_subjets("catKittyCat")
{
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(input_file, pt);
  auto sjets = pt.get<std::string>("subjet.collection");
  m_acc_subjets = SG::AuxElement::ConstAccessor<ParticleLinks>(sjets);
  m_n_subjets = pt.get<int>("subjet.number");
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

/*
std::map<std::string,double> HbbTaggerDNN::getJetProperties(const xAOD::Jet& jet) const{
    // Update the jet substructure variables for this jet
    std::map<std::string,double> DNN_inputValues;

    //mass and pT
    //it is assumed that these are the combined and calibrated mass and pT
    DNN_inputValues["CaloTACombinedMassUncorrelated"] = jet.m();
    DNN_inputValues["JetpTCorrByCombinedMass"] = jet.pt();

    // Splitting Scales
    DNN_inputValues["Split12"] = jet.getAttribute<float>("Split12");
    DNN_inputValues["Split23"] = jet.getAttribute<float>("Split23");

    // Energy Correlation Functions
    float ecf1(0.0);
    float ecf2(0.0);
    float ecf3(0.0);
    jet.getAttribute("ECF1", ecf1);
    jet.getAttribute("ECF2", ecf2);
    jet.getAttribute("ECF3", ecf3);
    if (!jet.isAvailable<float>("C2"))
        DNN_inputValues["C2"] = ecf3 * ecf1 / pow(ecf2, 2.0);
    else
        DNN_inputValues["C2"] = jet.getAttribute<float>("C2");
    if (!jet.isAvailable<float>("D2"))
        DNN_inputValues["D2"] = ecf3 * pow(ecf1, 3.0) / pow(ecf2, 3.0);
    else
        DNN_inputValues["D2"] = jet.getAttribute<float>("D2");

    // e3 := normalized ECF3/ECF1**3
    float e3(0.0);
    e3 = ecf3/pow(ecf1,3.0);
    DNN_inputValues["e3"] = e3;

    // N-subjettiness
    float tau1wta = jet.getAttribute<float>("Tau1_wta");
    float tau2wta = jet.getAttribute<float>("Tau2_wta");
    float tau3wta = jet.getAttribute<float>("Tau3_wta");
    DNN_inputValues["Tau1_wta"] = tau1wta;
    DNN_inputValues["Tau2_wta"] = tau2wta;
    DNN_inputValues["Tau3_wta"] = tau3wta;
    if (!jet.isAvailable<float>("Tau21_wta"))
        DNN_inputValues["Tau21_wta"] = tau2wta / tau1wta;
    else
        DNN_inputValues["Tau21_wta"] = jet.getAttribute<float>("Tau21_wta");
    if (!jet.isAvailable<float>("Tau32_wta"))
        DNN_inputValues["Tau32_wta"] = tau3wta/ tau2wta;
    else
        DNN_inputValues["Tau32_wta"] = jet.getAttribute<float>("Tau32_wta");

    // Qw observable for top tagging
    DNN_inputValues["Qw"] = jet.getAttribute<float>("Qw");
    DNN_inputValues["FoxWolfram20"] = jet.getAttribute<float>("FoxWolfram2") / jet.getAttribute<float>("FoxWolfram0");
    DNN_inputValues["PlanarFlow"] = jet.getAttribute<float>("PlanarFlow");
    DNN_inputValues["Angularity"] = jet.getAttribute<float>("Angularity");
    DNN_inputValues["Aplanarity"] = jet.getAttribute<float>("Aplanarity");
    DNN_inputValues["ZCut12"] = jet.getAttribute<float>("ZCut12");
    DNN_inputValues["KtDR"] = jet.getAttribute<float>("KtDR");

    return DNN_inputValues;
}

*/

