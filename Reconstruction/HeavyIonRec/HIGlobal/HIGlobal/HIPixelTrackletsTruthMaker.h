/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSTRUTHMAKER_H
#define HIPIXELTRACKLETSTRUTHMAKER_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "HIGlobal/HIPixelTrackletsTruthCollection.h"

class PixelID;
class StoreGateSvc;

class HIPixelTrackletsTruthMaker: public AthAlgorithm {
  typedef InDet::PixelCluster PixelCluster;
 public: 
  HIPixelTrackletsTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  
 private:
  void BuildTrackletsTruth(HIPixelTrackletsTruthCollection* pttc, const HIPixelTrackletsCollection &tracklets);
  
  std::string m_PRDTruthNamePixel;
  std::string m_pixelTrackletsCollectionName;
  std::string m_pixelTrackletsTruthName;

  const PixelID *pixelID;
  StoreGateSvc *m_storeGate;
  const PRD_MultiTruthCollection *m_truthCollectionPixel;
};

#endif
