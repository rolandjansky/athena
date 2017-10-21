//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file HLTTestApps/src/Event.cxx
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

    oss << '[';
    if(!l.empty())
    {
      std::copy(begin(l), --end(l),
          std::ostream_iterator<typename LIST::value_type>(oss, ", "));
      oss << *--end(l); // add last one without comma
    }
    oss << ']';

    return oss.str();
  }
}

// Static attributes
HLTTestApps::Event::ROB2ROSMap HLTTestApps::Event::sm_rob2ros{};
std::set<HLTTestApps::Event::ROBID> HLTTestApps::Event::sm_l1r_robs{};

// Static method
void HLTTestApps::Event::set_ros2rob_map(const boost::python::dict& d)
{
  sm_rob2ros.clear();

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

// Static method
void HLTTestApps::Event::set_l1r_robs(const boost::python::list& l)
{
  // get begin and end iterators to the python list
  stl_input_iterator<ROBID> itros{l}, endros{};

  sm_l1r_robs.clear();
  std::copy(itros, endros, std::inserter(sm_l1r_robs, begin(sm_l1r_robs)));

  DEBUG_PRINT_L1R_ROBS;
}

HLTTestApps::Event::Event(const eformat::read::FullEventFragment& event)
 : hltinterface::DataCollector()
{
  m_lvl1_id = event.lvl1_id();

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
  if(ers::debug_level() > 1)
  {
    ERS_DEBUG(2, "Hit ROSes: " << to_string_list(m_hit_roses));
  }
}
