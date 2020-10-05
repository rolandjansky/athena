/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "L1CaloSrcIdMap.h"

namespace LVL1BS {

L1CaloSrcIdMap::L1CaloSrcIdMap()
{
}

// Make a ROD Source ID

uint32_t L1CaloSrcIdMap::getRodID(int crate, int slink, int daqOrRoi,
                                             eformat::SubDetector subdet) const
{
  // module ID = r0sscccc (ROD-spec-version1_06d, P33)
  uint16_t moduleId = (daqOrRoi << 7) | (slink << 4) | crate;
  eformat::helper::SourceIdentifier helpID(subdet, moduleId);
  return helpID.code();
}

// Make a ROB Source ID from a ROD source ID

uint32_t L1CaloSrcIdMap::getRobID(uint32_t rod_id) const
{
  return rod_id;
}

// Make a ROS Source ID from a ROB source ID

uint32_t L1CaloSrcIdMap::getRosID(uint32_t rob_id) const
{
  eformat::helper::SourceIdentifier id(rob_id);
  eformat::helper::SourceIdentifier id2(id.subdetector_id(), 0);
  return id2.code();
}

// Make a SubDetector ID from ROS source ID

uint32_t L1CaloSrcIdMap::getDetID(uint32_t ros_id) const
{
  eformat::helper::SourceIdentifier id(ros_id);
  eformat::helper::SourceIdentifier id2(id.subdetector_id(), 0);
  return id2.code();
}

// Return crate from unpacked moduleID

int L1CaloSrcIdMap::crate(uint32_t code) const
{
  eformat::helper::SourceIdentifier id(code);
  return id.module_id() & 0xf;
}

// Return daqOrRoi from unpacked moduleID

int L1CaloSrcIdMap::daqOrRoi(uint32_t code) const
{
  eformat::helper::SourceIdentifier id(code);
  return (id.module_id() >> 7) & 0x1;
}

// Return slink from unpacked moduleID

int L1CaloSrcIdMap::slink(uint32_t code) const
{
  eformat::helper::SourceIdentifier id(code);
  return (id.module_id() >> 4) & 0x3;
}

// Return the maximum possible number of slinks given number of
// bits in module ID

int L1CaloSrcIdMap::maxSlinks() const
{
  return 4;
}

// Return sub-detector for given ID

eformat::SubDetector L1CaloSrcIdMap::subDet(uint32_t code) const {
  eformat::helper::SourceIdentifier id(code);
  return id.subdetector_id();
}

} // end namespace
