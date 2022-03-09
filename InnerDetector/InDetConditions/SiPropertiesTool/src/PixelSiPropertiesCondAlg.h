/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/ 
/**
 * @file SiPropertiesTool/PixelSiPropertiesCondAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Return pixel property.
 */

#ifndef PIXELSIPROPERTIESCONDALG
#define PIXELSIPROPERTIESCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelDCSHVData.h"
#include "PixelConditionsData/PixelDCSTempData.h"

#include "SiPropertiesTool/SiliconPropertiesVector.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

class PixelSiPropertiesCondAlg : public AthReentrantAlgorithm {  
  public:
    PixelSiPropertiesCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelSiPropertiesCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    const PixelID* m_pixid;

    SG::ReadCondHandleKey<PixelDCSTempData> m_readKeyTemp
    {this, "ReadKeyeTemp", "PixelDCSTempCondData", "Key of input sensor temperature conditions folder"};

    SG::ReadCondHandleKey<PixelDCSHVData> m_readKeyHV
    {this, "ReadKeyHV", "PixelDCSHVCondData", "Key of input bias voltage conditions folder"};

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey
    {this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

    SG::WriteCondHandleKey<InDet::SiliconPropertiesVector> m_writeKey
    {this, "WriteKey", "PixelSiliconPropertiesVector", "Key of output silicon properties conditions folder"};
};
    
#endif
