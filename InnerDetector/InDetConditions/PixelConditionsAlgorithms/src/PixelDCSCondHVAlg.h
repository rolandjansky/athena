/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELDCSCONDHVALG
#define PIXELDCSCONDHVALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelDCSCondData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelDCSCondHVAlg : public AthAlgorithm {  
  public:
    PixelDCSCondHVAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDCSCondHVAlg() = default;
    StatusCode initialize() override;
    StatusCode execute() override;
    StatusCode finalize() override;

  private:
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
    SG::WriteCondHandleKey<PixelDCSCondData> m_writeKey;

    ServiceHandle<ICondSvc> m_condSvc;

    BooleanProperty m_returnHVTemp;
};

#endif // PIXELDCSCONDHVALG
