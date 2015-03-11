/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// /***************************************************************************
//                           EnergyTrigger.cxx  -  description
//                              -------------------
//     begin                : Wed Dec 13 2000
//     copyright            : (C) 2000 by moyse
//     email                : moyse@heppch.ph.qmw.ac.uk
//  ***************************************************************************/
//
//    updated: June 20, 2001 M. Wielers
//             move to Storegate
//
//    updated: Sep 10 2007, Alan Watson
//             use L1EtTools for trigger algorithm emulation
//
//================================================
// EnergyTrigger class Implementation
// ================================================
//
//
//

// Utilities
#include <cmath>

// This algorithm includes
#include "TrigT1CaloSim/EnergyTrigger.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h" 
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/JetElementCollection.h"
#include "TrigT1CaloEvent/JEMEtSums_ClassDEF.h"


namespace LVL1 {


//--------------------------------
// Constructors and destructors
//--------------------------------

EnergyTrigger::EnergyTrigger
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ), 
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_EtTool("LVL1::L1EtTools/L1EtTools"),
      m_jemContainer(0),
      m_results(0),
      m_jemEtSumsContainer(0),
      m_energyRoI(0),
      m_energyCTP(0)
{
  m_JetElementLocation      = TrigT1CaloDefs::JetElementLocation;
  m_energyRoILocation       = TrigT1CaloDefs::EnergyROILocation ;
  m_energyCTPLocation       = TrigT1CaloDefs::EnergyCTPLocation ;
  m_jemEtSumsLocation       = TrigT1CaloDefs::JEMEtSumsLocation ;
  
  // This is how you declare the paramembers to Gaudi so that
  // they can be over-written via the job options file

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty( "JetElementLocation",   m_JetElementLocation ) ;
  declareProperty(  "EnergyRoILocation",   m_energyRoILocation );
  declareProperty(  "EnergyCTPLocation",   m_energyCTPLocation );
  declareProperty(  "JEMEtSumsLocation",   m_jemEtSumsLocation );
}

