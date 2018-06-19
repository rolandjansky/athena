/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELDCSCONDSTATEALG
#define PIXELDCSCONDSTATEALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelDCSCondData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelDCSCondStateAlg : public AthAlgorithm {  
  public:
    PixelDCSCondStateAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDCSCondStateAlg() = default;
    StatusCode initialize() override;
    StatusCode execute() override;
    StatusCode finalize() override;

  private:
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyState;
    SG::WriteCondHandleKey<PixelDCSCondData> m_writeKeyState;

    ServiceHandle<ICondSvc> m_condSvc;

    BooleanProperty m_readAllDBFolders;
};

#endif // PIXELDCSCONDSTATEALG
