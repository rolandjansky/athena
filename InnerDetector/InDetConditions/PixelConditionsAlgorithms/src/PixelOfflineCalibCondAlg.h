/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELOFFLINECALIBCONDALG
#define PIXELOFFLINECALIBCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "DetDescrConditions/DetCondCFloat.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class PixelOfflineCalibCondAlg : public AthReentrantAlgorithm {
  public:
    PixelOfflineCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelOfflineCalibCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    int m_inputSource;
    std::string m_textFileName1;
    std::string m_textFileName2;
    std::string m_textFileName3;
    int m_dump;

    SG::ReadCondHandleKey<DetCondCFloat> m_readKey{this, "ReadKey", "/PIXEL/PixReco", "Input key of pixreco conditions folder"};
    SG::WriteCondHandleKey<PixelCalib::PixelOfflineCalibData> m_writeKey{this, "WriteKey", "PixelOfflineCalibData", "Output key of pixel module data"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif
