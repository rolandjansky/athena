/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/

#include "ALP/FileDataSource.h"
#include <algorithm>
#include <unordered_set>
#include "ers/ers.h"
//#include "eformat/FullEventFragment.h"
#include "eformat/eformat.h"
#include "eformat/write/eformat.h"
#include "eformat/index.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/FullEventFragmentNoTemplates.h"
#include "EventStorage/DataReader.h"
#include "EventStorage/pickDataReader.h"
#include "EventStorage/DataWriter.h"
#include "ALP/DataSourceExceptions.h"

const std::unordered_set<uint32_t> L1R_ROBS =
{
  0x7300a8, 0x7300a9, 0x7300aa, 0x7300ab, // TDAQ_CALO_CLUSTER_PROC_ROI ROBs
  0x7500ac, 0x7500ad,                     // TDAQ_CALO_JET_PROC_ROI ROBs
  0x760001,                               // TDAQ_MUON_CTP_INTERFACE ROB
  0x770001,                               // TDAQ_CTP ROB
  0x910081, 0x910082,  0x910091, 0x910092  // TDAQ_CALO_TOPO_PROC ROBs
};

ALPNS::FileDataSource::~FileDataSource(){
  delete m_currReader;
  delete m_currEvent;
  delete m_IDmap;
  delete[] m_blob;
  delete m_l1r;
  delete m_collectedRobs;
}

ALPNS::FileDataSource::FileDataSource(){
  m_stride=0;
  m_currEventInFile=0;
  m_currFile=-1;
  m_start=1;
  m_currReader=0;
  m_currEvent=0;
  m_loopFiles=false;
  m_blob=0;
  m_fileNames=new std::vector<std::string>();
  m_IDmap=new std::unordered_map<uint32_t, const uint32_t*>(); ///< The ID <-> ROB map
  m_l1r=new std::vector<eformat::ROBFragment<const uint32_t*> >(); ///< The LVL1 result
  m_collectedRobs=new std::set<uint32_t>();
  m_comp=eformat::UNCOMPRESSED;
  m_compLevel=2;
  DataCollector::instance(this);
}

bool ALPNS::FileDataSource::configure(const boost::property_tree::ptree &pt){
  const boost::property_tree::ptree& args=pt.get_child("Configuration.ALPApplication.DataSource.HLTFileDataSource");
  ERS_LOG("Configuring FileDataSource");
  m_start=args.get("start_id",1);
  m_stride=args.get("stride",1);
  m_currFile=args.get("fileOffset",-1);
  m_loopFiles=(args.get("loopOverFiles","false")!="false");
  m_nMaxEvents=args.get("numEvents",-1);
  try{
    auto of=args.get_child("outputFileName");
    m_outFileName=std::string(of.data());
    ERS_LOG("Output file name "<<m_outFileName);
  }catch(boost::property_tree::ptree_bad_path &ex){
    ERS_DEBUG(1,"Output file name is not specified");
    ERS_LOG("Failed to get outputFileName");
  }
  try{
    auto of=args.get_child("compressionFormat");
    std::string ctype(of.data());
    if(ctype=="ZLIB")m_comp=eformat::ZLIB;
    m_compLevel=args.get("compressionLevel",2);
  }catch(boost::property_tree::ptree_bad_path &ex){
    ERS_DEBUG(1,"Compression is not specified");
    ERS_LOG("Failed to get Compression information");
  }

  m_runParams=std::make_unique<EventStorage::run_parameters_record>();
  m_runParams->run_number = 1234123412;
  m_runParams->max_events = 0;
  m_runParams->rec_enable = 0;
  m_runParams->trigger_type = 0;
  m_runParams->detector_mask_LS = 0;
  m_runParams->detector_mask_MS = 0;
  m_runParams->beam_type = 0;
  m_runParams->beam_energy = 0;

  std::pair<boost::property_tree::ptree::const_assoc_iterator, 
    boost::property_tree::ptree::const_assoc_iterator> r(args.get_child("fileList").equal_range("file"));
  for (boost::property_tree::ptree::const_assoc_iterator it(r.first); it != r.second; ++it) {
    m_fileNames->push_back(it->second.get_value<std::string>());
    ERS_LOG(" Adding file "+it->second.get_value<std::string>());
  }
  if(m_fileNames->size()==0){
    ERS_LOG("No input files specified");
    return false;
  }
  //open the first file
  return true;
}

