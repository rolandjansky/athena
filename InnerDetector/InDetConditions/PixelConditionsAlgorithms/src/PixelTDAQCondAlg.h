/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELTDAQCONDALG
#define PIXELTDAQCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelTDAQCondAlg : public AthReentrantAlgorithm {
  public:
    PixelTDAQCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelTDAQCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    const PixelID* m_pixelID;

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/TDAQ/Resources/ATLAS/PIXEL/Modules", "Input key of TDAQ deadmap conditions folder"};
    SG::WriteCondHandleKey<PixelModuleData> m_writeKey{this, "WriteKey", "PixelTDAQCondData", "Output key of pixel module data"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif // PIXELDEADMAPCONDALG
