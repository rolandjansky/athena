/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
/**
 * @file PixelConditionsAlgorithms/PixelOfflineCalibCondAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel offline calibration data in PixelOfflineCalibData.
 */

#ifndef PIXELOFFLINECALIBCONDALG
#define PIXELOFFLINECALIBCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "DetDescrConditions/DetCondCFloat.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class PixelOfflineCalibCondAlg : public AthReentrantAlgorithm {
  public:
    PixelOfflineCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelOfflineCalibCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    Gaudi::Property<int> m_inputSource
    {this, "InputSource",2,"Source of data: 0 (none), 1 (text file), 2 (database)"};

    Gaudi::Property<std::string> m_textFileName1
    {this, "PixelClusterErrorDataFile", "PixelClusterErrorData.txt","Read constants from this file"};

    Gaudi::Property<std::string> m_textFileName2
    {this, "PixelClusterOnTrackErrorDataFile", "PixelClusterOnTrackErrorData.txt","Read constants from this file"};

    Gaudi::Property<std::string> m_textFileName3
    {this, "PixelChargeInterpolationDataFile", "PixelChargeInterpolationData.txt","Read constants from this file"};

    Gaudi::Property<int> m_dump
    {this, "DumpConstants", 0, "Dump constants to text file"};

    SG::ReadCondHandleKey<DetCondCFloat> m_readKey
    {this, "ReadKey", "/PIXEL/PixReco", "Input key of pixreco conditions folder"};

    SG::WriteCondHandleKey<PixelCalib::PixelOfflineCalibData> m_writeKey
    {this, "WriteKey", "PixelOfflineCalibData", "Output key of pixel module data"};

    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
};

#endif
