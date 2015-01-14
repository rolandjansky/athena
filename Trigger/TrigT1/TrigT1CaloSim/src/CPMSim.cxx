
// /***************************************************************************
//                           CPMSim.cxx  -  description
//                              -------------------
//     begin                : Wed Mar 05 2014
//     copyright            : (C) 2014 by Alan Watson
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//
//================================================
// CPMSim class Implementation
// ================================================
//
//
//

// Utilities

// This algorithm includes
#include "TrigT1CaloSim/CPMSim.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/CPMTowerCollection.h"
#include "TrigT1CaloEvent/CPMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/CPMCMXData_ClassDEF.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
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

CPMSim::CPMSim
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm( name, pSvcLocator ), 
      m_allTOBs(0),
      m_CMXData(0),
      m_storeGate("StoreGateSvc", name), 
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_CPMTool("LVL1::L1CPMTools/L1CPMTools"),
      m_eventNumber(0)
{
    m_CPMTobRoILocation      = TrigT1CaloDefs::CPMTobRoILocation;
    m_CPMCMXDataLocation     = TrigT1CaloDefs::CPMCMXDataLocation ;
    m_CPMTowerLocation       = TrigT1CaloDefs::CPMTowerLocation;
    m_emTauSlinkLocation     = TrigT1CaloDefs::EmTauSlinkLocation;

    // This is how you declare the paramembers to Gaudi so that
    // they can be over-written via the job options file
    
    declareProperty( "EventStore",m_storeGate,"StoreGate Service");
    declareProperty( "CPMTOBRoILocation",       m_CPMTobRoILocation );
    declareProperty( "CPMCMXDataLocation",      m_CPMCMXDataLocation );
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty( "CPMTowerLocation", m_CPMTowerLocation ) ;
    declareProperty( "EmTauSlinkLocation",   m_emTauSlinkLocation );

       
    /** Clear SLink output vectors (RoI output) */
    for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
      m_CPRoIROD[i]=0;
    }

}

// Destructor
CPMSim::~CPMSim() {
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Destructor called" << endreq;
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode CPMSim::initialize()
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
  sc = m_CPMTool.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Problem retrieving CPMTool. Abort execution" << endreq;
    return StatusCode::SUCCESS;
  }
    
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode CPMSim::beginRun()
{

  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) printTriggerMenu();

  return StatusCode::SUCCESS ;
}



//---------------------------------
// finalise()
//---------------------------------

