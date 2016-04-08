// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXBEGINRUN_ATHEXCREATECONDALG_H
#define ATHEXBEGINRUN_ATHEXCREATECONDALG_H 1

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>


class StoreGateSvc;

/**
 * @class AthExCreateCondAlg
 * @brief Algorithm to create dummy conditions
 */
class AthExCreateCondAlg : public AthAlgorithm {
public:
  AthExCreateCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:

  UnsignedIntegerArrayProperty  m_iovRuns;
  StringProperty                m_key;
  StringProperty                m_tag;
  
  StatusCode createCondObjects();
};

#endif  // ATHEXBEGINRUN_ATHEXCREATECONDALG_H
