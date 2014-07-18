/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// /***************************************************************************
//                           JetTrigger.cxx  -  description
//                              -------------------
//     begin                : Wed Dec 13 2000
//     copyright            : (C) 2000 by moyse
//     email                : moyse@heppch.ph.qmw.ac.uk
//  ***************************************************************************/
//
//    updated: June 20, 2001 M. Wielers
//             move to Storegate
// 
//================================================
// JetTrigger class Implementation
// ================================================
//
//
//

// Utilities
#include <cmath>

// This algorithm includes
#include "TrigT1CaloSim/JetTrigger.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h"

#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/JetElementCollection.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "TrigT1CaloEvent/JEMHits_ClassDEF.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h"
#include "TrigT1CaloUtils/CoordToHardware.h"

namespace LVL1{


//--------------------------------
// Constructors and destructors
//--------------------------------

JetTrigger::JetTrigger
  ( const std::string& name, ISvcLocator* pSvcLocator ) 
    : Algorithm( name, pSvcLocator ), 
      m_storeGate("StoreGateSvc", name),
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_JetTool("LVL1::L1JetTools/L1JetTools"),
      m_vectorOfJetROIs(0),
      m_intROIContainer(0),
      m_jemHitsContainer(0)
{
  m_jetROIOutputLocation       = TrigT1CaloDefs::JetROILocation;
  m_jemHitsLocation            = TrigT1CaloDefs::JEMHitsLocation ;
  m_JetElementLocation         = TrigT1CaloDefs::JetElementLocation;


  // This is how you declare the paramembers to Gaudi so that
  // they can be over-written via the job options file

  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty( "JetROIOutputLocation",       m_jetROIOutputLocation );
  declareProperty( "JEMHitsLocation",            m_jemHitsLocation );
  declareProperty( "JetElementLocation",         m_JetElementLocation ) ;

}

// Destructor
JetTrigger::~JetTrigger() {
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Destructor called" << endreq;
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode JetTrigger::initialize()
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

  //
  // Connect to StoreGate:
  //
  sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName() 
        << endreq;
    return sc;
  }

