/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/// Name    : DL1Tag
/// Package : JetTagTools
/// Author  : Marie Lanfermann (Universite de Geneve), Dan Guest (UC Irvine)
/// Created : 2015
///
/// DESCRIPTION:
///
/// This class computes the Neural Network probability for being b-jet,
/// c-jet or uds-jet for a single event. The NN is constructed outside Atheana
/// using standard Data Science libraries, i.e. Keras (Theano or TensorFlow backend)
/// for the actual training.
///
/////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/IToolSvc.h"
#include "xAODTracking/TrackParticle.h"

#include "JetTagCalibration/CalibrationBroker.h"
#include "JetTagTools/DL1Tag.h"

#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"

#include "xAODBTagging/BTagging.h"
#include "xAODJet/Jet.h"

#include "TObjString.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cmath>

namespace{
  // copy the inputs, but with NaN replaced with default values. Don't
  // do anything with NaN values with no defined default.
  std::map<std::string, double> replace_nan_with_defaults(
							  const std::map<std::string, double>& inputs,
							  const std::map<std::string, double>& defaults);

  std::map<std::string, double> add_check_variables(
						const std::map<std::string, double>& inputs);

  // use this value when an output isn't defined by the NN
  const double missing_weight = 0.;
}

namespace Analysis {

  DL1Tag::DL1Tag(const std::string& name, const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p),
    m_calibrationDirectory("DL1"),
    m_calibrationSubDirectory("AntiKt4TopoEM"),
    m_calibrationTool("BTagCalibrationBroker"),
    m_n_compute_errors(0),
    m_runModus("analysis")
  {
    // python binding
    declareInterface<IMultivariateJetTagger>(this);

    // access to JSON NN configuration file from COOL:
    declareProperty("calibration_directory", m_calibrationDirectory);
    declareProperty("calibration_subdirectory", m_calibrationSubDirectory);
    declareProperty("calibrationTool", m_calibrationTool);
    declareProperty("forceDL1CalibrationAlias", m_forceDL1CalibrationAlias = true);
    declareProperty("DL1CalibAlias", m_DL1CalibAlias = "AntiKt4TopoEM");
    declareProperty("xAODBaseName", m_xAODBaseName);

    // global configuration
    declareProperty("Runmodus", m_runModus);
    declareProperty("LocalNNConfigurationFile", m_LocalNNConfigFile);
  }

  DL1Tag::~DL1Tag() {
    for ( auto network : m_NeuralNetworks) {
      delete network.second;
      network.second = 0;
    }
  }

  StatusCode DL1Tag::initialize() {
    // prepare calibraiton tool
    StatusCode sc = m_calibrationTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( "#BTAG# Failed to retrieve tool " << m_calibrationTool );
      return sc;
    }
    else {
      ATH_MSG_INFO( "#BTAG# Retrieved: " << m_calibrationTool );
    }

    // Read in the configuration of the neural net for DL1:
    if (m_LocalNNConfigFile.size() != 0) { // retrieve map of NN config and default values from local JSON file
      std::ifstream nn_config_ifstream(m_LocalNNConfigFile);
      build_nn("local", nn_config_ifstream);
    }
    else { // load the configuration from the COOL db:
      load_calibration_file();
    }

