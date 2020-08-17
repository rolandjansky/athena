/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaAnalysisAlg.cxx 

#include "TrigEgammaAnalysisAlg.h"
#include "Gaudi/Property.h"
#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"
#include "AsgTools/AsgTool.h"

using std::string;

//**********************************************************************

TrigEgammaAnalysisAlg::
TrigEgammaAnalysisAlg(const std::string& name, 
                    ISvcLocator* pSvcLocator )
: ::AthAlgorithm( name, pSvcLocator ) {
  declareProperty( "Tools", m_asgtools);
}

//**********************************************************************

TrigEgammaAnalysisAlg::~TrigEgammaAnalysisAlg() { }

//**********************************************************************

StatusCode TrigEgammaAnalysisAlg::initialize() {
  StatusCode sc;
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  sc = m_asgtools.retrieve();
  if ( ! sc.isSuccess() ) {
    ATH_MSG_ERROR("Tool retrieval failed.");
  }
  ATH_MSG_INFO("Tool retrieval completed.");
  unsigned int ntool = m_asgtools.size();
  ATH_MSG_INFO("  Tool count: " << ntool);
  for ( unsigned int itool=0; itool<ntool; ++itool ) {
    ATH_MSG_INFO("    " << m_asgtools[itool]->name());
    m_asgtools[itool]->print();
    sc = m_asgtools[itool]->book();
    if ( ! sc.isSuccess() ) {
        ATH_MSG_ERROR("Tool booking failed.");
    }
  }
  ATH_MSG_DEBUG("Initialize complete");
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrigEgammaAnalysisAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  StatusCode sc;
  for ( ToolHandleArray<ITrigEgammaAnalysisBaseTool>::const_iterator itoo=m_asgtools.begin();
        itoo!=m_asgtools.end(); ++itoo ) {
    ToolHandle<ITrigEgammaAnalysisBaseTool> htool = *itoo;
    sc = htool->finalize();
    if ( ! sc.isSuccess() ) {
        ATH_MSG_ERROR("Tool finalize failed.");
    }
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrigEgammaAnalysisAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  // Loop over hello tools.
  ATH_MSG_DEBUG("Looping over ASG tools.");
  string line = "---------------------------------------------------";
  ATH_MSG_DEBUG(line);
 StatusCode sc;
  for ( ToolHandleArray<ITrigEgammaAnalysisBaseTool>::const_iterator itoo=m_asgtools.begin();
        itoo!=m_asgtools.end(); ++itoo ) {
    ToolHandle<ITrigEgammaAnalysisBaseTool> htool = *itoo;
    sc = htool->execute();
    if ( ! sc.isSuccess() ) {
        ATH_MSG_ERROR("Tool execute failed.");
    }
    ATH_MSG_INFO(line);
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************