StatusCode CPMSim::finalize()
{
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Finalizing" << endreq;
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode CPMSim::execute( )
{
    
  /* Retrieve collection of CPMTowers
     Form TOBs from them
     Form backplane data from TOBs
     Store TOBs for BS simulation
     Form and store RoI SLink data for RoIB
  */

  //make a message logging stream

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "starting CPMSim" << endreq; 

  // For RoI output SLink record
  const EventInfo* evt;
  if (StatusCode::SUCCESS == m_storeGate->retrieve(evt)){
    m_eventNumber = evt->event_ID()->event_number();
  }else{
    log << MSG::ERROR<< " Unable to retrieve EventInfo from StoreGate "<< endreq;
  }

  // Create containers for this event
  m_CMXData    = new DataVector<CPMCMXData>;  //Container of backplane data objects 
  m_allTOBs    = new DataVector<CPMTobRoI>;  // Container to hold all TOB RoIs in event

  // Retrieve the CPMTowerContainer
  if (m_storeGate->contains<CPMTowerCollection>(m_CPMTowerLocation)) {
    const DataVector<CPMTower>* storedCPMTs;
    StatusCode sc = m_storeGate->retrieve(storedCPMTs, m_CPMTowerLocation);  
    if ( sc==StatusCode::SUCCESS ) {
       // Check size of TriggerTowerCollection - zero would indicate a problem
      if (storedCPMTs->size() == 0)
         log << MSG::WARNING << "Empty CPMTowerContainer - looks like a problem" << endreq;
	
      // Map the CPMTs
      std::map<int, CPMTower*>* towerMap = new std::map<int, CPMTower*>;
      m_CPMTool->mapTowers(storedCPMTs, towerMap);
	 
      // Loop over crates and modules
      for (int iCrate = 0; iCrate < 4; ++iCrate) {
	for (int iModule = 1; iModule <= 14; ++iModule) {
	    
	   // For each module, find TOBs and backplane data
	   std::vector<unsigned int> emCMXData;
	   std::vector<unsigned int> tauCMXData;
	   m_CPMTool->findCPMResults(towerMap,iCrate,iModule,m_allTOBs,emCMXData,tauCMXData);
	   // Push backplane data into output DataVectors
	   CPMCMXData* emBPData  = new CPMCMXData(iCrate,iModule,0,emCMXData);
	   CPMCMXData* tauBPData = new CPMCMXData(iCrate,iModule,1,tauCMXData);
	   m_CMXData -> push_back(emBPData);
	   m_CMXData -> push_back(tauBPData);
	   
	} // loop over modules
      } // loop over crates
      
      /// Clean up map
      delete towerMap;
  
    } // found TriggerTowers
    
    else log << MSG::WARNING << "Error retrieving CPMTowers" << endreq;
  }
  else log << MSG::WARNING << "No CPMTowerContainer at " << m_CPMTowerLocation << endreq;
  
      
  // Store RoIB output, module readout and backplane results in the TES
  storeBackplaneTOBs();
  storeSlinkObjects(); 
  storeModuleRoIs();
  
  // Clean up at end of event
  m_CMXData  = 0;
  m_allTOBs    = 0;

  return StatusCode::SUCCESS ;
}

/** place backplane data objects (CPM -> CMX) in StoreGate */
void CPMSim::storeBackplaneTOBs() {

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
    
  // Store backplane data objects
  StatusCode sc = m_storeGate->overwrite(m_CMXData, m_CPMCMXDataLocation,true,false,false);

  if (sc.isSuccess()) {
     if (outputLevel <= MSG::VERBOSE)
         log << MSG::VERBOSE << "Stored " << m_CMXData->size()
             << " CPMCMXData at " << m_CPMCMXDataLocation << endreq;
  }
  else {
     log << MSG::ERROR << "failed to write CPMCMXData to  "
         << m_CPMCMXDataLocation << endreq;
  } 

  return;

} //end storeBackplaneTOBs


/** place final ROI objects in the TES. */
void CPMSim::storeModuleRoIs() {

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
    
  StatusCode sc = m_storeGate->overwrite(m_allTOBs, m_CPMTobRoILocation,true,false,false);

  if (sc.isSuccess()) {
     if (outputLevel <= MSG::VERBOSE)
         log << MSG::VERBOSE << "Stored " << m_allTOBs->size()
             << " EM & Tau TOBs at " << m_CPMTobRoILocation << endreq;
  }
  else {
     log << MSG::ERROR << "failed to write CPMTobRoIs to  "
         << m_CPMTobRoILocation << endreq;
  } 

  return;

} //end storeModuleRoIs

/** Simulate SLink streams to RoIB */
void CPMSim::storeSlinkObjects(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "storeSlinkObjects" << endreq;

  /// Create words and add headers
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    m_CPRoIROD[i]=new DataVector<LVL1CTP::SlinkWord>;
    addHeader(m_CPRoIROD[i],0x72,i);//set modID to crate #
  }
  
  /// Push TOBs to ROD vectors
  /// This version makes no attempt to order them.
  DataVector<CPMTobRoI>::const_iterator it = m_allTOBs->begin();
  
  for ( ; it != m_allTOBs->end(); ++it) {
    int crate = (*it)->crate();
    m_CPRoIROD[crate]->push_back( getWord((*it)->roiWord()) );
  }
  
  /// Add tails
  const unsigned int headerWords=8+1;//8 in Slink head + control word
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    addTail(m_CPRoIROD[i], (m_CPRoIROD[i]->size()-headerWords) );
  }
  
  /// Store SLink data in SG
  std::string emTauSlinkLocation[4];
  emTauSlinkLocation[0]= m_emTauSlinkLocation+"0";
  emTauSlinkLocation[1]= m_emTauSlinkLocation+"1";
  emTauSlinkLocation[2]= m_emTauSlinkLocation+"2";
  emTauSlinkLocation[3]= m_emTauSlinkLocation+"3";
  
  /** TEMPORARY: dump contents to log */
  /*
  for (unsigned int slink=0; slink<(TrigT1CaloDefs::numOfCPRoIRODs);slink++){
    log << MSG::INFO<< "Slink cable  "<<slink
        <<" has "<<(m_CPRoIROD[slink]->size())<<" words"<<endreq;
    for (DataVector<LVL1CTP::SlinkWord>::iterator i=m_CPRoIROD[slink]->begin();
                                      i!=m_CPRoIROD[slink]->end();++i){
      log <<MSG::INFO<<"Word :"<<hex<<(*i)->word()<<dec<<endreq;
    }
  }
  */

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;++i){
    StatusCode sc = m_storeGate->overwrite(m_CPRoIROD[i],emTauSlinkLocation[i],true,false,false);
    if (sc.isSuccess() ){
      log << MSG::DEBUG << "Stored EM/Tau Slink object at "<< emTauSlinkLocation[i] <<" with "
          <<(m_CPRoIROD[i]->size())<<" words"<< endreq;
    } else {
      log << MSG::ERROR << "Failed to write EM/Tau Slink object!"<< endreq;
    } // endif
  }
  
  return;
  
}

