//  -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_TRIGGERTYPES_H
#define NSWL1_TRIGGERTYPES_H
#include <memory>
#include <stdint.h>

namespace NSWL1 {


/** @defgroup TriggerTypes
 *  Define a few types to be used in the NSWL1 trigger simulation
 *  @{
 */

/// bunch crossing tag
typedef uint16_t BcTag_t;
/// strip band id
typedef uint16_t BandId_t;

/** @} */ // end of group1

class PadData;
class PadTrigger;
class StripData;
class StripClusterData;
class StripOfflineData;
class PadOfflineData;

using spPadData= std::shared_ptr<PadData>;
using spPadOfflineData=std::shared_ptr<PadOfflineData>;
using spPadTrigger=std::shared_ptr<PadTrigger>;
using upPadTrigger=std::unique_ptr<PadTrigger>;
using upStripData=std::unique_ptr<StripData>;
using upStripClusterData=std::unique_ptr<StripClusterData>;
using upStripOfflineData=std::unique_ptr<StripOfflineData>;



} // NSWL1
#endif
