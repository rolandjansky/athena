/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCALIBALGS_PIXELCHARGETOTCONVERSION_H
#define PIXELCALIBALGS_PIXELCHARGETOTCONVERSION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#define private public
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

#include<string>
#include<vector>

class IPixelCalibSvc;
class IPixelOfflineCalibSvc;
class IBLParameterSvc;

class PixelChargeToTConversion: public AthAlgorithm{
  
 public:
  PixelChargeToTConversion(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelChargeToTConversion();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  ServiceHandle<IPixelCalibSvc> m_calibsvc;
  ServiceHandle<IBLParameterSvc> m_IBLParameterSvc;
  int                                  m_overflowIBLToT;                                                                                          
  ServiceHandle<IPixelOfflineCalibSvc> m_offlineCalibSvc;
  
  //std::vector<unsigned int> m_modules;
  std::string m_PixelsClustersName;
  const InDet::PixelClusterContainer* m_Pixel_clcontainer;

};

#endif
