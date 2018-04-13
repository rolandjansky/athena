/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HBB_TAGGING_ALGORITHM_HH
#define HBB_TAGGING_ALGORITHM_HH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
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
  std::string m_decoration_name;
  double m_min_pt;
  double m_max_eta;
  ToolHandle<HbbTaggerDNN> m_tagger;
  ToolHandle<JetCalibrationTool> m_jet_calib_tool;
  SG::AuxElement::Decorator<double> m_dec_jet;
};

#endif
