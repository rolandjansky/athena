
// /***************************************************************************
//                           EnergyCMX.cxx  -  description
//                              -------------------
 //     begin                : 04 04 2014
 //     copyright            : (C) 2014 by Alan Watson
 //     email                : Alan.Watson@cern.ch
//  ***************************************************************************/
//
//================================================
// EnergyCMX class Implementation
// ================================================
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//
//
//

// Utilities
#include <cmath>

#include "GaudiKernel/MsgStream.h"

// This algorithm includes
#include "TrigT1CaloSim/EnergyCMX.h"

namespace LVL1 {


//--------------------------------
// Constructors and destructors
//--------------------------------

EnergyCMX::EnergyCMX
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm( name, pSvcLocator ), 
      m_storeGate("StoreGateSvc", name),
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_EtTool("LVL1::L1EtTools/L1EtTools"),
      m_resultsFull(0),
      m_resultsTrunc(0)
{
  m_energyCMXDataLocation   = TrigT1CaloDefs::EnergyCMXDataLocation;
  m_energyRoILocation       = TrigT1CaloDefs::EnergyROILocation ;
  m_energyCTPLocation       = TrigT1CaloDefs::EnergyCTPLocation ;
  m_energyTopoLocation      = TrigT1CaloDefs::EnergyTopoDataLocation ;
  m_cmxEtsumsLocation       = TrigT1CaloDefs::CMXEtSumsLocation ;
  m_cmxRoILocation          = TrigT1CaloDefs::CMXRoILocation ;
  
  // This is how you declare the paramembers to Gaudi so that
  // they can be over-written via the job options file

  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty("EnergyRoILocation",       m_energyRoILocation );
  declareProperty("EnergyCTPLocation",       m_energyCTPLocation );
  declareProperty("EnergyCMXDataLocation",   m_energyCMXDataLocation );
}

// Destructor
EnergyCMX::~EnergyCMX() {
  MsgStream log( messageService(), name() ) ;
  log << MSG::INFO << "Destructor called" << endreq;
	
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode EnergyCMX::initialize()
{

  // We must here instantiate items which can only be made after
  // any job options have been set
  MsgStream log( messageService(), name() ) ;

  //
  // Connect to the LVL1ConfigSvc for the trigger configuration:
  //
  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_configSvc.typeAndName() 
        << endreq;
    return sc;
  } else {
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

StatusCode EnergyCMX::beginRun()
{

  //int outputLevel = msgSvc()->outputLevel( name() );
  setupTriggerMenuFromCTP();

  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise()
//---------------------------------

StatusCode EnergyCMX::finalize()
{
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Finalizing" << endreq;
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode EnergyCMX::execute( )
{
  //make a message logging stream

  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Executing" << endreq;
   
  // form module sums
  EnergyCMXDataCollection* jemContainer;
  if (m_storeGate->contains<EnergyCMXDataCollection>(m_energyCMXDataLocation)) {
  
    StatusCode sc = m_storeGate->retrieve(jemContainer,m_energyCMXDataLocation);
    if ( sc==StatusCode::SUCCESS ) {
      // Warn if we find an empty container
      if (jemContainer->size() == 0)
        log << MSG::WARNING << "Empty EnergyCMXDataCollection - looks like a problem" << endreq;
    
    }
    else log << MSG::WARNING << "Error retrieving module ET sums" << endreq;
  }
  else log << MSG::WARNING << "No EnergyCMXDataCollection at " << m_energyCMXDataLocation << endreq;
    
  // form crate sums (full eta range)
  DataVector<CrateEnergy>* cratesFull  = new DataVector<CrateEnergy>;
  m_EtTool->crateSums(jemContainer, cratesFull);
  // system summation and threshold tests
  SystemEnergy resultsFull = m_EtTool->systemSums(cratesFull);
  m_resultsFull = &resultsFull;
  
  // form crate sums (restricted eta range)
  float etaTrunc = 2.4;
  DataVector<CrateEnergy>* cratesTrunc  = new DataVector<CrateEnergy>;
  m_EtTool->crateSums(jemContainer, cratesTrunc, etaTrunc);
  // system summation and threshold tests
  SystemEnergy resultsTrunc = m_EtTool->systemSums(cratesTrunc);
  m_resultsTrunc = &resultsTrunc;
  
  // CTP Data
  saveCTPObjects();
  
  // RoI output
  saveRoIs();
  
  // Module readout simulation
  DataVector<CMXEtSums>* CMXSums = new DataVector<CMXEtSums>;
  std::vector<unsigned int> exVec;
  std::vector<unsigned int> eyVec;
  std::vector<unsigned int> etVec;
  std::vector<int> exErr;
  std::vector<int> eyErr;
  std::vector<int> etErr;
  exErr.assign(1,0);
  eyErr.assign(1,0);
  etErr.assign(1,0);
  const int peak = 0;
  const int system_crate = 1;
  
  // Full eta range
  DataVector<CrateEnergy>::const_iterator itCrate = cratesFull->begin();
  for ( ; itCrate != cratesFull->end(); ++itCrate) {
    exVec.clear();
    exVec.push_back((*itCrate)->exTC());
    eyVec.clear();
    eyVec.push_back((*itCrate)->eyTC());
    etVec.clear();
    etVec.push_back((*itCrate)->et());
    int crate = (*itCrate)->crate();
    CMXEtSums* crateEtSumFull = new CMXEtSums(crate, LVL1::CMXEtSums::LOCAL_STANDARD,
					      exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
    CMXSums->push_back(crateEtSumFull);
    
    if (crate != system_crate) {
      CMXEtSums* remoteEtSumFull = new CMXEtSums(system_crate, LVL1::CMXEtSums::REMOTE_STANDARD,
					         exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
      CMXSums->push_back(remoteEtSumFull); 
    }
       
  }
  
  exVec.clear();
  exVec.push_back(m_resultsFull->exTC());
  eyVec.clear();
  eyVec.push_back(m_resultsFull->eyTC());
  etVec.clear();
  etVec.push_back(m_resultsFull->et());
  CMXEtSums* systemEtSumFull = new CMXEtSums(system_crate, LVL1::CMXEtSums::TOTAL_STANDARD,
					     exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
  CMXSums->push_back(systemEtSumFull); 
  
  // Restricted eta range
  DataVector<CrateEnergy>::const_iterator itTrunc = cratesTrunc->begin();
  for ( ; itTrunc != cratesTrunc->end(); ++itTrunc) {
    exVec.clear();
    exVec.push_back((*itTrunc)->exTC());
    eyVec.clear();
    eyVec.push_back((*itTrunc)->eyTC());
    etVec.clear();
    etVec.push_back((*itTrunc)->et());
    int crate = (*itTrunc)->crate();
    CMXEtSums* crateEtSumTrunc = new CMXEtSums(crate, LVL1::CMXEtSums::LOCAL_RESTRICTED,
					      exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
    CMXSums->push_back(crateEtSumTrunc);
    
    if (crate != system_crate) {
      CMXEtSums* remoteEtSumTrunc = new CMXEtSums(system_crate, LVL1::CMXEtSums::REMOTE_RESTRICTED,
					         exVec, eyVec, etVec, exErr, eyErr, etErr, peak);
      CMXSums->push_back(remoteEtSumTrunc); 
    }
       
  }
  
  exVec.clear();
  exVec.push_back(m_resultsTrunc->exTC());
  eyVec.clear();
  eyVec.push_back(m_resultsTrunc->eyTC());
  etVec.clear();
  etVec.push_back(m_resultsTrunc->et());
  CMXEtSums* systemEtSumTrunc = new CMXEtSums(system_crate, LVL1::CMXEtSums::TOTAL_RESTRICTED,
					      exVec, eyVec, etVec, exErr, eyErr, etErr, peak);

  CMXSums->push_back(systemEtSumTrunc); 
  
  // save Sums in SG
  StatusCode sc1 = m_storeGate->overwrite(CMXSums, m_cmxEtsumsLocation,true,false,false);
  if (!sc1.isSuccess()) log << MSG::ERROR << "Failed to store CMXEtsums" << endreq;
   
  
  // Topo data
  EnergyTopoData* topoData = new EnergyTopoData();
  
  topoData->addEx(m_resultsFull->exTC(), m_resultsFull->exOverflow(), LVL1::EnergyTopoData::Normal);
  topoData->addEy(m_resultsFull->eyTC(), m_resultsFull->eyOverflow(), LVL1::EnergyTopoData::Normal);
  topoData->addEt(m_resultsFull->et(),   m_resultsFull->etOverflow(), LVL1::EnergyTopoData::Normal);
  
  topoData->addEx(m_resultsTrunc->exTC(), m_resultsTrunc->exOverflow(), LVL1::EnergyTopoData::Restricted);
  topoData->addEy(m_resultsTrunc->eyTC(), m_resultsTrunc->eyOverflow(), LVL1::EnergyTopoData::Restricted);
  topoData->addEt(m_resultsTrunc->et(),   m_resultsTrunc->etOverflow(), LVL1::EnergyTopoData::Restricted);

  StatusCode sc2 = m_storeGate->overwrite(topoData, m_energyTopoLocation,true,false,false);
  if (!sc2.isSuccess()) log << MSG::ERROR << "Failed to store EnergyTopoData" << endreq;

  // tidy up at end of event
  delete cratesFull;
  delete cratesTrunc;
  cleanup();
 
  return StatusCode::SUCCESS ;
}


} // end of LVL1 namespace bracket


/** delete pointers etc. */
void LVL1::EnergyCMX::cleanup(){

}

/** retrieves the Calo config put into detectorstore by TrigT1CTP and set up trigger menu */
void LVL1::EnergyCMX::setupTriggerMenuFromCTP(){
  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG<<"Loading Trigger Menu"<<endreq;
  
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {

    /// Debug printout of menu
    if ( (*it)->type() == m_def.xeType() || (*it)->type() == m_def.teType() || (*it)->type() == m_def.xsType() ) {
     TriggerThresholdValue* tv = (*it)->triggerThresholdValue(0,0); // ET trigger thresholds can only have one global value
     log << MSG::DEBUG << "TriggerThreshold " << (*it)->name() << " has:" << endreq
         << "--------------------------" << endreq
         << " Number   = " << (*it)->thresholdNumber() << endreq
         << " Type     = " << (*it)->type() << endreq
         << " Value    = " << (*tv).thresholdValueCount() << endreq;
	         
    } //  is type == energySum trigger
  } // end of loop over thresholds
  
  /// Should check whether this is still necessary
  m_def.setNewJEP3Cabling();

  return;
}

/** form CMXRoI & save in SG */
void LVL1::EnergyCMX::saveRoIs(){
  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG<<"saveRoIs"<<endreq;

  // copy values into roi words
  unsigned int roiWord0 = m_resultsFull->roiWord0();
  unsigned int roiWord2 = m_resultsFull->roiWord1();
  unsigned int roiWord4 = m_resultsFull->roiWord2();
  
  // Truncated eta range
  unsigned int roiWord1 = m_resultsTrunc->roiWord0();
  unsigned int roiWord3 = m_resultsTrunc->roiWord1();
  unsigned int roiWord5 = m_resultsTrunc->roiWord2();
 
  // DAQ readout object. 
  //CMXRoI* daqRoI = new CMXRoI(roiWord0, roiWord1, roiWord2, roiWord3, roiWord4, roiWord5);
  CMXRoI* daqRoI = new CMXRoI();
  // Add data to RoI object. The object will perform format checks on inputs
  bool added = daqRoI->setRoiWord(roiWord0);
  if (!added) log << MSG::WARNING << "Failed to add RoI Word 0: " << MSG::hex << roiWord0 << MSG::dec << endreq;
  added = daqRoI->setRoiWord(roiWord1);
  if (!added) log << MSG::WARNING << "Failed to add RoI Word 1: " << MSG::hex << roiWord1 << MSG::dec << endreq;
  added = daqRoI->setRoiWord(roiWord2);
  if (!added) log << MSG::WARNING << "Failed to add RoI Word 2: " << MSG::hex << roiWord2 << MSG::dec << endreq;
  added = daqRoI->setRoiWord(roiWord3);
  if (!added) log << MSG::WARNING << "Failed to add RoI Word 3: " << MSG::hex << roiWord3 << MSG::dec << endreq;
  added = daqRoI->setRoiWord(roiWord4);
  if (!added) log << MSG::WARNING << "Failed to add RoI Word 4: " << MSG::hex << roiWord4 << MSG::dec << endreq;
  added = daqRoI->setRoiWord(roiWord5);
  if (!added) log << MSG::WARNING << "Failed to add RoI Word 5: " << MSG::hex << roiWord5 << MSG::dec << endreq;

  // save RoIs in SG
  StatusCode sc = m_storeGate->overwrite(daqRoI, m_cmxRoILocation,true,false,false);
  if (!sc.isSuccess()) log << MSG::ERROR << "Failed to store CMXRoI object" << endreq;
  
  return;
}

/** form CTP word from ETmiss and ETsum hits */
unsigned int LVL1::EnergyCMX::ctpWord(unsigned int metSigPassed, unsigned int etMissPassed, unsigned int etSumPassed){

  return ( (metSigPassed<<(m_def.max_TE_Threshold_Number()+m_def.max_XE_Threshold_Number())) +
	   (etMissPassed<<m_def.max_TE_Threshold_Number()) + etSumPassed );
}

/** form CTP objects and store them in SG. */
void LVL1::EnergyCMX::saveCTPObjects(){
  MsgStream log( messageService(), name() );
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG<<"saveCTPObjects"<<endreq;

  // get bit words of thresholds passed
  unsigned int etSumHitsFull   = m_resultsFull->etSumHits();
  unsigned int etMissHitsFull  = m_resultsFull->etMissHits();
  unsigned int metSigHitsFull  = m_resultsFull->metSigHits();
  unsigned int etSumHitsTrunc  = m_resultsTrunc->etSumHits();
  unsigned int etMissHitsTrunc = m_resultsTrunc->etMissHits();

  // form CTP words
  unsigned int word0 = ctpWord(metSigHitsFull, etMissHitsFull, etSumHitsFull);
  unsigned int word1 = ctpWord(0, etMissHitsTrunc, etSumHitsTrunc);
  // Comment out parity generation for now - should be in bit 32 of a 34 bit word
  //Parity genParity;
  //temp += (genParity.odd(temp)<<31);

  // form CTP object
  EnergyCTP* energyCTP = new EnergyCTP( word0, word1 );

  // record in SG
  StatusCode sc = m_storeGate->overwrite(energyCTP, m_energyCTPLocation);
  if (sc.isSuccess() ){
    if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG
       << "Stored energy CTP object with words "<< std::hex
       << (energyCTP->cableWord0()) << ", " <<  (energyCTP->cableWord1())<< std::dec << endreq;
  }else{
    log << MSG::ERROR << "Failed to store energy CTP object "<< endreq;
  }
  return;  
}



