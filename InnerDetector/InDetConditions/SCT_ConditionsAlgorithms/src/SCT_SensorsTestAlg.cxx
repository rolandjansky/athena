/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"

// Include STL stuff
#include <string>
//for o/p to file
#include <iostream>
#include <cstdlib>

using namespace std;

SCT_SensorsTestAlg::SCT_SensorsTestAlg(const std::string& name, 
                                       ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ) {
  //nop
}

//Initialize
StatusCode SCT_SensorsTestAlg::initialize(){
  StatusCode sc(StatusCode::SUCCESS);
  ATH_MSG_INFO("Calling initialize");
  sc = m_SensorsTool.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    ATH_MSG_ERROR("Could not retrieve the service");
  }
  return sc;
}

//Execute
StatusCode SCT_SensorsTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  StatusCode sc(StatusCode::SUCCESS);
  ATH_MSG_INFO("Calling execute");
  std::vector<std::string> values;
  m_SensorsTool->getSensorsData(values);
  for (std::vector<std::string>::const_iterator i=values.begin();i!=values.end();++i){
    ATH_MSG_INFO("------------" << *i << "------------");
  }
  m_SensorsTool->printManufacturers();
  return sc;
}

//Finalize
StatusCode SCT_SensorsTestAlg::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  ATH_MSG_INFO("Calling finalize");
  return sc;
}
