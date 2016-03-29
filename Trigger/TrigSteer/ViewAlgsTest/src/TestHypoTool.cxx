/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./TestHypoTool.h"


TestHypoTool::TestHypoTool(const std::string& type, const std::string& name,  const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_etThreshold(0){  
  declareInterface<TestHypoTool>( this );
  declareProperty("EtThreshold", m_etThreshold);
}

StatusCode TestHypoTool::initialize() {
  return StatusCode::SUCCESS;
}
bool TestHypoTool::decision(const TestCluster * cl) const {
  ATH_MSG_DEBUG("Et " << TestEDM::getClusterEt(cl) << " threshold " << m_etThreshold);
  return TestEDM::getClusterEt(cl) > m_etThreshold;
}
