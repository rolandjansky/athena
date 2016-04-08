/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKTRUTHSIMILARITYSELECTOR_H
#define TRACKTRUTHSIMILARITYSELECTOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"

namespace Trk { class IDetailedTrackTruthSimilarity; }

class TrackTruthSimilaritySelector: public AthAlgorithm {
public:
  TrackTruthSimilaritySelector(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:
  // DetailedTrackTruthCollection input name
  std::string m_detailedTrackTruthName;
  
  // TrackTruthCollection output name
  std::string m_outputName;

  // Match quality tool
  ToolHandle<Trk::IDetailedTrackTruthSimilarity> m_matchTool;
  
  void fillOutput(TrackTruthCollection *out, const DetailedTrackTruthCollection *in);
};

#endif/*TRACKTRUTHSIMILARITYSELECTOR_H*/
