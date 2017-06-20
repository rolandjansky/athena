/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKTRUTHSELECTOR_H
#define TRACKTRUTHSELECTOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"

class TrackTruthSelector: public AthAlgorithm {
public:
  TrackTruthSelector(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:

   
  // DetailedTrackTruthCollection input name
  SG::ReadHandle<DetailedTrackTruthCollection>  m_detailed;  //  std::string m_detailedTrackTruthName;

  // TrackTruthCollection output name
  SG::WriteHandle<TrackTruthCollection> m_out; //std::string m_outputName;

  // Subdetector weights
  std::vector<double> m_subDetWeights;

  void fillOutput(TrackTruthCollection *out, const DetailedTrackTruthCollection *in);
  double getProbability(const DetailedTrackTruth& dt) const;
};

#endif/*TRACKTRUTHSELECTOR_H*/
