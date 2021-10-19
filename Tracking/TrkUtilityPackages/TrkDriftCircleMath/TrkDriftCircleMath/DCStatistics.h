/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __DC_STATISTICS_H__
#define __DC_STATISTICS_H__

#include <map>

namespace MuonGM {
    class MdtReadoutElement;
}

namespace TrkDriftCircleMath {

    /** This class offers no functionality, but to define a standard device for the maker to transfer to the finder
        objects which are neither road, hits or
        geometry, i.e. preprocessed variables that it would be inefficient to regenerate anew. */
    class DCStatistics : public std::map<const MuonGM::MdtReadoutElement*, int> {};
    typedef DCStatistics::iterator DCStatIt;
    typedef DCStatistics::const_iterator DCStatCit;
}  // namespace TrkDriftCircleMath

#endif  //__DC_STATISTICS_H__