bool ALPNS::FileDataSource::prepareForRun(const boost::property_tree::ptree& /*args*/){
  return true;
}

uint32_t ALPNS::FileDataSource::collect(
     std::vector<hltinterface::DCM_ROBInfo>& data,
     const uint32_t lvl1_id, const std::vector<uint32_t>& ids){
  data.reserve(ids.size());
  for (std::vector<uint32_t>::const_iterator 
	 it = ids.begin(); it != ids.end(); ++it) {
    auto comp = m_IDmap->find(*it);
    if (comp == m_IDmap->end()) {
      char buff[400];
      snprintf(buff,400,"Event with LVL1 id=%u does NOT contain ROB 0x%08x",lvl1_id,*it);
      // boost::format msg("Event with LVL1 id=%lu does NOT contain ROB 0x%08x");
      // msg % lvl1_id % *it;
      ERS_DEBUG(1, buff);
      continue;
    }
    //this will create a copy of the ROBFragment, but so what?
    eformat::ROBFragment<const uint32_t*> cf(comp->second);
    data.emplace_back(cf,true,std::chrono::steady_clock::now(),
					     std::chrono::steady_clock::now());
    m_collectedRobs->insert(*it);
  }
  ERS_DEBUG(1, "Request with LVL1 id. " << lvl1_id 
	    << " had requests for " << ids.size()
	    << " ROBs and got " << data.size() << " fragments.");
  return data.size();
}

uint32_t ALPNS::FileDataSource::collect(
					std::vector<hltinterface::DCM_ROBInfo>& data,
					uint32_t lvl1_id){
  ERS_DEBUG(1,"Called collect with "<<lvl1_id);  
  auto id=m_IDmap->begin(),idend=m_IDmap->end();
  for(;id!=idend;++id){
    if(m_collectedRobs->find(id->first)!=m_collectedRobs->end()){
      eformat::ROBFragment<const uint32_t*> cf(id->second);
      data.push_back(hltinterface::DCM_ROBInfo(cf,true,std::chrono::steady_clock::now(),std::chrono::steady_clock::now()));
      m_collectedRobs->insert(id->first);
    }
  }
  ERS_DEBUG(1,"returning "<<data.size()<<" fragments");
  return data.size();
}

bool ALPNS::FileDataSource::skipEvents(uint num){
  for(uint i=0;i<num;i++){
    uint32_t *evt=getNextEvent();
    delete evt;
  }
  return true;
}

uint32_t* ALPNS::FileDataSource::getNextEvent(){
  char *buff;
  unsigned int size=0;
  int error_code=m_currReader->getData(size,&buff);
  while (error_code == DRWAIT) {
    usleep(500000);
    ERS_INFO("[FileDataSource] Waiting for more data.");
    error_code = m_currReader->getData(size, &buff);
  }
  if (error_code == DRNOOK) {
    ERS_INFO("[FileDataSource] Reading of data NOT OK!");
    delete buff;
    return 0;
  }
  if (error_code == DROK) {
    ERS_DEBUG(1, "[FileDataSource] Event OK");
  }
  uint32_t *blob=reinterpret_cast<uint32_t*>(buff);
  ERS_LOG("READ a new event");
  return blob;
}

