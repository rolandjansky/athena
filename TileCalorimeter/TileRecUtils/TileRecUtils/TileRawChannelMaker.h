/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELMAKER_H
#define TILERECUTILS_TILERAWCHANNELMAKER_H

/********************************************************************
 *
 * NAME:     TileRawChanelMaker 
 * PACKAGE:  offline/TileCalorimeter/TileRecalgs
 *
 * AUTHOR :  K. Gellerstedt
 * CREATED:  Nov 17 2003
 *
 * PURPOSE:  Creation of TileRawChannels using different sub algorithms
 *
 *  Input:  TileDigits container
 *  Output: Containers with TileRawChannels
 *  Parameters:
 *    TileRawChannelContainer - Name of output containers
 *    TileRawChannelBuilder - Name of builder sub-algorithms
 *    TileDigitsContainer - Name of input container
 *   
 ********************************************************************/

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"

class TileRawChannelBuilder;

class TileRawChannelMaker: public AthAlgorithm {

  public:
    // constructor
    TileRawChannelMaker(const std::string& name, ISvcLocator* pSvcLocator);
    // destructor
    virtual ~TileRawChannelMaker();

    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  private:

    void fitOverflowedChannels(void);

    // name of TDS container with TileDigits
    std::string m_TileDigitsContainerID;

    /**
     * Vector with class name[/instance name] of builder sub-algs
     */
    std::vector<std::string> m_TileRawChannelBuilderIDVec;

    /**
     * Vector of builder algtools
     */
    //  std::vector<TileRawChannelBuilder*> m_TileRawChannelBuilderVec;
    ToolHandleArray<TileRawChannelBuilder> m_tileRawChannelBuilderList;

    bool m_fitOverflow;
    ToolHandle<TileRawChannelBuilder> m_tileRawChannelBuilderFitOverflow;

    float m_overflowReplaceTimeCut;
    float m_overflowReplacePedestalCut;
    float m_overflowReplaceChi2Cut;
};
#endif
