/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <sstream>

// Local
#include "TrigCostExec/ExecBase.h"

using namespace std;

//-----------------------------------------------------------------------------------------
Anp::Exec::Count::Count() :
  alg_id(0),
  det_id(0),
  ros_id(0),
  robin_id(0),
  roi_id(0),
  chn_id(0),
  seq_id(0),
  select_det(false),
  select_ros(false),
  select_robin(false),
  select_roi(false),
  limit_slow(0),
  nevent(0),
  nroi(0),
  timer_alg(0),
  timer_rob(0),
  timer_evt(0),
  timer_evt_t(0),
  alg_ncalls(0),
  alg_ncache(0),
  alg_nslow(0),      
  chn_nslow(0),      
  seq_nslow(0),      
  rob_nreq(0),
  rob_nreq_cache(0),
  rob_nreq_retrv(0),
  rob_nreq_other(0),
  rob_size(0),
  rob_size_cache(0),
  rob_size_retrv(0),
  rob_size_other(0),
  rob_nids(0),
  rob_nids_cache(0),
  rob_nids_retrv(0),
  rob_nids_other(0),
  ros_nreq(0),
  ros_nreq_cache(0),
  ros_nreq_retrv(0),
  ros_n0ret(0),
  ros_nreq_other(0),
  robin_nreq(0),
  robin_nreq_cache(0),
  robin_nreq_retrv(0),
  robin_n0ret(0),
  robin_nreq_other(0)
{
}

//-----------------------------------------------------------------------------------------
void Anp::Exec::Count::Clear()
{
  nevent     = 0;
  nroi       = 0;
  timer_alg  = 0;
  timer_rob  = 0;
  timer_evt  = 0;
  timer_evt_t= 0;
  alg_ncalls = 0;
  alg_ncache = 0;
  alg_nslow  = 0;      
  chn_nslow  = 0;      
  seq_nslow  = 0;      
  rob_nreq   = 0;
  rob_nreq_cache   = 0;
  rob_nreq_retrv   = 0;
  rob_nreq_other   = 0;
  rob_size         = 0;
  rob_size_cache   = 0;
  rob_size_retrv   = 0;
  rob_size_other   = 0;
  rob_nids         = 0;
  rob_nids_cache   = 0;
  rob_nids_retrv   = 0;
  rob_nids_other   = 0;
  ros_nreq         = 0;
  ros_nreq_cache   = 0;
  ros_nreq_retrv   = 0;
  ros_n0ret        = 0;
  ros_nreq_other   = 0;
  robin_nreq       = 0;
  robin_nreq_cache = 0;
  robin_nreq_retrv = 0;
  robin_n0ret      = 0;
  robin_nreq_other = 0;
  ros_nrob        .clear();
  ros_nrob_retrv  .clear();
  robin_nrob      .clear();
  robin_nrob_retrv.clear();
  //  lbtime.clear();
}

//-----------------------------------------------------------------------------------------
void Anp::Exec::Count::AddCount(const AlgCall &alg)
{
  //
  // Count one algorithm call
  //
  const float t_alg = alg.alg_data.getTimer();

  timer_alg += t_alg;
  timer_evt += t_alg;
  
  if(alg.alg_data.isCalled()) { 
    ++alg_ncalls;
    alg_nslow += (t_alg > limit_slow);
  }
  else {
    ++alg_ncache;
  }

  for(unsigned i = 0; i < alg.rob_data.size(); ++i) {
    const Exec::ROBCall &rob = alg.rob_data.at(i);
    AddCount(rob);
  }
}

