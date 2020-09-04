/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef AmbiguityProcessorUtility_h
#define AmbiguityProcessorUtility_h

#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkEventPrimitives/TrackScore.h"
#include <vector>
#include <set>
#include <array>
#include <string>
#include <memory> //unique_ptr

namespace Trk{
  class Track;
  class PrepRawData;
}


namespace AmbiguityProcessor{
  enum TrackFilterCategory{ ScoreIsZero, TrackIsDuplicate, TrackAccepted, nCategories};
  using AssociationTool = ToolHandle<Trk::IPRDtoTrackMapTool>;
  using AssociationMap = Trk::PRDtoTrackMap;
  using DuplicationCheckSet = std::set<std::vector<const Trk::PrepRawData*>> ;
  //
  //categorise the track as zero-score, duplicate or 'accepted'
  TrackFilterCategory
  categoriseTrack(const Trk::Track & track, const Trk::TrackScore & score, const bool dropDuplicates, const AssociationTool &, AssociationMap &, DuplicationCheckSet &);
  //
  //give appropriate text for each category
  const static std::array<std::string , nCategories> debugMessage {"Score is zero, reject.", "Track is duplicate, reject.", "Track is accepted."};
  //calculate a simple chi^2/ndof
  float calculateFitQuality(const Trk::Track & track);
  //create a track from a new FitQuality object looping over track-state-on-surfaces to calculate
  std::unique_ptr<Trk::Track> createNewFitQualityTrack(const Trk::Track & track);
}//namespace

#endif
