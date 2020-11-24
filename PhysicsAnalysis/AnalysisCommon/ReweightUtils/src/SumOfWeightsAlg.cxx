/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReweightUtils includes
#include "SumOfWeightsAlg.h"
#include "ReweightUtils/WeightToolBase.h"

#include "TString.h"
#include <string>

using std::string;

//**********************************************************************

SumOfWeightsAlg::SumOfWeightsAlg(const std::string& name, ISvcLocator* pSvcLocator ) : 
  ::AthFilterAlgorithm( name, pSvcLocator ), m_eventsProcessed(0), m_cutIDs() {
  declareProperty( "WeightTools", m_weightTools);
}

//**********************************************************************

SumOfWeightsAlg::~SumOfWeightsAlg() { }

//**********************************************************************

StatusCode SumOfWeightsAlg::initialize() {
  StatusCode sc;
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_MSG_DEBUG("Retrieving tools...");
  sc = m_weightTools.retrieve();
  if ( ! sc.isSuccess() ) {
    ATH_MSG_ERROR("Tool retrieval failed.");
    return sc;
  }
  ATH_MSG_DEBUG("Tool retrieval completed.");
  unsigned int ntool = m_weightTools.size();
  ATH_MSG_DEBUG("  Tool count: " << ntool);
  for (size_t itool = 0; itool < ntool; ++itool ) {
    ATH_MSG_DEBUG("    " << m_weightTools[itool]->name());
    if(msgLvl(MSG::DEBUG)) m_weightTools[itool]->print();
    // Get the tool's message property:
    const WeightToolBase* tool = dynamic_cast< const WeightToolBase* >( m_weightTools[ itool ].operator->() );
    if( ! tool ) {
       ATH_MSG_ERROR( "The received tool is not an WeightToolBase?!?" );
       return StatusCode::FAILURE;
    }
    //strip the 'ToolSvc.' off the weight name
    std::string toolName = m_weightTools[itool]->name();
    if(toolName.find("ToolSvc.")==0) toolName.replace(0,8,"");
    CutIdentifier cID = cutFlowSvc()->registerTopFilter( toolName,
                                                         toolName, // description (can be improved FIXME)
                                                         xAOD::CutBookkeeper::CutLogic::ALLEVENTSPROCESSED,
                                                         "AllStreams",
                                                         true);
    m_cutIDs.push_back(cID);
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode SumOfWeightsAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << ", will loop over WeightTools...");
  setFilterPassed(true);

  for (std::size_t i = 0; i < m_cutIDs.size(); ++i) {
    float weight = m_weightTools[i]->getWeight();
    ATH_MSG_DEBUG("   got weight = " << weight << " for " << m_weightTools[i]->name());
    const CutIdentifier cutID = m_cutIDs[i];
    cutFlowSvc()->addEvent(cutID, weight);
  }

  m_eventsProcessed += 1;
  return StatusCode::SUCCESS;
}


//**********************************************************************
StatusCode SumOfWeightsAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


