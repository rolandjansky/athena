/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExFortranAlgorithm/FortranAlgorithm.h"

extern "C" {
  void initialize_(const int& lun, const char*, int);
  void execute_(const int& lun);
  void finalize_(const int& lun);
}

/////////////////////////////////////////////////////////////////////////////

FortranAlgorithm::FortranAlgorithm(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_lun(16), m_fileName("FortranAlgorithmInput.data")
{
  declareProperty("LUN", m_lun);
  declareProperty("fileName", m_fileName);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode FortranAlgorithm::initialize(){
  ATH_MSG_INFO ("calling initialize_()");
  initialize_(m_lun, m_fileName.c_str(), m_fileName.size());
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode FortranAlgorithm::execute() {
  ATH_MSG_INFO ("calling execute_()");
  execute_(m_lun);
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode FortranAlgorithm::finalize() {
  ATH_MSG_INFO ("calling finalize_()");
  finalize_(m_lun);
  return StatusCode::SUCCESS;
}






