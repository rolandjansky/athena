///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LumiCalcSvc.cxx 
// Implementation file for class LumiCalcSvc
// Author: B.Radics<radbal@cern.ch>
//         based on Richard Hawkings' LumiCalc.py
/////////////////////////////////////////////////////////////////// 

// for size_t
#include <cstddef>

// GaudiKernel includes
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"

// Athena include files
#include "StoreGate/StoreGate.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// Gaudi/Athena access
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaBaseComps/AthService.h"

#include <string>

// ROOT
#include "TTree.h"

// stl includes
#include <iomanip>
#include <iostream>

#include "LumiBlockComps/LumiCalcSvc.h"
#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"

LumiCalcSvc::LumiCalcSvc(const std::string& name, 
		       ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  //  p_inputstore(0),
  //  p_metadatastore(0),
  m_pMetaDataStore ("StoreGateSvc/MetaDataStore",      name),
  m_pInputStore    ("StoreGateSvc/InputMetaDataStore", name),
  m_tHistSvc(0),
  m_sourcedb("COOLONL_TRIGGER/OFLP200"),
  m_parlumiestfolder("/TRIGGER/LUMI/LBLEST"),
  m_parlumilvl1folder("/TRIGGER/LUMI/LVL1COUNTERS"),
  m_parlumihltfolder("/TRIGGER/LUMI/HLTCOUNTERS"),
  m_parlvl1prescalesfolder("/TRIGGER/LVL1/Prescales"),
  m_parlvl1menufolder("/TRIGGER/LVL1/Menu"),
  m_parhltmenufolder("/TRIGGER/HLT/Menu"),
  m_recordTTree(true),
  m_fileCurrentlyOpened(false)
{
  // Properties for configuration of LumiCalcSvc
  declareProperty("SourceDb", m_sourcedb);
  declareProperty("LumiEstFolder", m_parlumiestfolder);
  declareProperty("LumiLVL1CountersFolder", m_parlumilvl1folder);
  declareProperty("LumiHLTCountersFolder", m_parlumihltfolder);
  declareProperty("LVL1PrescalesFolder", m_parlvl1prescalesfolder);
  declareProperty("LVL1MenuFolder", m_parlvl1menufolder);
  declareProperty("HLTMenuFolder", m_parhltmenufolder);
  declareProperty("Triggers", m_triggers);
  declareProperty("LBCollNames", m_lbcnames);
  declareProperty("UseMC", m_UseMC = false);
  declareProperty("OnlineLumi", m_Online = false);
  declareProperty("UseLumiTag", m_UseLumiTag = "OflLumi-Dec2009-001"); // ATLAS_PREFERRED or OflLumi_Fake0/1
  declareProperty("UseLumiMethod", m_UseLumiMethod = "ATLAS_PREFERRED"); // ATLAS_PREFERRED or OflLumi_Fake0/1
  declareProperty("UseLumiChannel", m_UseLumiChannel = 0); // 
  declareProperty("Verbose", m_Verbose = false);
  declareProperty("UseInputStore", m_UseInputStore = false);

  m_lumicalc = new LumiCalculator();

  m_LumiTree = NULL;
}

//===========================================================================
LumiCalcSvc::~LumiCalcSvc(){
  delete m_lumicalc;
}

//===========================================================================
StatusCode LumiCalcSvc::initialize(){

  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "In LumiCalcSvc::initialize " << endreq;

  // locate the conditions store ptr to it.

  // locate input metadata store

  ATH_CHECK( m_pMetaDataStore.retrieve() );
  ATH_CHECK( m_pInputStore.retrieve() );
  /*
  StatusCode sc = service("StoreGateSvc/InputMetaDataStore", p_inputstore);
  //sc = m_pInputStore.retrieve();
  if (!sc.isSuccess() || 0 == p_inputstore) {
    log << MSG::ERROR << "Could not find InputMetaDataStore" << endreq;
    return StatusCode::FAILURE;
  }

  // locate metadata store
  sc = service("StoreGateSvc/MetaDataStore", p_metadatastore);
  if (!sc.isSuccess() || 0 == p_metadatastore) {
    log << MSG::ERROR << "Could not find MetaDataStore" << endreq;
    return StatusCode::FAILURE;
  }

  */
  // Set to be listener for begin/end of event
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  StatusCode sc = incSvc.retrieve();
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to get the IncidentSvc" << endreq;
    return(sc);
  }
  incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
  incSvc->addListener(this, "EndInputFile", 50); // pri has to be > 10 to be before MetaDataSvc.
  incSvc->addListener(this, "EndRun", 50); // pri has to be > 10 to be before MetaDataSvc.


  /// Initialize histogram service
  sc = service("THistSvc", m_tHistSvc);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to retrieve pointer to THistSvc!" << endreq;
    return(sc);
  }

  /// Create TTree
  m_LumiTree = new TTree("LumiMetaData","LumiMetaData");
  sc=m_tHistSvc->regTree("/AANT/Lumi",m_LumiTree);
  if(!sc.isSuccess()){
    log << MSG::ERROR << "Cannot register TTree" << endreq;
    return(sc);
  }

  m_lumicalc->setTree(m_LumiTree);
  

  return sc;

}

