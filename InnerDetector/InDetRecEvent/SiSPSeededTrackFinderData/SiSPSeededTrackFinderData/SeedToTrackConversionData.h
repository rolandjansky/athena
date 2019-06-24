// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSEEDTOTRACKCONVERSIONDATA_H
#define INDETSEEDTOTRACKCONVERSIONDATA_H

#include "TrkTrack/TrackInfo.h"
#include "TrkTrack/TrackCollection.h"

#include <memory>
#include <string>

namespace InDet 
{

  /** @class SeedToTrackConversionData

      InDet::SeedToTrackConversionData holds event dependent data used in
      InDet::SeedToTrackConversionTool.

      @author Susumu Oda <Susumu.Oda@cern.ch>
  */  

  class SeedToTrackConversionData
  {
  public:
    SeedToTrackConversionData();
    ~SeedToTrackConversionData() = default;

    std::unique_ptr<TrackCollection>& seedSegmentsCollection();
    Trk::TrackInfo& trackInfo();
    std::string& patternName();

  private:
    std::unique_ptr<TrackCollection> m_seedSegmentsCollection; //!< output collection for seed
    Trk::TrackInfo m_trackInfo; //!< TrackInfo for seeds
    std::string m_patternName; //!< Name of the pattern recognition
  }; 
} // end of namespace

#endif // INDETSEEDTOTRACKCONVERSIONDATA_H
