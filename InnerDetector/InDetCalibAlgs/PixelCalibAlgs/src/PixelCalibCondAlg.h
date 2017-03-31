/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL_PIXELCALIBCONDALG
#define PIXEL_PIXELCALIBCONDALG 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "DetDescrConditions/DetCondCFloat.h"

class PixelCalibCondAlg : public AthAlgorithm
{
 public:

  PixelCalibCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelCalibCondAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  void PrintConstants(const float* constants);

  SG::ReadCondHandleKey<DetCondCFloat> m_readKey;
  SG::WriteCondHandleKey<PixelCalib::PixelOfflineCalibData>  m_writeKey;

  ServiceHandle<ICondSvc> m_condSvc;
};

#endif
