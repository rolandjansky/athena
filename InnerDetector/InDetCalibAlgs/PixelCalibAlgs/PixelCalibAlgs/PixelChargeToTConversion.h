/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCALIBALGS_PIXELCHARGETOTCONVERSION_H
#define PIXELCALIBALGS_PIXELCHARGETOTCONVERSION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#define private public
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelChargeCalibCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include<string>
#include<vector>

class IBLParameterSvc;

class PixelChargeToTConversion: public AthAlgorithm{
  
 public:
  PixelChargeToTConversion(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelChargeToTConversion();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  ServiceHandle<IBLParameterSvc> m_IBLParameterSvc;
  
  //std::vector<unsigned int> m_modules;
  std::string m_PixelsClustersName;
  const InDet::PixelClusterContainer* m_Pixel_clcontainer;
  
  ServiceHandle<IPixelCablingSvc> m_pixelCabling
  {this, "PixelCablingSvc", "PixelCablingSvc", "Pixel cabling service" };

  SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
  {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

  SG::ReadCondHandleKey<PixelChargeCalibCondData> m_chargeDataKey
  {this, "PixelChargeCalibCondData", "PixelChargeCalibCondData", "Charge calibration"};

};

#endif
