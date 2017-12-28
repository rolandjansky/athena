/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCORRELATEDNOISE_H
#define TILERECUTILS_TILERAWCORRELATEDNOISE_H

/********************************************************************
 *
 * NAME:     TileRawCorrelatedNoise 
 * PACKAGE:  offline/TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  F. Veloso
 * CREATED:  12.07.2010
 *
 ********************************************************************/

#include "TileEvent/TileDigitsContainer.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"


class TileRawCorrelatedNoise: public AthAlgorithm {

  public:
    // constructor
    TileRawCorrelatedNoise(const std::string& name, ISvcLocator* pSvcLocator);
    // destructor
    virtual ~TileRawCorrelatedNoise();

    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  private:

    SG::ReadHandleKey<TileDigitsContainer> m_inputDigitsContainerKey{this, "TileDigitsContainer", 
                                                                    "TileDigitsCnt", 
                                                                    "Input Tile digits container key"};

    SG::WriteHandleKey<TileDigitsContainer> m_outputDigitsContainerKey{this, "OutputTileDigitsContainer", 
                                                                      "NewDigitsContainer", 
                                                                      "Output Tile digits container key"};

    // RMS threshold
    float m_nRMS_threshold;

    // file names
    std::string m_alphaMatrixFilePrefix;
    std::string m_sample3RMSFilePrefix;
    std::string m_meanFilePrefix;

    // matrices
    float m_alphaMatrix[4][64][48][48];
    float m_meanSamples[4][64][48][7];
    float m_sample3RMS[4][64][48];

    bool m_useMeanFiles;
    bool m_pmtOrder;
};
#endif
