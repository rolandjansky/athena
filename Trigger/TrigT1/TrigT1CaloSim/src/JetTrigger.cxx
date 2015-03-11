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
    : AthAlgorithm( name, pSvcLocator ), 
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

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty( "JetROIOutputLocation",       m_jetROIOutputLocation );
  declareProperty( "JEMHitsLocation",            m_jemHitsLocation );
  declareProperty( "JetElementLocation",         m_JetElementLocation ) ;

}

// Destructor
JetTrigger::~JetTrigger() {
  
  ATH_MSG_INFO( "Destructor called" );
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode JetTrigger::initialize()
{
  ATH_CHECK(  m_configSvc.retrieve() );
  ATH_CHECK(  m_JetTool.retrieve() );
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode JetTrigger::beginRun()
{
  if (msgLvl(MSG::DEBUG)) setupTriggerMenuFromCTP();
  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise()
//---------------------------------

StatusCode JetTrigger::finalize()
{
   
   ATH_MSG_INFO( "Finalizing" );
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode JetTrigger::execute( )
{
  //make a message logging stream

  
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "Executing" );

  m_intROIContainer   =new DataVector<JetAlgorithm>;    //Container to hold the ROIs
  m_vectorOfJetROIs   =new DataVector<JetROI>;          //Container to hold Rois in TES.
  m_jemHitsContainer = new std::map<int, JEMHits *>;    // Map to hold JEM Hits

  // Retrieve JetElements
  if (evtStore()->contains<JetElementCollection>(m_JetElementLocation)) {
    const DataVector<JetElement>* storedJEs;
    StatusCode sc = evtStore()->retrieve(storedJEs,m_JetElementLocation);
    if ( sc==StatusCode::SUCCESS ) {
      // Warn if the stored collection is empty
      if (storedJEs->size() == 0)
         ATH_MSG_WARNING( "Empty JetElementCollection - looks like a problem" );
  
      // Use L1EJetTools to produce DataVector of RoIs passing trigger conditions
      m_JetTool->findRoIs(storedJEs, m_intROIContainer);
  
      // Form external RoIs
      DataVector<JetAlgorithm>::iterator intROI_it = m_intROIContainer->begin();
      for ( ; intROI_it!=m_intROIContainer->end() ; intROI_it++){
        JetROI* test = (*intROI_it)->produceExternal();
        if (outputLevel <= MSG::DEBUG)
            ATH_MSG_DEBUG( "*****InternalROI passes trigger : created an external ROI with ROI word "
                << std::hex << test->roiWord() << std::dec);
        m_vectorOfJetROIs->push_back( test );
      }//end for loop
    }
    else ATH_MSG_WARNING( "Error retrieving JetElements" );
  }
  else ATH_MSG_WARNING( "No JetElementCollection at " << m_JetElementLocation );
  
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
  

  StatusCode sc = evtStore()->overwrite(m_vectorOfJetROIs,
    m_jetROIOutputLocation,true,false,false);
  if (sc.isSuccess() ){
    ATH_MSG_VERBOSE( "Stored " <<m_vectorOfJetROIs->size()
        << " JetROls at "<< m_jetROIOutputLocation );
  } else {
    ATH_MSG_ERROR( "failed to write JetROIs to  "
        << m_jetROIOutputLocation );
  } // endif
  return;
}

/** Store JEM energy sums for bytestream simulation */
void LVL1::JetTrigger::formJEMHits() {
  
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "formJEMHits executing" );
  
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
    if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "Found RoI with JEM key = " << key );
    JEMHits* jemHits=0;
   // find whether corresponding JEMHits already exists
    std::map<int, JEMHits*>::iterator test=m_jemHitsContainer->find(key);
    // if not, create it
    if ( test==m_jemHitsContainer->end()){
      if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "New key. JEM has crate = "
                                                << crate << ", Module = " 
						<< module );
      if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "Create new JEMHits" ); 
      jemHits = new JEMHits(crate, module);
      if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "and insert into map" ); 
      m_jemHitsContainer->insert(std::map<int,JEMHits*>::value_type(key,jemHits));
      //m_jemHitsContainer->insert(key,jemHits);
    }
    else {
      if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "Existing JEMHits" ); 
      jemHits = test->second; // Already exists, so set pointer
    }
    // increment hit multiplicity. Word format different for FCAL JEMs
    if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "Update jet hits" ); 
    unsigned int hits = jemHits->JetHits();
    bool forward = (testKey.region(coord) != JetEnergyModuleKey::MidJEM); 
    hits = addHits(hits,(*it)->roiWord()&0xFFF,forward);
    std::vector<unsigned int> hitvec; // JEMHits stored as vector, to allow for multi-slice readout
    hitvec.push_back(hits);
    jemHits->addJetHits(hitvec);
    if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "All done for this one" ); 
  }
  
  // Now add them to the TES
  saveJEMHits();
  
  return;
}

