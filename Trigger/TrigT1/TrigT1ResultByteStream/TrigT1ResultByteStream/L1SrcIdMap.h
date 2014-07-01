/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_L1SRCIDMAP_H
#define TRIGT1RESULTBYTESTREAM_L1SRCIDMAP_H

#include <stdint.h>

/** this class provides conversion between Lower level Source ID to
    higher level source ID for L1 ByteStream fragments.

    This is to be used in assembling the fragments from ROD fragments
	Hong Ma Oct 18, 2002

*/
class L1SrcIdMap {

public:

  /** The RoIB source ID:
      reserved / RoIB module type / subdet ID / module ID
      This already follows the new version 2.2 of ATLAS-DAQ-98-129.
  */

  /** constrcutor
  */
  L1SrcIdMap();

  uint32_t getRodID();

  /** Make a ROB Source ID from a ROD source ID
  */
  uint32_t getRobID( uint32_t rod_id );

  /** Make a ROS Source ID from a ROB source ID
  */
  uint32_t getRosID( uint32_t rob_id );

  /** Make a SubDetector ID from ROS source ID
  */
  uint32_t getDetID( uint32_t ros_id );

};

#endif // TRIGT1RESULTBYTESTREAM_L1SRCIDMAP_H
