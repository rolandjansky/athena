/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "TRT_ConditionsData/TRTPIDNN.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

// lwtnn includes
//#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/parse_json.hh"

// JSON parsing
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/exceptions.hpp"

double InDet::TRTPIDNN::evaluate(std::map<std::string, std::map<std::string, double>>& scalarInputs,
        std::map<std::string, std::map<std::string, std::vector<double>>>& vectorInputs) const {
  const auto result = m_nn->compute(scalarInputs, vectorInputs);
  return result.at(m_outputNode);
}

StatusCode InDet::TRTPIDNN::configure(const std::string& json) {
  MsgStream log(Athena::getMessageSvc(),"TRTPIDNN");
  try {
    std::istringstream inputCfg(json);
    m_nnConfig = lwt::parse_json_graph(inputCfg);
  } catch (boost::property_tree::ptree_error& err) {
    log << MSG::ERROR << " NN not readable: " << err.what() << endmsg;
    return StatusCode::FAILURE;
  }
  std::cout << "============= Successfully read json config" << std::endl;

  try {
    m_nn.reset(new lwt::LightweightGraph(m_nnConfig));
  } catch (lwt::NNConfigurationException& err) {
    log << MSG::ERROR << " NN configuration failed: " << err.what() << endmsg;
    return StatusCode::FAILURE;
  }
  std::cout << "============= Successfully read json config" << std::endl;

  // store templates of the structure of the inputs to the NN
  m_scalarInputs.clear();
  for (auto input : m_nnConfig.inputs) {
    std::cout << "============= Scalar input: " << input.name << std::endl;
    m_scalarInputs[input.name] = {};
    for (auto variable : input.variables) {
      std::cout << "============= Input variable: " << variable.name << ", default: " << input.defaults[variable.name] << std::endl;
      m_scalarInputs[input.name][variable.name] = input.defaults[variable.name];
    }
  }

  m_vectorInputs.clear();
  for (auto input : m_nnConfig.input_sequences) {
    std::cout << "============= Vector input: " << input.name << std::endl;
    m_vectorInputs[input.name] = {};
    for (auto variable : input.variables) {
      std::cout << "============= Input variable: " << variable.name << ", default: " << input.defaults[variable.name] << std::endl;
      m_vectorInputs[input.name][variable.name] = {};
    }
  }


  return StatusCode::SUCCESS;
}