/** adds slink header as defined in DAQ-98-129*/
void CPMSim::addHeader( DataVector<LVL1CTP::SlinkWord>* slink, unsigned int subDetID, unsigned int moduleId){
    slink->push_back(getWord(0xb0f00000));      // beginning of fragment word (control word)
    slink->push_back(getWord(0xee1234ee));      // start of header marker
    slink->push_back(getWord(8));               // header length
    slink->push_back(getWord(4));               // format version number
    unsigned int moduleType=0x00;  // ROD
    slink->push_back(getWord( (moduleType<<16)+(subDetID<<8)+moduleId ));  // 1st 8b but source id: CP system
    slink->push_back(getWord(m_eventNumber));       // 8b ECR_count (set to 0) | 24b L1 id = evt number
    slink->push_back( getWord(1 ));                 // 8b BCID
    slink->push_back( getWord(1 ));                 // 8b L1 trig type
    slink->push_back( getWord(0 ));                 // detector evt type
}

/** add Slink tail */
void CPMSim::addTail(DataVector<LVL1CTP::SlinkWord>* slink, unsigned int numberOfDataWords){
  slink->push_back(getWord(0 ));                           // write status word 1
  slink->push_back(getWord(0 ));                           // write status word 2
//^^^ not simulating any errors.

  slink->push_back(getWord(2 ));                           // status block length
  slink->push_back(getWord(numberOfDataWords));            // calculated data block length
  slink->push_back(getWord(1 ));                           // status block position
// ^^^ indicates that data precedes status block
  slink->push_back(getWord(0xe0f00000 ));                  // end of fragment word
}


/** creates a new SlinkWord object with the passed word, and returns a pointer.*/
LVL1CTP::SlinkWord* CPMSim::getWord(unsigned int tword){
  return new LVL1CTP::SlinkWord(tword);
}


} // end of LVL1 namespace bracket


/** print trigger configuration, for debugging purposes */
void LVL1::CPMSim::printTriggerMenu(){
  MsgStream log( messageService(), name() );
  
  /** This is all going to need updating for the new menu structure.
      Comment out in the meanwhile 
  
  L1DataDef def;

  std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.emType() || (*it)->type() == def.tauType() ) {
      log << MSG::DEBUG << "TriggerThreshold " << (*it)->id() << " has name " << (*it)->name() << endreq
          << "  threshold number " << (*it)->thresholdNumber() << endreq
          << "  number of values = " << (*it)->numberofValues() << endreq;
      for (std::vector<TriggerThresholdValue*>::const_iterator tv = (*it)->thresholdValueVector().begin();
           tv != (*it)->thresholdValueVector().end(); ++tv) {
        ClusterThresholdValue* ctv;
        ctv = dynamic_cast<ClusterThresholdValue*> (*tv);
	if (!ctv) {
          log << MSG::ERROR << "Threshold type name is EM/Tau, but is not a ClusterThreshold object!" << endreq;
          continue;
        }
        log << MSG::DEBUG << "ClusterThresholdValue: " << endreq
            << "  Threshold value = " << ctv->thresholdValueCount() << endreq
            << "  EM isolation = " << ctv->emIsolationCount() << endreq
            << "  Had isolation = " << ctv->hadIsolationCount() << endreq
            << "  Had veto = " << ctv->hadVetoCount() << endreq
            << "  EtaMin = " << ctv->etamin() << ", EtaMax = " << ctv->etamax() << endreq;
        
      } // end of loop over threshold values
    } //  is type == em or tau?
  } // end of loop over thresholds
  
  */
    
}