    ATH_MSG_INFO(" Initialization of DL1Tag successful" );
    return StatusCode::SUCCESS;
  }


  void DL1Tag::load_calibration_file() {
    std::string file_name = "/net_configuration"; // directory of NN calibration (starting from specific jet collection directory) in COOL db
    m_calibrationTool->registerHistogram(m_calibrationDirectory, file_name);  //register the calibration file for later access

    ATH_MSG_DEBUG(" #BTAG# Registered NN histograms with directory: " <<
		  m_calibrationDirectory << " and subdirectory " <<
		  m_calibrationSubDirectory);

    m_calibrationTool->printStatus();
  }


  void DL1Tag::build_nn(const std::string& jetauthor, std::istream& nn_config_istream) {
    if (m_NeuralNetworks.count(jetauthor)) {
      delete m_NeuralNetworks[jetauthor];
      m_map_variables.erase(jetauthor);
      m_map_defaults.erase(jetauthor);
      m_NeuralNetworks.erase(jetauthor);
    }

    lwt::JSONConfig nn_config = lwt::parse_json(nn_config_istream);
    ATH_MSG_DEBUG("making NN with " << nn_config.layers.size() << " layers");

    if (!(std::find((nn_config.outputs).begin(), (nn_config.outputs).end(), "bottom") != (nn_config.outputs).end())) {
      ATH_MSG_WARNING( "#BTAG# b-tagger without b-tagging option 'bottom' - please check the NN output naming convention.");
    }

    m_NeuralNetworks.insert(std::make_pair(jetauthor, new lwt::LightweightNeuralNetwork(nn_config.inputs, nn_config.layers, nn_config.outputs)));
    m_map_variables.insert(std::make_pair(jetauthor, nn_config.inputs));
    m_map_defaults.insert(std::make_pair(jetauthor, nn_config.defaults));
  }


  void DL1Tag::cache_calibration(const std::string& jetauthor) {
    bool update = m_calibrationTool->retrieveHistogram(m_calibrationDirectory, jetauthor, "net_configuration").second;

    // check if this network is alread cached, so do nothing
    if ( (!update) && (m_NeuralNetworks.count(jetauthor) == 1) ) {
      return;
    }
    // get the NN configuration
    std::string calib = get_calib_string(jetauthor);
    ATH_MSG_DEBUG("Reading NN for " + jetauthor + ": "
                  << calib.size() << " characters");

    // Read in the configuration of the neural net for DL1 and build net:
    std::istringstream nn_config_sstream(calib);
    build_nn(jetauthor, nn_config_sstream);
  }

  std::string DL1Tag::get_calib_string(std::string jetauthor) {
    std::string file_name = "/net_configuration";
    std::pair<TObject*, bool> stringpair = m_calibrationTool->retrieveTObject<TObject>(m_calibrationDirectory, jetauthor, file_name);
    if (stringpair.second == true) {
      m_calibrationTool->updateHistogramStatus(m_calibrationDirectory, jetauthor, "net_configuration",false);
    }
    TObjString* cal_string = dynamic_cast<TObjString*>(stringpair.first);

    if (cal_string == 0){  //catch if no string was found
      std::string fuller_name = m_calibrationDirectory + "/" +
	jetauthor + "/net_configuration";
      if (stringpair.first) {
	fuller_name.append(" [but an object was found]");
      }
      throw std::logic_error("Cannot retrieve string: " + fuller_name);
    }
    std::string calibration(cal_string->GetString().Data());
    return calibration;
  }


  StatusCode DL1Tag::finalize() { // all taken care of in destructor
    if (m_n_compute_errors > 0) {
      ATH_MSG_WARNING("Neural network was unable to compute. Number of errors: "+ std::to_string(m_n_compute_errors));
    }
    ATH_MSG_INFO(" #BTAG# Finalization of DL1Tag successfull" );
    return StatusCode::SUCCESS;
  }

  void DL1Tag::fill_outputs(xAOD::BTagging *BTag, var_map outputs) {
    if(m_runModus=="analysis") {
      if (!outputs.count("charm")) {
	BTag->setVariable<double>(m_xAODBaseName, "pc", missing_weight);
      }
      else if (outputs.count("charm")) {
	BTag->setVariable<double>(m_xAODBaseName, "pc", outputs.at("charm"));
      }

      if (!outputs.count("light") && !outputs.count("bottom")) {
	BTag->setVariable<double>(m_xAODBaseName, "pu", missing_weight);
      }
      else if (outputs.count("light")) {
	BTag->setVariable<double>(m_xAODBaseName, "pu", outputs.at("light"));
      }

      if (outputs.count("bottom")) {
	BTag->setVariable<double>(m_xAODBaseName, "pb", outputs.at("bottom"));
	if (!outputs.count("light")) {
	  BTag->setVariable<double>(m_xAODBaseName, "pu", 1.-outputs.at("bottom"));
	}
      }
      else if (!outputs.count("bottom")) {
	BTag->setVariable<double>(m_xAODBaseName, "pb", missing_weight);
	ATH_MSG_WARNING( "#BTAG# b-tagger without b-tagging option: " << outputs.at("bottom"));
      }
      ATH_MSG_DEBUG("#BTAG# NN output b: " << outputs.at("bottom") <<
		    " c: " << outputs.at("charm") << " u: " <<  outputs.at("light"));
    }
  }

  void DL1Tag::assignProbability(xAOD::BTagging *BTag,
				 const var_map &inputs,
				 const std::string& assigned_jet_author){

    std::string jetauthor = assigned_jet_author;

    if (m_LocalNNConfigFile.size()==0) {
      if (m_forceDL1CalibrationAlias) {
	jetauthor = m_DL1CalibAlias;
      }
      ATH_MSG_DEBUG("#BTAG# Jet author set (via forceDL1CalibrationAlias) to: " << jetauthor );
      if ( jetauthor.empty() ) {
	ATH_MSG_WARNING(" #BTAG# Hypothesis or jetauthor is empty."
			" No likelihood value given back. ");
      }
      try {
	cache_calibration(jetauthor);
      } catch (std::exception& e) {
	ATH_MSG_WARNING(
	  "problem loading calibration for " + jetauthor +
	  " (" + e.what() + "), skipping this jet");
	return;
      }
    }
    else {
      jetauthor = "local";
    }

    if(!m_NeuralNetworks.count(jetauthor)) {
      ATH_MSG_WARNING("NN not loaded correctly.");
      return;
    }

    var_map complete_inputs = add_check_variables(inputs);
    var_map cleaned_inputs = replace_nan_with_defaults(complete_inputs, m_map_defaults.at(jetauthor));

    for (const auto& var: m_map_variables.at(jetauthor)) {
      if (std::isnan(cleaned_inputs.at(var.name)) && cleaned_inputs.count(var.name)) {
      ATH_MSG_WARNING( "#BTAG# 'nan' input for variable " + var.name + " --> will result in 'nan' classification output. Check NN configuration file for default settings.");
      }
    }

    var_map outputs;
    try {
      outputs = m_NeuralNetworks.at(jetauthor)->compute(cleaned_inputs);
    } catch (lwt::NNEvaluationException& e) {
      ATH_MSG_WARNING("Can't compute outputs, probably missing input values");
      m_n_compute_errors++;
      return;
    }

    fill_outputs(BTag, outputs);
  } //end assign_probability
}

namespace {
  std::map<std::string, double> replace_nan_with_defaults(
							  const std::map<std::string, double>& inputs,
							  const std::map<std::string, double>& defaults) {
    // return a new map with the NaN values replaced where possible.
    std::map<std::string, double> outputs;

    // loop over all inputs and create map of cleaned inputs
    for (const auto& in: inputs) {
      if (std::isnan(in.second) && defaults.count(in.first)) {
	outputs[in.first] = defaults.at(in.first);
      } else {
	outputs[in.first] = in.second;
      }
    }
    return outputs;
  } // end replace_nan_with_defaults

  std::map<std::string, double> add_check_variables(
						const std::map<std::string, double>& inputs) {
    // return a new map with additional binary variables for each input variable
    std::map<std::string, double> complete_inputs;

    for (const auto& in: inputs) {
      // copy everything from the inputs map
      complete_inputs.insert(std::make_pair(in.first, in.second));
      // add binary check variables
      if (std::isnan(in.second)) {
	complete_inputs.insert(std::make_pair(in.first + "_check", 1.));
      }
      else {
	complete_inputs.insert(std::make_pair(in.first + "_check", 0.));
      }
    }
    return complete_inputs;
  }// end add_check_variables
} // end namespace
