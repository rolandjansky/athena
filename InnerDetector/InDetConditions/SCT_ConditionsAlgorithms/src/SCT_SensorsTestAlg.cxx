/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SensorsTestAlg.cxx
 *
 * @brief Implementation file for the SCT_SensorsTestAlg class 
 * in package SCT_ConditionsAlgorithms
 *
 * @author Shaun Roe
 **/

#include "SCT_SensorsTestAlg.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

// Include STL stuff
#include <vector>

SCT_SensorsTestAlg::SCT_SensorsTestAlg(const std::string& name, 
                                       ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name, pSvcLocator) {
  //nop
}

//Initialize
StatusCode SCT_SensorsTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");
  ATH_CHECK(m_SensorsTool.retrieve());
  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_SensorsTestAlg::execute(const EventContext& ctx) const {
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  ATH_MSG_INFO("Calling execute");
  std::vector<std::string> values;
  m_SensorsTool->getSensorsData(values, ctx);
  for (const std::string& value: values) {
    ATH_MSG_INFO("------------" << value << "------------");
  }
  m_SensorsTool->printManufacturers(ctx);
  return StatusCode::SUCCESS;
}

//Finalize
StatusCode SCT_SensorsTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
