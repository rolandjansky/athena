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
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
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
  : Algorithm( name, pSvcLocator ), 
    m_storeGate("StoreGateSvc", name),
    m_CPMTowerTool("LVL1::L1CPMTowerTools/L1CPMTowerTools")
{
  m_triggerTowerLocation     = "xAODTriggerTowers" ;
  m_cpmTowerLocation         = TrigT1CaloDefs::CPMTowerLocation;

  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty( "TriggerTowerLocation", m_triggerTowerLocation ) ;
  declareProperty( "CPMTowerLocation", m_cpmTowerLocation ) ;
}
  
Run2CPMTowerMaker::~Run2CPMTowerMaker() {
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Destructor called" << endreq;
} 


  /** the initialise() method is called at the start of processing, so we set up any histograms
      etc. here*/
StatusCode Run2CPMTowerMaker::initialize()
{
  // We must here instantiate items which can only be made after
  // any job options have been set

   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Initialising" << endreq;

   StatusCode sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName() 
        << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName() 
        << endreq;
  }
   
  // Retrieve L1CPMTowerTool
  sc = m_CPMTowerTool.retrieve();
  if (sc.isFailure())
    log << MSG::ERROR << "Problem retrieving CPMTowerTool. There will be trouble." << endreq;

   return StatusCode::SUCCESS ;
   
}


/** the finalise() method is called at the end of processing, so it is used
for deleting histograms and general tidying up*/
StatusCode Run2CPMTowerMaker::finalize()
{
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Finalizing" << endreq;
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

  //................................
  // make a message logging stream

  MsgStream log( messageService(), name() ) ;
  int outputLevel = msgSvc()->outputLevel( name() );
  StatusCode sc;
	
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Executing" << endreq;

  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG 
                                     << "looking for trigger towers at "
				     << m_triggerTowerLocation << endreq;
				      
  // Vector to store CPMTs in
  CPMTCollection* vectorOfCPMTs = new  CPMTCollection;
  
  // Retrieve TriggerTowers from StoreGate 
  if (m_storeGate->contains<xAOD::TriggerTowerContainer>(m_triggerTowerLocation)) {
    const DataVector<xAOD::TriggerTower>* vectorOfTTs;
    StatusCode sc = m_storeGate->retrieve(vectorOfTTs, m_triggerTowerLocation);
    if (sc.isSuccess()) {
      // Fill a DataVector of CPMTowers using L1CPMTowerTools
      m_CPMTowerTool->makeCPMTowers(vectorOfTTs, vectorOfCPMTs, true);
      if (outputLevel <= MSG::DEBUG)
         log <<MSG::DEBUG<< vectorOfCPMTs->size()<<" CPMTowers have been generated"<<endreq;
    }
    else log << MSG::WARNING << "Failed to retrieve TriggerTowers from " << m_triggerTowerLocation << endreq;
  }
  else log << MSG::WARNING << "No TriggerTowerContainer at " << m_triggerTowerLocation << endreq;

  if (outputLevel <= MSG::DEBUG) {
    log << MSG::DEBUG
        << "Formed " << vectorOfCPMTs->size() << " CPM Towers "
        << endreq ;
    
    CPMTCollection::const_iterator itCPMT;
    for (itCPMT = vectorOfCPMTs->begin(); itCPMT != vectorOfCPMTs->end(); ++itCPMT)
      log << MSG::DEBUG << "CPMT has coords (" << (*itCPMT)->eta() <<", "<< (*itCPMT)->phi() << ") and energies : "
          << (*itCPMT)->emEnergy() <<", "<< (*itCPMT)->hadEnergy() <<" (Em,Had)" << endreq;

  }
      
  // Finally, store CPMTs and we are done
  sc = m_storeGate->overwrite(vectorOfCPMTs, m_cpmTowerLocation,true,false,false);
  
  // Report success or failure
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Error registering CPM Tower collection in TES " << endreq;
  }
  else if (outputLevel <= MSG::DEBUG) {
    log << MSG::DEBUG << "Stored CPM Towers in TES at "<< m_cpmTowerLocation << endreq;
  }
  
  // Report success in any case, or else job will terminate
  vectorOfCPMTs=0;
  return StatusCode::SUCCESS;
  
}//end execute

} // end of namespace bracket


