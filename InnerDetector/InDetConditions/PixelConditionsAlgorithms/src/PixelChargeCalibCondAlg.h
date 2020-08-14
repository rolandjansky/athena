/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/ 
/**
 * @file PixelConditionsAlgorithms/PixelChargeCalibCondAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date December, 2019
 * @brief Store pixel charge calibration constants in PixelChargeCalibCondData.
 */

#ifndef PIXELCHARGECALIBCONDALG
#define PIXELCHARGECALIBCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelChargeCalibCondData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class PixelChargeCalibCondAlg : public AthReentrantAlgorithm {
  public:
    PixelChargeCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    const PixelID* m_pixelID{nullptr};

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey
    {this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

    SG::ReadCondHandleKey<PixelModuleData> m_configKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
    {this, "ReadKey", "/PIXEL/PixCalib", "Iput charge calibration folder"};

    SG::WriteCondHandleKey<PixelChargeCalibCondData> m_writeKey
    {this, "WriteKey", "PixelChargeCalibCondData", "Output charge caliblation data"};

    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
};

#endif
