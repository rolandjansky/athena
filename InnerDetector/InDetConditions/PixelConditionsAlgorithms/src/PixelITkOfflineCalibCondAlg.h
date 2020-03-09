/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef PIXELITKOFFLINECALIBCONDALG
#define PIXELITKOFFLINECALIBCONDALG

#include "CLIDSvc/CLASS_DEF.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "DetDescrConditions/DetCondCFloat.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelITkOfflineCalibData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

//For 21.9 only, to be removed in master
#include "StoreGate/StoreGateSvc.h"

class PixelITkOfflineCalibCondAlg : public AthReentrantAlgorithm {
  public:
    PixelITkOfflineCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelITkOfflineCalibCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute_r(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:
    int m_inputSource;
    std::string m_textFileName;
    int m_dump;

    SG::ReadCondHandleKey<DetCondCFloat> m_readKey{this, "ReadKey", "/PIXEL/PixReco", "Input key of pixreco conditions folder"};
    SG::WriteCondHandleKey<PixelCalib::PixelITkOfflineCalibData> m_writeKey{this, "WriteKey", "PixelITkOfflineCalibData", "Output key of pixel module data"};

    ServiceHandle<ICondSvc> m_condSvc;

    //For 21.9 only, to be removed in master
    StoreGateSvc* m_detStore;

};


#endif