//-----------------------------------------------------------------------------------------
void Anp::Exec::Count::AddCount(const ROBCall &rob)
{
  //
  // Count one ROB request
  //
  unsigned _count       = 0;
  unsigned _count_retrv = 0;
  unsigned _count_other = 0;
  ros_all  .clear(); 
  ros_cache.clear(); 
  ros_retrv.clear(); 
  ros_other.clear(); 
  robin_all  .clear(); 
  robin_cache.clear(); 
  robin_retrv.clear(); 
  robin_other.clear(); 
  rob_all  .clear(); 
  rob_retrv.clear();
	  
  for(unsigned d = 0; d < rob.rob_data.size(); ++d) {
    const ROBData &data = rob.rob_data.at(d);
    //cout << "I_AM_A_ROB: " << d << " with ROS id: " << data.ros_id << " and size: " << data.rob_data.getROBSize() << endl;
    if(select_det   && det_id   != data.det_id)   continue;
    if(select_ros   && ros_id   != data.ros_id)   continue;
    if(select_robin && robin_id != data.robin_id) continue;
    
    ++_count;
    
    const TrigMonROBData::History rob_hist = data.rob_data.getHistory();
    const unsigned size = data.rob_data.getROBSize();
    
    if(rob_hist == TrigMonROBData::kRETRIEVED) {
      ++_count_retrv;
      ++rob_nids_retrv;
      rob_size_retrv += size;
      ros_retrv.insert(data.ros_id);
      robin_retrv.insert(data.robin_id);
      rob_retrv.insert(data.rob_data.getROBId());
    }
    else if(rob_hist == TrigMonROBData::kCACHED) {
      ++rob_nids_cache;
      rob_size_cache += size;
      ros_cache.insert(data.ros_id);
      robin_cache.insert(data.robin_id);
    }
    else {
      ++_count_other;
      ++rob_nids_other;
      rob_size_other += size;
      ros_other.insert(data.ros_id);
      robin_other.insert(data.robin_id);
    }
    
    ++rob_nids;
    rob_size += size;
    ros_all  .insert(data.ros_id);
    robin_all.insert(data.robin_id);
    rob_all  .insert(data.rob_data.getROBId());
  }
  
  //
  // Return if no matching ROBs found
  //
  if(!_count) return;
	  
  timer_rob += rob.rob_timer;
  
  ++rob_nreq;   
  rob_nreq_retrv += (_count_retrv > 0);
  rob_nreq_other += (_count_other > 0);
  rob_nreq_cache += (_count_retrv == 0 && _count_other == 0);
  
  //
  // Count ROSs
  //
  ros_nreq       += ros_all.size();
  ros_nreq_retrv += ros_retrv.size();
  ros_nreq_other += ros_other.size();

  robin_nreq       += robin_all.size();
  robin_nreq_retrv += robin_retrv.size();
  robin_nreq_other += robin_other.size();
  
  for(set<uint32_t>::const_iterator rit = ros_cache.begin(); rit != ros_cache.end(); ++rit) {
    const uint32_t ros_id = *rit;
    if(!ros_retrv.count(ros_id) && !ros_other.count(ros_id)) {
      ++ros_nreq_cache;
    }
  }
  if(select_ros && rob_retrv.size()>0) ros_nrob.push_back(rob_all.size());
  if(select_ros && rob_retrv.size()>0) ros_nrob_retrv.push_back(rob_retrv.size());

  for(set<uint32_t>::const_iterator rit = robin_cache.begin(); rit != robin_cache.end(); ++rit) {
    const uint32_t robin_id = *rit;
    if(!robin_retrv.count(robin_id) && !robin_other.count(robin_id)) {
      ++robin_nreq_cache;
    }
  }
  if(select_robin && rob_retrv.size()>0) robin_nrob.push_back(rob_all.size());
  if(select_robin && rob_retrv.size()>0) robin_nrob_retrv.push_back(rob_retrv.size());
}

//-----------------------------------------------------------------------------------------
void Anp::Exec::Count::AddCount(const Event &event)
{
  ++nevent;
  nroi += event.nRoi;

  //
  // If this is a chain specific counter, then check to see if chain time is within limits
  //
  if(chn_id>0 && limit_slow<timer_evt) ++chn_nslow;
  if(seq_id>0 && limit_slow<timer_evt) ++seq_nslow;
  timer_evt = 0.0;
  timer_evt_t += event.timer_steer;
  //
  // Add chn and alg time to the RoI maps
  //
  //if(get_roi_time && (chn_id>0 || alg_id>0)){ 
  //  lbtime[event.lb]+=timer_alg;
  //}
}

//-----------------------------------------------------------------------------------------
void Anp::Exec::Count::AddCount(const Count &rhs)
{
  //
  // Add another count
  //
  nevent     += rhs.nevent;
  nroi       += rhs.nroi;
  timer_alg  += rhs.timer_alg;
  timer_rob  += rhs.timer_rob;
  timer_evt  += rhs.timer_evt;
  timer_evt_t+= rhs.timer_evt_t;

  alg_ncalls += rhs.alg_ncalls;
  alg_ncache += rhs.alg_ncache;
  alg_nslow  += rhs.alg_nslow;

  chn_nslow  += rhs.chn_nslow;
  seq_nslow  += rhs.seq_nslow;

  rob_nreq       += rhs.rob_nreq;
  rob_nreq_cache += rhs.rob_nreq_cache;
  rob_nreq_retrv += rhs.rob_nreq_retrv;
  rob_nreq_other += rhs.rob_nreq_other;

  rob_size       += rhs.rob_size;
  rob_size_cache += rhs.rob_size_cache;
  rob_size_retrv += rhs.rob_size_retrv;
  rob_size_other += rhs.rob_size_other;

  rob_nids       += rhs.rob_nids;
  rob_nids_cache += rhs.rob_nids_cache;
  rob_nids_retrv += rhs.rob_nids_retrv;
  rob_nids_other += rhs.rob_nids_other;

  ros_nreq       += rhs.ros_nreq;
  ros_nreq_cache += rhs.ros_nreq_cache;
  ros_nreq_retrv += rhs.ros_nreq_retrv;
  ros_n0ret      += rhs.ros_n0ret;

  robin_nreq       += rhs.robin_nreq;
  robin_nreq_cache += rhs.robin_nreq_cache;
  robin_nreq_retrv += rhs.robin_nreq_retrv;
  robin_n0ret      += rhs.robin_n0ret;

}