  // Retrieve L1JetTool
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

StatusCode JetTrigger::beginRun()
{

  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) setupTriggerMenuFromCTP();

  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise()
//---------------------------------

StatusCode JetTrigger::finalize()
{
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Finalizing" << endreq;
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode JetTrigger::execute( )
{
  //make a message logging stream

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Executing" << endreq;

  m_intROIContainer   =new DataVector<JetAlgorithm>;    //Container to hold the ROIs
  m_vectorOfJetROIs   =new DataVector<JetROI>;          //Container to hold Rois in TES.
  m_jemHitsContainer = new std::map<int, JEMHits *>;    // Map to hold JEM Hits

  // Retrieve JetElements
  if (m_storeGate->contains<JetElementCollection>(m_JetElementLocation)) {
    const DataVector<JetElement>* storedJEs;
    StatusCode sc = m_storeGate->retrieve(storedJEs,m_JetElementLocation);
    if ( sc==StatusCode::SUCCESS ) {
      // Warn if the stored collection is empty
      if (storedJEs->size() == 0)
         log << MSG::WARNING << "Empty JetElementCollection - looks like a problem" << endreq;
  
      // Use L1EJetTools to produce DataVector of RoIs passing trigger conditions
      m_JetTool->findRoIs(storedJEs, m_intROIContainer);
  
      // Form external RoIs
      DataVector<JetAlgorithm>::iterator intROI_it = m_intROIContainer->begin();
      for ( ; intROI_it!=m_intROIContainer->end() ; intROI_it++){
        JetROI* test = (*intROI_it)->produceExternal();
        if (outputLevel <= MSG::DEBUG)
            log << MSG::DEBUG << "*****InternalROI passes trigger : created an external ROI with ROI word "
                << std::hex << test->roiWord() << std::dec<<endreq;
        m_vectorOfJetROIs->push_back( test );
      }//end for loop
    }
    else log << MSG::WARNING << "Error retrieving JetElements" << endreq;
  }
  else log << MSG::WARNING << "No JetElementCollection at " << m_JetElementLocation << endreq;
  
  // save external RoIs in TEX
  saveExternalROIs();

  // For ByteStream simulation
  formJEMHits();

  delete m_intROIContainer;
  delete m_jemHitsContainer;
  m_intROIContainer=0;
  m_vectorOfJetROIs=0;
  m_jemHitsContainer=0;

  return StatusCode::SUCCESS ;
}

/** save JetROI objects */
void LVL1::JetTrigger::saveExternalROIs(){
  MsgStream log( messageService(), name() );

  StatusCode sc = m_storeGate->overwrite(m_vectorOfJetROIs,
    m_jetROIOutputLocation,true,false,false);
  if (sc.isSuccess() ){
    log << MSG::VERBOSE << "Stored " <<m_vectorOfJetROIs->size()
        << " JetROls at "<< m_jetROIOutputLocation << endreq;
  } else {
    log << MSG::ERROR << "failed to write JetROIs to  "
        << m_jetROIOutputLocation << endreq;
  } // endif
  return;
}

/** Store JEM energy sums for bytestream simulation */
void LVL1::JetTrigger::formJEMHits() {
  MsgStream log(messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "formJEMHits executing" << endreq;
  
  JetEnergyModuleKey testKey;
  CoordToHardware conv;
  DataVector<LVL1::JetROI>::const_iterator it;
  
  for (it=m_vectorOfJetROIs->begin(); it!=m_vectorOfJetROIs->end(); ++it){
    // Offset coordinates to avoid possible rounding errors
    double eta = (*it)->eta() - 0.051;
    double phi = (*it)->phi() - 0.051;
    if (phi < 0) phi += 2.*M_PI;
    Coordinate coord = Coordinate(phi, eta);
    unsigned int crate = conv.jepCrate(coord);
    unsigned int module = conv.jepModule(coord);
    unsigned int key = testKey.jemKey(crate, module);
    if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Found RoI with JEM key = " << key << endreq;
    JEMHits* jemHits=0;
   // find whether corresponding JEMHits already exists
    std::map<int, JEMHits*>::iterator test=m_jemHitsContainer->find(key);
    // if not, create it
    if ( test==m_jemHitsContainer->end()){
      if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "New key. JEM has crate = "
                                                << crate << ", Module = " 
						<< module << endreq;
      if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Create new JEMHits" << endreq; 
      jemHits = new JEMHits(crate, module);
      if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "and insert into map" << endreq; 
      m_jemHitsContainer->insert(std::map<int,JEMHits*>::value_type(key,jemHits));
      //m_jemHitsContainer->insert(key,jemHits);
    }
    else {
      if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Existing JEMHits" << endreq; 
      jemHits = test->second; // Already exists, so set pointer
    }
    // increment hit multiplicity. Word format different for FCAL JEMs
    if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Update jet hits" << endreq; 
    unsigned int hits = jemHits->JetHits();
    bool forward = (testKey.region(coord) != JetEnergyModuleKey::MidJEM); 
    hits = addHits(hits,(*it)->roiWord()&0xFFF,forward);
    std::vector<unsigned int> hitvec; // JEMHits stored as vector, to allow for multi-slice readout
    hitvec.push_back(hits);
    jemHits->addJetHits(hitvec);
    if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "All done for this one" << endreq; 
  }
  
  // Now add them to the TES
  saveJEMHits();
  
  return;
}

/** put JEMHits into SG */
void LVL1::JetTrigger::saveJEMHits(){
  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG<<"saveJEMHits running"<<endreq;
  
  JEMHitsCollection* JEMHvector = new  JEMHitsCollection;

  std::map<int, JEMHits*>::iterator it;
  for( it=m_jemHitsContainer->begin(); it!=m_jemHitsContainer->end(); ++it ){
     JEMHvector->push_back(it->second);
  }

  if (outputLevel <= MSG::DEBUG) log <<MSG::DEBUG<< m_jemHitsContainer->size()<<" JEMHits have been saved"<<endreq;
  StatusCode sc = m_storeGate->overwrite(JEMHvector, m_jemHitsLocation,true,false,false);
  if (sc == StatusCode::SUCCESS) {
    StatusCode sc2 = m_storeGate->setConst(JEMHvector);
    if (sc2 != StatusCode::SUCCESS) log << MSG::ERROR << "problem setting JEMHits vector constant" << endreq;
  }
  else {
    log << MSG::ERROR
        << "Error registering JEMHits collection in TES "
        << endreq;
  }
  
  return;
}

/** Increment JEM hit word */
unsigned int LVL1::JetTrigger::addHits(unsigned int hitMult, unsigned int hitVec, bool forward) {
  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  
  if (outputLevel <= MSG::DEBUG) 
     log << MSG::DEBUG<<"addHits: Original hitMult = " << hex << hitMult
         << ". Add hitWord = " << hitVec << dec<<endreq;
	 
  int nthresh = TrigT1CaloDefs::numOfJetThresholds;
  if (forward) nthresh += TrigT1CaloDefs::numOfFwdJetThresholds;
  
  int nbits = 24/nthresh;  // 24 bit multiplicity word -> number of bits/threshold
  
  if (outputLevel <= MSG::DEBUG) 
     log << MSG::DEBUG<<"Forward JEM = " << forward
         << ". Bits per threshold = " << nbits <<endreq;
  
  int max = (1<<nbits) - 1;
  unsigned int multMask = max;
  unsigned int hitMask = 1;
  unsigned int shift = 0;
  
  unsigned int hits = 0;
    
  for (int i = 0; i < nthresh; i++) {
    int mult = (hitMult&multMask) + (hitVec&hitMask);
    mult = ( (mult<=max) ? mult : max);
    hits += (mult<<shift);
    
    hitMult >>= nbits;
    hitVec  >>= 1;
    shift += nbits;
  }
  
  if (outputLevel <= MSG::DEBUG) 
     log << MSG::DEBUG<<"addHits returning hitMult = " << hex << hits << dec << endreq;
  
  return hits;
}

} // end of LVL1 namespace bracket

/** retrieves the Calo config put into detectorstore by TrigT1CTP and set up trigger menu */
void LVL1::JetTrigger::setupTriggerMenuFromCTP(){
  MsgStream log( messageService(), name() );

  
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
}
