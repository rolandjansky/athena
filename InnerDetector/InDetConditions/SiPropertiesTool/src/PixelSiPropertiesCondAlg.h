/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELSIPROPERTIESCONDALG
#define PIXELSIPROPERTIESCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "GaudiKernel/ICondSvc.h"

#include "SiPropertiesTool/SiliconPropertiesVector.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

class PixelSiPropertiesCondAlg : public AthAlgorithm {  
  public:
    PixelSiPropertiesCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelSiPropertiesCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    const PixelID* m_pixid;
    
    ServiceHandle<ICondSvc> m_condSvc;

    SG::ReadCondHandleKey<PixelModuleData> m_readKeyTemp      {this, "ReadKeyeTemp", "PixelDCSTempCondData",         "Key of input sensor temperature conditions folder"};
    SG::ReadCondHandleKey<PixelModuleData> m_readKeyHV        {this, "ReadKeyHV",    "PixelDCSHVCondData",           "Key of input bias voltage conditions folder"};
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
    SG::WriteCondHandleKey<InDet::SiliconPropertiesVector> m_writeKey{this, "WriteKey",     "PixelSiliconPropertiesVector", "Key of output silicon properties conditions folder"};

};
    
#endif // PIXELSIPROPERTIESCONDALG
