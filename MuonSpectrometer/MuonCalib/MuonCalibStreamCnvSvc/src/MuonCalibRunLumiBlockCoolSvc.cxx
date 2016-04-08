/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStreamCnvSvc/MuonCalibRunLumiBlockCoolSvc.h"

// COOL includes
#include "CoraCool/CoraCoolDatabaseSvcFactory.h"
#include "CoraCool/CoraCoolDatabaseSvc.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/ChannelSelection.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"
#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObjectIter.h"
#include "CoraCool/CoraCoolObject.h"
#include "CoralBase/Blob.h"

MuonCalibRunLumiBlockCoolSvc::MuonCalibRunLumiBlockCoolSvc(const std::string & name, ISvcLocator *svc_locator) : AthService(name, svc_locator), 
        m_lumi_block_fldr_by_lumiblock("/TRIGGER/LUMI/LBLB"),
	m_lumibloc_fldr_by_time("/TRIGGER/LUMI/LBTIME"),
	m_valid_for_run(-1),
	m_min_run_time(UINT_MAX), 
	m_max_run_time(0),
	m_current_lumi_block(0),
	m_min_lb_time(UINT_MAX),
	m_max_lb_time(0) {
  declareProperty("LumiBlockFolderByLB", m_lumi_block_fldr_by_lumiblock);
  declareProperty("LumiBlockFolderByTime", m_lumibloc_fldr_by_time);
}
	
StatusCode MuonCalibRunLumiBlockCoolSvc::queryInterface(const InterfaceID& riid, void** ppvUnknown) {
  if (IID_IMuonCalibRunLumiBlockCoolSvc.versionMatch(riid)) { 
    *ppvUnknown = (MuonCalibRunLumiBlockCoolSvc *)this; 
  } else { 
    return AthService::queryInterface(riid, ppvUnknown); 
  }

  return StatusCode::SUCCESS;
}

MuonCalibRunLumiBlockCoolSvc::~MuonCalibRunLumiBlockCoolSvc() {
}
	
StatusCode MuonCalibRunLumiBlockCoolSvc::initialize() {
  return StatusCode::SUCCESS;
}
	
StatusCode MuonCalibRunLumiBlockCoolSvc::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode MuonCalibRunLumiBlockCoolSvc::GetRunEventNumber(unsigned int timestamp,  int &run_number,  int &lb_nr) {
  if(run_number<0) {
    if( m_min_run_time<=timestamp && timestamp<=m_max_run_time ) {
      run_number=m_valid_for_run;
    } else {
      if(!get_run_number(timestamp, run_number).isSuccess()) {
	return StatusCode::FAILURE;
      }
    }
  }
  if( run_number!=m_valid_for_run ) {
    if(!load_lumiblock_map(run_number).isSuccess()) {
      return StatusCode::FAILURE;
    }
  }
  if( timestamp<m_min_run_time ) {
    ATH_MSG_WARNING("timestamp is below run range - return lb=0");
    time_t t = timestamp;
    ATH_MSG_WARNING("timestamp="<<ctime(&t));
    t=m_min_run_time;
    ATH_MSG_WARNING("min= "<<ctime(&t));
    lb_nr=0;
    return StatusCode::SUCCESS;
  }
  if(timestamp>m_max_run_time) {
    ATH_MSG_WARNING("timestamp is above run range - return lb=max");
    time_t t=timestamp;
    ATH_MSG_WARNING("timestamp="<<ctime(&t));
    t=m_max_run_time;
    ATH_MSG_WARNING("max= "<<ctime(&t));
    std::map<unsigned int, int>::const_iterator it=m_lumiblock_map.end();
    it--;
    lb_nr=it->second;
    return StatusCode::SUCCESS;
  }
  if(m_min_lb_time<=timestamp && timestamp<m_max_lb_time) {
    lb_nr=m_current_lumi_block;
    return StatusCode::SUCCESS;
  }
  std::map<unsigned int, int>::const_iterator it=m_lumiblock_map.lower_bound(timestamp);
  m_current_lumi_block=it->second;
  m_min_lb_time=it->first;
  it++;
  if(it==m_lumiblock_map.end()) {
    m_max_lb_time=UINT_MAX;
  } else {
    m_max_lb_time=it->first;
  }
  lb_nr=m_current_lumi_block;
  return StatusCode::SUCCESS;
}
	