//-----------------------------------------------------------------------------------------
double Anp::Exec::Count::GetMeanAlgTimePerCall() const
{
  if(alg_ncalls > 0) {
    return timer_alg/double(alg_ncalls);
  }
  
  return 0.0;
}

//-----------------------------------------------------------------------------------------
double Anp::Exec::Count::GetMeanCPUTimePerCall() const
{
  if(alg_ncalls > 0) {
    return (timer_alg-timer_rob)/double(alg_ncalls);
  }
  
  return 0.0;
}

//-----------------------------------------------------------------------------------------
double Anp::Exec::Count::GetMeanROBTimePerRetr() const
{
  if(rob_nreq_retrv > 0) {
    return timer_rob/double(rob_nreq_retrv);
  }
  
  return 0.0;
}

//-----------------------------------------------------------------------------------------
double Anp::Exec::Count::GetMeanStrTimePerEvent() const
{
  if(nevent > 0) {
    return timer_evt_t/double(nevent);
  }

  return 0.0;
}

//-----------------------------------------------------------------------------------------
double Anp::Exec::Count::GetMeanAlgTimePerEvent() const
{
  if(nevent > 0) {
    return timer_alg/double(nevent);
  }
  
  return 0.0;
}

//-----------------------------------------------------------------------------------------
double Anp::Exec::Count::GetMeanCPUTimePerEvent() const
{
  if(nevent > 0) {
    return (timer_alg-timer_rob)/double(nevent);
  }
  
  return 0.0;
}

//-----------------------------------------------------------------------------------------
double Anp::Exec::Count::GetMeanNumRoIsPerEvent() const
{
  if(nevent > 0) {
    return double(nroi)/double(nevent);
  }
  
  return 0.0;
}

//-----------------------------------------------------------------------------------------
double Anp::Exec::Count::GetMeanROBTimePerEvent() const
{
  if(nevent > 0) {
    return timer_rob/double(nevent);
  }
  
  return 0.0;
}

//---------------------------------------------------------------------------------------
double Anp::Exec::ConvertWordsToMB(unsigned size)
{
  return 4.0*size/(1024.0*1024.0);
}

//---------------------------------------------------------------------------------------
double Anp::Exec::ConvertWordsToKB(unsigned size)
{
  return 4.0*size/1024.0;
}

//---------------------------------------------------------------------------------------
double Anp::Exec::ConvertTimerToSec(const TrigMonTimer &timer)
{
  return timer.getSec()+timer.getMicroSec()*1.0e-6;

}

//---------------------------------------------------------------------------------------
std::string Anp::Exec::ConvertHistToString(const TrigMonROBData &rob)
{
  std::string hist = "";
  if( rob.isUnclassified()) return hist.append("Unclassified");
  if( rob.isIgnored()     ) return hist.append("Ignored");
  if( rob.isDisabled()    ) return hist.append("Disabled");
  if( rob.isCached()      ) return hist.append("Cached");
  if( rob.isRetrieved()   ) return hist.append("Retrieved");
  return hist;
}

//---------------------------------------------------------------------------------------
std::string Anp::Exec::ConvertSeqToStatus(const TrigMonSeq &seq)
{
  std::string hist = "";
  if( seq.isInitial())          hist="Initialized";
  if( seq.isExecuted()       )  hist="Executed";
  if( seq.isAlreadyExecuted())  hist="Cached";
  if( seq.isPrevious()       )  hist="Called as Input";
  if( seq.isPrevious() && seq.isAlreadyExecuted())  hist="Cached Input";
  return hist;
}

//-----------------------------------------------------------------------------------------
void Anp::Exec::Summary::ClearMenuConfig()
{
  chn_conf->clear();
  seq_conf->clear();
  alg_conf->clear();
}

