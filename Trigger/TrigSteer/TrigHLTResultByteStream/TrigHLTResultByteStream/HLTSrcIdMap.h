/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: HLTResultByteStream
 * @Class  : HLTSrcIdMap
 *
 * @brief this class provides conversion between Lower level Source ID to
 *        higher level source ID for HLT ByteStream fragments.
 *
 *
 * File and Version Information:
 * $Id: HLTSrcIdMap.h,v 1.3 2007-07-05 13:40:31 eifert Exp $
 **********************************************************************************/

#ifndef HLTSrcIdMap_h
#define HLTSrcIdMap_h

#include <stdint.h>
#include "eformat/SourceIdentifier.h"

namespace HLT {

  /**
     @class HLTSrcIdMap
     @brief this class provides conversion between Lower level Source ID to
     higher level source ID for HLT ByteStream fragments.

     This is to be used in assembling the fragments from ROD fragments
     Hong Ma Oct 18, 2002
  */
  class HLTSrcIdMap {

  public:

    /** @brief constrcutor
     */
    HLTSrcIdMap() : m_detId(eformat::OTHER) {}

    /** @brief Make a ROB Source ID from a ROD source ID
     */
    uint32_t getRobID  ( uint32_t rod_id=0);

    /** @brief Make a ROS Source ID from a ROB source ID
     */
    uint32_t getRosID  ( uint32_t rob_id=0);

    /** @brief Make a SubDetector ID from ROS source ID
     */
    uint32_t getDetID  ( uint32_t ros_id=0);

    void setDetId(eformat::SubDetector detId) { m_detId = detId; } //!< setter for the detector ID

  private:

    eformat::SubDetector m_detId; //!<  sub detector ID

  };
}

#endif
