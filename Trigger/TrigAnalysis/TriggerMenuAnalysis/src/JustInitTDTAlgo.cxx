/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  JustInitTDTAlgo.cxx
*/
#include "TriggerMenuAnalysis/JustInitTDTAlgo.hxx"

JustInitTDTAlgo::JustInitTDTAlgo(const std::string& name, ISvcLocator* svcloc) : 
  AthAlgorithm(name, svcloc), 
  m_trigAccessTool("TrigAccessTool/TrigAccessTool", this) {
  declareProperty("TrigAccessTool", m_trigAccessTool, "TrigAccessTool");
}

JustInitTDTAlgo::~JustInitTDTAlgo() {
}

StatusCode JustInitTDTAlgo::initialize() {
  if (m_trigAccessTool.retrieve().isFailure()) {
    ATH_MSG_WARNING ("Cannot retrieve TrigAccessTool");
  }
  return StatusCode::SUCCESS;
}

StatusCode JustInitTDTAlgo::execute() {
  return StatusCode::SUCCESS;
}

StatusCode JustInitTDTAlgo::finalize() {
  return StatusCode::SUCCESS;
}
