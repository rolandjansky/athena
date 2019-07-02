/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELHITDISCCNFGALG_H
#define PIXELHITDISCCNFGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelHitDiscCnfgData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelHitDiscCnfgAlg : public AthAlgorithm {  
  public:
    PixelHitDiscCnfgAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelHitDiscCnfgAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    ServiceHandle<ICondSvc> m_condSvc;
    int m_defaultHitDiscCnfg;

    SG::ReadCondHandleKey<AthenaAttributeList> m_readKey
    {this, "ReadKey", "/PIXEL/HitDiscCnfg", "Input HitDiscCnfg folder"};

    SG::WriteCondHandleKey<PixelHitDiscCnfgData> m_writeKey
    {this, "WriteKey", "PixelHitDiscCnfgData", "Output HitDiscCnfg data"};

};

#endif
