/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsAlgorithms/PixelDistortionAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date December, 2019
 * @brief Store pixel distortion data in PixelDistortionData.
 */

#ifndef PIXELDISTORTIONALG_H
#define PIXELDISTORTIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "DetDescrConditions/DetCondCFloat.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelDistortionData.h"

#include "InDetIdentifier/PixelID.h"
#include "AthenaKernel/IAthRNGSvc.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class PixelDistortionAlg : public AthAlgorithm {  
  public:
    PixelDistortionAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDistortionAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:
    const PixelID* m_pixelID{nullptr};
    ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc"};  //!< Random number service
    ServiceHandle<ICondSvc>   m_condSvc{this, "CondSvc", "CondSvc"};

    SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

    SG::ReadCondHandleKey<DetCondCFloat> m_readKey
    {this, "ReadKey", "/Indet/PixelDist", "Input readout distortion folder"};

    SG::WriteCondHandleKey<PixelDistortionData> m_writeKey
    {this, "WriteKey", "PixelDistortionData", "Output readout distortion data"};

};

#endif
