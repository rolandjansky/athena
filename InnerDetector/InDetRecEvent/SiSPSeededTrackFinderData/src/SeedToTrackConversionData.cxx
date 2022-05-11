// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// Source file for class SeedToTrackConversionData
/////////////////////////////////////////////////////////////////////////////////
// Class for event dependent data used in SeedToTrackConversionTool
/////////////////////////////////////////////////////////////////////////////////

#include "SiSPSeededTrackFinderData/SeedToTrackConversionData.h"

namespace InDet {

  // Constructor
  SeedToTrackConversionData::SeedToTrackConversionData() = default;
  
  // Get methods
  std::unique_ptr<TrackCollection>& SeedToTrackConversionData::seedSegmentsCollection() {
    return m_seedSegmentsCollection;
  }

  Trk::TrackInfo& SeedToTrackConversionData::trackInfo() {
    return m_trackInfo;
  }

  std::string& SeedToTrackConversionData::patternName() {
    return m_patternName;
  }

} // end of name space
