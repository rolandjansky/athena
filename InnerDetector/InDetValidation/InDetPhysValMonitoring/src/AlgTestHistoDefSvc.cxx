/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AlgTestHistoDefSvc.cxx
 * Implementation file for  HistogramDefinitionSvc service test algorithm
 * @author Shaun Roe
 * @date 5 September, 2015
 **/

// STL
// to access environment


// Gaudi
#include "GaudiKernel/StatusCode.h"
#include "InDetPhysValMonitoring/HistogramDefinitionSvc.h"
// Athena
// Package
#include "InDetPhysValMonitoring/AlgTestHistoDefSvc.h"



using namespace std;

AlgTestHistoDefSvc::AlgTestHistoDefSvc (const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name,
                                                                                                          pSvcLocator),
  m_histoDefSvc("HistogramDefinitionSvc", name) { // nop
}

AlgTestHistoDefSvc::~AlgTestHistoDefSvc() {
  // nop
}

StatusCode
AlgTestHistoDefSvc::initialize() {
  ATH_CHECK(m_histoDefSvc.retrieve());
  ATH_MSG_INFO("Test algorithm for HistogramDefinitionSvc");
  //
  ATH_MSG_INFO("Printing histogram definition for 002_Al_N_01");
  std::string definition = m_histoDefSvc->definition("002_Al_N_01", "").str();
  ATH_MSG_INFO("Titles : " << m_histoDefSvc->definition("002_Al_N_01", "").allTitles);
  ATH_MSG_INFO(definition);
  //
  ATH_MSG_INFO("Printing histogram definition for nparticles");
  definition = m_histoDefSvc->definition("nparticles", "").str();
  ATH_MSG_INFO(definition);
  //
  ATH_MSG_INFO("Printing histogram definition for testProfile");
  definition = m_histoDefSvc->definition("testProfile", "").str();
  ATH_MSG_INFO(definition);
  //
  ATH_MSG_INFO("Printing histogram definition for test2D");
  definition = m_histoDefSvc->definition("test2D", "").str();
  ATH_MSG_INFO(definition);
  //
  ATH_MSG_INFO("Printing histogram definition for testEfficiency");
  definition = m_histoDefSvc->definition("testEfficiency", "").str();
  ATH_MSG_INFO(definition);
  ATH_MSG_INFO("Printing definition for inclusion test");
  definition = m_histoDefSvc->definition("include").str();
  ATH_MSG_INFO(definition);
  //
  ATH_MSG_INFO("Printing histogram definition for non-existent definition");
  definition = m_histoDefSvc->definition("utterRubbish", "").str();
  ATH_MSG_INFO(definition);

  return StatusCode::SUCCESS;
}

StatusCode
AlgTestHistoDefSvc::execute() {
  return StatusCode::SUCCESS;
}

StatusCode
AlgTestHistoDefSvc::finalize() {
  msg(MSG::INFO) << "finalize()" << endmsg;
  return StatusCode::SUCCESS;
}
