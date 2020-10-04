/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_L1CALOSRCIDMAP_H
#define TRIGT1CALOBYTESTREAM_L1CALOSRCIDMAP_H

#include <stdint.h>

#include "eformat/SourceIdentifier.h"

namespace LVL1BS {

/** This class provides conversion between Lower level Source ID to
 *  higher level source ID for L1Calo ByteStream fragments.
 *
 *  This is to be used in assembling the fragments from ROD fragments
 *
 *  @author Peter Faulkner
 */

class L1CaloSrcIdMap {

public:
  L1CaloSrcIdMap();

  /// Make a ROD Source ID
  uint32_t getRodID (int crate, int slink, int daqOrRoi,
                                eformat::SubDetector subdet) const;

  /// Make a ROB Source ID from a ROD source ID
  uint32_t getRobID (uint32_t rod_id) const;

  /// Make a ROS Source ID from a ROB source ID
  uint32_t getRosID (uint32_t rob_id) const;

  /// Make a SubDetector ID from ROS source ID
  uint32_t getDetID (uint32_t ros_id) const;

  /// Return crate from unpacked moduleID
  int      crate(uint32_t code) const;

  /// Return daqOrRoi from unpacked moduleID
  int      daqOrRoi(uint32_t code) const;

  /// Return slink from unpacked moduleID
  int      slink(uint32_t code) const;

  /// Return the maximum possible number of slinks
  int      maxSlinks() const;

  /// Return sub-detector for given ID
  eformat::SubDetector subDet(uint32_t code) const;

  /// Return ROD header minor version to use when writing BS
  uint16_t minorVersion() const {return 0x1004;}                     // Or may go up to 0x2000, CHECK

  /// Return last ROD header minor version for pre-LS1 data
  uint16_t minorVersionPreLS1() const {return 0x1003;}

};

} // end namespace

#endif
