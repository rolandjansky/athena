/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_NN_ALG_HH
#define BTAGGING_NN_ALG_HH

#include "AthenaBaseComps/AthAlgorithm.h"

namespace FlavorTagDiscriminants {
  class DL2HighLevel;
}

namespace Analysis {

  class BTagNNAlg: public AthAlgorithm {
  public:
    BTagNNAlg(const std::string& name,
              ISvcLocator* pSvcLocator );

    StatusCode initialize() override final;
    StatusCode execute() override final;
    StatusCode finalize() override final;
  private:
    std::unique_ptr<FlavorTagDiscriminants::DL2HighLevel> m_dl2;
    std::string m_nnPath;
    std::vector<std::string> m_jetCollections;
    std::map<std::string, std::string> m_variableRemapping;
  };

}

#endif
