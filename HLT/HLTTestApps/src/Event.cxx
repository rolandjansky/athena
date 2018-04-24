//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file Event.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: ricab $
 * $Revision: 64 $
 * $Date: 2013-06-24 20:16:17 +0200 (Mon, 24 Jun 2013) $
 *
 * Implements all the Event functionality
 */

#include <chrono>
#include <sstream>
#include <algorithm>
#include <array>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include "issue.h"
#include "Event.h"
#include "L1_ROBs.h"
#include "eformat/eformat.h"
#include "eformat/index.h"
#include "eformat/SourceIdentifier.h"

#ifndef NDEBUG

#define DEBUG_WARN_NO_ROB(l1id, robid) \
  do { debug_warn_no_rob(l1id, robid); } while(false)
#define DEBUG_PRINT_ROB_ROS_MAP \
  do { debug_print_rob_ros_map(); } while(false)
#define DEBUG_OUT_ROBINFOS(data) \
  do { debug_out_robinfos(data); } while(false)
#define DEBUG_PRINT_HIT_ROSES \
  do { debug_print_hit_roses(); } while(false)
#define DEBUG_PRINT_L1R_ROBS \
  do { debug_print_l1r_robs(); } while(false)

#else

#define DEBUG_WARN_NO_ROB(l1id, robid) {}
#define DEBUG_PRINT_ROB_ROS_MAP {}
#define DEBUG_OUT_ROBINFOS(data) {}
#define DEBUG_PRINT_HIT_ROSES {}
#define DEBUG_PRINT_L1R_ROBS {}

#endif

using namespace boost::python;
using namespace std::chrono;
using time_point = time_point<steady_clock>;
using std::vector;
using std::string;
using std::map;
using std::array;
using std::find;

namespace
{

#pragma GCC diagnostic ignored "-Wunused-function"
  void debug_warn_no_rob(uint32_t l1id, uint32_t robid)
  {
    if(ers::debug_level() > 0)
    {
      boost::format msg("Event with LVL1 id=%lu does NOT contain ROB 0x%08x");
      ERS_DEBUG(1, (msg % l1id % robid).str());
    }
  }

#pragma GCC diagnostic ignored "-Wunused-function"
  void debug_out_robinfos(const std::vector<hltinterface::DCM_ROBInfo>& data)
  {
    if(ers::debug_level() >= 2)
    {
      for(const auto& rob : data)
      {
        boost::format rmsg("0x%08x");

        auto millis0 = duration_cast<milliseconds>(
            rob.robRequestTime.time_since_epoch()).count();
        auto millis1 = duration_cast<milliseconds>(
            rob.robDeliveryTime.time_since_epoch()).count();
        auto deltamillis = duration_cast<milliseconds>(
            rob.robDeliveryTime - rob.robRequestTime).count();

        ERS_DEBUG(2, "Output ROBInfo:\n[ROB ID: "
                     << (rmsg % rob.robFragment.rob_source_id()).str()
                     << ", cached: " << rob.robIsCached
                     << ", robRequestTime: " << millis0
                     << " milliseconds since epoch, robDeliveryTime: "
                     << millis1 << " milliseconds since epoch (elapsed time = "
                     << deltamillis << "ms)]");
      }
    }
  }

  template <typename LIST>
  string to_string_list(const LIST& l)
  {
    std::ostringstream oss;
    oss << std::hex << "[";
    if(!l.empty())
    {
      oss << "0x";
      std::copy(begin(l), --end(l),
          std::ostream_iterator<typename LIST::value_type>(oss, ", 0x"));
      oss << *--end(l); // add last one without comma
    }
    oss << std::dec << ']';
    return oss.str();
  }
}

// Static attributes
HLTTestApps::Event::ROB2ROSMap HLTTestApps::Event::sm_rob2ros{};
HLTTestApps::Event::ROSHitMap  HLTTestApps::Event::sm_rosHitMap{};
HLTTestApps::Event::ROSHitMap  HLTTestApps::Event::sm_rosHitMapReject{};
HLTTestApps::Event::ROSHitMap  HLTTestApps::Event::sm_rosHitMapAccept{};
std::set<HLTTestApps::Event::ROBID> HLTTestApps::Event::sm_l1r_robs{};
std::vector<int> HLTTestApps::Event::sm_eventsForROSStat{};
int HLTTestApps::Event::sm_strategy{};

