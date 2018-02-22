/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoCondAlg.cxx
 *
 * @brief Implementation file for the SCT_RODVetoCondAlg class 
 * in package SCT_ConditionsAlgorithms
 *
 * @author Susumu Oda
 **/

#include "SCT_RODVetoCondAlg.h"

SCT_RODVetoCondAlg::SCT_RODVetoCondAlg(const std::string& name, 
                                                 ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_badRODElements("BadRODIdentifiers"),
  m_badRODElementsInput{0x240100} {
  declareProperty("WriteKeyBadRODIdentifiers", m_badRODElements);
  declareProperty("BadRODIdentifiers", m_badRODElementsInput);
}

StatusCode SCT_RODVetoCondAlg::initialize() {
  ATH_CHECK(m_badRODElements.initialize());

  return StatusCode::SUCCESS;
}

StatusCode SCT_RODVetoCondAlg::execute() {
  SG::WriteHandle<std::vector<unsigned int> > out(m_badRODElements);
  ATH_CHECK( out.record( std::make_unique<std::vector<unsigned int> >() ) );
  for (auto itr=m_badRODElementsInput.begin(); itr!=m_badRODElementsInput.end(); itr++) {
    out->push_back(*itr);
  }
 
  return StatusCode::SUCCESS;
}

StatusCode SCT_RODVetoCondAlg::finalize() {
  return StatusCode::SUCCESS;
}
