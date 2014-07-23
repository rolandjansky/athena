/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   CTPExtraWordsFormat.cxx
 * @brief  Definition of the CTP extra words payload format
 * @author Frank Winklmeier
 *
 * $Id:$
 */

#include "TrigConfL1Data/CTPExtraWordsFormat.h"

using namespace CTP;

CTP::ExtraPayload::ExtraPayload(const std::vector<uint32_t>& payload,
                                std::size_t maxPayloadWords) :
  m_l1psk(0),
  m_maxPayload(maxPayloadWords)
{
  if (payload.size()>m_maxPayload) {
    throw ExtraPayloadTooLong();
  }

  if (payload.size()>0) m_l1psk = payload[0];
  
  if (payload.size()>1) {
    uint32_t nFolders = payload[1];
    for (size_t i=0; i<nFolders; ++i) {    
      FolderEntry e(payload[i+2]);
      m_entries[e.folderIndex] = e;
    }
  }
}


/*
 * Return the length of the payload
 * !!! This needs to be updated whenever you change the payload format !!!
 */
std::size_t CTP::ExtraPayload::size() const
{
  return 2+m_entries.size();
}

std::vector<uint32_t> CTP::ExtraPayload::serialize() const
{
  std::vector<uint32_t> v(size());
  serialize(v.begin());
  return v;
}

void CTP::ExtraPayload::serialize(std::vector<uint32_t>::iterator first) const
{
  *first++ = m_l1psk;
  *first++ = m_entries.size();
  std::map<FolderIndex, FolderEntry>::const_iterator iter = m_entries.begin();
  for (; iter!=m_entries.end(); ++iter) {
    *first++ = iter->second.serialize();
  }
}

/*
 * Add a folder update (overriding the previous update of the same folder)
 */
void CTP::ExtraPayload::updateFolder(FolderEntry& e)
{
  if (size()==m_maxPayload && m_entries.find(e.folderIndex)==m_entries.end()) {
    throw ExtraPayloadTooLong();
  }
  m_entries[e.folderIndex] = e;
}


std::ostream& operator <<(std::ostream& os, const CTP::ExtraPayload& p) {
  os << "RAW = [";
  std::vector<uint32_t> v = p.serialize();
  for (std::size_t i=0; i<v.size(); ++i) {
    os << v[i] << " ";
  }
  os << "]";
  os << " L1PSK = " << p.getL1PSK();
  os << ", nFolders = " << p.getFolderUpdates().size();

  if (p.getFolderUpdates().size()>0) {
    os << ", [LB,Idx] =";
    std::map<CTP::FolderIndex, CTP::FolderEntry>::const_iterator iter = p.getFolderUpdates().begin();
    for (; iter!=p.getFolderUpdates().end(); ++iter) {
      os << " [" << iter->second.lumiBlock << "," << iter->second.folderIndex << "]";
    }
  }
  return os;
}

