/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StationSelector_H
#define StationSelector_H

#include "MuonGeoModel/Station.h"

#include <atomic>
#include <map>
#include <string>
#include <vector>

namespace MuonGM {

    class StationSelector {
      public:
        typedef std::map<std::string, Station *, std::less<std::string>>::const_iterator StationIterator;
        StationSelector(std::string filename);
        StationSelector(std::vector<std::string> s);
        StationIterator begin();
        StationIterator end();
        static void SetSelectionType(int t);

      private:
        const StationIterator m_iterator;
        std::vector<std::string> m_selector;
        std::map<std::string, Station *, std::less<std::string>> m_theMap;
        bool select(StationIterator it);
        static std::atomic<int> m_selectType ATLAS_THREAD_SAFE;
    };

} // namespace MuonGM

#endif