void ALPNS::FileDataSource::getL1Result(std::vector<eformat::ROBFragment<const uint32_t*> > &l1r,
					uint32_t &lvl1_id,
					uint64_t &gid, 
					uint64_t &lumiBlock){

  delete[] m_blob;
  delete m_currEvent;
  m_currEvent=0;
  m_blob=0;
  m_collectedRobs->clear();
  m_IDmap->clear();
  m_l1r->resize(0);//keep the buffer;
  if((m_nMaxEvents>0) && (m_nEvents>=m_nEventsToRead)){
    throw ALPNS::DSErrors::NoMoreEvents("Event count reached");
    return;
  }
  uint eventsInFile=m_currReader->eventsInFile();
  //read gid and lumiblock from file and fill it in here
  gid=0;
  lumiBlock=0;

  if(m_currEventInFile+m_stride<eventsInFile){
    unsigned int target=m_currEventInFile+m_stride-1;
    while(m_currEventInFile<target){//skip events
      m_blob=getNextEvent();
      m_currEventInFile++;
      delete[] m_blob;
    }
    m_blob=getNextEvent();
    m_currEventInFile++;
  }else{
    int newoffset=m_currEventInFile+m_stride-eventsInFile;
    if(nextFile()){
      eventsInFile=m_currReader->eventsInFile();
      if(eventsInFile>(uint)newoffset){
	if(newoffset>0){
	  skipEvents(newoffset-1);
	}
	m_blob=getNextEvent();
	m_currEventInFile=newoffset;
      }else{
	lvl1_id=0;
	throw ALPNS::DSErrors::NoMoreEvents("File don't contain enough events");
	//throwexception
	return;
      }
    }else{
      lvl1_id=0;
      //throw exception
      throw ALPNS::DSErrors::NoMoreEvents("Can't open next file");
      return;
    }
  }
  if(!m_blob){
      throw ALPNS::DSErrors::NoMoreEvents("Can't read anymore events!");
  }
  m_nEvents++;
  m_currEvent=new eformat::read::FullEventFragment(m_blob);
  // Build a subdetector based table-of-contents of this event
  lvl1_id=m_currEvent->lvl1_id();
  gid=m_currEvent->global_id();
  lumiBlock=m_currEvent->lumi_block();
  ERS_LOG("READ EVENT l1="<<lvl1_id<<" gid= "<<gid<<" LB= "<<lumiBlock<<" count= "<<m_nEvents<<"/"<<m_nEventsToRead);
  //  std::map<eformat::SubDetector, std::vector<const uint32_t*> > sd_toc;
  std::map<uint32_t, const uint32_t* > sd_toc;
  eformat::helper::build_toc(*m_currEvent, sd_toc);

  for(auto it = sd_toc.begin(); it != sd_toc.end(); ++it) {
    auto sourceId=eformat::helper::SourceIdentifier(it->first);
    auto subId=sourceId.subdetector_id();
    switch (subId) {
      case eformat::TDAQ_BEAM_CRATE:
      case eformat::TDAQ_SFI:
      case eformat::TDAQ_SFO:
      case eformat::TDAQ_LVL2:
      case eformat::TDAQ_HLT:
      case eformat::OTHER:
        //we ignore these
        break;
      default:
	auto robId=sourceId.simple_code();
	// char bb[100];
	// snprintf(bb,100,"0x%x",robId);
	// ERS_LOG("Running for ROBId "<<bb);
	if((subId==eformat::TDAQ_CALO_FEAT_EXTRACT_ROI)||
	   (L1R_ROBS.find(robId)!=L1R_ROBS.end())
	  ){
	  // ERS_LOG("Found RobID "<<robId);
	  m_l1r->emplace_back(it->second);
	  l1r.emplace_back(it->second);
	}else{
        //these, we include in our internal data map for fast access
	  eformat::ROBFragment<const uint32_t*> rob(it->second);
	  auto mit =  m_IDmap->find(rob.source_id());
	  //if I already have an entry there...
	  if (mit != m_IDmap->end()) {
	    ERS_DEBUG(1,"Duplicate ROBS in file. Ignoring");
	  }else{
	    m_IDmap->operator[](rob.source_id()) = it->second;
	    
	  }
	}
	break;
    }
  }
  ERS_DEBUG(1,"returning l1id="<<lvl1_id);
}
  
