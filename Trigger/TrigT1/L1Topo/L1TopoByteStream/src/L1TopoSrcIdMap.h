/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOBYTESTREAM_L1TOPOSRCIDMAP_H
#define L1TOPOBYTESTREAM_L1TOPOSRCIDMAP_H

#include <cstdint>

/** This class provides conversion between Lower level Source ID to
    higher level source ID for L1Topo ByteStream fragments.
    This is to be used in assembling the fragments from ROD fragments
    Based on CTPSrcIdMap and L1CaloSrcIdMap.

    * @author Simon George
*/
class L1TopoSrcIdMap {

public:
  /** constructor
  */
  L1TopoSrcIdMap();

  /** get a ROD Source ID (deprecated)
  */
  uint32_t getRodID () const;

  /** get a ROD Source ID given a module ID, i.e. combine with the subdet ID
  */
  uint32_t getRodID ( int moduleId ) const;

  /** Make a ROB Source ID from a ROD source ID; note they are now the same.
  */
  uint32_t getRobID (uint32_t rod_id) const;

  /** Make a ROS Source ID from a ROB source ID
  */
  uint32_t getRosID (uint32_t rob_id) const;

  /** Make a SubDetector ID from ROS source ID
  */
  uint32_t getDetID (uint32_t ros_id) const;

};

#endif // L1TOPOBYTESTREAM_L1TOPOSRCIDMAP_H
