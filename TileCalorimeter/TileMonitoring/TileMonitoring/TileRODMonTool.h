/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileRODMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHORS:  Michael Miller (Michael.Earl.Miller@cern.ch)
//	     Luca Fiorini   (Luca.Fiorini@cern.ch)
// ********************************************************************

#ifndef TILERODMONTOOL_H
#define TILERODMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

#include <vector>
#include <string>

class TileCondToolEmscale;
class ITileBadChanTool;
class TileBeamInfoProvider;

/** @class TileRODMonTool
 *  @brief Class for TileROD based monitoring
 */

class TileRODMonTool: public TileFatherMonTool {

  public:

    TileRODMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileRODMonTool();

    StatusCode initialize();

    //pure virtual methods
    StatusCode bookHistograms();
    StatusCode fillHistograms();
    StatusCode procHistograms();
    StatusCode checkHists(bool fromFinalize);

    StatusCode bookHistTrig(int trig);

    void cleanHistVec();

  private:

    int m_tileRODTrig; //trigger index
    //int  m_runType;
    int m_compUnit;
    bool m_useFitRef;
    bool m_useOFRef;
    bool m_OFI;
    bool m_OFNI;
    bool m_corrtime;
    bool m_isOnline;

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;
    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;

    std::string m_contName;
    std::string m_contNameDSP;
    std::string m_contNameOF;
    std::string m_contNameOFNI;
    std::string dspName;

    std::vector<TH2F *> m_TileAvgDspRefEn[4];        // 4 partitions
    std::vector<TH2F *> m_TileAvgDspRefEnPhase[4];        // 4 partitions
    std::vector<TH1F *> m_TileDspRefEn[4][8][4];     // 4 partitions - 8 RODs - 4 DSPs
    std::vector<TProfile *> m_TileAvgDspRefTim[4];       // 4 partitions
    std::vector<TProfile *> m_TileAvgDspRefTimPhase[4];       // 4 partitions
    std::vector<TH1F *> m_TileDspRefTim[4][8][4];    // 4 partitions - 8 RODs - 4 DSPs
    std::vector<TProfile *> m_TileDspRefSummary[2];

    std::vector<TProfile2D*> m_TdspProfile[4]; //4 partitions

    double m_evEref[4][64][48];    //partitions - 64 modules - 48 channels
    double m_evTref[4][64][48];    //partitions - 64 modules - 48 channels

    // Borrowed from TileRawChannelMonTool
    int m_chMap_LB[48];
    int m_chMap_EB[48];
    int m_chMap_EBsp[48];

    // Tells if a PMT is disconnected or not
    // Special modules are considered too.
    // NB Input is Chan number (0-47)
    // BEWARE: ugly code below this line!
    inline bool isDisconnected(int ros, int drawer, int ch) {

      if (ros < 3) { //LB, all standard. Chan 30,31,43 are disconnected
        if (m_chMap_LB[ch]) return false; //connected
        else return true;
      } else {

        if (((ros == 3) && (drawer == 14))
            || ((ros == 4) && (drawer == 17))) {//EB, EBA15 and EBC18 are special

          if (m_chMap_EBsp[ch]) return false; //connected
          else return true;

        } else {//EB standard module

          if (m_chMap_EB[ch]) return false; //connected
          else return true;
        }

      } //end if LB else EB

    }

};
#endif

