/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/CTPExtraWordsFormat.h"
#include "CTPfragment/CTPdataformat.h"
#include <algorithm>

// Declare some additional types
//template class std::map<const CTP::FolderIndex, CTP::FolderEntry>;
//template class std::pair<const CTP::FolderIndex, CTP::FolderEntry>;

namespace {
  std::map<const CTP::FolderIndex, CTP::FolderEntry>::iterator my_iter;  
  std::pair<const CTP::FolderIndex, CTP::FolderEntry> my_pair;
}

/* Type conversion. The reference returned by CTP::ExtraPayload::getFolderUpdates
   doesn't work as python iterator. */
namespace CTP {
  std::map<const FolderIndex, FolderEntry> getFolderUpdates(const CTP::ExtraPayload& x) {
    std::map<const FolderIndex, FolderEntry> m;
    const std::map<FolderIndex, FolderEntry>& f = x.getFolderUpdates();
    std::copy(f.begin(), f.end(), std::inserter(m,m.end()));
    return m;
  }
}
