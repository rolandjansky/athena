/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   HLTExtraData.cxx
 * @brief  Definition of the HLT extra data in the HLTResult payload
 * @author Frank Winklmeier
 *
 * $Id: $
 */

#include "TrigSteeringEvent/HLTExtraData.h"

using namespace HLT;

HLTExtraData::HLTExtraData() :
  statusCode(0)
{
}

HLTExtraData::HLTExtraData(const std::vector<uint32_t>& storage) :
  statusCode(0)
{
  deserialize(storage);
}

HLTExtraData::HLTExtraData(const HLTExtraData& rhs)
  : appName{rhs.appName}
  , statusCode{rhs.statusCode}
  , anonymous{rhs.anonymous}
  , m_stringSerializer{} // not copied (default always good)
{}
  
HLTExtraData::HLTExtraData(HLTExtraData&& rhs)
  : HLTExtraData{}
{
  swap(*this, rhs);
}

HLTExtraData& HLTExtraData::operator=(HLTExtraData rhs)
{
  swap(*this, rhs);
  return *this;
}

void HLTExtraData::serialize(std::vector<uint32_t>& storage)
{
  if (empty()) return;
  
  // add the application name and the host name
  m_stringSerializer.serialize( appName, storage );
  
  // one word for update status
  storage.push_back( statusCode );

  // Anonymous words
  storage.insert(storage.end(), anonymous.begin(), anonymous.end());
}


void HLTExtraData::deserialize(const std::vector<uint32_t>& storage)
{
  if (storage.empty()) return;
  
  // StringSerializer knows how long the string is
  std::size_t offset = m_stringSerializer.deserialize(storage, appName);
  
  if (offset < storage.size()) {
    statusCode = storage[offset++];
    anonymous.clear();
    anonymous.insert(anonymous.begin(), storage.begin()+offset, storage.end());
  }
}

void HLT::swap(HLTExtraData& lhs, HLTExtraData& rhs)
{
  using std::swap;
  
  swap(lhs.appName, rhs.appName);
  swap(lhs.statusCode, rhs.statusCode);
  swap(lhs.anonymous, rhs.anonymous);
  // default m_stringSerializer is good for every object
}
