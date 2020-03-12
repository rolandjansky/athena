/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_JET_AUGMENTER_ALG_HH
#define BTAG_JET_AUGMENTER_ALG_HH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class BTagJetAugmenter;

class BTagJetAugmenterAlg: public AthAlgorithm {
public:
  BTagJetAugmenterAlg(const std::string& name,
                      ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
private:
  std::string m_jet_collection;
  std::unique_ptr<BTagJetAugmenter> m_augmenter;
};

#endif
