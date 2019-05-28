/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELCORRECTIONALG_H
#define TILERECUTILS_TILERAWCHANNELCORRECTIONALG_H

// Tile includes
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/ITileRawChannelTool.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

/** 
 *  @class TileRawChannelCorrectionAlg
 *  @brief This algorithm applies noise filter tools to input Tile raw channel container
 */
class TileRawChannelCorrectionAlg: public AthReentrantAlgorithm {
  public:

    using AthReentrantAlgorithm::AthReentrantAlgorithm;

    virtual ~TileRawChannelCorrectionAlg() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:

    SG::ReadHandleKey<TileRawChannelContainer> m_inputRawChannelContainerKey{this,
        "InputRawChannelContainer", "TileRawChannelCnt", "Input Tile raw channels container key"};

    SG::WriteHandleKey<TileRawChannelContainer> m_outputRawChannelContainerKey{this,
        "OutputRawChannelContainer", "TileRawChannelCntCorrected", "Output Tile digits container key"};

    ToolHandleArray<ITileRawChannelTool> m_noiseFilterTools{this,
        "NoiseFilterTools", {}, "Tile noise filter tools"};
};

#endif // TILERECUTILS_TILERAWCHANNELCORRECTIONALG_H
