/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCM_DIGITIZATION_BCM_DIGITIZATION_H
#define BCM_DIGITIZATION_BCM_DIGITIZATION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPileUpTool;

/** Top algorithm class for BCM digitization */
class BCM_Digitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  BCM_Digitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
   ToolHandle<IPileUpTool> m_digTool;
};

#endif // BCM_DIGITIZATION_BCM_DIGITIZATION_H
