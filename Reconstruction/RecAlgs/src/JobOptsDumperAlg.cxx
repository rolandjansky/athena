/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IJobOptionsSvc.h"

#include "JobOptsDumperAlg.h"
#include <fstream>

StatusCode JobOptsDumperAlg::initialize() {

  std::ofstream fout(m_fileName);
  if (!fout.good()) {
      ATH_MSG_ERROR("Failed to open output file " << m_fileName);
      return StatusCode::FAILURE;
  }


  IJobOptionsSvc* p_jobOptionSvc;
  ATH_CHECK(service("JobOptionsSvc", p_jobOptionSvc));

  std::vector<std::string> options;
  for (const std::string& compName :  p_jobOptionSvc->getClients()) {
    for (const auto& props:  *p_jobOptionSvc->getProperties(compName)) {
      std::ostringstream os;
      os << compName << "." << props->name() << " = " << props->toString() << std::endl;
      options.emplace_back(os.str());
    }
  }

  std::sort(options.begin(), options.end()); 
  for (const std::string& opt : options) {
    fout << opt << std::endl;
  }
  fout.close();
  return StatusCode::SUCCESS;
}
