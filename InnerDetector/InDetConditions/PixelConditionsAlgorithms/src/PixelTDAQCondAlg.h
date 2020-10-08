/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/ 
/**
 * @file PixelConditionsAlgorithms/PixelTDAQCondAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel TDAQ module state to PixelTDAQData.
 */

#ifndef PIXELTDAQCONDALG
#define PIXELTDAQCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelTDAQData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class PixelTDAQCondAlg : public AthReentrantAlgorithm {
  public:
    PixelTDAQCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelTDAQCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    const PixelID* m_pixelID{nullptr};
    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
    {this, "ReadKey", "", "Input key of TDAQ deadmap conditions folder"};

    SG::WriteCondHandleKey<PixelTDAQData> m_writeKey
    {this, "WriteKey", "PixelTDAQCondData", "Output key of pixel module data"};

};

#endif