//===========================================================================
StatusCode LumiCalcSvc::finalize(){

  return StatusCode::SUCCESS;

}

//===========================================================================
void LumiCalcSvc::print(){
  
}


//===========================================================================
// Calculates lumi on a user defined input MetaData LumiBlockCollection
StatusCode LumiCalcSvc::calcLumi(){
  MsgStream log(msgSvc(), name());
  StatusCode status = doDbQuery(m_pMetaDataStore);
  if (!status.isSuccess()){
    log << MSG::WARNING << "Couldn't do Db Query for LumiCalcSvc" << endreq;
  }
  
  return status;

}

//===========================================================================
StatusCode LumiCalcSvc::doDbQuery(StoreGateSvc_t sg) {
  MsgStream log(msgSvc(), name());
  StatusCode status = StatusCode::SUCCESS;
    
  doRecordTree(true);// record history to m_LumiTree TTree
  StoreGateSvc_t store = sg;
  m_LumiTree=m_lumicalc->getTree();

  if (!m_triggers.value().empty() && !m_lbcnames.value().empty()) {
    // collect trigger names
    std::list<TString> trignames;
    for (std::vector<std::string>::const_iterator it = m_triggers.value().begin(); it != m_triggers.value().end(); ++it)
      trignames.push_back(*it);
    // fill registry with lbc names+triggers
    for(std::vector<std::string>::const_iterator itr = m_lbcnames.value().begin(); itr != m_lbcnames.value().end(); itr++) {
      const std::string lbcollname = (*itr);
      this->registerLBCollection(lbcollname,"no description available",trignames);
    }
  }
  
  // Try to Calculate lumi based on list of LumiBlocks in MetaData store
  //===========================================================================

  // loop over registered lbc-trigger combinations
  std::map<TString,tvtPair>::iterator ritr = m_registry.begin();
  for (; ritr!=m_registry.end(); ++ritr) {

    const std::string& lbcollname = ritr->first.Data();
    const std::string& description = ritr->second.first.Data();
    std::list<TString> trignames = ritr->second.second;
    
    if (trignames.empty()) {
      log << MSG::INFO << "No trigger names registered for lb coll: " << lbcollname << ". Copying trigger names from LumiCalcSvc." << endreq;
      for (std::vector<std::string>::const_iterator it = m_triggers.value().begin(); it != m_triggers.value().end(); ++it)
        trignames.push_back(*it);
    }

    if (store->contains<xAOD::LumiBlockRangeContainer>(lbcollname)) {
      //      const DataHandle<xAOD::LumiBlockRangeContainer> iovc;
      const xAOD::LumiBlockRangeContainer* lbcoll = 0;
      status = store->retrieve(lbcoll, lbcollname);

      if (!status.isSuccess()) {
        log << MSG::DEBUG << "Could not find LumiBlockRangeContainer >>" << lbcollname << "<< in " << store->name() << endreq;
      }else {
        log << MSG::DEBUG << " Found LumiBlockRangeContainer >>" << lbcollname << "<< in " << store->name() << " OK " << endreq;
        log << MSG::INFO << " Calculating Integrated Luminosity based on >>" << lbcollname <<  "<< in " << store->name() << endreq;
        if (!description.empty()) { log << MSG::INFO << " Description of >>" << lbcollname << "<< = " << description << endreq; }

	/*
        const LumiBlockRangeContainer* lbcoll = iovc;
        // check against problems
        if(dynamic_cast< const LumiBlockCollection * >(lbcoll) == 0){
          log << MSG::ERROR << "Input LumiBlockRangeContainer to LumiCalcSvc is not of type LumiBlockRangeContainer" << endreq;
          status = StatusCode::FAILURE;
          return status;
        }else{
          if(lbcoll->empty()){
            log << MSG::WARNING << "Input LumiBlockCollection is empty, not calculating!" << endreq;
            status = StatusCode::SUCCESS;
            return status;
          }
        } 
	*/

        for( std::list<TString>::const_iterator it = trignames.begin(); it != trignames.end(); ++it){
          const std::string trigname = it->Data();
          m_lumicalc->SetCollName(lbcollname);
	  m_lumicalc->UseLumiTag(m_UseLumiTag);
	  if(m_UseLumiChannel == -1)m_lumicalc->UseLumiMethod(m_UseLumiMethod);
	  if(m_UseLumiChannel != -1)m_lumicalc->UseLumiChannel(m_UseLumiChannel);
          m_lumicalc->UseMC(m_UseMC.value());
	  m_lumicalc->UseOnlineLumi(m_Online.value());
          m_lumicalc->Verbose(m_Verbose.value());
          m_lumicalc->IntegrateLumi(lbcoll, trigname);
          log << MSG::INFO << "==========================================================" << endreq;
          if(m_Verbose.value() != true)print();
          // detailed list of calculation to screen on demand
          if(m_Verbose.value() == true)printTree();
        }
      }
    } else {
      log << MSG::WARNING << "Could not find LumiBlockCollection >>" << lbcollname << "<< in " << store->name() << endreq;
    }
  } // end loop over registered lbc-trigger combinations  

  return status;
}

