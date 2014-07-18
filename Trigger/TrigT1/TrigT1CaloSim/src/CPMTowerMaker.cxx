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
#include "TrigT1CaloSim/CPMTowerMaker.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloEvent/CPMTower_ClassDEF.h"



namespace LVL1 {

/** This is the constructor for JEMaker and is where you define the relevant
    parameters.
    Currently these are :
    - "TriggerTowerLocation" : the location of the jes in Storegate You shouldn't have to touch this.
    - "CPMTowerLocation" : the location of the CPMT in StoreGate. You shouldn't have to touch this.

  Alter the values of these in jobOptions.txt
*/
  
CPMTowerMaker::CPMTowerMaker( const std::string& name, ISvcLocator* pSvcLocator ) 
  : Algorithm( name, pSvcLocator ), 
    m_storeGate("StoreGateSvc", name)
{
  m_triggerTowerLocation     = TrigT1CaloDefs::TriggerTowerLocation ;
  m_cpmTowerLocation         = TrigT1CaloDefs::CPMTowerLocation;

  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty( "TriggerTowerLocation", m_triggerTowerLocation ) ;
  declareProperty( "CPMTowerLocation", m_cpmTowerLocation ) ;
}
  
CPMTowerMaker::~CPMTowerMaker() {
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Destructor called" << endreq;
} 


  /** the initialise() method is called at the start of processing, so we set up any histograms
      etc. here*/
StatusCode CPMTowerMaker::initialize()
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

   return StatusCode::SUCCESS ;
   
}


/** the finalise() method is called at the end of processing, so it is used
for deleting histograms and general tidying up*/
StatusCode CPMTowerMaker::finalize()
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


StatusCode CPMTowerMaker::execute( )
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
				     
  // Retrieve TriggerTowers from TES
  const TTCollection* triggerTowers = 0;
  if (m_storeGate->contains<TriggerTowerCollection>(m_triggerTowerLocation)) {
    sc = m_storeGate->retrieve(triggerTowers, m_triggerTowerLocation);
  
    // Check success, return if not, with optional printout
    if (sc != StatusCode::SUCCESS) {
      log << MSG::WARNING << "No TriggerTowers found in TES at "
          << m_triggerTowerLocation << endreq ;
      triggerTowers = 0;
    }
    if (triggerTowers) {
      if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG
          << "Got " << triggerTowers->size() << " Trigger Towers from TES"
          << endreq ;
 
      if (triggerTowers->size() == 0) log << MSG::WARNING << "REGTEST: Empty TriggerTowerContainer - there must be some problem" << endreq;
    }
  }
  else log << MSG::WARNING << "No TriggerTowerContainer at " << m_triggerTowerLocation << endreq;
 
  // Vector to store CPMTs in
  CPMTCollection* vectorOfCPMTs = new  CPMTCollection; 
  
  // Step through TTs, form CPMTs and add to vector
  if (triggerTowers) {
    CPMTower* cpmTower;
    TTCollection::const_iterator it ;
    for ( it  = triggerTowers->begin(); it < triggerTowers->end(); ++it ){
      double eta=(*it)->eta();
      // Only use towers within CP system coverage
      if (fabs(eta) < 2.5) {
        double phi=(*it)->phi();
        std::vector<int> emEt = (*it)->emLUT();
        std::vector<int> hadEt = (*it)->hadLUT();
        std::vector<int> emError;
        emError.push_back((*it)->emError());
        std::vector<int> hadError;
        hadError.push_back((*it)->hadError());
        int peak = (*it)->emPeak();
    
        cpmTower = new CPMTower(phi, eta, emEt, emError, hadEt, hadError, peak);
	  
        if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "CPMT has coords (" << phi 
            <<", "<< eta << ") and energies : "<< emEt <<", "<< hadEt <<" (Em,Had)" << endreq;
    
        vectorOfCPMTs->push_back(cpmTower);
      }
    }
  }

  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG
      << "Formed " << vectorOfCPMTs->size() << " CPM Towers "
      << endreq ;
      
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