enum RosHitMapIndex {
  // total number of ROBs in ROS
  rh_number_of_robs=0,
  // for all events: average number of hits/evt., average fraction of ROBs retrieved/evt.
  rh_all_hit_fra=1,
  rh_all_rob_fra=2,
  // normal collect (no evt. building): average number of hits/evt., average fraction of ROBs retrieved/evt.
  rh_normal_hit_fra=3,
  rh_normal_rob_fra=4,
  // collect for evt. building: average number of hits/evt., average fraction of ROBs retrieved/evt.
  rh_evbld_hit_fra=5,
  rh_evbld_rob_fra=6,
  // total size of rosHitMap per ROS
  rh_total_size=7
};

enum RosHitMapIndexPerEvent {
  // total number of ROBs in ROS
  rh_number_ev_of_robs=0,
  // for all events: average number of hits/evt., average fraction of ROBs retrieved/evt.
  rh_all_ev_hit_fra=1,    // for single event 
  rh_all_ev_rob_fra=2,    // for single event
  // normal collect (no evt. building): average number of hits/evt., average fraction of ROBs retrieved/evt.
  rh_normal_ev_hit_fra=3, // for single event
  rh_normal_ev_rob_fra=4, // for single event
  // collect for evt. building: average number of hits/evt., average fraction of ROBs retrieved/evt.
  rh_evbld_ev_hit_fra=5, // for single event
  rh_evbld_ev_rob_fra=6, // for single event
  // total size of rosHitMap per ROS per event
  rh_total_ev_size=7
};

enum EventCounterIndex {
  // total number of rejected events
  rh_count_reject=0,
  // total number of accepted events
  rh_count_accept=1,
  // total number of accepted events
  rh_count_size=2
};

// Static method
void HLTTestApps::Event::set_ros2rob_map(const boost::python::dict& d)
{
  sm_rob2ros.clear();
  sm_rosHitMap.clear();
  sm_rosHitMapReject.clear();
  sm_rosHitMapAccept.clear();
  sm_eventsForROSStat.clear();

  // initialize event counter
  sm_eventsForROSStat.assign(rh_count_size,0);

  // get begin and end iterators to the python dictionary's keys
  stl_input_iterator<ROSID> itros{d}, endros{};

  // for each ROSID in the dictionary...
  std::for_each(itros, endros,
                [&d](ROSID rosid){

    // ... initialize ROS hit map
    sm_rosHitMap[rosid]       = std::vector<float>(rh_total_ev_size,0.0);
    sm_rosHitMapReject[rosid] = std::vector<float>(rh_total_size,0.0);
    sm_rosHitMapAccept[rosid] = std::vector<float>(rh_total_size,0.0);

    // ... get begin and end iterators to the list of corresponding ROBIDs
    stl_input_iterator<ROBID> itrob1{extract<list>(d.get(rosid))()},
                              endrob{};

    // ... fill in the ROB2ROS map and get the number of ROBs in this ROS 
    std::for_each(itrob1, endrob,
                  [rosid](ROBID robid){
		    sm_rob2ros[robid] = rosid;
		    (sm_rosHitMap[rosid])[rh_number_ev_of_robs]    += 1.;
		    (sm_rosHitMapReject[rosid])[rh_number_of_robs] += 1.;
		    (sm_rosHitMapAccept[rosid])[rh_number_of_robs] += 1.;
		  });
  });

  DEBUG_PRINT_ROB_ROS_MAP;
}

// Static method
void HLTTestApps::Event::set_l1r_robs(const boost::python::list& l)
{
  // get begin and end iterators to the python list
  stl_input_iterator<ROBID> itros{l}, endros{};

  sm_l1r_robs.clear();
  std::copy(itros, endros, std::inserter(sm_l1r_robs, begin(sm_l1r_robs)));

  DEBUG_PRINT_L1R_ROBS;
}

// Static method
void HLTTestApps::Event::set_dcm_strategy(const boost::python::list& s)
{
  // get begin and end iterators to the python list
  stl_input_iterator<int> itros{s}, endstrategy{};

  // ROS prefetching strategy 
  //  0 = strategy as in Run 1,i.e. use of the prefetching list only when a ROB is needed 
  //  1 = strategy as at begin of Run 2, i.e. immediate retrieval of all ROBs on the prefetching list
  sm_strategy = *itros;
  ERS_DEBUG(1, " DCM prefetching strategy set to " << sm_strategy);
}

