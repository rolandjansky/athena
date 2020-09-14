/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "Gaudi/Interfaces/IOptionsSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "JobOptsDumperAlg.h"
#include <fstream>

StatusCode JobOptsDumperAlg::initialize() {

  std::ofstream fout(m_fileName);
  if (!fout.good()) {
      ATH_MSG_ERROR("Failed to open output file " << m_fileName);
      return StatusCode::FAILURE;
  }

  ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc", name());
  ATH_CHECK(joSvc.retrieve());

  std::vector<std::string> options;
  for (const auto& [name, value] : joSvc->items()) {
    std::ostringstream os;
    os << name << " = " << value << std::endl;
    options.emplace_back(os.str());
  }

  std::sort(options.begin(), options.end()); 
  for (const std::string& opt : options) {
    fout << opt << std::endl;
  }
  fout.close();
  return StatusCode::SUCCESS;
}
