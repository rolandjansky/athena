/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "TRT_ConditionsNN/TRTPIDNN.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

// lwtnn includes
//#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/parse_json.hh"

// JSON parsing
#define BOOST_BIND_GLOBAL_PLACEHOLDERS // Needed to silence Boost pragma message
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/exceptions.hpp"

double InDet::TRTPIDNN::evaluate(std::map<std::string, std::map<std::string, double>>& scalarInputs,
        std::map<std::string, std::map<std::string, std::vector<double>>>& vectorInputs,
        const std::string& outputNode, const std::string& outputLabel) const {
  MsgStream log(Athena::getMessageSvc(),"TRTPIDNN");
  const auto result = m_nn->compute(scalarInputs, vectorInputs, outputNode);
  const auto itResult = result.find(outputLabel);
  if (itResult == result.end()) {
    log << MSG::ERROR << " unable to find output: node=" << outputNode << ", label=" << outputLabel << endmsg;
    return 0.5;
  }
  return itResult->second;
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

  try {
    m_nn.reset(new lwt::LightweightGraph(m_nnConfig));
  } catch (lwt::NNConfigurationException& err) {
    log << MSG::ERROR << " NN configuration failed: " << err.what() << endmsg;
    return StatusCode::FAILURE;
  }

  // set the default output node name
  if (m_nnConfig.outputs.empty() or m_nnConfig.outputs.begin()->second.labels.empty()) {
    log << MSG::ERROR << " unable to define NN output." << endmsg;
    return StatusCode::FAILURE;
  }
  m_outputNode = m_nnConfig.outputs.begin()->first;
  m_outputLabel = *(m_nnConfig.outputs[m_outputNode].labels.begin());

  // store templates of the structure of the inputs to the NN
  m_scalarInputs.clear();
  for (auto input : m_nnConfig.inputs) {
    m_scalarInputs[input.name] = {};
    for (auto variable : input.variables) {
      m_scalarInputs[input.name][variable.name] = input.defaults[variable.name];
    }
  }
  m_vectorInputs.clear();
  for (auto input : m_nnConfig.input_sequences) {
    m_vectorInputs[input.name] = {};
    for (auto variable : input.variables) {
      m_vectorInputs[input.name][variable.name] = {};
    }
  }

  return StatusCode::SUCCESS;
}
