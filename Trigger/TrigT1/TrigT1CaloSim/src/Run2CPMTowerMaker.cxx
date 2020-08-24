/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// Run2CPMTowerMaker class Implementation
// ================================================
//
//
//

#include <cmath>

// This algorithm includes
#include "Run2CPMTowerMaker.h"
#include "TrigT1CaloEvent/CPMTower_ClassDEF.h"



namespace LVL1 {

Run2CPMTowerMaker::Run2CPMTowerMaker( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm( name, pSvcLocator ), 
    m_CPMTowerTool("LVL1::L1CPMTowerTools/L1CPMTowerTools") {}
  

  /** the initialise() method is called at the start of processing, so we set up any histograms
      etc. here*/
StatusCode Run2CPMTowerMaker::initialize()
{
  ATH_CHECK( m_CPMTowerTool.retrieve() );
  ATH_CHECK(m_triggerTowerKey.initialize());
  ATH_CHECK(m_cpmTowerKey.initialize());
  return StatusCode::SUCCESS ;
}


/** the finalise() method is called at the end of processing, so it is used
for deleting histograms and general tidying up*/
StatusCode Run2CPMTowerMaker::finalize()
{
  ATH_MSG_INFO("Finalizing" );
  return StatusCode::SUCCESS ;
}


/**----------------------------------------------
   execute() method called once per event
   ----------------------------------------------

There is so little to do that this routine does it all itself:
  get TriggerTowers from TES
  form CPMTowers from TriggerTowers
  store CPMTowers in TES
*/


StatusCode Run2CPMTowerMaker::execute( )
{
  ATH_MSG_DEBUG ( "Executing" ) ;
				      
  // Vectors to store CPMTs in
  auto CPMTs = SG::makeHandle(m_cpmTowerKey);
  auto vectorOfCPMTs = std::make_unique<CPMTCollection>();
  auto cpmtAuxVector = std::make_unique<CPMTAuxCollection>();
  vectorOfCPMTs->setStore(cpmtAuxVector.get());
  
  // Retrieve TriggerTowers from StoreGate 
  auto vectorOfTTs = SG::makeHandle(m_triggerTowerKey);
  ATH_CHECK(vectorOfTTs.isValid());
  // Fill a DataVector of CPMTowers using L1CPMTowerTools
  m_CPMTowerTool->makeCPMTowers(vectorOfTTs.get(), vectorOfCPMTs.get(), true);
  ATH_MSG_DEBUG( vectorOfCPMTs->size()<<" CPMTowers have been generated") ;

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG ( "Formed " << vectorOfCPMTs->size() << " CPM Towers " )  ;
    
    CPMTCollection::const_iterator itCPMT;
    for (itCPMT = vectorOfCPMTs->begin(); itCPMT != vectorOfCPMTs->end(); ++itCPMT)
      ATH_MSG_DEBUG("CPMT has coords (" << (*itCPMT)->eta() <<", "<< (*itCPMT)->phi() << ") and energies : "
          << (*itCPMT)->emEnergy() <<", "<< (*itCPMT)->hadEnergy() <<" (Em,Had)" );
  }
  ATH_CHECK(CPMTs.record(std::move(vectorOfCPMTs), std::move(cpmtAuxVector)));
  
  return StatusCode::SUCCESS;
  
}//end execute

} // end of namespace bracket


