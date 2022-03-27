/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKTRUTHSIMILARITYSELECTOR_H
#define TRACKTRUTHSIMILARITYSELECTOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkToolInterfaces/IDetailedTrackTruthSimilarity.h"

class TrackTruthSimilaritySelector: public AthAlgorithm {
public:
  TrackTruthSimilaritySelector(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:
  // DetailedTrackTruthCollection input
  SG::ReadHandleKey<DetailedTrackTruthCollection> m_detailed{this, "DetailedTrackTruthName", "", ""};
  
  // TrackTruthCollection output
  SG::WriteHandleKey<TrackTruthCollection> m_out{this, "OutputName", "", ""};

  // Match quality tool
  ToolHandle<Trk::IDetailedTrackTruthSimilarity> m_matchTool
    {this, "TrackTruthSimilarityTool", "Trk::TruthMatchRatio", "Track-truth similarity tool"};
  
  void fillOutput(TrackTruthCollection *out, const DetailedTrackTruthCollection *in);
};

#endif/*TRACKTRUTHSIMILARITYSELECTOR_H*/
