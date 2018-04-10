/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HBB_TAGGING_ALGORITHM_HH
#define HBB_TAGGING_ALGORITHM_HH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/AnaToolHandle.h"
#include "AthContainers/AuxElement.h"
// #include "GeoPrimitives/GeoPrimitives.h"
// #include "xAODTracking/TrackingPrimitives.h"
// #include "xAODTracking/TrackParticleContainer.h"

class HbbTaggerDNN;
class JetCalibrationTool;

class HbbTaggingAlgorithm: public AthAlgorithm {
public:
  HbbTaggingAlgorithm(const std::string& name,
                      ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
private:
  std::string m_jet_collection;
  asg::AnaToolHandle<HbbTaggerDNN> m_tagger;
  asg::AnaToolHandle<JetCalibrationTool> m_jet_calib_tool;
};

#endif
