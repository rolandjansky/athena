//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file Event.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre Anjos</a> 
 * $Author: ricab $
 * $Revision: 89 $
 * $Date: 2013-07-05 15:44:02 +0200 (Fri, 05 Jul 2013) $
 *
 * @brief Describes a utility class to handle the event data in an Event-basis
 * way.
 */

#ifndef HLTTESTAPPS_EVENT_H
#define HLTTESTAPPS_EVENT_H 

#include <map>
#include <set>
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include "eformat/ROBFragment.h"
#include "eformat/FullEventFragment.h"
#include "hltinterface/DataCollector.h"
#include "hltinterface/DCM_ROBInfo.h"

// Fwd decl
namespace boost
{
  namespace python
  {
    class dict;
  }
}

namespace HLTTestApps {

  /**
   * The hltinterface::DataCollector implementation. This class keeps both the
   * event map and the original data that should be deleted when the event is
   * supposed to be deleted. It provides the HLT with a transparent emulation
   * of online data collection.
   */
  class Event: public hltinterface::DataCollector {
    
  public: //interface

    /**
     * C'tor.
     *
     * @param event The self contained input event
     */
    Event(const eformat::read::FullEventFragment& event);

    /**
     * D'tor
     */
    virtual ~Event() {}
    
    /**
     * Access the LVL1 identifier for this event
     */
    inline uint32_t lvl1_id (void) const { return m_lvl1_id; }
    
    /**
     * Accesses the LVL1 result
     */
    inline const std::vector<eformat::ROBFragment<const uint32_t*> >& 
    l1r (void) const { return m_l1r; }

    /**
     * Reserve a series of ROB's for possible future retrieval. This method allows to
     * tell the DataCollector what ROBs may be needed based on the geometrical RoI
     * dimensions. In this way ROB requests can be grouped and the ROS access rate can
     * be reduced. The method can be called several times before an actual ROB retreval
     * happens with the "collect" method.
     *
     * @param lvl1_id The LVL1 identifier for the data of interest
     * @param ids The identifiers of the ROBs to reserve.
     */
    virtual void reserveROBData(const uint32_t lvl1_id,
        const std::vector<uint32_t>& ids);

    /**
     * Collects a series of ROB's, given their identifiers. This method should
     * return in addition to the number of ROB's successfully collected also their
     * corresponding ROBInfo objects with additional information for cost monitoring
     *
     * @param data Output vector of ROBInfo objects
     * @param lvl1_id The LVL1 identifier for the data of interest
     * @param ids The identifiers of each ROB requested.
     * @return Number of output ROBInfo objects
     */
     virtual uint32_t
     collect(std::vector<hltinterface::DCM_ROBInfo>& data,
              const uint32_t lvl1_id,
              const std::vector<uint32_t>& ids);

     /**
      * Collect all remaining data for this event, AKA event building. This method should
      * return in addition to the number of ROB's successfully collected also their
      * corresponding ROBInfo objects with additional information for cost monitoring.
      *
      * @param data Output vector of ROBInfo objects
      * @param lvl1_id The LVL1 identifier for the data of interest
      * @return Number of output ROBInfo objects
      */
     virtual uint32_t
     collect(std::vector<hltinterface::DCM_ROBInfo>& data,
              const uint32_t lvl1_id);
    
    /**
     * Register this instance as the current DataCollector instance
     */
    inline void register_datacollector(void) {
      hltinterface::DataCollector::instance(this);
    }

  // Static stuff
  public:

    /**
     * Method that is exposed to python to set the ROS2ROB map (fills in the
     * internal ROB2ROS map).
     *
     * @param d A dictionary mapping ROSID to lists of ROBID's.
     * @throws Boost.Python.ArgumentError if d is not a dictionary.
     * @throws Boost.Python.TypeError if d does not map ROSID's to lists of
     * ROBID's.
     * @throws Boost.Python.OverflowError if d's keys/values do not fit into
     * ROSID and ROBID types respectively.
     */
    static void set_ros2rob_map(const boost::python::dict& d);