void ALPNS::FileDataSource::reserveROBData(const uint32_t /*lvl1_id*/, 
					   const std::vector<uint32_t>& /*ids*/){
  ERS_LOG("Called reserveROBData ");
}


void ALPNS::FileDataSource::sendResult(const bool accept,
					const uint32_t l1id, 
					const hltinterface::HLTResult& res){
  ERS_LOG("Got result for l1id="<<l1id<< (accept?" ACCEPT ":" REJECT "));
  if(!m_writer)return;
  if(accept){
    if (res.stream_tag.size()==0){
      ERS_LOG("Event is accepted but stream tags are empty!");
      return;
    }
    try{
      eformat::ROBFragment<const uint32_t*>(res.fragment_pointer).check(); 
    }catch(ers::Issue& iss){
      ERS_LOG("Invalid HLT result fragment"<<iss.what());
      return;
    }
    eformat::write::FullEventFragment newEvent;
    newEvent.copy_header(m_currEvent->start());
    //set compression type
    newEvent.compression_type(m_comp);
    newEvent.compression_level(m_compLevel);
    newEvent.lvl2_trigger_info(0,nullptr);
    // check if there are L1 simulation bits
    if(res.l1Simul_robs.size()){
      newEvent.lvl1_trigger_info(res.l1Triggers.size(),&(res.l1Triggers[0]));
    }
    // set hlt result bits
    if(res.trigger_info.size()){
      newEvent.hlt_info(res.trigger_info.size(),&(res.trigger_info[0]));
    }
    //check stream tags
    //this needs to be changed for multi-stream output.
    uint32_t* streamtag=nullptr;
    if(res.stream_tag.size()){
      auto size=eformat::helper::size_word(res.stream_tag);
      streamtag=new uint32_t[size];
      eformat::helper::encode(res.stream_tag,size,streamtag);
      newEvent.stream_tag(size,streamtag);
    }
    std::vector<eformat::read::ROBFragment> currRobs;
    m_currEvent->robs(currRobs);
    
    std::set<uint32_t> robset;
    std::set<eformat::SubDetector> detset;
    for(const auto &t:res.stream_tag){
      if(t.robs.empty()&& t.dets.empty()){
	robset.clear();
	detset.clear();
	break;
      }else{
	std::copy(std::begin(t.robs), std::end(t.robs), 
		  std::inserter(robset,std::begin(robset)));
	std::copy(std::begin(t.dets), std::end(t.dets), 
		  std::inserter(detset,std::begin(detset)));
      }
    }
    std::vector<const uint32_t*> newRobs;
    newRobs.reserve(currRobs.size()+res.hltResult_robs.size()+res.l1Simul_robs.size());
    //copy old robs
    std::set<uint32_t> l1Overrides;
    for(const auto & r : res.l1Simul_robs){
      l1Overrides.insert(r.source_id());
    }
    for(const auto& r:currRobs){
      auto sId=r.source_id();
      auto subId=eformat::helper::SourceIdentifier{sId}.subdetector_id();
      if((subId!=eformat::TDAQ_HLT)&&(subId!=eformat::TDAQ_LVL2)){//filter HLT and L2 results
	if(robset.empty()&&detset.empty()){
	  if(l1Overrides.find(sId)==l1Overrides.end()){//don't copy l1 results if simulated exists
	    newRobs.push_back(r.start());
	  }
	}else{
	  if(robset.find(sId)!=robset.end()||
	     detset.find(subId)!=detset.end()){
	    if(l1Overrides.find(sId)==l1Overrides.end()){
	      newRobs.push_back(r.start());
	    }
	  }
	}
      }
    }
    //append l1 results to the new robs. This would work iff
    // L1 results are unique
    for(const auto& r:res.l1Simul_robs){
      newRobs.push_back(r.start());
    }

    //copy new robs
    for(const auto& r:res.hltResult_robs){
      auto sId=r.source_id();
      auto subId=eformat::helper::SourceIdentifier{sId}.subdetector_id();
      if(robset.empty()&&detset.empty()){
	newRobs.push_back(r.start());
      }else{
	if(robset.find(sId)!=robset.end()||
	   detset.find(subId)!=detset.end()){
	  newRobs.push_back(r.start());
	}
      } 
    }
    //do I need to do that?
    std::vector<eformat::write::ROBFragment> robs2write;
    robs2write.reserve(newRobs.size());
    for(size_t t=0;t<newRobs.size();t++){
      robs2write.emplace_back(newRobs[t]);
      newEvent.append(&(robs2write.back()));
    }
    //compression happens here
    const eformat::write::node_t* top=newEvent.bind();
    auto finalSize=newEvent.size_word();
    uint32_t* finalEvent=new uint32_t[finalSize];
    auto res=eformat::write::copy(*top,finalEvent,finalSize);
    if(res!=finalSize){
      ERS_LOG("ERROR Event serialization failed. l1id= "<<l1id);
      delete[] finalEvent;
      delete[] streamtag;
      return;
    }
    auto wres=m_writer->putData(finalSize,finalEvent);
    if(wres){
      ERS_LOG("Writing event failed");
    }
    delete[] finalEvent;
    delete[] streamtag;
    
  }
}

