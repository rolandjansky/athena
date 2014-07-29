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
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include "issue.h"
#include "Event.h"
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

#else

#define DEBUG_WARN_NO_ROB(l1id, robid) {}
#define DEBUG_PRINT_ROB_ROS_MAP {}
#define DEBUG_OUT_ROBINFOS(data) {}
#define DEBUG_PRINT_HIT_ROSES {}

#endif

using namespace boost::python;
using namespace std::chrono;
using time_point = time_point<steady_clock>;
using std::vector;
using std::map;

namespace
{
  void debug_warn_no_rob(uint32_t l1id, uint32_t robid)
  {
    if(ers::debug_level() > 0)
    {
      boost::format msg("Event with LVL1 id=%lu does NOT contain ROB 0x%08x");
      ERS_DEBUG(1, (msg % l1id % robid).str());
    }
  }

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
}

// Static attributes
HLTTestApps::Event::ROB2ROSMap HLTTestApps::Event::sm_rob2ros{};

// Static member
void HLTTestApps::Event::set_ros2rob_map(const boost::python::dict& d)
{
  // get begin and end iterators to the python dictionary's keys
  stl_input_iterator<ROSID> itros{d}, endros{};

  // for each ROSID in the dictionary...
  std::for_each(itros, endros,
                [&d](ROSID rosid){
    // ... get begin and end iterators to the list of corresponding ROBIDs
    stl_input_iterator<ROBID> itrob1{extract<list>(d.get(rosid))()},
                              endrob{};

    // ... fill in the ROB2ROS map
    std::for_each(itrob1, endrob,
                  [rosid](ROBID robid){sm_rob2ros[robid] = rosid;});
  });

  DEBUG_PRINT_ROB_ROS_MAP;
}

HLTTestApps::Event::Event(eformat::read::FullEventFragment& event)
 : hltinterface::DataCollector()
{
  m_lvl1_id = event.lvl1_id();

  // Build a subdetector based table-of-contents of this event
  map<eformat::SubDetector, vector<const uint32_t*> > sd_toc;
  eformat::helper::build_toc(event, sd_toc);

  for(map<eformat::SubDetector, vector<const uint32_t*> >::iterator
      it = sd_toc.begin(); it != sd_toc.end(); ++it) {
    switch (it->first) {
      case eformat::TDAQ_BEAM_CRATE:
      case eformat::TDAQ_SFI:
      case eformat::TDAQ_SFO:
      case eformat::TDAQ_HLT:
      case eformat::OTHER:
        //we ignore these
        break;
      case eformat::TDAQ_L2SV:
      case eformat::TDAQ_CALO_PREPROC:
      case eformat::TDAQ_CALO_CLUSTER_PROC_DAQ:
      case eformat::TDAQ_CALO_CLUSTER_PROC_ROI:
      case eformat::TDAQ_CALO_JET_PROC_DAQ:
      case eformat::TDAQ_CALO_JET_PROC_ROI:
      case eformat::TDAQ_MUON_CTP_INTERFACE:
      case eformat::TDAQ_CTP:
        //these, we treat as Level-1 result
        m_l1r.insert(m_l1r.end(), it->second.begin(), it->second.end());
        break;
      default:
        //these, we include in our internal data map for fast access
        for(size_t k=0; k<it->second.size(); ++k) {
          eformat::ROBFragment<const uint32_t*> rob(it->second[k]);
          auto robid = rob.source_id();
          auto mit = m_map.find(robid);
          //if I already have an entry there...
          if (mit != m_map.end()) {
            boost::format msg("Found duplicate ROB id's (0x%08x) while "
                "configuring event with LVL1 id=%lu. I will ignore the "
                "current ROB fragment.");
            msg % robid % m_lvl1_id;
            ers::warning(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
          }
          else
            m_map.insert(std::make_pair(robid, ROB{robid, it->second[k]}));
            // emplace still not available in gcc4.7.2
            // m_map.emplace(robid, ROB{robid, it->second[k]});
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
    for(const auto& id : ids)
    {
      auto it = m_map.find(id);
      if(it != m_map.end())
      {
        auto& rob = it->second;
        data.emplace_back(rob.rob, is_cached(rob), t0, steady_clock::now());
        rob.reserved = true; // it was already retrieved, so it should
                             // be marked as cached next time around
      }
      else
        DEBUG_WARN_NO_ROB(lvl1_id, id);
    }

    // couldn't use the previous loop for caching. Otherwise, would be marking
    // as cached, robs that could still be added to the output, in a subsequent
    // iteration
    hit_roses(ids);

    ERS_DEBUG(1, "Request with LVL1 id " << lvl1_id << " had requests for "
                 << ids.size() << " ROBs and got " << data.size()
                 << " fragments.");
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
    for(auto& elem : m_map)
    {
      auto& rob = elem.second;
      data.emplace_back(rob.rob, is_cached(rob), t0, steady_clock::now());
      rob.reserved = true; // it was already retrieved, so it should
                           // be marked as cached next time around
    }

    hit_roses();

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
    for(auto id : ids)
    {
      auto it = m_map.find(id);
      if(it != m_map.end())
        it->second.reserved = true;
      else
        DEBUG_WARN_NO_ROB(lvl1_id, id);
    }
  }
}

void HLTTestApps::Event::hit_roses()
{
  std::for_each(begin(m_map), end(m_map),
                [this](std::pair<uint32_t, ROB> elem){
                  m_hit_roses.insert(get_rosid(elem.first));
                });

  DEBUG_PRINT_HIT_ROSES;
}

void HLTTestApps::Event::hit_roses(const std::vector<ROBID>& robids)
{
  std::for_each(begin(robids), end(robids),
                [this](uint32_t id){m_hit_roses.insert(get_rosid(id));});

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
  std::ostringstream oss;
  boost::format rmsg("0x%04x");
  auto sid = eformat::helper::SourceIdentifier(robid);
  oss << "FAKE_ROS_" << sid.human_detector() << "_"
      << (rmsg % sid.subdetector_id()).str();
  return oss.str();
}

bool HLTTestApps::Event::is_cached(const ROB& rob) const
{
  return (rob.reserved && m_hit_roses.count(get_rosid(rob.id)));
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

void HLTTestApps::Event::debug_print_hit_roses() const
{
  if(ers::debug_level() > 1)
  {
    bool fst = true;
    std::ostringstream oss;
    for(const auto& ros : m_hit_roses)
    {
      if(fst)
        fst = false;
      else
        oss << ", ";
      oss << ros;
    }

    ERS_DEBUG(2, "Hit ROSes: [" << oss.str() << "]");
  }
}
