/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/HbbTaggerDNN.h"

#include "PathResolver/PathResolver.h"

// #include "lwtnn/LightweightGraph.hh"

#include <fstream>

namespace {
  const static std::string APP_NAME = "HbbTaggerDNN";
}

HbbTaggerDNN::HbbTaggerDNN( const std::string& name ) :
  asg::AsgTool(name),
  m_configFile("")
  // m_lwnn(nullptr)
  {
    declareProperty( "ConfigFile",   m_configFile="BoostedJetTaggers/HbbTaggerDNN/Boost2017/bullshit.json");
}

HbbTaggerDNN::~HbbTaggerDNN() {}

StatusCode HbbTaggerDNN::initialize(){

  /* Initialize the DNN tagger tool */
  ATH_MSG_INFO(APP_NAME+": Initializing HbbTaggerDNN tool");
  ATH_MSG_INFO(APP_NAME+": Using config file :"+m_configFile);

  if( m_configFile.empty() ) {
    // Assume the cut functions have been set through properties.
    ATH_MSG_ERROR( "No config file provided" ) ;
    // ATH_MSG_ERROR( "Please read the TWiki for this tool" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Using config file : "<< m_configFile );
  // check for the existence of the configuration file
  std::string configPath;
  configPath = PathResolverFindCalibFile(m_configFile.c_str());

  // read json file for DNN weights
  ATH_MSG_INFO(APP_NAME + ": DNN Tagger configured with: " + configPath);

  std::ifstream input_cfg( configPath );

  if(input_cfg.is_open()==false){
    ATH_MSG_INFO(APP_NAME + ": Error openning config file : "+ configPath);
    ATH_MSG_INFO(APP_NAME+": Are you sure that the file exists at this path?" );
    return StatusCode::FAILURE;
  }

/*
  lwt::JSONConfig cfg = lwt::parse_json( input_cfg );

  ATH_MSG_INFO( (APP_NAME+": Keras Network NLayers : "+std::to_string(cfg.layers.size()).c_str() ));

  m_lwnn = std::unique_ptr<lwt::LightweightNeuralNetwork>
              (new lwt::LightweightNeuralNetwork(cfg.inputs, cfg.layers, cfg.outputs) );
*/

  return StatusCode::SUCCESS;
}

StatusCode HbbTaggerDNN::finalize(){
    // Delete or clear anything
    return StatusCode::SUCCESS;
}

int HbbTaggerDNN::keep(const xAOD::Jet& jet) const {
  return 1;
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