//-----------------------------------------------------------------------------------------
std::string Anp::Exec::Summary::GetChnName(uint32_t chn_id) const
{
  if(!chn_conf) return "UNKNOWN_CHN";

  for(ChnConfMap::const_iterator it = chn_conf->begin(); it != chn_conf->end(); ++it) {
    const TrigConfChain &chn = it->second.ref();
    if(chn.getEncodedId() == chn_id) {
      return chn.getName();
    }
  }
  
  return "UNKNOWN_CHN";
}

//-----------------------------------------------------------------------------------------
std::string Anp::Exec::Summary::GetSeqName(uint32_t seq_id) const
{
  if(!seq_conf) return "UNKNOWN_SEQ";

  for(SeqConfMap::const_iterator it = seq_conf->begin(); it != seq_conf->end(); ++it) {
    const TrigConfSeq &seq = it->second.ref();
    if(seq.getIndex() == seq_id) {
      return seq.getName();
    }
  }
  
  return "UNKNOWN_SEQ";
}


//-----------------------------------------------------------------------------------------
std::string Anp::Exec::Summary::GetAlgType(uint32_t alg_id) const
{
  if(!alg_conf) return "UNKNOWN_ALG";

  for(AlgConfMap::const_iterator it = alg_conf->begin(); it != alg_conf->end(); ++it) {
    const TrigConfAlg &alg = it->second.ref();
    if(alg.getTypeId() == alg_id) {
      return alg.getType();
    }
  }
  
  return "UNKNOWN_ALG";
}

//-----------------------------------------------------------------------------------------
std::string Anp::Exec::Summary::GetAlgName(uint32_t alg_id) const
{
  if(!alg_conf) return "UNKNOWN_ALG";

  for(AlgConfMap::const_iterator it = alg_conf->begin(); it != alg_conf->end(); ++it) {
    const TrigConfAlg &alg = it->second.ref();
    if(alg.getNameId() == alg_id) {
      return alg.getName();
    }
  }

  if(alg_id == 1498476240) return "TrigSteer_L2";
  
  return "UNKNOWN_ALG";
}

//-----------------------------------------------------------------------------------------
std::string Anp::Exec::Summary::GetDetName(uint32_t det_id) const
{
  if(!rob_conf) return "UNKNOWN_DET";

  for(ROBConfMap::const_iterator it = rob_conf->begin(); it != rob_conf->end(); ++it) {
    const ROBConf &rob = it->second.ref();
    if(rob.det_id == det_id) {
      return rob.det_name;
    }
  }
  
  return "UNKNOWN_DET";
}

//-----------------------------------------------------------------------------------------
std::string Anp::Exec::Summary::GetROSName(uint32_t ros_id) const
{
  if(!rob_conf) return "UNKNOWN_ROS";

  for(ROBConfMap::const_iterator it = rob_conf->begin(); it != rob_conf->end(); ++it) {
    const ROBConf &rob = it->second.ref();
    if(rob.ros_id == ros_id) {
      return rob.ros_name;
    }
  }
  
  cout << "Summary::GetROSName - " << html_base << ": missing ROB for ROS id: " << ros_id << endl;

  cout << "-------------------------------------------------------------------------------" << endl;
  for(ROBConfMap::const_iterator it = rob_conf->begin(); it != rob_conf->end(); ++it) {
    const ROBConf &rob = it->second.ref();
    cout << "   " << rob.ros_name << ": " << rob.ros_id << endl;
  }
  cout << "-------------------------------------------------------------------------------" << endl;

  return "UNKNOWN_ROS";
}

//-----------------------------------------------------------------------------------------
std::string Anp::Exec::Summary::GetROBINName(uint32_t robin_id) const
{
  if(!rob_conf) return "UNKNOWN_ROBIN";

  for(ROBConfMap::const_iterator it = rob_conf->begin(); it != rob_conf->end(); ++it) {
    const ROBConf &rob = it->second.ref();
    if(rob.robin_id == robin_id) {
      return rob.robin_name;
    }
  }
  
  cout << "Summary::GetROBINName - " << html_base << ": missing ROBIN for ROBIN id: " << robin_id << endl;

  cout << "-------------------------------------------------------------------------------" << endl;
  for(ROBConfMap::const_iterator it = rob_conf->begin(); it != rob_conf->end(); ++it) {
    const ROBConf &rob = it->second.ref();
    cout << "   " << rob.robin_name << ": " << rob.robin_id << endl;
  }
  cout << "-------------------------------------------------------------------------------" << endl;

  return "UNKNOWN_ROBIN";
}

