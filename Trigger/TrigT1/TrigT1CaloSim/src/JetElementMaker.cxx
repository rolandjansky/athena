/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// JetElementMaker class Implementation
// ================================================
//
//
//


// This algorithm includes
#include "TrigT1CaloSim/JetElementMaker.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloEvent/JetElement_ClassDEF.h"


namespace LVL1 {

/** This is the constructor for JEMaker and is where you define the relevant
    parameters.
    Currently these are :
    - "TriggerTowerLocation" : the location of the jes in Storegate You shouldn't have to touch this.
    - "JetElementLocation" : the location of the JE in StoreGate. You shouldn't have to touch this.

  Alter the values of these in jobOptions.txt
*/
  
JetElementMaker::JetElementMaker( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm( name, pSvcLocator ), 
    m_JetElementTool("LVL1::L1JetElementTools/L1JetElementTools")
{
  m_triggerTowerLocation = TrigT1CaloDefs::TriggerTowerLocation ;
  m_jetElementLocation   = TrigT1CaloDefs::JetElementLocation;

  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty( "TriggerTowerLocation", m_triggerTowerLocation ) ;
  declareProperty( "JetElementLocation", m_jetElementLocation ) ;
}
  

  /** the initialise() method is called at the start of processing, so we set up any histograms
      etc. here*/
StatusCode JetElementMaker::initialize()
{
  ATH_MSG_INFO ( "Initialising" );
  ATH_CHECK( m_JetElementTool.retrieve() );
  return StatusCode::SUCCESS ;
}


/** the finalise() method is called at the end of processing, so it is used
for deleting histograms and general tidying up*/
StatusCode JetElementMaker::finalize()
{
  
  ATH_MSG_INFO( "Finalizing" );
  return StatusCode::SUCCESS ;
}


/**----------------------------------------------
   execute() method called once per event
   ----------------------------------------------

Checks that the Cell Type is supported (terminates with errors if not)
and calls relevant routine to look for the cells.
*/


StatusCode JetElementMaker::execute( )
{
  ATH_MSG_DEBUG ( "Executing" );

  // What we are (hopefully) going to make:
  JECollection* vectorOfJEs = new JECollection;
	
  // Retrieve TriggerTowers from StoreGate 
  if (evtStore()->contains<TriggerTowerCollection>(m_triggerTowerLocation)) {
    const DataVector<TriggerTower>* vectorOfTTs;
    StatusCode sc = evtStore()->retrieve(vectorOfTTs, m_triggerTowerLocation);
    if (sc.isSuccess()) {
      // Fill a DataVector of JetElements using L1JetElementTools
      m_JetElementTool->makeJetElements(vectorOfTTs, vectorOfJEs);
      ATH_MSG_DEBUG( vectorOfJEs->size()<<" JetElements have been generated");
    }
    else ATH_MSG_WARNING( "Failed to retrieve TriggerTowers from " << m_triggerTowerLocation );
  }
  else ATH_MSG_WARNING( "No TriggerTowerContainer at " << m_triggerTowerLocation );
  
  // Save JetElements in the TES
  StatusCode sc = evtStore()->overwrite(vectorOfJEs, m_jetElementLocation, true);
  if (sc.isFailure())
    ATH_MSG_WARNING( "Failed to write JetElements to TES at " << m_jetElementLocation );
																	 
  // and we're done
  vectorOfJEs=0;
  return StatusCode::SUCCESS;
}//end execute

} // end of namespace bracket

