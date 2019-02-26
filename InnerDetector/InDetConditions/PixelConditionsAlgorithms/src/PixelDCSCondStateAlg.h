/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELDCSCONDSTATEALG
#define PIXELDCSCONDSTATEALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelModuleData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelDCSCondStateAlg : public AthReentrantAlgorithm {  
  public:
    PixelDCSCondStateAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDCSCondStateAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyState   {this, "ReadKeyState",   "/PIXEL/DCS/FSMSTATE",    "Key of input (raw) State conditions folder"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyStatus  {this, "ReadKeyStatus",  "/PIXEL/DCS/FSMSTATUS",   "Key of input (raw) Status conditions folder"};

    SG::WriteCondHandleKey<PixelModuleData> m_writeKeyState {this, "WriteKeyState",  "PixelDCSStateCondData",  "Key of output (derived) State conditions folder"};
    SG::WriteCondHandleKey<PixelModuleData> m_writeKeyStatus{this, "WriteKeyStatus", "PixelDCSStatusCondData", "Key of output (derived) Status conditions folder"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif // PIXELDCSCONDSTATEALG
