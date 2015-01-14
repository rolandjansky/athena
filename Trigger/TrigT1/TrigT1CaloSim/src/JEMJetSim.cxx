
// /***************************************************************************
//                           JEMJetSim.cxx  -  description
//                              -------------------
//     begin                : Wed Mar 12 2014
//     copyright            : (C) 2014 by Alan Watson
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//
//================================================
// JEMJetSim class Implementation
// ================================================
//
//
//

// Utilities

// This algorithm includes
#include "TrigT1CaloSim/JEMJetSim.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/JetElementCollection.h"
#include "TrigT1CaloEvent/JEMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/JetCMXData_ClassDEF.h"
#include "TrigT1CaloEvent/JetInput.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"



namespace LVL1{

using namespace TrigConf;

//--------------------------------
// Constructors and destructors
//--------------------------------

JEMJetSim::JEMJetSim
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm( name, pSvcLocator ), 
      m_allTOBs(0),
      m_JetCMXData(0),
      m_storeGate("StoreGateSvc", name), 
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_JetTool("LVL1::L1JEMJetTools/L1JEMJetTools")
{
    m_JEMTobRoILocation      = TrigT1CaloDefs::JEMTobRoILocation;
    m_JetCMXDataLocation     = TrigT1CaloDefs::JetCMXDataLocation;
    m_JetElementLocation     = TrigT1CaloDefs::JetElementLocation;

    // This is how you declare the paramembers to Gaudi so that
    // they can be over-written via the job options file
    
    declareProperty( "EventStore",m_storeGate,"StoreGate Service");
    declareProperty( "JEMTOBRoILocation",       m_JEMTobRoILocation );
    declareProperty( "JetCMXDataLocation",       m_JetCMXDataLocation );
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty( "JetElementLocation", m_JetElementLocation ) ;

}

// Destructor
JEMJetSim::~JEMJetSim() {
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Destructor called" << endreq;
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode JEMJetSim::initialize()
{

  // We must here instantiate items which can only be made after
  // any job options have been set
  MsgStream log( messageService(), name() ) ;
  int outputLevel = msgSvc()->outputLevel( name() );

     //
    // Connect to the LVL1ConfigSvc for the trigger configuration:
    //
  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_configSvc.typeAndName() 
        << endreq;
    return sc;
  } else if (outputLevel <= MSG::DEBUG) {
    log << MSG::DEBUG << "Connected to " << m_configSvc.typeAndName() 
        << endreq;
  }

  // Now connect to the StoreGateSvc

  sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName() 
        << endreq;
    return sc;
  } else if (outputLevel <= MSG::DEBUG) {
    log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName() 
        << endreq;
  }

  // Retrieve L1CPMTool
  sc = m_JetTool.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Problem retrieving JetTool. Abort execution" << endreq;
    return StatusCode::SUCCESS;
  }
    
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode JEMJetSim::beginRun()
{

  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) printTriggerMenu();

  return StatusCode::SUCCESS ;
}



//---------------------------------
// finalise()
//---------------------------------

