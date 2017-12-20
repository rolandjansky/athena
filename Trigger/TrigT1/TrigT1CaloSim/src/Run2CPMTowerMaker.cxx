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
#include "TrigT1CaloSim/Run2CPMTowerMaker.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/CPMTower_ClassDEF.h"



namespace LVL1 {

/** This is the constructor for JEMaker and is where you define the relevant
    parameters.
    Currently these are :
    - "TriggerTowerLocation" : the location of the jes in Storegate You shouldn't have to touch this.
    - "CPMTowerLocation" : the location of the CPMT in StoreGate. You shouldn't have to touch this.

  Alter the values of these in jobOptions.txt
*/
  
Run2CPMTowerMaker::Run2CPMTowerMaker( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm( name, pSvcLocator ), 
    m_CPMTowerTool("LVL1::L1CPMTowerTools/L1CPMTowerTools")
{
  m_triggerTowerLocation     = TrigT1CaloDefs::xAODTriggerTowerLocation;
  m_cpmTowerLocation         = TrigT1CaloDefs::CPMTowerLocation;

  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty( "TriggerTowerLocation", m_triggerTowerLocation ) ;
  declareProperty( "CPMTowerLocation", m_cpmTowerLocation ) ;
}
  

  /** the initialise() method is called at the start of processing, so we set up any histograms
      etc. here*/
StatusCode Run2CPMTowerMaker::initialize()
{
  // We must here instantiate items which can only be made after
  // any job options have been set

  ATH_MSG_INFO ( "Initialising" ) ;
  ATH_CHECK( m_CPMTowerTool.retrieve() );
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
  ATH_MSG_DEBUG  ( "looking for trigger towers at "
                   << m_triggerTowerLocation ) ;
				      
  // Vectors to store CPMTs in
  CPMTCollection*    vectorOfCPMTs = new  CPMTCollection;
  CPMTAuxCollection* cpmtAuxVector = new  CPMTAuxCollection;
  vectorOfCPMTs->setStore(cpmtAuxVector);
  
  // Retrieve TriggerTowers from StoreGate 
  if (evtStore()->contains<xAOD::TriggerTowerContainer>(m_triggerTowerLocation)) {
    const DataVector<xAOD::TriggerTower>* vectorOfTTs;
    StatusCode sc = evtStore()->retrieve(vectorOfTTs, m_triggerTowerLocation);
    if (sc.isSuccess()) {
      // Fill a DataVector of CPMTowers using L1CPMTowerTools
      m_CPMTowerTool->makeCPMTowers(vectorOfTTs, vectorOfCPMTs, true);
      ATH_MSG_DEBUG( vectorOfCPMTs->size()<<" CPMTowers have been generated") ;
    }
    else ATH_MSG_WARNING("Failed to retrieve TriggerTowers from " << m_triggerTowerLocation );
  }
  else ATH_MSG_WARNING("No TriggerTowerContainer at " << m_triggerTowerLocation );

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG ( "Formed " << vectorOfCPMTs->size() << " CPM Towers " )  ;
    
    CPMTCollection::const_iterator itCPMT;
    for (itCPMT = vectorOfCPMTs->begin(); itCPMT != vectorOfCPMTs->end(); ++itCPMT)
      ATH_MSG_DEBUG("CPMT has coords (" << (*itCPMT)->eta() <<", "<< (*itCPMT)->phi() << ") and energies : "
          << (*itCPMT)->emEnergy() <<", "<< (*itCPMT)->hadEnergy() <<" (Em,Had)" );
  }
      
  // Finally, store CPMTs and we are done
  CHECK( evtStore()->record( cpmtAuxVector, m_cpmTowerLocation + "Aux." ) );
  CHECK( evtStore()->record( vectorOfCPMTs, m_cpmTowerLocation ) );
  
  // Report success for debug purposes
  ATH_MSG_DEBUG("Stored CPM Towers in TES at "<< m_cpmTowerLocation );

  
  // Report success in any case, or else job will terminate
  vectorOfCPMTs=0;
  cpmtAuxVector=0;
  return StatusCode::SUCCESS;
  
}//end execute

} // end of namespace bracket