//===========================================================================
void LumiCalcSvc::handle(const Incident& inc) {
  MsgStream log(msgSvc(), name());
  const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
  std::string fileName;
  if (fileInc == 0) { fileName = "Undefined "; }
  else { fileName = fileInc->fileName(); }
  log << MSG::DEBUG << "handle() " << inc.type() << " for file: " << fileName << endreq;

  if (inc.type() == "BeginInputFile") {
    log << MSG::DEBUG << "BeginTagFile incident fired!" << endreq;
    m_fileCurrentlyOpened = true;
    if(m_UseInputStore == true){
      doDbQuery(m_pInputStore);
    }
  }
  else if (inc.type() == "EndInputFile") {
    m_fileCurrentlyOpened = false;

  }
  else if (inc.type() == "EndRun") {
  }
}

StatusCode LumiCalcSvc::stop(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "LumCalcSvc::stop() method is called" << endreq;
  if (m_fileCurrentlyOpened == true)log << MSG::DEBUG << "File was read partially" << endreq;
  if (m_fileCurrentlyOpened == false)log << MSG::DEBUG << "File was read fully" << endreq; 

  return StatusCode::SUCCESS;

}


//===========================================================================
void LumiCalcSvc::printTree(){

  if(m_LumiTree != NULL)m_LumiTree->Scan("*");

}

//===========================================================================
StatusCode LumiCalcSvc::queryInterface( const InterfaceID& riid, void** ppvi ){
  // valid placeholder? 
  if ( 0 == ppvi ) { return StatusCode::FAILURE ; }  // RETURN 
  if ( ILumiCalcSvc::interfaceID() == riid ) 
    {
      *ppvi = static_cast<ILumiCalcSvc*>(this);
      addRef(); // NB! : inrement the reference count!
      return StatusCode::SUCCESS;                     // RETURN
    }
  // Interface is not directly availible: try out a base class
  return Service::queryInterface( riid, ppvi );
}


//===========================================================================
bool 
LumiCalcSvc::registerLBCollection(const TString& tname, const TString& regexpr, const std::list<TString>& trigpar)
{
  MsgStream log(msgSvc(), name());

  if (m_registry.find(tname)!=m_registry.end()) {
    log << MSG::WARNING << "registerLBCollection() :: LB collection with name <" << tname << "> already registered. Return false." << endreq;
    return false;
  }

  log << MSG::DEBUG << "registerLBCollection() :: LB collection with name <" << tname << "> registered." << endreq;
  m_registry[tname] = tvtPair(regexpr,trigpar);

  return true;
}

