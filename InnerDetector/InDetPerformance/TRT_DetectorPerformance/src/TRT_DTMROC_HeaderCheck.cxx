/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DTMROC_HeaderCheck.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TRT_DetectorPerformance/TRT_DTMROC_HeaderCheck.h"
#include <set>
#include <fstream>
#include <string>


using namespace std;

//================ Constructor =================================================

InDet::TRT_DTMROC_HeaderCheck::TRT_DTMROC_HeaderCheck(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_sgSvc("StoreGateSvc",name),
  m_histSvc("THistSvc", name),
  m_bytestream_conditionsSvc("TRT_ByteStream_ConditionsSvc",name),
  eventCount(-1)
{
  declareProperty("StoreGateSvc", m_sgSvc);
  declareProperty("THistSvc", m_histSvc);
  declareProperty("TRT_ByteStream_ConditionsSvc", m_bytestream_conditionsSvc);
}

//================ Destructor =================================================

InDet::TRT_DTMROC_HeaderCheck::~TRT_DTMROC_HeaderCheck()
{}


//================ Initialisation =================================================

StatusCode InDet::TRT_DTMROC_HeaderCheck::initialize()
{
 
  if (msgLvl(MSG::VERBOSE)) msg(MSG::INFO) << name() << " initialize()" << endreq;

  // retrieve the StoreGate Service
  if ( m_sgSvc.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_sgSvc << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved service " << m_sgSvc << endreq;
  
  //retrieve Bytestream_conditions Service
  if ( m_bytestream_conditionsSvc.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve service " <<  m_bytestream_conditionsSvc << endreq;
    return StatusCode::FAILURE;
  } else 
    if (msgLvl(MSG::VERBOSE)) msg(MSG::INFO) << "Retrieved service " << m_bytestream_conditionsSvc << endreq;
  
  //retrieve Histogramming Service
  if ( m_histSvc.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Failed to retrieve service " <<  m_histSvc << endreq;
    return StatusCode::FAILURE;
  } else 
    if (msgLvl(MSG::VERBOSE)) msg(MSG::INFO) << "Retrieved service " << m_histSvc << endreq;
  
  //create map for DTMROC id's
  
  ifstream f("../src/mapping.txt"); //This assumes that the jobOptions are run from a run directory at the same leves as src
  if (f.fail()) {
     msg(MSG::FATAL) << "Unable to find mapping file" << endreq;
     return StatusCode::FAILURE;
  }	
  dtmroc_map = new std::map<uint32_t,uint32_t>;
  uint32_t header_word;
  uint32_t output_id;
  std::string line;

  while (getline(f, line)){
    sscanf(line.c_str(),"%x, %u", &header_word, &output_id);
    dtmroc_map->insert(std::pair<uint32_t,uint32_t>(header_word, output_id));
  }
  

  ///////////////
  //Initialize TTree and register with THistSvc
  ///////////////

  m_tree   = new TTree("dtmrocHeaders", "dtmrocHeaders");

  std::string fullTreeName =  "/AANT/dtmrocHeaders" ;
  if((m_histSvc->regTree(fullTreeName, m_tree)).isFailure()) {
    msg(MSG::ERROR) << "Unable to register TTree : " << fullTreeName << endreq;
    return StatusCode::FAILURE;
  }

  ////////////
  //Add Branches to TTree to be output to Ntuple
  ////////////
   
  m_tree->Branch("event_number", &m_event_number, "event_number/i");
  m_tree->Branch("time_stamp", &m_time_stamp, "time_stamp/i");
  m_tree->Branch("time_stamp_ns_offset", &m_time_stamp_ns_offset, "time_stamp_ns_offset/i");
 
  m_tree->Branch("total_num_l1id_err",  &m_total_num_l1id, "total_num_l1id_err/i");
  m_tree->Branch("total_num_bcid_err",  &m_total_num_bcid, "total_num_bcid_err/i");
  m_tree->Branch("total_num_missing_err",  &m_total_num_missing, "total_num_missing_err/i");
  m_tree->Branch("total_num_general_err",  &m_total_num_general, "total_num_general_err/i");
  m_tree->Branch("total_num_sendid_err",  &m_total_num_sendid, "total_num_sendid_err/i");
 
  m_tree->Branch("bcid_returned_by_ROD", &m_bcid_returned_by_ROD, "bcid_returned_by_ROD/i");
  m_tree->Branch("l1id_returned_by_ROD", &m_l1id_returned_by_ROD, "l1id_returned_by_ROD/i");
   
  m_tree->Branch("l1id_dtmroc_index",  &m_l1id_dtmroc_index);
  m_tree->Branch("l1id_dtmroc_index_remap",  &m_l1id_dtmroc_index_remap);
  m_tree->Branch("l1id_rod_index",  &m_l1id_rod_index);
  m_tree->Branch("l1id_returned_l1id",  &m_l1id_returned_l1id);

  m_tree->Branch("bcid_dtmroc_index",  &m_bcid_dtmroc_index);
  m_tree->Branch("bcid_dtmroc_index_remap",  &m_bcid_dtmroc_index_remap); 
  m_tree->Branch("bcid_rod_index",  &m_bcid_rod_index);
  m_tree->Branch("bcid_returned_bcid",  &m_bcid_returned_bcid);

  m_tree->Branch("missing_dtmroc_index",  &m_missing_dtmroc_index);
  m_tree->Branch("missing_dtmroc_index_remap",  &m_missing_dtmroc_index_remap);
  m_tree->Branch("missing_rod_index",  &m_missing_rod_index);

  m_tree->Branch("general_dtmroc_index",  &m_general_dtmroc_index);
  m_tree->Branch("general_dtmroc_index_remap",  &m_general_dtmroc_index_remap);
  m_tree->Branch("general_rod_index",  &m_general_rod_index);

  m_tree->Branch("sendid_dtmroc_index",  &m_sendid_dtmroc_index);
  m_tree->Branch("sendid_dtmroc_index_remap",  &m_sendid_dtmroc_index_remap);
  m_tree->Branch("sendid_rod_index",  &m_sendid_rod_index);

  //Initialize variables
  m_event_number = 0;
  m_time_stamp = 0;
  m_time_stamp_ns_offset = 0;
  
  m_total_num_l1id=0;
  m_total_num_bcid=0;
  m_total_num_missing=0;
  m_total_num_general=0;
  m_total_num_sendid=0;

  m_bcid_returned_by_ROD = 0;
  m_l1id_returned_by_ROD = 0;

  m_l1id_dtmroc_index = new std::vector<uint32_t>;
  m_l1id_dtmroc_index_remap = new std::vector<uint32_t>;
  m_l1id_rod_index = new std::vector<uint32_t>;
  m_l1id_returned_l1id = new std::vector<uint32_t>;

  m_bcid_dtmroc_index = new std::vector<uint32_t>;
  m_bcid_dtmroc_index_remap = new std::vector<uint32_t>;
  m_bcid_rod_index = new std::vector<uint32_t>;
  m_bcid_returned_bcid = new std::vector<uint32_t>; 

  m_missing_dtmroc_index = new std::vector<uint32_t>;
  m_missing_dtmroc_index_remap = new std::vector<uint32_t>;
  m_missing_rod_index = new std::vector<uint32_t>;

  m_general_dtmroc_index = new std::vector<uint32_t>;
  m_general_dtmroc_index_remap = new std::vector<uint32_t>;
  m_general_rod_index = new std::vector<uint32_t>;

  m_sendid_dtmroc_index = new std::vector<uint32_t>;
  m_sendid_dtmroc_index_remap = new std::vector<uint32_t>;
  m_sendid_rod_index = new std::vector<uint32_t>;

  if (msgLvl(MSG::VERBOSE)) msg(MSG::INFO) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::TRT_DTMROC_HeaderCheck::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode InDet::TRT_DTMROC_HeaderCheck::execute()
{

  ++eventCount;
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "InDet::TRT_DetectorPlots::execute() event " << eventCount << endreq;

  //////////////////////////
  /// Retrieve event info
  //////////////////////////
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Retrieving event info for event " << eventCount << endreq;
  
  m_bcid_returned_by_ROD = 0;
  TRT_BCIDColl = 0;
/*
  StatusCode sc = m_sgSvc->retrieve(TRT_BCIDColl,"TRT_BCID");
  if (sc.isFailure()){
    msg(MSG::ERROR) << "BCID Collection not found in Store Gate" <<endreq;
    m_bcid_returned_by_ROD = -1;
  } else {
    InDetTimeCollection::const_iterator itrt_bcid = TRT_BCIDColl->begin();
    m_bcid_returned_by_ROD = (*itrt_bcid)->second; // perhaps not the best method - just trusting the first ROD to always have the right BCID
  }
*/
  m_l1id_returned_by_ROD = 0;
  TRT_L1IDColl = 0;
/*
  sc = m_sgSvc->retrieve(TRT_L1IDColl,"TRT_LVL1ID");
  if (sc.isFailure()){
    msg(MSG::ERROR) << "L1ID Collection not found in Store Gate" <<endreq;
    m_l1id_returned_by_ROD = -1;
  } else {
    InDetTimeCollection::const_iterator itrt_l1id = TRT_L1IDColl->begin();
    m_l1id_returned_by_ROD = (*itrt_l1id)->second; // perhaps not the best method - just trusting the first ROD to always have the right L1ID
  }
  */
  const EventInfo* eventInfo;
  if (m_sgSvc->retrieve(eventInfo).isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve event identifier from Store Gate"<<endreq;
    m_event_number =0;
  } else {
    EventID* evtid = eventInfo->event_ID();
    m_event_number = evtid->event_number();
    m_time_stamp = evtid->time_stamp();
    m_time_stamp_ns_offset = evtid->time_stamp_ns_offset();
  }    
  

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "InDet::TRT_DTMROC_HeaderCheck:: execute() retrieved event info for event " << eventCount << endreq;

  ///////////////////
  //Initialize containers
  ///////////////////
  
  m_l1id_dtmroc_index->clear();
  m_l1id_dtmroc_index_remap->clear();
  m_l1id_rod_index->clear();
  m_l1id_returned_l1id->clear();

  m_bcid_dtmroc_index->clear();
  m_bcid_dtmroc_index_remap->clear();
  m_bcid_rod_index->clear();
  m_bcid_returned_bcid->clear();


  m_missing_dtmroc_index->clear();
  m_missing_dtmroc_index_remap->clear();
  m_missing_rod_index->clear();

  m_general_dtmroc_index->clear();
  m_general_dtmroc_index_remap->clear();
  m_general_rod_index->clear();
 
  m_sendid_dtmroc_index->clear();
  m_sendid_dtmroc_index_remap->clear();
  m_sendid_rod_index->clear();

  /////////////////////////
  //Load/derive Ntuple Data
  /////////////////////////

  m_total_num_l1id = m_bytestream_conditionsSvc->getNumberOfErrors(0);
  m_total_num_bcid = m_bytestream_conditionsSvc->getNumberOfErrors(1);
  m_total_num_missing = m_bytestream_conditionsSvc->getNumberOfErrors(2);
  m_total_num_general = m_bytestream_conditionsSvc->getNumberOfErrors(3);
  m_total_num_sendid = m_bytestream_conditionsSvc->getNumberOfErrors(4);

  std::set<std::pair<uint32_t, uint32_t> >::iterator errIt;
 
  errIt = m_bytestream_conditionsSvc->getL1idErrorSet().begin();
  int size = m_bytestream_conditionsSvc->getL1idErrorSet().size();
  
  for (int i =0; i != size; i++) {

    m_l1id_dtmroc_index_remap->push_back(dtmroc_map->find(errIt->first &0xff)->second); 
    m_l1id_dtmroc_index->push_back(errIt->first & 0xff); 
    m_l1id_rod_index->push_back((errIt->first & 0xffffff00)/0x100);
    m_l1id_returned_l1id->push_back(errIt->second);
    ++errIt;
  }

  errIt = m_bytestream_conditionsSvc->getBcidErrorSet().begin();
  size = m_bytestream_conditionsSvc->getBcidErrorSet().size();
  
  for (int i =0; i != size; i++) {
   
     m_bcid_dtmroc_index_remap->push_back(dtmroc_map->find(errIt->first &0xff)->second);
     m_bcid_dtmroc_index->push_back(errIt->first & 0xff);
     m_bcid_rod_index->push_back((errIt->first & 0xffffff00)/0x100);
     m_bcid_returned_bcid->push_back(errIt->second);
     ++errIt;
  }

  std::set<uint32_t>::iterator errIt2;

  errIt2 = m_bytestream_conditionsSvc->getMissingErrorSet().begin();
  size = m_bytestream_conditionsSvc->getMissingErrorSet().size();
  
  for (int i =0; i != size; i++) {
  
    m_missing_dtmroc_index_remap->push_back(dtmroc_map->find(*errIt2 & 0xff)->second);
    m_missing_dtmroc_index->push_back(*errIt2 & 0xff); 
    m_missing_rod_index->push_back((*errIt2 & 0xffffff00)/0x100);
    ++errIt2;
  }


  errIt2 = m_bytestream_conditionsSvc->getErrorErrorSet().begin();
  size = m_bytestream_conditionsSvc->getErrorErrorSet().size();
  
  for (int i =0; i != size; i++) {
    m_general_dtmroc_index_remap->push_back(dtmroc_map->find(*errIt2 & 0xff)->second); 
    m_general_dtmroc_index->push_back(*errIt2 & 0xff); 
    m_general_rod_index->push_back((*errIt2 & 0xffffff00)/0x100);
    ++errIt2;
  }
  
  errIt2 = m_bytestream_conditionsSvc->getSidErrorSet().begin();
  size = m_bytestream_conditionsSvc->getSidErrorSet().size();
  
  for (int i =0; i != size; i++) {
    m_sendid_dtmroc_index_remap->push_back(dtmroc_map->find(*errIt2 & 0xff)->second); 
    m_sendid_dtmroc_index->push_back(*errIt2 & 0xff); 
    m_sendid_rod_index->push_back((*errIt2 & 0xffffff00)/0x100);
    ++errIt2;
  }


  if (m_total_num_missing > 4000){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << m_total_num_missing <<" Missing Chips in Event #: " <<eventCount << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ROD BCID is : 0x" << hex << m_bcid_returned_by_ROD << " and L1ID is : 0x" << m_l1id_returned_by_ROD << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of bcid errors is : " << dec << m_total_num_bcid << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of L1id errors is : " << m_total_num_l1id << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of generalized error bits set is : " <<  m_total_num_sendid <<endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of sendid errors is : " << m_total_num_sendid<< endreq;

  }

  m_tree->Fill();
  
  return StatusCode::SUCCESS;
}

//============================================================================================
