/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <string>
#include <sstream>

// Framework
#include "AthenaKernel/errorcheck.h"

// Trigger and TDAQ
#include "TrigConfHLTData/HLTUtils.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"
#include "eformat/SourceIdentifier.h"

// Local
#include "TrigCostMonitor/TrigNtRobsTool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtRobsTool::TrigNtRobsTool(const std::string &name,
             const std::string &type,
             const IInterface  *parent)
  :AthAlgTool(name, type, parent),
   m_config(0)
{
  declareInterface<Trig::ITrigNtTool>(this);
  declareInterface<Trig::TrigNtRobsTool>(this);

  declareProperty("keyROB",     m_keyROB     = "ROBDataMonitorCollection");
  declareProperty("printDebug", m_printDebug = false);
  declareProperty("cleanROBs",  m_cleanROBs  = false);
  declareProperty("keepSubDet", m_keepSubDet = true);
}

//---------------------------------------------------------------------------------------
Trig::TrigNtRobsTool::~TrigNtRobsTool()
{
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtRobsTool::initialize()
{    

  ATH_MSG_DEBUG("initialize()" );

  ATH_MSG_INFO("TrigNtRobsTool::initialize()..." );
  ATH_MSG_INFO("   cleanROBs  = " << m_cleanROBs );
  ATH_MSG_INFO("   keepSubDet = " << m_keepSubDet );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtRobsTool::finalize()
{
  //
  // Clean up
  //  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtRobsTool::Fill(TrigMonConfig *confg)
{
  //
  // Make local copy of new configuration
  //
  m_config = 0;

  if(!confg) {
    ATH_MSG_WARNING("Null TrigMonConfig pointer" );
    return false;
  }
  
  m_config = confg;
  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtRobsTool::Fill(TrigMonEvent &event)
{
  //
  // Process current event
  //
  ATH_MSG_VERBOSE("TrigNtRobsTool::Fill - start function ..." );

  if(!m_config) {
    ATH_MSG_WARNING("Invalid TrigMonConfig" );
    return false;
  }
  
  //
  // Access ROBDataMonitorStruct (2011/08/24 try empty key for backward compatibility)
  //
  std::string currROBKey;

  if(!evtStore() -> transientContains<ROBDataMonitorCollection>(currROBKey)) {
    ATH_MSG_DEBUG( "ROBDataMonitorCollection does not exist with key=" << currROBKey );

    if(!evtStore() -> transientContains<ROBDataMonitorCollection>(m_keyROB)) {
      ATH_MSG_DEBUG( "ROBDataMonitorCollection does not exist with key=" << m_keyROB );
      return false;
    } else {
      currROBKey = m_keyROB;
    }
  }

  const ROBDataMonitorCollection *robcol = 0;
  if(evtStore() -> retrieve<ROBDataMonitorCollection>(robcol, currROBKey).isFailure()) {
    ATH_MSG_WARNING( "Failed to find ROBDataMonitorCollection" );
    return false;
  }

  unsigned lvl1_id = 0;
  unsigned robDebug = 0;

  for(ROBDataMonitorCollection::const_iterator it = robcol->begin(); it != robcol->end(); ++it) {
    const robmonitor::ROBDataMonitorStruct *rob = *it;
    if(!rob) {
      ATH_MSG_DEBUG("Null ROBDataMonitorStruct pointer" );
      continue;
    }
    
    if(lvl1_id == 0) { 
      lvl1_id = rob->lvl1ID;
    } else if(lvl1_id != rob->lvl1ID) {
      ATH_MSG_DEBUG("Mismatch L1ID: " << lvl1_id << "!=" << rob->lvl1ID );
    }

    //
    // find hash id for algorithm name:
    // - first iterate over all configured sequences
    //
    bool found_id = 0;
    uint32_t alg_id = 0;

    if (m_algNameToIDMap.size() == 0) {
for(unsigned int i = 0; i < m_config->size<TrigConfSeq>(); ++i) {
      const TrigConfSeq &seq = m_config->at<TrigConfSeq>(i);
      const std::vector<TrigConfAlg> &avec = seq.getAlg();
      
      // iterate over algorithms
      for(unsigned int j = 0; j < avec.size(); ++j) {
        const TrigConfAlg &alg = avec[j];
        
          m_algNameToIDMap[alg.getName()] = alg.getNameId();
          
        }
      }
}
    auto algFinder = m_algNameToIDMap.find(rob->requestor_name);

    if (algFinder != m_algNameToIDMap.end() ){
      alg_id = algFinder->second;
      found_id = true;
    }
    
    

    if(!found_id) {
      alg_id = TrigConf::HLTUtils::string2hash(rob->requestor_name, "ALG");
      
      if(m_algIds.insert(alg_id).second) {
        if (rob->requestor_name == "<NONE>" || rob->requestor_name == "TrigSteer_HLT") {
          ATH_MSG_DEBUG("Algorithm not in a menu: " << rob->requestor_name << " use hash id=" << alg_id );
        } else {
          ATH_MSG_INFO("Algorithm not in a menu: " << rob->requestor_name << " use hash id=" << alg_id );
        }
      }
    }

    if(++robDebug < 10) {
      ATH_MSG_DEBUG("ROB requestor name/id: " << rob->requestor_name << "/" << alg_id );
      if (robDebug == 9) ATH_MSG_DEBUG("Additional ROBs not shown unless VERBOSE level..." );
    } else {
      ATH_MSG_VERBOSE("ROB requestor name/id: " << rob->requestor_name << "/" << alg_id );
    }


    // create ntuple entry for ROB monitoring data
    TrigMonROB myrob(alg_id, 
         rob->start_time_of_ROB_request.tv_sec,
         rob->start_time_of_ROB_request.tv_usec,
         rob->end_time_of_ROB_request.tv_sec,
         rob->end_time_of_ROB_request.tv_usec);
    
    if(m_printDebug) {
      ATH_MSG_DEBUG("ROBDataMonitorStruct: " << rob->requestor_name << " request for " << rob->requested_ROBs.size() << " ROB(s) took "
        << rob->elapsedTime() << " ?= " << myrob.getTimer() << " ms" );
    }

    // iterator over requested robs
    std::map<const uint32_t, robmonitor::ROBDataStruct>::const_iterator rob_it = rob->requested_ROBs.begin();
    for(; rob_it != rob->requested_ROBs.end(); ++rob_it) {
      const robmonitor::ROBDataStruct &data = rob_it->second;
      
      // create rob data class
      TrigMonROBData::History myhist = TrigMonROBData::kUNCLASSIFIED;
      
      if(data.rob_history==robmonitor::UNCLASSIFIED) myhist = TrigMonROBData::kUNCLASSIFIED;
      if(data.rob_history==robmonitor::RETRIEVED)    myhist = TrigMonROBData::kRETRIEVED;
      if(data.rob_history==robmonitor::CACHED)       myhist = TrigMonROBData::kCACHED;
      if(data.rob_history==robmonitor::IGNORED)      myhist = TrigMonROBData::kIGNORED;
      if(data.rob_history==robmonitor::DISABLED)     myhist = TrigMonROBData::kDISABLED;

      TrigMonROBData mydata(data.rob_id, data.rob_size);
      mydata.setHistory(myhist);

      if(data.isStatusOk()) mydata.setStatusOK();

      myrob.addData(mydata);

      if(m_printDebug) {
        ATH_MSG_DEBUG("ROBDataStruct:  id, size=" << data.rob_id       << ", " << data.rob_size       );
        ATH_MSG_DEBUG("TrigMonROBData: id, size=" << mydata.getROBId() << ", " << mydata.getROBSize() );
      }
    }   

    if(m_cleanROBs) { 
      CleanROB(myrob);
      ATH_MSG_DEBUG("Executed CleanROB()" );
    }
    
    event.add<TrigMonROB>(myrob);

    CheckROB(myrob, *rob);
  }

  ATH_MSG_VERBOSE("TrigNtRobsTool::Fill - finished." );

  return true;
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtRobsTool::CleanROB(TrigMonROB &data) const
{
  //
  // Fill TrigMonRobSum and clear robs
  //
  ATH_MSG_VERBOSE("TrigNtRobsTool::CleanROB() - start..." );
  ATH_MSG_VERBOSE("  number ROBs  before cleaning: " << data.getData().size() );
  ATH_MSG_VERBOSE("  number words before cleaning: " << data.getWord().size() );

  std::map<std::pair<TrigMonROBData::History, int>, TrigMonROBSum> smap;
  
  std::vector<TrigMonROBData> &data_vec = data.getData();
  std::vector<TrigMonROBData>::iterator rit = data_vec.begin();

  while(rit != data_vec.end()) {
    // Keep bad ROBs
    if(!rit->isStatusOk()) { 
      ++rit;
      continue;
    }
    
    const eformat::helper::SourceIdentifier robS(rit->getROBId());
    int subdet = -1;
    
    if(m_keepSubDet) { 
      subdet = robS.subdetector_id();
    }
    
    const std::pair<TrigMonROBData::History, int> keyP(rit->getHistory(), subdet);

    TrigMonROBSum &sum = smap[keyP];
    
    //
    // Reached maximum number of entries, add word and start new sum
    //
    if(!sum.addROB(*rit, subdet)) {
      data.addWord(sum.getWord());
      sum = TrigMonROBSum();
      sum.addROB(*rit);
    }

    rit = data_vec.erase(rit);
  }

  //
  // Record all collect summary objects
  //
  for(std::map<std::pair<TrigMonROBData::History, int>, TrigMonROBSum>::iterator it = smap.begin(); it != smap.end(); ++it) {
    const TrigMonROBSum &sum = it->second;
    data.addWord(sum.getWord());
  }
  
  ATH_MSG_VERBOSE("  number ROBs  after cleaning: " << data.getData().size() );
  ATH_MSG_VERBOSE("  number words after cleaning: " << data.getWord().size() );
  ATH_MSG_VERBOSE("TrigNtRobsTool::CleanROB() - done. " );
}

//---------------------------------------------------------------------------------------
namespace Tmp
{
  struct RobSum {
    RobSum() : nrob(0), size(0) {}
    unsigned nrob;
    unsigned size;

    std::vector<unsigned> subs;
    std::vector<unsigned> rids;
    
    std::string print() {
      std::stringstream str;
      str << "nrob=" << nrob << " size=" << size;

      for(unsigned i = 0; i < subs.size(); ++i) {
        str << " sd=" << std::hex << subs.at(i);
      }
      
      for(unsigned i = 0; i < rids.size(); ++i) {
        str << " id=" << std::hex << rids.at(i);
      }
      
      return str.str();
    }
  };
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtRobsTool::CheckROB(const TrigMonROB &data, 
                                    const robmonitor::ROBDataMonitorStruct &rob) const
{
  //
  // Check consistency between between two input structures
  //
  if(msgLvl(MSG::DEBUG) || !m_printDebug) return;
  
  ATH_MSG_DEBUG("TrigNtRobsTool::CheckROB() - start function: number of requested robs: " << rob.requested_ROBs.size() );

  //
  // Sum data from cost monitoring structure
  //
  std::map<TrigMonROBData::History, Tmp::RobSum> sum_cost;

  const std::vector<TrigMonROBData> &dvec = data.getData();
  const std::vector<TrigMonROBSum>   svec = data.getSum();

  for(std::vector<TrigMonROBData>::const_iterator it = dvec.begin(); it != dvec.end(); ++it) {
    Tmp::RobSum &sum = sum_cost[it->getHistory()];
    
    sum.nrob++;
    sum.size+= it->getROBSize();
    sum.rids.push_back(it->getROBId());
  }

  for(std::vector<TrigMonROBSum>::const_iterator it = svec.begin(); it != svec.end(); ++it) {
    Tmp::RobSum &sum = sum_cost[it->getHistory()];

    sum.nrob+= it->getNROB();
    sum.size+= it->getSize();
    sum.subs.push_back(it->getSubDet());
  }
 
  //
  // Sum data from rob monitoring
  //
  std::map<TrigMonROBData::History, Tmp::RobSum> sum_robs;

  std::map<const uint32_t, robmonitor::ROBDataStruct>::const_iterator it = rob.requested_ROBs.begin();
  for(; it != rob.requested_ROBs.end(); ++it) {
    const robmonitor::ROBDataStruct &robData = it->second;
      
    TrigMonROBData::History myhist = TrigMonROBData::kUNCLASSIFIED;
    
    if(robData.rob_history==robmonitor::UNCLASSIFIED) myhist = TrigMonROBData::kUNCLASSIFIED;
    if(robData.rob_history==robmonitor::RETRIEVED)    myhist = TrigMonROBData::kRETRIEVED;
    if(robData.rob_history==robmonitor::CACHED)       myhist = TrigMonROBData::kCACHED;
    if(robData.rob_history==robmonitor::IGNORED)      myhist = TrigMonROBData::kIGNORED;
    if(robData.rob_history==robmonitor::DISABLED)     myhist = TrigMonROBData::kDISABLED;
    
    Tmp::RobSum &sum = sum_robs[myhist];
    
    sum.nrob++;
    sum.size+= robData.rob_size;
    sum.rids.push_back(robData.rob_id);
  }

  const std::string pref = "CheckROB for " + rob.requestor_name + " ";

  for(std::map<TrigMonROBData::History, Tmp::RobSum>::iterator rit = sum_robs.begin(); rit != sum_robs.end(); ++rit) {
    //
    // 1) Check for same history entry
    //
    std::map<TrigMonROBData::History, Tmp::RobSum>::iterator cit = sum_cost.find(rit->first);
    if(cit == sum_cost.end()) {
      ATH_MSG_DEBUG(pref << "- error: missing history=: " << rit->first );
      continue;
    }

    //
    // 2) Check for rob size
    //
    if(cit->second.size != rit->second.size) {
      ATH_MSG_DEBUG(pref << "- error: ROB size mismatch: " << cit->second.size << "!=" << rit->second.size );
      continue;
    }
    
    if(m_cleanROBs && m_keepSubDet) { 
      ATH_MSG_DEBUG(pref << "2: good match: " );
      ATH_MSG_DEBUG("  cost: " << cit->second.print() );
      ATH_MSG_DEBUG("  robs: " << rit->second.print() );
      continue;
    }

    //
    // 3) Check for number of rob ids
    //
    if(cit->second.nrob != rit->second.nrob) {
      ATH_MSG_DEBUG(pref << "- error: ROB ids mismatch: " << cit->second.nrob << "!=" << rit->second.nrob );
      continue;
    }

    ATH_MSG_DEBUG(pref << "3: good match: " );
    ATH_MSG_DEBUG("  cost: " << cit->second.print() );
    ATH_MSG_DEBUG("  robs: " << rit->second.print() );
  }

  ATH_MSG_DEBUG("TrigNtRobsTool::CheckROB() - done." );
}
