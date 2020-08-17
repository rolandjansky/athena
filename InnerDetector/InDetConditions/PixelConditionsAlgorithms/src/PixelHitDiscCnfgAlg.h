/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsAlgorithms/PixelHitDiscCnfgAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store FEI4 HitDiscCofig parameter in PixelHitDiscCnfgData.
 */

#ifndef PIXELHITDISCCNFGALG_H
#define PIXELHITDISCCNFGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelHitDiscCnfgData.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class PixelHitDiscCnfgAlg : public AthReentrantAlgorithm {
  public:
    PixelHitDiscCnfgAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelHitDiscCnfgAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

    SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

    SG::ReadCondHandleKey<AthenaAttributeList> m_readKey
    {this, "ReadKey", "/PIXEL/HitDiscCnfg", "Input HitDiscCnfg folder"};

    SG::WriteCondHandleKey<PixelHitDiscCnfgData> m_writeKey
    {this, "WriteKey", "PixelHitDiscCnfgData", "Output HitDiscCnfg data"};

};

#endif