// Destructor
EnergyTrigger::~EnergyTrigger() {
  
  ATH_MSG_INFO( "Destructor called" );
	
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode EnergyTrigger::initialize()
{
  ATH_CHECK( m_configSvc.retrieve() );
  ATH_CHECK( m_EtTool.retrieve() );
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode EnergyTrigger::beginRun()
{

  //int outputLevel = msgSvc()->outputLevel( name() );
  setupTriggerMenuFromCTP();

  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise()
//---------------------------------

StatusCode EnergyTrigger::finalize()
{
   
   ATH_MSG_INFO( "Finalizing" );
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode EnergyTrigger::execute( )
{
  ATH_MSG_DEBUG ( "Executing" );

  m_jemEtSumsContainer = new std::map<int, JEMEtSums *>; // all removed in cleanup()
   
  // form module sums
  m_jemContainer = new DataVector<ModuleEnergy>;  
  if (evtStore()->contains<JetElementCollection>(m_JetElementLocation)) {
    const DataVector<JetElement>* jetelements;
  
    StatusCode sc = evtStore()->retrieve(jetelements,m_JetElementLocation);
    if ( sc==StatusCode::SUCCESS ) {
      // Warn if we find an empty container
      if (jetelements->size() == 0)
        ATH_MSG_WARNING( "Empty JetElementContainer - looks like a problem" );
    
      m_EtTool->moduleSums(jetelements, m_jemContainer);
    }
    else ATH_MSG_WARNING( "Error retrieving JetElements" );
  }
  else ATH_MSG_WARNING( "No JetElementCollection at " << m_JetElementLocation );
    
  // form crate sums 
  DataVector<CrateEnergy>* crates  = new DataVector<CrateEnergy>;
  m_EtTool->crateSums(m_jemContainer, crates);
  // system summation and threshold tests
  SystemEnergy results = m_EtTool->systemSums(crates);
  m_results = &results;

  // Done the processing. Now form & save the various output data

  // for Bytestream simulation
  formJEMEtSums();
  saveJEMEtSums();
  
  // RoI output
  saveRoIs();

  // CTP output
  saveCTPObjects();
  
  // tidy up at end of event
  delete crates;
  cleanup();
 
  return StatusCode::SUCCESS ;
}


} // end of LVL1 namespace bracket

/** Store JEM energy sums for bytestream simulation */
void LVL1::EnergyTrigger::formJEMEtSums() {
  ATH_MSG_DEBUG ( "formJEMEtSums executing" );
  
  JetEnergyModuleKey get;
  DataVector<ModuleEnergy>::iterator it;
  
  for (it=m_jemContainer->begin(); it!=m_jemContainer->end(); ++it){
    unsigned int crate = (*it)->crate();
    unsigned int module = (*it)->module();
    unsigned int key = get.jemKey(crate, module);
    ATH_MSG_DEBUG ( "Found JEM with key = " << key );
    JEMEtSums* jemEtSums=0;
   // find whether corresponding JEMEtSums already exists
    std::map<int, JEMEtSums*>::iterator test=m_jemEtSumsContainer->find(key);
    // if not, create it
    if ( test==m_jemEtSumsContainer->end()){
      ATH_MSG_DEBUG ( "New key. JEM has crate = "
                      << crate << ", Module = " 
                      << module );
      ATH_MSG_DEBUG ( "Create new JEMEtSums" ); 
      jemEtSums = new JEMEtSums(crate, module);
      ATH_MSG_DEBUG ( "and insert..." ); 
      m_jemEtSumsContainer->insert(std::map<int,JEMEtSums*>::value_type(key,jemEtSums));
    }
    else {
      ATH_MSG_DEBUG ( "Existing JEMEtSums" ); 
      jemEtSums = test->second; // Already exists, so set pointer
    }
    // add energy sums to JEMEtSums
    ATH_MSG_DEBUG ( "Update content" ); 
    std::vector<unsigned int> ExVec; // JEMEtSums store vectors, to allow for multi-slice readout
    std::vector<unsigned int> EyVec;
    std::vector<unsigned int> EtVec;
    ExVec.push_back((*it)->ex()); // Crate sums no longer quad-linear in Run 2
    EyVec.push_back((*it)->ey());
    EtVec.push_back((*it)->et());
    jemEtSums->addEx(ExVec);
    jemEtSums->addEy(EyVec);
    jemEtSums->addEt(EtVec);
    ATH_MSG_DEBUG ( "All done for this one" ); 
  }
  return;
}


/** delete pointers etc. */
void LVL1::EnergyTrigger::cleanup(){

  delete m_jemContainer;
  delete m_jemEtSumsContainer;
}

/** retrieves the Calo config put into detectorstore by TrigT1CTP and set up trigger menu */
void LVL1::EnergyTrigger::setupTriggerMenuFromCTP(){
  ATH_MSG_DEBUG("Loading Trigger Menu");
  
  /// Old or new CTP word format?
  bool oldMenu = false;

  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {

    /// Old or new CTP word format?
    if ( ((*it)->type() == m_def.xeType()) && ((*it)->cableStart() < 8) ) oldMenu = true;
	  
    /// Debug printout of menu
    if ( (*it)->type() == m_def.xeType() || (*it)->type() == m_def.teType() || (*it)->type() == m_def.xsType() ) {
     TriggerThresholdValue* tv = (*it)->triggerThresholdValue(0,0); // ET trigger thresholds can only have one global value
     ATH_MSG_DEBUG( "TriggerThreshold " << (*it)->name() << " has:" << endreq
         << "--------------------------" << endreq
         << " Number   = " << (*it)->thresholdNumber() << endreq
         << " Type     = " << (*it)->type() << endreq
         << " Value    = " << (*tv).thresholdValueCount() );
	         
    } //  is type == em or tau?
  } // end of loop over thresholds
  
  /// Configure L1DataDef store for old menu type, if necessary
  if (oldMenu) m_def.setOldJEP3Cabling();
  else         m_def.setNewJEP3Cabling();

  return;
}

/** form EnergyRoI & save in SG */
void LVL1::EnergyTrigger::saveRoIs(){
  ATH_MSG_DEBUG("saveRoIs");

  // set value into roi words
  unsigned int roiWord0 = m_results->roiWord0();
  unsigned int roiWord1 = m_results->roiWord1();
  unsigned int roiWord2 = m_results->roiWord2();
  
  // form RoI 
  m_energyRoI = new EnergyRoI(roiWord0,roiWord1,roiWord2);

  // save RoI in SG
  StatusCode sc = evtStore()->overwrite(m_energyRoI, m_energyRoILocation,true,false,false);
  if (sc.isSuccess() ){
    ATH_MSG_DEBUG ( "Stored energy RoI object with "<<std::endl
                    << "Word 0: "<<std::hex<<(m_energyRoI->roiWord0())<<std::dec<<std::endl
                    << "Word 1: "<<std::hex<<(m_energyRoI->roiWord1())<<std::dec<<std::endl
                    << "Word 2: "<<std::hex<<(m_energyRoI->roiWord2())<<std::dec<<std::endl
                    );
  }else{
    ATH_MSG_ERROR( "Failed to store energy RoI object ");
  }
  return;
}

/** put JEMEtSums into SG */
void LVL1::EnergyTrigger::saveJEMEtSums(){
  ATH_MSG_DEBUG("saveJEMEtSums running");
  
  JEMEtSumsCollection* JEMRvector = new  JEMEtSumsCollection;

  std::map<int, JEMEtSums*>::iterator it;
  for( it=m_jemEtSumsContainer->begin(); it!=m_jemEtSumsContainer->end(); ++it ){
     JEMRvector->push_back(it->second);
  }

  ATH_MSG_DEBUG( m_jemEtSumsContainer->size()<<" JEMEtSums are being saved");
  StatusCode sc = evtStore()->overwrite(JEMRvector, m_jemEtSumsLocation,true,false,false);
  if (sc == StatusCode::SUCCESS) {
    StatusCode sc2 = evtStore()->setConst(JEMRvector);
    if (sc2 != StatusCode::SUCCESS) ATH_MSG_ERROR ( "error setting JEMResult vector constant" );
  }
  else {
    ATH_MSG_ERROR( "Error registering JEMEtSums collection in TDS ");
  }
  return;
}

/** form CTP word from ETmiss and ETsum hits */
unsigned int LVL1::EnergyTrigger::ctpWord(unsigned int metSigPassed, unsigned int etMissPassed, unsigned int etSumPassed){

  return ( (metSigPassed<<(m_def.max_TE_Threshold_Number()+m_def.max_XE_Threshold_Number())) +
	   (etMissPassed<<m_def.max_TE_Threshold_Number()) + etSumPassed );
}

/** form CTP objects and store them in SG. */
void LVL1::EnergyTrigger::saveCTPObjects(){
  ATH_MSG_DEBUG("saveCTPObjects");

  // get bit words of thresholds passed
  unsigned int etSumPassed = m_results->etSumHits();
  unsigned int etMissPassed = m_results->etMissHits();
  unsigned int metSigPassed = m_results->metSigHits();

  // form CTP word
  unsigned int temp = ctpWord(metSigPassed, etMissPassed, etSumPassed);
  // Comment out parity generation for now - should be in bit 32 of a 34 bit word
  //Parity genParity;
  //temp += (genParity.odd(temp)<<31);

  // form CTP object
  m_energyCTP = new EnergyCTP( temp );

  // record in SG
  StatusCode sc = evtStore()->overwrite(m_energyCTP, m_energyCTPLocation,true,false,false);
  if (sc.isSuccess() ){
    ATH_MSG_DEBUG ( "Stored energy CTP object with word "<< std::hex
                    << (m_energyCTP->cableWord0()) << std::dec );
  }else{
    ATH_MSG_ERROR( "Failed to store energy CTP object ");
  }
  return;  
}



