/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCALIBALGS_PIXELCHARGETOTCONVERSION_H
#define PIXELCALIBALGS_PIXELCHARGETOTCONVERSION_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelChargeCalibCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"

#include <string>
#include <vector>

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

  SG::ReadHandleKey<InDet::PixelClusterContainer> m_pixelsClustersKey
  {this, "PixelClusterContainer",  "PixelClusters", ""};

  ServiceHandle<IPixelCablingSvc> m_pixelCabling
  {this, "PixelCablingSvc", "PixelCablingSvc", "Pixel cabling service" };

  SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
  {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

  SG::ReadCondHandleKey<PixelChargeCalibCondData> m_chargeDataKey
  {this, "PixelChargeCalibCondData", "PixelChargeCalibCondData", "Charge calibration"};

  // For P->T converter of PixelClusters
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection>
    m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection",
      "Key of SiDetectorElementCollection for Pixel"};
};

#endif