inline StatusCode MuonCalibRunLumiBlockCoolSvc::get_run_number(const unsigned int & timestamp, int &run_number) {
  CoraCoolDatabaseSvc& corasvc=CoraCoolDatabaseSvcFactory::databaseService();
  CoraCoolDatabasePtr coradb;
  try {
    coradb=corasvc.openDatabase("COOLONL_TRIGGER/COMP200",true);
  }
  catch (std::exception&e) {
    ATH_MSG_FATAL("Exception when opening database: "<<e.what());
    return StatusCode::FAILURE;
  }
  cool::IDatabasePtr cooldb=coradb->coolDatabase();
  cool::IFolderPtr folder;
  try {
    folder=cooldb->getFolder(m_lumibloc_fldr_by_time);
  } 
  catch (cool::Exception& e) {
    ATH_MSG_FATAL("Exception when getting folder " << m_lumibloc_fldr_by_time << ": "<< e.what());
    return StatusCode::FAILURE;
  }
  try {
    cool::ValidityKey vk=timestamp;
    vk*=1e9;
    const cool::IObjectPtr obj(folder->findObject(vk, 0));
    const cool::IRecord & payload = obj->payload();
    run_number=payload["Run"].data<uint32_t>();
    ATH_MSG_INFO("Run number for "<<timestamp<<" is "<<run_number);
    return StatusCode::SUCCESS;
  }
  catch(std::exception&e) {
    ATH_MSG_FATAL("Failed to retrieve run number "<<e.what());
    return StatusCode::FAILURE;
  }
}
	
inline StatusCode MuonCalibRunLumiBlockCoolSvc :: load_lumiblock_map(const int &run_number) {
  m_lumiblock_map.clear();
  CoraCoolDatabaseSvc& corasvc=CoraCoolDatabaseSvcFactory::databaseService();
  CoraCoolDatabasePtr coradb;
  try {
    coradb=corasvc.openDatabase("COOLONL_TRIGGER/COMP200",true);
  }
  catch (std::exception&e) {
    ATH_MSG_FATAL("Exception when opening database: "<<e.what());
    return StatusCode::FAILURE;
  }
  cool::IDatabasePtr cooldb=coradb->coolDatabase();
  cool::IFolderPtr folder;
  try {
    folder=cooldb->getFolder(m_lumi_block_fldr_by_lumiblock);
  } 
  catch (cool::Exception& e) {
    ATH_MSG_FATAL("Exception when getting folder " << m_lumi_block_fldr_by_lumiblock << ": "<< e.what());
    return StatusCode::FAILURE;
  }
  cool::ValidityKey since(run_number);
  cool::ValidityKey until(since+1);
  since <<= 32; // upper 31 bits are run number
  until <<= 32;	
  cool::IObjectIteratorPtr objects(folder->browseObjects( since, until, 0) );
  if(objects->size()==0) {
    ATH_MSG_FATAL("No lumiblocks found for run "<<run_number);
    return StatusCode::FAILURE;
  }
  m_min_run_time=UINT_MAX;
  m_max_run_time=0;
  while ( objects->goToNext() ) {
    const cool::IObject& obj = objects->currentRef();
    const cool::IRecord & payload = obj.payload();
    uint64_t stime=payload["StartTime"].data<uint64_t>();
    unsigned int stime_sec=stime/1e9;
    cool::ValidityKey current_since=obj.since();
    m_lumiblock_map[stime_sec]=(current_since & 0xffffffff);
    ATH_MSG_DEBUG("LB start="<<stime_sec);
    time_t t=stime_sec;
    ATH_MSG_DEBUG("LB start="<<ctime(&t));
    ATH_MSG_DEBUG("LB="<<(current_since & 0xffffffff));
    uint64_t stime_max=payload["EndTime"].data<uint64_t>();
    unsigned int stime_max_sec=stime_max/1e9;
    if(m_min_run_time>stime_sec) {
      m_min_run_time=stime_sec;
    }
    if(m_max_run_time<stime_max_sec) {
      m_max_run_time=stime_max_sec;
    }
    t=stime_max_sec;
    ATH_MSG_DEBUG("LB end="<<stime_max_sec);
    ATH_MSG_DEBUG("LB end="<<ctime(&t));
  }
  //force lookup
  m_min_lb_time=UINT_MAX;
  m_max_lb_time=0;
  ATH_MSG_INFO("Loaded " << m_lumiblock_map.size() << " lumiblocks for run "<<run_number);
  time_t min=m_min_run_time, max=m_max_run_time;
  ATH_MSG_INFO("Run Range min: "<<ctime(&min));
  ATH_MSG_INFO("Run Range max: "<<ctime(&max));
  m_valid_for_run=run_number;
  return StatusCode::SUCCESS;
}