    /**
     * Method that is exposed to python to set the set of ROB IDs of the custom
     * ROBs that should also be included in the L1 Result
     *
     * @param l A list of ROBID's
     * @throws Boost.Python.ArgumentError if l is not a list
     * @throws Boost.Python.TypeError if any of l's elements is not ROBID's
     * @throws Boost.Python.OverflowError if any of l's elements does not fit
     * into the ROBID type.
     */
    static void set_l1r_robs(const boost::python::list& l);

    /**
     * Method that is exposed to python to set the ROB prefetching strategy used in
     * the DCM
     *
     * @param s A list with one element and with a value of either 0 or 1
     * @throws Boost.Python.ArgumentError if s is not a list
     * @throws Boost.Python.TypeError if any of s's elements of type integer
     * @throws Boost.Python.OverflowError if any of s's elements does not fit
     */
    static void set_dcm_strategy(const boost::python::list& s);
    /**
     * Method that is exposed to python to print the map of ROS hits
     *
     * @param number of events
    */
    static void debug_print_ros_hit_map(const int);

    /**
     * Method to accumulate statistics for the map of ROS hits
     *
     * @param number of stream tags
    */
    void accumulateStatistics(const int); 

  // More static stuff
  private:
    typedef std::string ROSID; // must be unique - used for logging
    typedef uint32_t ROBID;    // as received by collect methods
    typedef std::map<ROBID, ROSID> ROB2ROSMap;
    typedef std::map<ROSID, std::vector<float> > ROSHitMap;

    // valid for all Events in the whole run
    // this should be set with set_ros2rob_map
    static ROB2ROSMap sm_rob2ros;
    static ROSHitMap  sm_rosHitMap;
    static ROSHitMap  sm_rosHitMapReject;
    static ROSHitMap  sm_rosHitMapAccept;

    // valid for all Events in the whole run
    // this specifies a set of custom source IDs for the ROBs that should be
    // considered part of the L1 Result
    static std::set<ROBID> sm_l1r_robs;
   
    // Number of events used for ROS hit map
    static std::vector<int> sm_eventsForROSStat;

    // ROS prefetching strategy 
    //  0 = strategy as in Run 1,i.e. use of the prefetching list only when a ROB is needed 
    //  1 = strategy as at begin of Run 2, i.e. immediate retrieval of all ROBs on the prefetching list
    static int sm_strategy;

    // for debug purposes
    static void debug_print_rob_ros_map();
    static void debug_print_l1r_robs();

    // data output files
    static std::ofstream sm_ros_rejected_outfile;      ///< name of output file with ROS hits for rejected events
    static std::ofstream sm_ros_accepted_outfile;      ///< name of output file with ROS hits for accepted events

  // Representation
  private:
    struct ROB
    {
      ROB(ROBID id, const uint32_t * r) : id(id), rob{r} {}
      ROB(ROBID id, const uint32_t * r, const uint32_t s) : id(id), rob{r} , size_word(s) {}
      const ROBID id;
      const uint32_t * rob = nullptr;
      const uint32_t size_word = 0;
      bool reserved = false;
      bool prefetched = false;
    };

    std::map<uint32_t, ROB> m_map; ///< The ROB ID to ROB info map
    std::set<ROSID> m_hit_roses; ///< ROSes ROBs were retrieved from
    std::vector<eformat::ROBFragment<const uint32_t*> > m_l1r; ///< LVL1 result
    uint32_t m_lvl1_id; ///< My unique LVL1 identifier
    std::map<std::string, std::set<uint32_t> > m_Det_Robs_for_retrieval; ///< for ROB prefetching

    // Mark all ROSes with ROBs in the event as hit
    void hit_roses();
    // Mark all ROSes with specified ROBs as hit
    void hit_roses(const std::vector<ROBID>& robids);
    // get the ROSID this ROBID corresponds to. Default to a fake ROSID
    ROSID get_rosid(ROBID robid) const;
    // get a fake ROSID for this ROBID
    ROSID get_fake_rosid(ROBID robid) const;
    // check whether a ROB is cached
    bool is_cached(const ROB& rob) const;
    // check an L1 ID against the current event
    bool check_l1id(const uint32_t l1id) const;
    // for debug purposes
    void debug_print_hit_roses() const;
  };
}

#endif /* HLTTESTAPPS_EVENT_H */
