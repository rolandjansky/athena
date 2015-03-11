/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CTPfragment/CTPExtraWordsFormat.h"
#include "CTPfragment/CTPdataformat.h"
#include <algorithm>

// Declare some additional types
//template class std::map<const CTPfragment::FolderIndex, CTPfragment::FolderEntry>;
//template class std::pair<const CTPfragment::FolderIndex, CTPfragment::FolderEntry>;

namespace {
  std::map<const CTPfragment::FolderIndex, CTPfragment::FolderEntry>::iterator my_iter;  
  std::pair<const CTPfragment::FolderIndex, CTPfragment::FolderEntry> my_pair;
}

// Hack for ROOT6 (see ATR-10297)
namespace CTPdataformat {
  struct ROOT6_NamespaceAutoloadHook{};
}

/* Type conversion. The reference returned by CTPfragment::ExtraPayload::getFolderUpdates
   doesn't work as python iterator. */
namespace CTPfragment {
  std::map<const FolderIndex, FolderEntry> getFolderUpdates(const CTPfragment::ExtraPayload& x) {
    std::map<const FolderIndex, FolderEntry> m;
    const std::map<FolderIndex, FolderEntry>& f = x.getFolderUpdates();
    std::copy(f.begin(), f.end(), std::inserter(m,m.end()));
    return m;
  }
}
