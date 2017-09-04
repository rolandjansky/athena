/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_VERTEX_AUGMENTER_HH
#define BTAGGING_VERTEX_AUGMENTER_HH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class BTagVertexAugmenter: public AthAlgorithm {
public:
  BTagVertexAugmenter(const std::string& name,
                      ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
private:
};

#endif
