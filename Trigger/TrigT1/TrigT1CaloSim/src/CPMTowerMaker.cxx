/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// CPMTowerMaker class Implementation
// ================================================
//
//
//

#include <cmath>

// This algorithm includes
#include "CPMTowerMaker.h"



namespace LVL1 {

/** This is the constructor for JEMaker and is where you define the relevant
    parameters.
    Currently these are :
    - "TriggerTowerLocation" : the location of the jes in Storegate You shouldn't have to touch this.
    - "CPMTowerLocation" : the location of the CPMT in StoreGate. You shouldn't have to touch this.

  Alter the values of these in jobOptions.txt
*/
  
CPMTowerMaker::CPMTowerMaker( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm( name, pSvcLocator ), 
    m_CPMTowerTool("LVL1::L1CPMTowerTools/L1CPMTowerTools")
{ }
  

  /** the initialise() method is called at the start of processing, so we set up any histograms
      etc. here*/
StatusCode CPMTowerMaker::initialize()
{
  ATH_CHECK( m_CPMTowerTool.retrieve() );
  ATH_CHECK(m_triggerTowerLocation.initialize());
  ATH_CHECK(m_cpmTowerLocation.initialize());
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


StatusCode CPMTowerMaker::execute( )
{
				      
  // Vector to store CPMTs in
  CPMTCollection* vectorOfCPMTs = new  CPMTCollection;
  ATH_CHECK(SG::makeHandle(m_cpmTowerLocation).record( std::unique_ptr<CPMTCollection>(vectorOfCPMTs) ));
  SG::ReadHandle<TriggerTowerCollection> vectorOfTTs = SG::makeHandle(m_triggerTowerLocation);
  
  // Fill a DataVector of CPMTowers using L1CPMTowerTools
  m_CPMTowerTool->makeCPMTowers(&(*vectorOfTTs), &(*vectorOfCPMTs), true);
  ATH_MSG_DEBUG( vectorOfCPMTs->size()<<" CPMTowers have been generated");

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG ( "Formed " << vectorOfCPMTs->size() << " CPM Towers " ) ;
    for (const CPMTower* cpmt : *vectorOfCPMTs)
      ATH_MSG_DEBUG ( "CPMT has coords (" << cpmt->eta() << ", "<< cpmt->phi() << ") and energies : "
                      << static_cast<unsigned int>(cpmt->emEnergy()) << ", "<< static_cast<unsigned int>(cpmt->hadEnergy()) <<" (Em,Had)" );

  }
  
  return StatusCode::SUCCESS;
  
}//end execute

} // end of namespace bracket


