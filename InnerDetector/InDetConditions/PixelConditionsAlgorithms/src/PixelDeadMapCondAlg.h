/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELDEADMAPCONDALG
#define PIXELDEADMAPCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelDeadMapCondAlg : public AthAlgorithm {  
  public:
    PixelDeadMapCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDeadMapCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/PIXEL/PixMapOverlay", "Input key of deadmap conditions folder"};
    SG::WriteCondHandleKey<PixelModuleData> m_writeKey{this, "WriteKey", "PixelModuleData", "Output key of pixel module data"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif // PIXELDEADMAPCONDALG
