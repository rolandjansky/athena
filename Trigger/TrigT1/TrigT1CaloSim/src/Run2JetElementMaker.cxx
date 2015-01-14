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
#include "TrigT1CaloSim/Run2JetElementMaker.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TrigT1CaloEvent/JetElement_ClassDEF.h"


namespace LVL1 {

/** This is the constructor for JEMaker and is where you define the relevant
    parameters.
    Currently these are :
    - "TriggerTowerLocation" : the location of the jes in Storegate You shouldn't have to touch this.
    - "JetElementLocation" : the location of the JE in StoreGate. You shouldn't have to touch this.

  Alter the values of these in jobOptions.txt
*/
  
Run2JetElementMaker::Run2JetElementMaker( const std::string& name, ISvcLocator* pSvcLocator ) 
  : Algorithm( name, pSvcLocator ), 
    m_storeGate("StoreGateSvc", name),
    m_JetElementTool("LVL1::L1JetElementTools/L1JetElementTools")
{
  m_triggerTowerLocation = "xAODTriggerTowers" ;
  m_jetElementLocation   = TrigT1CaloDefs::JetElementLocation;

  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty( "TriggerTowerLocation", m_triggerTowerLocation ) ;
  declareProperty( "JetElementLocation", m_jetElementLocation ) ;
}
  
Run2JetElementMaker::~Run2JetElementMaker() {
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Destructor called" << endreq;
} 


  /** the initialise() method is called at the start of processing, so we set up any histograms
      etc. here*/
StatusCode Run2JetElementMaker::initialize()
{
  // We must here instantiate items which can only be made after
  // any job options have been set

   MsgStream log( messageService(), name() ) ;
   int outputLevel = msgSvc()->outputLevel( name() );
   log << MSG::INFO << "Initialising" << endreq;

   StatusCode sc = m_storeGate.retrieve();
   if ( sc.isFailure() ) {
     log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName() 
         << endreq;
     return sc;
   } else {
     if (outputLevel <= MSG::DEBUG)
        log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName() << endreq;
   }
   
  // Retrieve L1JetElementTool
  sc = m_JetElementTool.retrieve();
  if (sc.isFailure())
    log << MSG::ERROR << "Problem retrieving JetElementTool. There will be trouble." << endreq;
  
   return StatusCode::SUCCESS ;
   
}


/** the finalise() method is called at the end of processing, so it is used
for deleting histograms and general tidying up*/
StatusCode Run2JetElementMaker::finalize()
{
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Finalizing" << endreq;
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

  //................................
  // make a message logging stream

  MsgStream log( messageService(), name() ) ;
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Executing" << endreq;

  // What we are (hopefully) going to make:
  JECollection* vectorOfJEs = new JECollection;
	
  // Retrieve TriggerTowers from StoreGate 
  if (m_storeGate->contains<xAOD::TriggerTowerContainer>(m_triggerTowerLocation)) {
    const DataVector<xAOD::TriggerTower>* vectorOfTTs;
    StatusCode sc = m_storeGate->retrieve(vectorOfTTs, m_triggerTowerLocation);
    if (sc.isSuccess()) {
      // Fill a DataVector of JetElements using L1JetElementTools
      m_JetElementTool->makeJetElements(vectorOfTTs, vectorOfJEs);
      if (outputLevel <= MSG::DEBUG)
         log <<MSG::DEBUG<< vectorOfJEs->size()<<" JetElements have been generated"<<endreq;
    }
    else log << MSG::WARNING << "Failed to retrieve TriggerTowers from " << m_triggerTowerLocation << endreq;
  }
  else log << MSG::WARNING << "No TriggerTowerContainer at " << m_triggerTowerLocation << endreq;
  
  // Save JetElements in the TES
  StatusCode sc = m_storeGate->record(vectorOfJEs, m_jetElementLocation);
  if (sc.isFailure())
    log << MSG::WARNING << "Failed to write JetElements to TES at " << m_jetElementLocation << endreq;
																	 
  // and we're done
  vectorOfJEs=0;
  return StatusCode::SUCCESS;
}//end execute

} // end of namespace bracket

