/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/HbbTaggerDNN.h"

#include "PathResolver/PathResolver.h"

#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "boost/property_tree/exceptions.hpp"

#include <fstream>

namespace {
  const static std::string APP_NAME = "HbbTaggerDNN";
}

HbbTaggerDNN::HbbTaggerDNN( const std::string& name ) :
  asg::AsgTool(name),
  m_configFile(""),
  m_lwnn(nullptr),
  m_tag_threshold(INFINITY),
  m_output_node_name(""),
  m_output_value_name(""),
  m_decoration_name(""),
  m_decorator("")
{
  declareProperty( "ConfigFile",   m_configFile);
  declareProperty( "ScoreCut", m_tag_threshold);
  declareProperty( "Decoration", m_decoration_name);
}

HbbTaggerDNN::~HbbTaggerDNN() {}

StatusCode HbbTaggerDNN::initialize(){

  /* Initialize the DNN tagger tool */
  ATH_MSG_INFO(APP_NAME+": Initializing HbbTaggerDNN tool");
  ATH_MSG_INFO(APP_NAME+": Using config file :"+m_configFile);

  // find the config file
  if( m_configFile.empty() ) {
    ATH_MSG_ERROR( "No config file provided" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Using config file : "<< m_configFile );
  std::string configPath;
  configPath = PathResolverFindCalibFile(m_configFile.c_str());

  // read json file for DNN weights
  ATH_MSG_INFO(APP_NAME + ": DNN Tagger configured with: " + configPath);
  std::ifstream input_cfg( configPath );
  if(input_cfg.is_open()==false){
    ATH_MSG_INFO(APP_NAME + ": Error openning config file : "+ configPath);
    ATH_MSG_INFO(APP_NAME+": Are you sure that the file exists?" );
    return StatusCode::FAILURE;
  }

  // parse the configuration file and build the network
  lwt::GraphConfig config;
  try {
    config = lwt::parse_json_graph(input_cfg);
  } catch (boost::property_tree::ptree_error& err) {
    ATH_MSG_ERROR(APP_NAME + ": Input file is garbage");
    return StatusCode::FAILURE;
  }
  if (config.outputs.size() != 1) {
    ATH_MSG_ERROR(APP_NAME + ": Graph needs one output node");
    return StatusCode::FAILURE;
  }
  m_output_node_name = config.outputs.begin()->first;
  auto out_names = config.outputs.at(m_output_node_name).labels;
  if (out_names.size() != 1) {
    ATH_MSG_ERROR(APP_NAME + ": Graph needs one output value");
    return StatusCode::FAILURE;
  }
  m_output_value_name = out_names.at(1);
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

