/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  static uint32_t getRodID (int crate,
						 int slink,
						 int daqOrRoi,
                         eformat::SubDetector subdet);

  /// ROD -> ROB
  static uint32_t getRobID (uint32_t rod_id);

  /// ROB -> ROS
  static uint32_t getRosID (uint32_t rob_id);

  /// ROS -> SubDetector
  static uint32_t getDetID (uint32_t ros_id);

  /// moduleID -> crate
  static int      crate(uint32_t code);

  // What is this ??
  /// moduleID -> daqOrRoi
  static int      daqOrRoi(uint32_t code);

  /// moduleID -> slink
  static int      slink(uint32_t code);

  /// Return the maximum possible number of slinks
  static int      maxSlinks();

  /// ID -> sub-detector
  static eformat::SubDetector subDet(uint32_t code);
};

#endif
