/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoTestWriteAlg.cxx
 *
 * @brief Implementation file for the SCT_RODVetoTestWriteAlg class 
 * in package SCT_ConditionsServices
 *
 * @author Susumu Oda
 **/

#include "SCT_RODVetoTestWriteAlg.h"


//Gaudi includes
#include "GaudiKernel/StatusCode.h"

SCT_RODVetoTestWriteAlg::SCT_RODVetoTestWriteAlg(const std::string& name, 
                                                 ISvcLocator* pSvcLocator ) : 
  AthAlgorithm(name, pSvcLocator),
  m_badRODElements("BadRODIdentifiers"),
  m_badRODElementsInput{0x240100} {
  declareProperty("WriteKeyBadRODIdentifiers", m_badRODElements);
  declareProperty("BadRODIdentifiers", m_badRODElementsInput);
}

SCT_RODVetoTestWriteAlg::~SCT_RODVetoTestWriteAlg() { 
}

StatusCode SCT_RODVetoTestWriteAlg::initialize() {
  ATH_CHECK(m_badRODElements.initialize());

  return StatusCode::SUCCESS;
}

StatusCode SCT_RODVetoTestWriteAlg::execute() {
  SG::WriteHandle<std::vector<unsigned int> > out(m_badRODElements);
  ATH_CHECK( out.record( std::make_unique<std::vector<unsigned int> >() ) );
  for (auto itr=m_badRODElementsInput.begin(); itr!=m_badRODElementsInput.end(); itr++) {
    out->push_back(*itr);
  }
 
  return StatusCode::SUCCESS;
}

StatusCode SCT_RODVetoTestWriteAlg::finalize() {
  return StatusCode::SUCCESS;
}
