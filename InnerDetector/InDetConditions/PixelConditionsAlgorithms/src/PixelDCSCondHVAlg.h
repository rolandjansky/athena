/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
/**
 * @file PixelConditionsAlgorithms/PixelDCSCondHVAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel bias voltage data in PixelDCSHVData.
 */

#ifndef PIXELDCSCONDHVALG
#define PIXELDCSCONDHVALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelDCSHVData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class PixelDCSCondHVAlg : public AthReentrantAlgorithm {
  public:
    PixelDCSCondHVAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDCSCondHVAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    const PixelID* m_pixelID{nullptr};
    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

    SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
    {this, "ReadKey", "/PIXEL/DCS/HV", "Key of input (raw) HV conditions folder"};

    SG::WriteCondHandleKey<PixelDCSHVData> m_writeKey
    {this, "WriteKey", "PixelDCSHVCondData", "Key of output (derived) HV conditions data"};

};

#endif
