/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "TRT_ConditionsData/TRTPIDNN.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

// lwtnn includes
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
  lwt::GraphConfig config;
  try {
    std::istringstream inputCfg(json);
    config = lwt::parse_json_graph(inputCfg);
  } catch (boost::property_tree::ptree_error& err) {
    log << MSG::ERROR << " NN not readable: " << err.what() << endmsg;
    return StatusCode::FAILURE;
  }

  try {
    //m_nn.reset(new lwt::LightWeightGraph(config));
  } catch (lwt::NNConfigurationException& err) {
    log << MSG::ERROR << " NN configuration failed: " << err.what() << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
