/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELCHARGECALIBCONDALG
#define PIXELCHARGECALIBCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelChargeCalibCondData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelChargeCalibCondAlg : public AthAlgorithm {  
  public:
    PixelChargeCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelChargeCalibCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    const PixelID* m_pixelID;

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey
    {this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

    SG::ReadCondHandleKey<PixelModuleData> m_configKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
    {this, "ReadKey", "/PIXEL/PixCalib", "Iput charge calibration folder"};

    SG::WriteCondHandleKey<PixelChargeCalibCondData> m_writeKey
    {this, "WriteKey", "PixelChargeCalibCondData", "Output charge caliblation data"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif
