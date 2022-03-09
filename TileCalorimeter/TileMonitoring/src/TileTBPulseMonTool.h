/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILETBPULSEMONTOOL_H
#define TILEMONITORING_TILETBPULSEMONTOOL_H

#include "TileFatherMonTool.h"

class ITileBadChanTool;

#include <vector>
#include <string>

/** @class
 *  @brief Class for TileCal pulse shape monitoring
 */

class TileTBPulseMonTool : public TileFatherMonTool {

  public:

    TileTBPulseMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileTBPulseMonTool();

    virtual StatusCode initialize();

    //pure virtual methods
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();

  private:

    virtual StatusCode bookHistogramsPerModule(int ros, int drawer);

    std::string m_digitsContainerName;
    std::string m_rawChannelContainerName;

    ToolHandle<ITileBadChanTool> m_tileBadChanTool;

    TH2F* m_pulseHist2[5][64][48][2];
    TProfile* m_pulseProfile[5][64][48][2];

    void initFirstEvent(void);

    bool m_isFirstEvent;

    int m_useDemoCabling;
};

#endif
