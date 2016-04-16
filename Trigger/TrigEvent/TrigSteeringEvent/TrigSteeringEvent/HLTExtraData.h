/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#if !defined(TRIGSTEERINGEVENT_HLTEXTRADATA_H)  && !defined(XAOD_STANDALONE)
#define TRIGSTEERINGEVENT_HLTEXTRADATA_H

/**
 * @file   HLTExtraData.h
 * @brief  Definition of the HLT extra data in the HLTResult payload
 * @author Frank Winklmeier
 *
 * $Id: $
 */

#include "TrigSteeringEvent/StringSerializer.h"
#include <string>
#include <vector>

namespace HLT {
  
  /**
   * Class representing the HLT extra payload stored in HLT::HLTResult::getExtras()
   *
   * The members storing the data are public. New members should be added whenever
   * additional storage is needed. The reserved #anonymous member should only be
   * used as a last resort.
   *
   * If all members of this class are set to their default values (see empty())
   * a call to serialize() will do nothing. A deserialization from empty storage
   * will result in a default constructed object.
   *
   * Whenever you modify this class run 'make check'.
   */
  class HLTExtraData {
  public:
    friend void swap(HLTExtraData&, HLTExtraData&);
    /**
     * Construct empty object
     */
    HLTExtraData();

    /**
     * Construct from storage and fill members
     */
    HLTExtraData(const std::vector<uint32_t>& storage);
    
    HLTExtraData(const HLTExtraData&); //!< copy ctor
    HLTExtraData(HLTExtraData&&); //!< move ctor
    HLTExtraData& operator=(HLTExtraData); //!< unified assignement op
  
    /**
     * Serialize the data and append it to the vector
     */
    void serialize(std::vector<uint32_t>& storage);

    /**
     * Deserialize storage into members
     */
    void deserialize(const std::vector<uint32_t>& storage);


    std::string           appName;    //!< application name
    uint32_t              statusCode; //!< various status codes (for prescale/COOL updates)
    std::vector<uint32_t> anonymous;  //!< For future use-cases (only use as last resort)
  
  private:

    /// Return true if all members have default values (no serialization needed)
    inline bool empty() {
      return (appName.empty() && statusCode==0 && anonymous.empty());
    }
  
    TrigSteeringEvent::StringSerializer m_stringSerializer;  
  };

  void swap(HLTExtraData&, HLTExtraData&);

} // namespace HLT

#endif
