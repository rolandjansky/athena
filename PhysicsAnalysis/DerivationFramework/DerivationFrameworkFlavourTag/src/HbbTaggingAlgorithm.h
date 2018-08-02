/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HBB_TAGGING_ALGORITHM_HH
#define HBB_TAGGING_ALGORITHM_HH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/AuxElement.h"

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
  double m_min_pt;
  double m_max_eta;
  ToolHandle<HbbTaggerDNN> m_tagger;
  ToolHandle<JetCalibrationTool> m_jet_calib_tool;
  std::vector<SG::AuxElement::ConstAccessor<float> > m_var_checkers;
  std::vector<SG::AuxElement::Decorator<float> > m_default_filler;
};

#endif
