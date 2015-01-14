
// /***************************************************************************
//                           JEMEnergySim.cxx  -  description
//                              -------------------
//     begin                : Monday 12 May 2014
//     copyright            : (C) 2014 by Alan Watson
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//================================================
// JEMEnergySim class Implementation
// ================================================
//
//
//

// Utilities
#include <cmath>

#include "GaudiKernel/MsgStream.h"

// This algorithm includes
#include "TrigT1CaloSim/JEMEnergySim.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h" 
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/JetElementCollection.h"
#include "TrigT1CaloEvent/JEMEtSums_ClassDEF.h"
#include "TrigT1CaloEvent/EnergyCMXData_ClassDEF.h"
#include "TrigT1CaloUtils/ModuleEnergy.h"


namespace LVL1 {


//--------------------------------
// Constructors and destructors
//--------------------------------

JEMEnergySim::JEMEnergySim
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm( name, pSvcLocator ), 
      m_storeGate("StoreGateSvc", name),
      m_EtTool("LVL1::L1EtTools/L1EtTools"),
      m_jemContainer(0)
{
  m_JetElementLocation      = TrigT1CaloDefs::JetElementLocation;
  m_jemEtSumsLocation       = TrigT1CaloDefs::JEMEtSumsLocation;
  m_energyCMXDataLocation   = TrigT1CaloDefs::EnergyCMXDataLocation;
  
  // This is how you declare the paramembers to Gaudi so that
  // they can be over-written via the job options file

  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty( "JetElementLocation",    m_JetElementLocation ) ;
  declareProperty( "JEMEtSumsLocation",     m_jemEtSumsLocation );
  declareProperty( "EnergyCMXDataLocation", m_energyCMXDataLocation );
}

// Destructor
JEMEnergySim::~JEMEnergySim() {
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Destructor called" << endreq;
	
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode JEMEnergySim::initialize()
{

  // We must here instantiate items which can only be made after
  // any job options have been set
  MsgStream log( messageService(), name() ) ;

  //
  // Connect to StoreGate:
  //
  StatusCode sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName() 
        << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName() 
        << endreq;
  }

  // Retrieve L1EtTool tool
  sc = m_EtTool.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Problem retrieving EtTool. Abort execution" << endreq;
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode JEMEnergySim::beginRun()
{

  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise()
//---------------------------------

StatusCode JEMEnergySim::finalize()
{
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Finalizing" << endreq;
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//

StatusCode JEMEnergySim::execute( )
{
  //make a message logging stream

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Executing" << endreq;

  // form module sums
  m_jemContainer = new DataVector<ModuleEnergy>;  
  if (m_storeGate->contains<JetElementCollection>(m_JetElementLocation)) {
    const DataVector<JetElement>* jetelements;
  
    StatusCode sc = m_storeGate->retrieve(jetelements,m_JetElementLocation);
    if ( sc==StatusCode::SUCCESS ) {
      // Warn if we find an empty container
      if (jetelements->size() == 0)
        log << MSG::WARNING << "Empty JetElementContainer - looks like a problem" << endreq;
    
      m_EtTool->moduleSums(jetelements, m_jemContainer);
    }
    else log << MSG::WARNING << "Error retrieving JetElements" << endreq;
  }
  else log << MSG::WARNING << "No JetElementCollection at " << m_JetElementLocation << endreq;
    
  // Done the processing. Now form & save the various output data

  // For CMX simulation
  storeBackplaneData();
  
  // for Bytestream simulation
  storeJEMEtSums();
  
  cleanup();
 
  return StatusCode::SUCCESS ;
}


} // end of LVL1 namespace bracket

/** delete pointers etc. */
void LVL1::JEMEnergySim::cleanup(){

  delete m_jemContainer;
}

/** Form JEMEtSums and put into SG */
void LVL1::JEMEnergySim::storeJEMEtSums() {
  
  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG<<"storeJEMEtSums running"<<endreq;
  
  JEMEtSumsCollection* JEMRvector = new  JEMEtSumsCollection;

  DataVector<ModuleEnergy>::iterator it;
  for ( it=m_jemContainer->begin(); it!=m_jemContainer->end(); ++it ) {
    std::vector<unsigned int> eX;
    eX.push_back((*it)->ex());
    std::vector<unsigned int> eY;
    eY.push_back((*it)->ey());
    std::vector<unsigned int> eT;
    eT.push_back((*it)->et());
    JEMEtSums* jemEtSums = new JEMEtSums((*it)->crate(), (*it)->module(),eT,eX,eY,0);
    JEMRvector->push_back(jemEtSums);
  }

  if (outputLevel <= MSG::DEBUG) log <<MSG::DEBUG<< JEMRvector->size()<<" JEMEtSums objects are being saved"<<endreq;
  
  StatusCode sc = m_storeGate->overwrite(JEMRvector, m_jemEtSumsLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) log << MSG::ERROR << "Error registering JEMEtSums collection in TDS " << endreq;
  else {
    StatusCode sc2 = m_storeGate->setConst(JEMRvector);
    if (sc2 != StatusCode::SUCCESS) log << MSG::ERROR << "error setting JEMResult vector constant" << endreq;
  }
  
  return;
}

/** Form EnergyCMXData and put into SG */
void LVL1::JEMEnergySim::storeBackplaneData() {
  
  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG<<"storeBackplaneData running"<<endreq;
  
  EnergyCMXDataCollection* bpVector = new  EnergyCMXDataCollection;

  DataVector<ModuleEnergy>::iterator it;
  for ( it=m_jemContainer->begin(); it!=m_jemContainer->end(); ++it ) {
    EnergyCMXData* bpData = new EnergyCMXData((*it)->crate(), (*it)->module(),
					      (*it)->ex(),(*it)->ey(),(*it)->et());
    bpVector->push_back(bpData);
  }

  if (outputLevel <= MSG::DEBUG) log <<MSG::DEBUG<< bpVector->size()<<" EnergyCMXData objects are being saved"<<endreq;
  
  StatusCode sc = m_storeGate->overwrite(bpVector, m_energyCMXDataLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) log << MSG::ERROR << "Error registering EnergyCMXData collection in TDS " << endreq;
  
  return;
}



