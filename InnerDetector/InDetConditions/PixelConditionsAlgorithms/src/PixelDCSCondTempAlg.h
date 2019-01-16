/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELDCSCONDTEMPALG
#define PIXELDCSCONDTEMPALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelDCSCondTempAlg : public AthAlgorithm {  
  public:
    PixelDCSCondTempAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDCSCondTempAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    const PixelID* m_pixelID;

    bool m_useConditions;
    float m_defaultTemperature;

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/PIXEL/DCS/TEMPERATURE", "Key of input (raw) temperature conditions folder"};
    SG::WriteCondHandleKey<PixelModuleData> m_writeKey{this, "WriteKey", "PixelDCSTempCondData", "Key of output (derived) temperature conditions folder"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif // PIXELDCSCONDTEMPALG
