/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELREADOUTSPEEDALG_H
#define PIXELREADOUTSPEEDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelReadoutSpeedData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelReadoutSpeedAlg : public AthAlgorithm {  
  public:
    PixelReadoutSpeedAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelReadoutSpeedAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    ServiceHandle<ICondSvc> m_condSvc;

    SG::ReadCondHandleKey<AthenaAttributeList> m_readKey
    {this, "ReadKey", "/PIXEL/ReadoutSpeed", "Input readout speed folder"};

    SG::WriteCondHandleKey<PixelReadoutSpeedData> m_writeKey
    {this, "WriteKey", "PixelReadoutSpeedData", "Output readout speed data"};

};

#endif