StatusCode JEMJetSim::finalize()
{
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Finalizing" << endreq;
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode JEMJetSim::execute( )
{
    
  /* 
    Retrieve collection of JetElements 
    Fill a std::map of JetInputs from them
    Loop over crates/modules
      For each crate, loop over towers and find TOBs 
                      form backplane data objects
                      form daq objects    
  */

  //make a message logging stream

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "starting JEMJetSim" << endreq; 

  // Create containers for this event
  m_JetCMXData   = new DataVector<JetCMXData>;  //Container of backplane data objects
  m_allTOBs      = new DataVector<JEMTobRoI>;  // Container to hold all TOB RoIs in event

  // Retrieve the JetElementContainer
  if (m_storeGate->contains<JetElementCollection>(m_JetElementLocation)) {
    const DataVector<JetElement>* storedJEs;
    StatusCode sc = m_storeGate->retrieve(storedJEs,m_JetElementLocation);
    if ( sc==StatusCode::SUCCESS ) {
       // Check size of JetElementCollection - zero would indicate a problem
      if (storedJEs->size() == 0)
         log << MSG::WARNING << "Empty JetElementContainer - looks like a problem" << endreq;
	
      // Form JetInputs and put them in a map
      std::map<int, JetInput*>* inputMap = new std::map<int, JetInput*> ;
      m_JetTool->mapJetInputs(storedJEs, inputMap);
	 
      // Loop over crates and modules
      for (int iCrate = 0; iCrate < 2; ++iCrate) {
	for (int iModule = 0; iModule < 16; ++iModule) {
	    
	   // For each module, find TOBs and backplane data
	   std::vector<unsigned int> jetCMXData;
	   m_JetTool->findJEMResults(inputMap,iCrate,iModule,m_allTOBs,jetCMXData);
	   // Push backplane data into output DataVectors
	   JetCMXData* bpData = new JetCMXData(iCrate,iModule,jetCMXData);
	   m_JetCMXData -> push_back(bpData);
	   
	} // loop over modules
      } // loop over crates
  
      /// Clean up JetInputs
      for (std::map<int, JetInput*>::iterator it = inputMap->begin(); it != inputMap->end(); ++it) {
        delete (*it).second;
      }
      delete inputMap;
  
    } // found TriggerTowers
    
    else log << MSG::WARNING << "Error retrieving JetElements" << endreq;
  }
  else log << MSG::WARNING << "No JetElementContainer at " << m_JetElementLocation << endreq;
  
       
  // Store module readout and backplane results in the TES
  storeBackplaneTOBs();
  storeModuleRoIs();
  
  // Clean up at end of event
  m_JetCMXData  = 0;
  m_allTOBs    = 0;

  return StatusCode::SUCCESS ;
}

/** place backplane data objects (CPM -> CMX) in StoreGate */
void LVL1::JEMJetSim::storeBackplaneTOBs() {

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
    
  // Store backplane data objects
  StatusCode sc = m_storeGate->overwrite(m_JetCMXData, m_JetCMXDataLocation,true,false,false);

  if (sc.isSuccess()) {
     if (outputLevel <= MSG::VERBOSE)
         log << MSG::VERBOSE << "Stored " << m_JetCMXData->size()
             << " JetCMXData at " << m_JetCMXDataLocation << endreq;
  }
  else {
     log << MSG::ERROR << "failed to write JetCMXData to  "
         << m_JetCMXDataLocation << endreq;
  } 
    
  return;

} //end storeBackplaneTOBs


/** place final ROI objects in the TES. */
void LVL1::JEMJetSim::storeModuleRoIs() {

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
    
  StatusCode sc = m_storeGate->overwrite(m_allTOBs, m_JEMTobRoILocation,true,false,false);

  if (sc.isSuccess()) {
     if (outputLevel <= MSG::VERBOSE)
         log << MSG::VERBOSE << "Stored " << m_allTOBs->size()
             << " Jet TOBs at " << m_JEMTobRoILocation << endreq;
  }
  else {
     log << MSG::ERROR << "failed to write JEMTobRoIs to  "
         << m_JEMTobRoILocation << endreq;
  } 

  return;

} //end storeModuleRoIs


} // end of LVL1 namespace bracket


/** print trigger configuration, for debugging purposes */
void LVL1::JEMJetSim::printTriggerMenu(){
  MsgStream log( messageService(), name() );
  
  /** This is all going to need updating for the new menu structure.
      Comment out in the meanwhile 
  
  L1DataDef def;

  unsigned int numThresh=0;  
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.jetType() ||
         (*it)->type() == def.fjetType()  ||
         (*it)->type() == def.jfType()  ||
         (*it)->type() == def.jbType()) {
      log << MSG::DEBUG << "TriggerThreshold " << (*it)->id() << " has name " << (*it)->name() << endreq
          << "  threshold number " << (*it)->thresholdNumber() << endreq
          << "  number of values = " << (*it)->numberofValues() << endreq;
      for (std::vector<TrigConf::TriggerThresholdValue*>::const_iterator tv = (*it)->thresholdValueVector().begin();
           tv != (*it)->thresholdValueVector().end(); ++tv) {
        TrigConf::JetThresholdValue* jtv;
        jtv = dynamic_cast<JetThresholdValue*> (*tv);
        if (!jtv) {
          log << MSG::ERROR << "Threshold type name is Jet, but is not a JetThreshold object!" << endreq;
          continue;
        }
        log << MSG::DEBUG << "JetThresholdValue: " << endreq
            << "  Type = " << (*it)->type() << endreq
            << "  Threshold value = " << jtv->thresholdValueCount() << endreq
            << "  Cluster size = " << jtv->window() << endreq
            << "  EtaMin = " << jtv->etamin() << ", EtaMax = " << jtv->etamax() << endreq;
        
      } // end of loop over threshold values
      numThresh++;
    } //  is type == jet or forward jet
  } // end of loop over thresholds

  return;
  
  */
    
}


