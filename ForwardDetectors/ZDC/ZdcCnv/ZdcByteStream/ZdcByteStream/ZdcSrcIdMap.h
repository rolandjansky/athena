/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCSRCIDMAP_H
#define ZDCSRCIDMAP_H

#include <stdint.h>

#include "eformat/SourceIdentifier.h"


/** This class provides conversion between Lower level Source ID to
 *  higher level source ID for L1Calo ByteStream fragments.
 *
 *  This is to be used in assembling the fragments from ROD fragments
 *
 *  @author Peter Faulkner
 *
 *  TRY_LINK_LVL1
 *  Marco Leite
 */

class ZdcSrcIdMap {

public:
  ZdcSrcIdMap();

  /// Make a ROD Source ID
  uint32_t getRodID (int crate,
						 int slink,
						 int daqOrRoi,
                         eformat::SubDetector subdet);

  /// ROD -> ROB
  uint32_t getRobID (uint32_t rod_id);

  /// ROB -> ROS
  uint32_t getRosID (uint32_t rob_id);

  /// ROS -> SubDetector
  uint32_t getDetID (uint32_t ros_id);

  /// moduleID -> crate
  int      crate(uint32_t code);

  // What is this ??
  /// moduleID -> daqOrRoi
  int      daqOrRoi(uint32_t code);

  /// moduleID -> slink
  int      slink(uint32_t code);

  /// Return the maximum possible number of slinks
  int      maxSlinks();

  /// ID -> sub-detector
  eformat::SubDetector subDet(uint32_t code);
};

#endif