/** put JEMHits into SG */
void LVL1::JetTrigger::saveJEMHits(){
  
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG("saveJEMHits running");
  
  JEMHitsCollection* JEMHvector = new  JEMHitsCollection;

  std::map<int, JEMHits*>::iterator it;
  for( it=m_jemHitsContainer->begin(); it!=m_jemHitsContainer->end(); ++it ){
     JEMHvector->push_back(it->second);
  }

  if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( m_jemHitsContainer->size()<<" JEMHits have been saved");
  StatusCode sc = evtStore()->overwrite(JEMHvector, m_jemHitsLocation,true,false,false);
  if (sc == StatusCode::SUCCESS) {
    StatusCode sc2 = evtStore()->setConst(JEMHvector);
    if (sc2 != StatusCode::SUCCESS) ATH_MSG_ERROR( "problem setting JEMHits vector constant" );
  }
  else {
    ATH_MSG_ERROR("Error registering JEMHits collection in TES ");
  }
  
  return;
}

/** Increment JEM hit word */
unsigned int LVL1::JetTrigger::addHits(unsigned int hitMult, unsigned int hitVec, bool forward) {
  
  int outputLevel = msgSvc()->outputLevel( name() );
  
  if (outputLevel <= MSG::DEBUG) 
     ATH_MSG_DEBUG("addHits: Original hitMult = " << hex << hitMult
         << ". Add hitWord = " << hitVec << dec);
	 
  int nthresh = TrigT1CaloDefs::numOfJetThresholds;
  if (forward) nthresh += TrigT1CaloDefs::numOfFwdJetThresholds;
  
  int nbits = 24/nthresh;  // 24 bit multiplicity word -> number of bits/threshold
  
  if (outputLevel <= MSG::DEBUG) 
     ATH_MSG_DEBUG("Forward JEM = " << forward
         << ". Bits per threshold = " << nbits );
  
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
     ATH_MSG_DEBUG("addHits returning hitMult = " << hex << hits << dec );
  
  return hits;
}

} // end of LVL1 namespace bracket

/** retrieves the Calo config put into detectorstore by TrigT1CTP and set up trigger menu */
void LVL1::JetTrigger::setupTriggerMenuFromCTP(){
  

  
  L1DataDef def;

  unsigned int numThresh=0;  
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.jetType() ||
         (*it)->type() == def.fjetType()  ||
         (*it)->type() == def.jfType()  ||
         (*it)->type() == def.jbType()) {
      ATH_MSG_DEBUG( "TriggerThreshold " << (*it)->id() << " has name " << (*it)->name() << endreq
          << "  threshold number " << (*it)->thresholdNumber() << endreq
          << "  number of values = " << (*it)->numberofValues() );
      for (std::vector<TrigConf::TriggerThresholdValue*>::const_iterator tv = (*it)->thresholdValueVector().begin();
           tv != (*it)->thresholdValueVector().end(); ++tv) {
        TrigConf::JetThresholdValue* jtv;
        jtv = dynamic_cast<JetThresholdValue*> (*tv);
	if (!jtv) {
          ATH_MSG_ERROR( "Threshold type name is Jet, but is not a JetThreshold object!" );
          continue;
        }
        ATH_MSG_DEBUG( "JetThresholdValue: " << endreq
            << "  Type = " << (*it)->type() << endreq
            << "  Threshold value = " << jtv->thresholdValueCount() << endreq
            << "  Cluster size = " << jtv->window() << endreq
            << "  EtaMin = " << jtv->etamin() << ", EtaMax = " << jtv->etamax() );
        
      } // end of loop over threshold values
      numThresh++;
    } //  is type == jet or forward jet
  } // end of loop over thresholds

  return;
}
