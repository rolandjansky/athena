/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GoodRunsLists includes
#include <GoodRunsLists/GRLSelectorAlg.h>
#include <AnaAlgorithm/FilterReporter.h>
#include <AsgTools/MessageCheck.h>
#include "xAODEventInfo/EventInfo.h"

GRLSelectorAlg::GRLSelectorAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AnaAlgorithm( name, pSvcLocator )
										    , m_grlTool("GoodRunsListSelectionTool", this)
{

  declareProperty( "Tool", m_grlTool , "The GoodRunsListSelectionTool" );

}


GRLSelectorAlg::~GRLSelectorAlg() {}


StatusCode GRLSelectorAlg::initialize() {
  //ATH_MSG_INFO ("Initializing " << name() << "...");
  ANA_CHECK( m_grlTool.retrieve() );
  ANA_CHECK( m_filterParams.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode GRLSelectorAlg::finalize() {
  //  ATH_MSG_INFO ("Finalizing " << name() << "...");

  ANA_CHECK (m_filterParams.finalize());

  return StatusCode::SUCCESS;
}

StatusCode GRLSelectorAlg::execute() {  
  EL::FilterReporter filter (m_filterParams, false);

  const xAOD::EventInfo* evtInfo = 0;
  ANA_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  if(!m_grlTool->passRunLB(*evtInfo)) return StatusCode::SUCCESS;

  filter.setPassed (true);

  return StatusCode::SUCCESS;
}
