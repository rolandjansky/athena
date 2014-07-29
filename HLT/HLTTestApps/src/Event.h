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
   * This class keeps both the event map and the original data that should be
   * deleted when the event is supposed to be deleted.
   */
  class Event: public hltinterface::DataCollector {
    
  public: //interface

    /**
     * Builds a new Event object from scratch
     *
     * @param event The event pointer that should be deleted when the event
     * is deleted.
     */
    Event (eformat::read::FullEventFragment& event);

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
     * @return data returned vector of ROBInfo objects
     * @param  lvl1_id The LVL1 identifier for the data of interest
     * @param  ids The identifiers of each ROB requested.
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
      * @return data returned vector of ROBInfo objects
      * @param  lvl1_id The LVL1 identifier for the data of interest
      */
     virtual uint32_t
     collect(std::vector<hltinterface::DCM_ROBInfo>& data,
              const uint32_t lvl1_id);
    
    /**
     * Registers itself as the "DataCollector" for the current thread
     * of execution.
     */
    inline void register_datacollector(void) {
      hltinterface::DataCollector::instance(this);
    }

  // Static stuff
  public:

    // method that is exposed to python to set the ROS2ROB map (fills in the
    // internal ROB2ROS map).
    // This throws OverflowError if the keys/values of the dictionary do not
    // fit into ROSID and ROBID respectively (see below). It throws TypeError
    // if this is not a dictionary of ROSIDs to lists of ROBIDs
    static void set_ros2rob_map(const boost::python::dict& d);

  // More static stuff
  private:
    typedef std::string ROSID; // must be unique - used for logging
    typedef uint32_t ROBID;    // as received by collect methods
    typedef std::map<ROBID, ROSID> ROB2ROSMap;

    // valid for all Events in the whole run
    // these should be set with set_ros2rob_map
    static ROB2ROSMap sm_rob2ros;

    // for debug purposes
    static void debug_print_rob_ros_map();

  // Representation
  private:
    struct ROB
    {
      ROB(ROBID id, const uint32_t * r) : id(id), rob{r} {}
      const ROBID id;
      const uint32_t * rob = nullptr;
      bool reserved = false;
    };

    std::map<uint32_t, ROB> m_map; ///< The ROB ID to ROB info map
    std::set<ROSID> m_hit_roses; ///< ROSes ROBs were retrieved from
    std::vector<eformat::ROBFragment<const uint32_t*> > m_l1r; ///< LVL1 result
    uint32_t m_lvl1_id; ///< My unique LVL1 identifier

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