HLTTestApps::Event::Event(const eformat::read::FullEventFragment& event)
 : hltinterface::DataCollector()
{
  m_lvl1_id = event.lvl1_id();

  // Reset the ROB prefetching map
  m_Det_Robs_for_retrieval.clear(); 
  m_l1r.clear();
  m_map.clear();

  // Build a source_id based table-of-contents of this event
  map<uint32_t, const uint32_t*> sid_toc;
  eformat::helper::build_toc(event, sid_toc);

  for(const auto& pair : sid_toc) {
    auto sid = eformat::helper::SourceIdentifier(pair.first);
    auto sdid = sid.subdetector_id();
    switch (sdid) {
      case eformat::TDAQ_BEAM_CRATE:
      case eformat::TDAQ_SFI:
      case eformat::TDAQ_SFO:
      case eformat::TDAQ_LVL2:
      case eformat::TDAQ_HLT:
      case eformat::OTHER:
        //we ignore these
        break;

      default:
        auto robid = sid.simple_code();
        if(find(begin(L1R_SDS), end(L1R_SDS), sdid) != end(L1R_SDS) ||
           find(begin(L1R_ROBS), end(L1R_ROBS), robid) != end(L1R_ROBS) ||
           find(begin(sm_l1r_robs), end(sm_l1r_robs), robid) !=end(sm_l1r_robs))
        {
          // these, we treat as Level-1 result robs
          m_l1r.emplace_back(pair.second);
        }
        else
        {
          //these, we include in our internal data map for fast access
          if(m_map.find(robid) != end(m_map))
          {
            boost::format msg("Found duplicate ROB id's (0x%08x) while "
                              "configuring event with LVL1 id=%lu. I will "
                              "ignore the current ROB fragment.");
            msg % robid % m_lvl1_id;
            ers::warning(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
          } 
	  else if ((!sm_rob2ros.empty()) && (sm_rob2ros.find(robid)==sm_rob2ros.end())) 
          {
            boost::format msg("Found ROB id (0x%08x) while "
                              "configuring event with LVL1 id=%lu, which is not in the provided ROS to ROB mapping. "
                              "This ROB fragment will be ignored.");
            msg % robid % m_lvl1_id;
            ers::warning(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
	  }
          else
          {
            m_map.insert(std::make_pair(robid, ROB{robid, pair.second}));
            // std::map::emplace still not available in gcc4.7.2
            // m_map.emplace(robid, ROB{robid, it->second[k]});
          }
        }
        break;
    }
  }
}

uint32_t HLTTestApps::Event::
collect(std::vector<hltinterface::DCM_ROBInfo>& data,
         const uint32_t lvl1_id,
         const std::vector<uint32_t>& ids)
{
  auto t0 = steady_clock::now();
  if(check_l1id(lvl1_id))
  {
    std::vector<uint32_t> ids_for_ros_collection;
    ids_for_ros_collection.reserve(ids.size());

    for(const auto& id : ids)
    {
      auto it = m_map.find(id);
      if(it != m_map.end())
      {
        auto& rob = it->second;
        data.emplace_back(rob.rob, is_cached(rob), t0, steady_clock::now());
	if (!is_cached(rob)) ids_for_ros_collection.push_back(id);
        rob.reserved = true; // it was already retrieved, so it should
                             // be marked as cached next time around
      }
      else
        DEBUG_WARN_NO_ROB(lvl1_id, id);
    }

    // couldn't use the previous loop for caching. Otherwise, would be marking
    // as cached, robs that could still be added to the output, in a subsequent
    // iteration
    hit_roses(ids_for_ros_collection);

    int n_additional_robs(0);
    if (sm_strategy == 0) {
      for(auto ros : m_hit_roses) {
	// retrieve also all ROBs in the hit ROS into the cache which are
	// on the prefetch list and mark them as retrieved
	auto it_prefetch_ros = m_Det_Robs_for_retrieval.find(ros);
	if (it_prefetch_ros != m_Det_Robs_for_retrieval.end()) {
	  // mark all prefetch ROBs on the list for this ROS as retrieved
	  for (const auto& it_prefetch_rob_id : it_prefetch_ros->second) {
	    auto rob_prefetched = m_map.find(it_prefetch_rob_id);
	    if(rob_prefetched != m_map.end()) { 
	      (rob_prefetched->second).reserved = true ;
	      n_additional_robs++;
	    } 
	  }
	  // reset the list of prefetch ROBs for this ROS
	  (it_prefetch_ros->second).clear();
	}
      }
    }
    ERS_DEBUG(2, "Request with LVL1 id " << lvl1_id << " had requests for "
                 << ids.size() << " ROBs and got " << data.size()
                 << " fragments.");
    ERS_DEBUG(2, "Request with LVL1 id " << lvl1_id << " had "
	         << n_additional_robs << " additional ROBs retrieved from " << m_hit_roses.size()
	         << " ROS(es) for prefetching strategy = " << sm_strategy);
    DEBUG_OUT_ROBINFOS(data);
    return data.size();
  }

  return 0;
}

uint32_t HLTTestApps::Event::
collect(std::vector<hltinterface::DCM_ROBInfo>& data,
         const uint32_t lvl1_id)
{
  auto t0 = steady_clock::now();
  if(check_l1id(lvl1_id))
  {
    // find out what ROSes are still needed for retrieval
    // (do this before all elements are set to retrieved)
    hit_roses();

    for(auto& elem : m_map)
    {
      auto& rob = elem.second;
      data.emplace_back(rob.rob, is_cached(rob), t0, steady_clock::now());
      rob.reserved = true; // it was already retrieved, so it should
                           // be marked as cached next time around
    }

    DEBUG_OUT_ROBINFOS(data);
    return data.size();
  }

  return 0;
}

void HLTTestApps::Event::reserveROBData(const uint32_t lvl1_id,
                                        const vector<uint32_t>& ids)
{
  if(check_l1id(lvl1_id))
  {
    // for old strategy (sm_strategy=1) the ROBs are immediately retrieved
    std::vector<uint32_t> ids_for_ros_collection;
    ids_for_ros_collection.reserve(ids.size());

    for(auto id : ids)
    {
      auto it = m_map.find(id);
      if(it != m_map.end())
	if (sm_strategy == 0) {
	  m_Det_Robs_for_retrieval[get_rosid(id)].insert(id);
	  it->second.prefetched = true;
	} else {
	  // old strategy: immediately retrieve ROBs on prefetching list
	  auto& rob = it->second;
	  if (!is_cached(rob)) ids_for_ros_collection.push_back(id);
	  it->second.reserved = true;
	}
      else
        DEBUG_WARN_NO_ROB(lvl1_id, id);
    }

    // increase ROS hits for old strategy
    if (sm_strategy == 1) {
      hit_roses(ids_for_ros_collection);
    }
  }
}

void HLTTestApps::Event::hit_roses()
{
  m_hit_roses.clear();
  std::for_each(begin(m_map), end(m_map),
                [this](std::pair<uint32_t, ROB> elem){
		  if (!is_cached(elem.second)) {
		    m_hit_roses.insert(get_rosid(elem.first));
		    if (!sm_rob2ros.empty()) {
		      (sm_rosHitMap[get_rosid(elem.first)])[rh_all_ev_rob_fra] += 1.;
		      (sm_rosHitMap[get_rosid(elem.first)])[rh_evbld_ev_rob_fra] += 1.;
		    }
		  }
                });

  // increase the ROS hit counter
  if (!sm_rob2ros.empty()) {
    for(const auto& ros : m_hit_roses) {
      (sm_rosHitMap[ros])[rh_all_ev_hit_fra] += 1.;
      (sm_rosHitMap[ros])[rh_evbld_ev_hit_fra] += 1.;
    }
  }

  DEBUG_PRINT_HIT_ROSES;
}

void HLTTestApps::Event::hit_roses(const std::vector<ROBID>& robids)
{
  m_hit_roses.clear();
  std::for_each(begin(robids), end(robids),
                [this](uint32_t id){
		  m_hit_roses.insert(get_rosid(id));
		  if (!sm_rob2ros.empty()) {
		    (sm_rosHitMap[get_rosid(id)])[rh_all_ev_rob_fra] += 1.;
		    (sm_rosHitMap[get_rosid(id)])[rh_normal_ev_rob_fra] += 1.;
		  }
		});

  // increase the ROS hit counter
  if (!sm_rob2ros.empty()) {
    for(const auto& ros : m_hit_roses) {
      (sm_rosHitMap[ros])[rh_all_ev_hit_fra] += 1.;
      (sm_rosHitMap[ros])[rh_normal_ev_hit_fra] += 1.;
    }
  }

  DEBUG_PRINT_HIT_ROSES;
}

auto HLTTestApps::Event::get_rosid(ROBID robid) const -> ROSID
{
  auto it = sm_rob2ros.find(robid);
  if(it != sm_rob2ros.end())
    return it->second;

  // by default, assume each subdetector corresponds to a ROS and work with that
  return get_fake_rosid(robid);
}

auto HLTTestApps::Event::get_fake_rosid(ROBID robid) const -> ROSID
{
  if(!sm_rob2ros.empty())
  {
    boost::format msg("The ROB with ID 0x%08X is being collected, but it is "
                      "not part of the ROS2ROB mapping. You probably provided "
                      "a ROS2ROB mapping that is not in agreement with your "
                      "data (not all ROBs are covered by the map). This could "
                      "create problems online. You can compile in dbg mode and "
                      "run with a high debug output level to get detailed "
                      "printouts of the internal mapping and caching "
                      "simulation (hit ROSes).");
    msg % robid;
    ers::warning(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }

  std::ostringstream oss;
  boost::format rmsg("0x%04x");
  auto sid = eformat::helper::SourceIdentifier(robid);
  oss << "FAKE_ROS_" << sid.human_detector() << "_"
      << (rmsg % sid.subdetector_id()).str();
  return oss.str();
}

bool HLTTestApps::Event::is_cached(const ROB& rob) const
{
  return (rob.reserved);
}

bool HLTTestApps::Event::check_l1id(const uint32_t lvl1_id) const
{
  if (m_lvl1_id != lvl1_id) {
    boost::format msg("You are not processing event %lu, but %lu. "
                      "Ignoring request.");
    msg % lvl1_id % m_lvl1_id;
    ers::warning(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
    return false;
  }

  return true;
}

// static
void HLTTestApps::Event::debug_print_rob_ros_map()
{
  if(ers::debug_level() > 0)
  {
    std::ostringstream oss;
    if(sm_rob2ros.empty())
      oss << "{}";
    else
    {
      oss << "\n{" << std::hex;
      for(const auto& robitem : sm_rob2ros)
        oss << "\n\tROB 0x" << robitem.first << ": " << robitem.second;
      oss << "\n}";
    }

    ERS_DEBUG(1, "Internal ROB2ROS map: " << oss.str());
  }
}

// static
void HLTTestApps::Event::debug_print_l1r_robs()
{
  if(ers::debug_level() > 0)
  {
    ERS_DEBUG(1, "Custom L1R rob ids: " << to_string_list(sm_l1r_robs));
  }
}

void HLTTestApps::Event::debug_print_hit_roses() const
{
  if(ers::debug_level() > 0)
  {
    ERS_DEBUG(2, "Hit ROSes: " << to_string_list(m_hit_roses));
  }
}

void HLTTestApps::Event::debug_print_ros_hit_map(const int nevent)
{
  if (sm_rob2ros.empty()) {
    ERS_DEBUG(1," No ROS-ROB mapping available");
    return;
  }
  std::ostringstream oss;
  float hit_fraction, rob_fraction ;
  float hit_fraction_normal, rob_fraction_normal ;
  float hit_fraction_evbuild, rob_fraction_evbuild ;

  oss << "\n\tprint_ros_hit_map: DCM prefetching strategy                      = " << sm_strategy ;
  oss << "\n\tprint_ros_hit_map: Total number of events processed              = " << nevent ;

  oss << "\n\tprint_ros_hit_map: +-----------------+ " ;
  oss << "\n\tprint_ros_hit_map: | Rejected Events | " ;
  oss << "\n\tprint_ros_hit_map: +-----------------+ " ;
  oss << "\n\tprint_ros_hit_map: Number of rejected events used for statistics = " << sm_eventsForROSStat[rh_count_reject] <<"\n" ;
  if (sm_eventsForROSStat[rh_count_reject] > 0) {
    oss << "\n" << std::setw(27) << std::left << std::setfill(' ') << " " 
	<< std::setw(24) << std::left << std::setfill(' ') << " | total"
	<< std::setw(24) << std::left << std::setfill(' ') << " | no evt. bld."
	<< std::setw(24) << std::left << std::setfill(' ') << " | evt. bld." 
	<< " | ";

    oss << "\n" << std::setw(18) << std::left << std::setfill(' ') << "ROS" 
	<< std::setw(6)  << std::left << std::setfill(' ') << " | # ROBs" 
	<< std::setw(12) << std::left << std::setfill(' ') 
	<< " | Hits/Evt. " << ", ROBs/Evt."
	<< " | Hits/Evt. " << ", ROBs/Evt."
	<< " | Hits/Evt. " << ", ROBs/Evt."
	<< " | ";


    for(const auto& ros : sm_rosHitMapReject) {
      hit_fraction = (ros.second)[rh_all_hit_fra]/float(sm_eventsForROSStat[rh_count_reject]);
      rob_fraction = (ros.second)[rh_all_rob_fra]/float(sm_eventsForROSStat[rh_count_reject]);

      hit_fraction_normal = (ros.second)[rh_normal_hit_fra]/float(sm_eventsForROSStat[rh_count_reject]);
      rob_fraction_normal = (ros.second)[rh_normal_rob_fra]/float(sm_eventsForROSStat[rh_count_reject]);

      hit_fraction_evbuild = (ros.second)[rh_evbld_hit_fra]/float(sm_eventsForROSStat[rh_count_reject]);
      rob_fraction_evbuild = (ros.second)[rh_evbld_rob_fra]/float(sm_eventsForROSStat[rh_count_reject]);

      oss << "\n" << std::setw(18) << std::left  << std::setfill(' ') << ros.first 
	  << " | " << std::setw(6) << std::setfill(' ') << std::fixed << std::setprecision(0) << std::right << (ros.second)[rh_number_of_robs]
	  << " | " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << hit_fraction << " , "
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << rob_fraction << " | " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << hit_fraction_normal << " , " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << rob_fraction_normal << " | " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << hit_fraction_evbuild << " , " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << rob_fraction_evbuild << " | " ;
    }
  }

  oss << "\n\tprint_ros_hit_map: +-----------------+ " ;
  oss << "\n\tprint_ros_hit_map: | Accepted Events | " ;
  oss << "\n\tprint_ros_hit_map: +-----------------+ " ;
  oss << "\n\tprint_ros_hit_map: Number of accepted events used for statistics = " << sm_eventsForROSStat[rh_count_accept] <<"\n" ;
  if (sm_eventsForROSStat[rh_count_accept] > 0) {
    // Accepted events
    oss << "\n" << std::setw(27) << std::left << std::setfill(' ') << " " 
	<< std::setw(24) << std::left << std::setfill(' ') << " | total"
	<< std::setw(24) << std::left << std::setfill(' ') << " | no evt. bld."
	<< std::setw(24) << std::left << std::setfill(' ') << " | evt. bld." 
	<< " | ";

    oss << "\n" << std::setw(18) << std::left << std::setfill(' ') << "ROS" 
	<< std::setw(6)  << std::left << std::setfill(' ') << " | # ROBs" 
	<< std::setw(12) << std::left << std::setfill(' ') 
	<< " | Hits/Evt. " << ", ROBs/Evt."
	<< " | Hits/Evt. " << ", ROBs/Evt."
	<< " | Hits/Evt. " << ", ROBs/Evt."
	<< " | ";

    for(const auto& ros : sm_rosHitMapAccept) {
      hit_fraction = (ros.second)[rh_all_hit_fra]/float(sm_eventsForROSStat[rh_count_accept]);
      rob_fraction = (ros.second)[rh_all_rob_fra]/float(sm_eventsForROSStat[rh_count_accept]);

      hit_fraction_normal = (ros.second)[rh_normal_hit_fra]/float(sm_eventsForROSStat[rh_count_accept]);
      rob_fraction_normal = (ros.second)[rh_normal_rob_fra]/float(sm_eventsForROSStat[rh_count_accept]);

      hit_fraction_evbuild = (ros.second)[rh_evbld_hit_fra]/float(sm_eventsForROSStat[rh_count_accept]);
      rob_fraction_evbuild = (ros.second)[rh_evbld_rob_fra]/float(sm_eventsForROSStat[rh_count_accept]);

      oss << "\n" << std::setw(18) << std::left  << std::setfill(' ') << ros.first 
	  << " | " << std::setw(6) << std::setfill(' ') << std::fixed << std::setprecision(0) << std::right << (ros.second)[rh_number_of_robs]
	  << " | " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << hit_fraction << " , "
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << rob_fraction << " | " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << hit_fraction_normal << " , " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << rob_fraction_normal << " | " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << hit_fraction_evbuild << " , " 
	  << std::setw(9) << std::left  << std::setfill(' ') << std::setprecision(4) << rob_fraction_evbuild << " | " ;
    }
  } else {
    oss << " print_ros_hit_map: number of input events - 0 ";
  }
  ERS_DEBUG(1,oss.str());
}

void HLTTestApps::Event::accumulateStatistics(const int numberOfStreamTags)
{
  // No ROS-ROB mapping available
  if (sm_rob2ros.empty()) {
    return;
  }

  // Increase event conter
  if (numberOfStreamTags == 0) {
    // rejected events
    sm_eventsForROSStat[rh_count_reject]++;
  } else {
    // accpeted events
    sm_eventsForROSStat[rh_count_accept]++;
  }

  // Accumulate ROB/ROS statistics for accepted/rejected events
  for (auto& ros : sm_rosHitMap) {
    if (numberOfStreamTags == 0) {
      // total
      sm_rosHitMapReject[ros.first][rh_all_hit_fra] += (ros.second)[rh_all_ev_hit_fra];
      if ((ros.second)[rh_all_ev_hit_fra] != 0.) 
	sm_rosHitMapReject[ros.first][rh_all_rob_fra] += ( (ros.second)[rh_all_ev_rob_fra]/(ros.second)[rh_all_ev_hit_fra]/(ros.second)[rh_number_of_robs] );
      // normal collect
      sm_rosHitMapReject[ros.first][rh_normal_hit_fra] += (ros.second)[rh_normal_ev_hit_fra];
      if ((ros.second)[rh_normal_ev_hit_fra] != 0.) 
	sm_rosHitMapReject[ros.first][rh_normal_rob_fra] += ( (ros.second)[rh_normal_ev_rob_fra]/(ros.second)[rh_normal_ev_hit_fra]/(ros.second)[rh_number_of_robs] );
      // eventbuilding collect
      sm_rosHitMapReject[ros.first][rh_evbld_hit_fra] += (ros.second)[rh_evbld_ev_hit_fra];
      if ((ros.second)[rh_evbld_ev_hit_fra] != 0.) 
	sm_rosHitMapReject[ros.first][rh_evbld_rob_fra] += ( (ros.second)[rh_evbld_ev_rob_fra]/(ros.second)[rh_evbld_ev_hit_fra]/(ros.second)[rh_number_of_robs] );
    } else {
      // total
      sm_rosHitMapAccept[ros.first][rh_all_hit_fra] += (ros.second)[rh_all_ev_hit_fra];
      if ((ros.second)[rh_all_ev_hit_fra] != 0.) 
	sm_rosHitMapAccept[ros.first][rh_all_rob_fra] += ( (ros.second)[rh_all_ev_rob_fra]/(ros.second)[rh_all_ev_hit_fra]/(ros.second)[rh_number_of_robs] );
      // normal collect
      sm_rosHitMapAccept[ros.first][rh_normal_hit_fra] += (ros.second)[rh_normal_ev_hit_fra];
      if ((ros.second)[rh_normal_ev_hit_fra] != 0.) 
	sm_rosHitMapAccept[ros.first][rh_normal_rob_fra] += ( (ros.second)[rh_normal_ev_rob_fra]/(ros.second)[rh_normal_ev_hit_fra]/(ros.second)[rh_number_of_robs] );
      // eventbuilding collect
      sm_rosHitMapAccept[ros.first][rh_evbld_hit_fra] += (ros.second)[rh_evbld_ev_hit_fra];
      if ((ros.second)[rh_evbld_ev_hit_fra] != 0.) 
	sm_rosHitMapAccept[ros.first][rh_evbld_rob_fra] += ( (ros.second)[rh_evbld_ev_rob_fra]/(ros.second)[rh_evbld_ev_hit_fra]/(ros.second)[rh_number_of_robs] );
    }
    // total
    (ros.second)[rh_all_ev_hit_fra] = 0.; 
    (ros.second)[rh_all_ev_rob_fra] = 0.; 
    // normal collect
    (ros.second)[rh_normal_ev_hit_fra] = 0.; 
    (ros.second)[rh_normal_ev_rob_fra] = 0.; 
    // eventbuilding collect
    (ros.second)[rh_evbld_ev_hit_fra] = 0.; 
    (ros.second)[rh_evbld_ev_rob_fra] = 0.; 
  }
}
