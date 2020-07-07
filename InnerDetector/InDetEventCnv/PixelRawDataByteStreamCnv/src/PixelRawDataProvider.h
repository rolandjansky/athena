/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Top algorithm to decode the Pixel BS
///////////////////////////////////////////////////////////////////

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDER_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include <string>

#include "PixelConditionsData/PixelCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "PixelRawDataByteStreamCnv/IPixelRawDataProviderTool.h"
#include "IRegionSelector/IRegSelSvc.h"

// Forward declarations
class PixelID;

class PixelRawDataProvider : public AthReentrantAlgorithm {

 public:

  //! Constructor.
  PixelRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  StatusCode initialize() override;
  //! Execute
  StatusCode execute(const EventContext& ctx) const override;
  //! Don't need to override Finalize

  
  //! Destructur
  ~PixelRawDataProvider();

private:
  const PixelID* m_pixel_id { nullptr };

  ServiceHandle<IRegSelSvc> m_regionSelector                            { this, "RegSelSvc",  "RegSelSvc", "Region selector" };
  ServiceHandle<IROBDataProviderSvc>   m_robDataProvider                { this, "ROBDataProvider", "ROBDataProviderSvc" };
  ToolHandle<IPixelRawDataProviderTool> m_rawDataTool                   { this, "ProviderTool", "PixelRawDataProviderTool"};
  Gaudi::Property<bool> m_roiSeeded                                     { this, "isRoI_Seeded", false, "Use RoI" }; // TODO, doubled information, empty RoIs collection name would be sufficent
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey     { this, "RoIs", "", "If RoI driven unpacking to be used, this is the key"};
  SG::WriteHandleKey<PixelRDO_Container> m_rdoContainerKey              { this, "RDOKey", "PixelRDOs"};
  SG::UpdateHandleKey<PixelRDO_Cache> m_rdoCacheKey;
  SG::WriteHandleKey<IDCInDetBSErrContainer> m_bsErrorsKey              { this, "BSErrorsKey", "PixBSErr"};  
  SG::UpdateHandleKey<IDCInDetBSErrContainer_Cache> m_bsErrorsCacheKey;
  SG::ReadCondHandleKey<PixelCablingCondData> m_condCablingKey          { this, "PixelCablingCondData", "PixelCablingCondData", "Pixel cabling key"};

};

#endif

