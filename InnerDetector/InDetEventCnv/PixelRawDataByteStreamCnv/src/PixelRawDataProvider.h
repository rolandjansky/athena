/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Top algorithm to decode the Pixel BS
///////////////////////////////////////////////////////////////////

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDER_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include <string>

// Forward declarations
class IPixelRawDataProviderTool;
class PixelID;
class IROBDataProviderSvc;
class IRegSelSvc;
class IPixelCablingSvc;

class PixelRawDataProvider : public AthAlgorithm {

 public:

  //! Constructor.
  PixelRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  StatusCode initialize() override;
  //! Execute
  StatusCode execute() override;
  //! Don't need to override Finalize

  
  //! Destructur
  ~PixelRawDataProvider();

private:

  ServiceHandle<IRegSelSvc>             m_regionSelector;     
  ServiceHandle<IPixelCablingSvc>      m_pixelCabling;
  ServiceHandle<IROBDataProviderSvc>   m_robDataProvider;
  ToolHandle<IPixelRawDataProviderTool> m_rawDataTool;
  const PixelID*                       m_pixel_id; 
  bool m_roiSeeded;
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  SG::WriteHandleKey<PixelRDO_Container> m_rdoContainerKey;
  SG::UpdateHandleKey<PixelRDO_Cache> m_rdoCacheKey;
};

#endif

