/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELDCSCONDHVALG
#define PIXELDCSCONDHVALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelDCSConditionsData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelDCSCondHVAlg : public AthReentrantAlgorithm {
  public:
    PixelDCSCondHVAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDCSCondHVAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/PIXEL/DCS/HV", "Key of input (raw) HV conditions folder"};
    SG::WriteCondHandleKey<PixelDCSConditionsData> m_writeKey{this, "WriteKey", "PixelDCSHVCondData", "Key of output (derived) HV conditions data"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif // PIXELDCSCONDHVALG
