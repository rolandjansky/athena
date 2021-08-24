/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// Run2JetElementMaker class Implementation
// ================================================
//
//
//


// This algorithm includes
#include "Run2JetElementMaker.h"


namespace LVL1 {

Run2JetElementMaker::Run2JetElementMaker( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm( name, pSvcLocator ), 
    m_JetElementTool("LVL1::L1JetElementTools/L1JetElementTools")
{
}
  

  /** the initialise() method is called at the start of processing, so we set up any histograms
      etc. here*/
StatusCode Run2JetElementMaker::initialize()
{
  ATH_CHECK( m_JetElementTool.retrieve() );
  ATH_CHECK(m_triggerTowerKey.initialize());
  ATH_CHECK(m_jetElementKey.initialize());
  return StatusCode::SUCCESS ;
}


/**----------------------------------------------
   execute() method called once per event
   ----------------------------------------------

Checks that the Cell Type is supported (terminates with errors if not)
and calls relevant routine to look for the cells.
*/


StatusCode Run2JetElementMaker::execute( )
{
  ATH_MSG_DEBUG ( "Executing" ) ;

  // Vectors to store JetElements in
  auto jetElements = SG::makeHandle(m_jetElementKey);
  auto vectorOfJEs = std::make_unique<JECollection>();
  auto jeAuxVector = std::make_unique<JEAuxCollection>();
  vectorOfJEs->setStore(jeAuxVector.get());
	
  // Retrieve TriggerTowers from StoreGate 
  auto vectorOfTTs = SG::makeHandle(m_triggerTowerKey);
  ATH_CHECK(vectorOfTTs.isValid());
  // Fill a DataVector of JetElements using L1JetElementTools
  m_JetElementTool->makeJetElements(vectorOfTTs.get(), vectorOfJEs.get());
  ATH_MSG_DEBUG( vectorOfJEs->size()<<" JetElements have been generated") ;
  ATH_CHECK(jetElements.record(std::move(vectorOfJEs), std::move(jeAuxVector)));
  
  return StatusCode::SUCCESS;
}//end execute

} // end of namespace bracket

