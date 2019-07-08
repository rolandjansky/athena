/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDEADMAPCONDALG_H
#define PIXELDEADMAPCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelDeadMapCondData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelDeadMapCondAlg : public AthReentrantAlgorithm {
  public:
    PixelDeadMapCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    ServiceHandle<ICondSvc> m_condSvc;

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
    {this, "ReadKey", "/PIXEL/PixelModuleFeMask", "Input deadmap folder"};

    SG::WriteCondHandleKey<PixelDeadMapCondData> m_writeKey
    {this, "WriteKey", "PixelDeadMapCondData", "Output deadmap data"};

};

#endif
