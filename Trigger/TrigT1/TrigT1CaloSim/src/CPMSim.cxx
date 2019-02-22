/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// /***************************************************************************
//                           CPMSim.cxx  -  description
//                              -------------------
//     begin                : Wed Mar 05 2014
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
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "TrigT1CaloEvent/CPMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/CPMCMXData_ClassDEF.h"

#include "xAODEventInfo/EventInfo.h"

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
    : AthAlgorithm( name, pSvcLocator ), 
      //m_cpmTowers(nullptr),
      m_allTOBs(nullptr),
      m_CMXData(nullptr),
      m_CPMTowerLocation(TrigT1CaloDefs::CPMTowerLocation),
      m_CPMTobRoILocation(TrigT1CaloDefs::CPMTobRoILocation),
      m_CPMCMXDataLocation(TrigT1CaloDefs::CPMCMXDataLocation ),
      m_emTauSlinkLocation(TrigT1CaloDefs::EmTauSlinkLocation),
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_CPMTool("LVL1::L1CPMTools/L1CPMTools"),
      m_eventNumber(0)
{
    declareProperty( "CPMTOBRoILocation", m_CPMTobRoILocation );
    declareProperty( "CPMCMXDataLocation", m_CPMCMXDataLocation );
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty( "CPMTowerLocation", m_CPMTowerLocation ) ;
    declareProperty( "EmTauSlinkLocation", m_emTauSlinkLocation );
       
    /** Clear SLink output vectors (RoI output) */
    for (unsigned int i = 0; i < TrigT1CaloDefs::numOfCPRoIRODs; ++i) {
      m_CPRoIROD[i] = nullptr;
    }
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode CPMSim::initialize()
{
  ATH_CHECK( m_configSvc.retrieve() );
  ATH_CHECK( m_CPMTool.retrieve() );
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

  ATH_MSG_DEBUG ( "starting CPMSim" ); 

  // For RoI output SLink record
  const xAOD::EventInfo* evt;
  if (StatusCode::SUCCESS == evtStore()->retrieve(evt)){
    m_eventNumber =evt->eventNumber();
  }else{
    ATH_MSG_ERROR(" Unable to retrieve EventInfo from StoreGate ");
  }

  // Create containers for this event
  m_CMXData    = new DataVector<CPMCMXData>;  //Container of backplane data objects 
  m_allTOBs    = new DataVector<CPMTobRoI>;  // Container to hold all TOB RoIs in event

  // Retrieve the CPMTowerContainer
  if (evtStore()->contains<xAOD::CPMTowerContainer>(m_CPMTowerLocation)) {
    const DataVector<xAOD::CPMTower>* storedCPMTs;
    StatusCode sc = evtStore()->retrieve(storedCPMTs, m_CPMTowerLocation);  
    if ( sc==StatusCode::SUCCESS ) {
       // Check size of TriggerTowerCollection - zero would indicate a problem
      if (storedCPMTs->size() == 0)
         ATH_MSG_WARNING("Empty CPMTowerContainer - looks like a problem" );
	
      // Map the CPMTs
      xAOD::CPMTowerMap_t* towerMap = new xAOD::CPMTowerMap_t;
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
    
    else ATH_MSG_WARNING("Error retrieving CPMTowers" );
  }
  else ATH_MSG_WARNING("No CPMTowerContainer at " << m_CPMTowerLocation );
  
      
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

  // Store backplane data objects
  StatusCode sc = evtStore()->overwrite(m_CMXData, m_CPMCMXDataLocation,true);

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE ( "Stored " << m_CMXData->size()
                      << " CPMCMXData at " << m_CPMCMXDataLocation );
  }
  else {
     ATH_MSG_ERROR("failed to write CPMCMXData to  "
         << m_CPMCMXDataLocation );
  } 

  return;

} //end storeBackplaneTOBs


/** place final ROI objects in the TES. */
void CPMSim::storeModuleRoIs() {

  StatusCode sc = evtStore()->overwrite(m_allTOBs, m_CPMTobRoILocation,true);

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE ( "Stored " << m_allTOBs->size()
                      << " EM & Tau TOBs at " << m_CPMTobRoILocation );
  }
  else {
     ATH_MSG_ERROR("failed to write CPMTobRoIs to  "
         << m_CPMTobRoILocation );
  } 

  return;

} //end storeModuleRoIs

/** Simulate SLink streams to RoIB */
void CPMSim::storeSlinkObjects(){
    
  ATH_MSG_DEBUG("storeSlinkObjects" );

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
    ATH_MSG_INFO("Slink cable  "<<slink
        <<" has "<<(m_CPRoIROD[slink]->size())<<" words");
    for (DataVector<LVL1CTP::SlinkWord>::iterator i=m_CPRoIROD[slink]->begin();
                                      i!=m_CPRoIROD[slink]->end();++i){
      ATH_MSG_INFO<<"Word :"<<hex<<(*i)->word()<<dec);
    }
  }
  */

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;++i){
    StatusCode sc = evtStore()->overwrite(m_CPRoIROD[i],emTauSlinkLocation[i],true);
    if (sc.isSuccess() ){
      ATH_MSG_DEBUG("Stored EM/Tau Slink object at "<< emTauSlinkLocation[i] <<" with "
          <<(m_CPRoIROD[i]->size())<<" words");
    } else {
      ATH_MSG_ERROR("Failed to write EM/Tau Slink object!");
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
  
  /** This is all going to need updating for the new menu structure.
      Comment out in the meanwhile 
  
  L1DataDef def;

  std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.emType() || (*it)->type() == def.tauType() ) {
      ATH_MSG_DEBUG("TriggerThreshold " << (*it)->id() << " has name " << (*it)->name() << endmsg
          << "  threshold number " << (*it)->thresholdNumber() << endmsg
          << "  number of values = " << (*it)->numberofValues() );
      for (std::vector<TriggerThresholdValue*>::const_iterator tv = (*it)->thresholdValueVector().begin();
           tv != (*it)->thresholdValueVector().end(); ++tv) {
        ClusterThresholdValue* ctv;
        ctv = dynamic_cast<ClusterThresholdValue*> (*tv);
	if (!ctv) {
          ATH_MSG_ERROR("Threshold type name is EM/Tau, but is not a ClusterThreshold object!" );
          continue;
        }
        ATH_MSG_DEBUG("ClusterThresholdValue: " << endmsg
            << "  Threshold value = " << ctv->thresholdValueCount() << endmsg
            << "  EM isolation = " << ctv->emIsolationCount() << endmsg
            << "  Had isolation = " << ctv->hadIsolationCount() << endmsg
            << "  Had veto = " << ctv->hadVetoCount() << endmsg
            << "  EtaMin = " << ctv->etamin() << ", EtaMax = " << ctv->etamax() );
        
      } // end of loop over threshold values
    } //  is type == em or tau?
  } // end of loop over thresholds
  
  */
    
}


