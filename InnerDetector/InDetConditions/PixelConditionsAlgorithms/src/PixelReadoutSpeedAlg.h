/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsAlgorithms/PixelReadoutSpeedAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel FE link data in PixelReadoutSpeedData.
 */

#ifndef PIXELREADOUTSPEEDALG_H
#define PIXELREADOUTSPEEDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelReadoutSpeedData.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class PixelReadoutSpeedAlg : public AthReentrantAlgorithm {
  public:
    PixelReadoutSpeedAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelReadoutSpeedAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

    SG::ReadCondHandleKey<AthenaAttributeList> m_readKey
    {this, "ReadKey", "/PIXEL/ReadoutSpeed", "Input readout speed folder"};

    SG::WriteCondHandleKey<PixelReadoutSpeedData> m_writeKey
    {this, "WriteKey", "PixelReadoutSpeedData", "Output readout speed data"};

};

#endif