bool ALPNS::FileDataSource::finalize(const boost::property_tree::ptree& /*args*/){
  return true;
}

bool ALPNS::FileDataSource::nextFile(){
  if(m_currFile<(int)m_fileNames->size()){
    if(m_currReader){
      delete m_currReader;
      m_currReader=0;
    }
    m_currFile++;
    ERS_LOG("Openning file "<<m_fileNames->at(m_currFile));
    m_currReader=pickDataReader(m_fileNames->at(m_currFile));
    if(!m_currReader){
      ERS_LOG("Failed to open file \""<<m_fileNames->at(m_currFile)<<"\" good() call= "<<m_currReader->good()); 
      if(m_loopFiles&&m_currFile>=(int)(m_fileNames->size()))m_currFile=-1;
      return false;
    }
    ERS_DEBUG(1,"Opened file \""<<m_fileNames->at(m_currFile)<<"\" good() call= "<<m_currReader->good()); 
    if(m_loopFiles&&m_currFile>=((int)(m_fileNames->size())))m_currFile=-1;
    return true;
  }
  ERS_DEBUG(1,"m_currFile="<<m_currFile<<", m_fileNames->size()="<<m_fileNames->size());
  return false;
}

bool ALPNS::FileDataSource::prepareWorker(const boost::property_tree::ptree& args){
  nextFile();
  m_start=args.get("start_id",1);
  m_stride=args.get("stride",1);
  //return true;
  if(m_start>1){
    skipEvents(m_start-1);
  }
  m_nEventsToRead=m_nMaxEvents/m_stride;
  if((m_nMaxEvents%m_stride)>=(m_start)){
    m_nEventsToRead++;
  }
  m_nEvents=0;
  if(!m_outFileName.empty()){
    char buff[2000];
    snprintf(buff,2000,"%s_Child-%03d",m_outFileName.c_str(),m_start);
    ERS_LOG("Opening outputfile "<<buff);
    m_writer=std::make_unique<EventStorage::DataWriter>(std::string("."),//use work Directory right now
							std::string(buff),
							*m_runParams,
							std::vector<std::string>(),
							1);
    m_writer->setMaxFileMB(2000);
  }
  ERS_LOG("Prepare Worker done m_start="<<m_start<<" stride="<<m_stride);

  return true;
}

bool ALPNS::FileDataSource::finalizeWorker(const boost::property_tree::ptree& /*args*/){
  return true;
}