//-----------------------------------------------------------------------------------------
std::string Anp::Exec::Summary::GetNodeName(uint32_t node_id) const
{
  if(!farm_node) return "UNKNOWN_FARM_NODE";

  FarmNodeMap::const_iterator it = farm_node->find(node_id);
  if(it != farm_node->end()) return it->second;
  
  return "UNKNOWN_FARM_NODE";
}

//-----------------------------------------------------------------------------------------
Anp::Exec::AlgHist::AlgHist() :
  hAlgTime(0),
  hAlgTimeFrac(0),
  hCpuTime(0),
  hROBTime(0),
  hAlgTimeFirstCall(0),
  hAlgNCalls(0),
  hAlgNCache(0)
{
}

//-----------------------------------------------------------------------------------------
Anp::Exec::ROBHist::ROBHist() :
  hROBNreq(0),
  hROBNreqCache(0),
  hROBNreqRetrv(0),
  hROBSize(0),
  hROBSizeCache(0),
  hROBSizeRetrv(0),
  hROBNids(0),
  hROBNidsCache(0),
  hROBNidsRetrv(0)
{
}

//-----------------------------------------------------------------------------------------
Anp::Exec::ROSHist::ROSHist() :
  hROSNreq(0),
  hROSNreqCache(0),
  hROSNreqRetrv(0),
  hROSSize(0),
  hROSSizeCache(0),
  hROSSizeRetrv(0),
  hROSNrobids(0),
  hROSNrobidsCache(0),
  hROSNrobidsRetrv(0)
{
}

//-----------------------------------------------------------------------------------------
Anp::Exec::FarmHist::FarmHist() :
  hUseVsLBSteer(0),
  hUseVsLBAlg(0)
{
}

//-----------------------------------------------------------------------------------------
std::string Anp::ExecClient::GetLevelStr() const
{
  if     (fLevel == 0) return "AL";
  if     (fLevel == 2) return "L2";
  else if(fLevel == 3) return "EF";
  
  return "UN";
}

//---------------------------------------------------------------------------------------
bool Anp::SortExecCount::operator()(const Exec::Count &lhs, const Exec::Count &rhs) const
{
  //
  // Compare two objects based on key
  //
  if(fKey == "timer_alg") {
    return lhs.timer_alg > rhs.timer_alg;
  }
  else if(fKey == "timer_rob") {
    return lhs.timer_rob > rhs.timer_rob;
  }
  else if(fKey == "rob_nreq") {
    return lhs.rob_nreq > rhs.rob_nreq;
  }
  else if(fKey == "rob_nreq_retrv") {
    return lhs.rob_nreq_retrv > rhs.rob_nreq_retrv;
  }

  //
  // Compare by total time
  //
  return lhs.timer_alg > rhs.timer_alg;
}

//---------------------------------------------------------------------------------------
bool Anp::SortExecClient::operator()(const ExecClient *lhs, const ExecClient *rhs) const
{
  //
  // Compare two objects based on key
  //
  if(!lhs || !rhs) { 
    cerr << "SortExecClient::operator() - invalid ptr(s)" << endl;
    return lhs < rhs;
  }

  if(fSort.fKey == "name" || fSort.fKey.empty()) {
    //
    // Compare by name or type
    //
    return lhs->GetClientName() < rhs->GetClientName();
  }

  //
  // Compare by variables
  //
  return fSort(lhs->GetCount(), rhs->GetCount());
}

//---------------------------------------------------------------------------------------
bool Anp::SortAlgCall::operator()(const Anp::Exec::AlgCall &lhs, const Anp::Exec::AlgCall &rhs) const
{
  //
  // Compare two objects based on key
  //
  if(fKey == "timer_alg") {
    return lhs.alg_data.getTimer() > rhs.alg_data.getTimer();
  }

  //
  // Sort Chronologically
  //
  return lhs.alg_data.start() < rhs.alg_data.start();
}

//---------------------------------------------------------------------------------------
bool Anp::SortAlgCall::operator()(const Ptr<Exec::AlgCall> &lhs, const Ptr<Exec::AlgCall> &rhs) const
{
  //
  // Compare two objects based on key
  //
  if(!lhs.valid() || !rhs.valid()) {
    cout << "SortAlgCall::operator() - invalid AlgCall pointer(s)" << endl;
    return lhs.get() < rhs.get();
  }

  if(fKey == "timer_alg") {
    return lhs->alg_data.getTimer() > rhs->alg_data.getTimer();
  }

  //
  // Sort Chronologically
  //
  return lhs->alg_data.start() < rhs->alg_data.start();
}
