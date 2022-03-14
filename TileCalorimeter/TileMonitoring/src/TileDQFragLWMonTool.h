/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDQFragLWMonTool.h
// PACKAGE:  
//
// AUTHOR:   Natanael Nunes de Moura Junior
//
//
// ********************************************************************
#ifndef TILEMONITORING_TILEDQFRAGLWMONTOOL_H
#define TILEMONITORING_TILEDQFRAGLWMONTOOL_H

#include "TileFatherMonTool.h"
#include "TileEvent/TileDQstatus.h"
#include "TileConditions/ITileDCSTool.h"
#include "StoreGate/ReadHandleKey.h"

#include <array>

class ITileBadChanTool;
class TileDQstatus;
class TileRawChannel;
class TileRawChannelCollection;
class TileInfo;


/** @class TileDQFragLWMonTool
 *  @brief Class for TileCal monitoring of Data Quality fragment
 */

class TileDQFragLWMonTool: public TileFatherMonTool {

  public:

    TileDQFragLWMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    virtual ~TileDQFragLWMonTool();

    virtual StatusCode initialize() override;

    //pure virtual methods

    /*---------------------------------------------------------*/
    /* njunior@cern.ch */
    virtual StatusCode bookHistograms() override;
    virtual StatusCode fillHistograms() override;
    virtual StatusCode procHistograms() override;

    /*---------------------------------------------------------*/

  private:

    void bookFirstEventHistograms();
    void bookErrorsHistograms(unsigned int ros, unsigned int drawer);
    void fillErrorsHistograms(unsigned int ros, unsigned int drawer);
    void fillGlobalHistograms(unsigned int ros, unsigned int drawer);

    void fillMasking();
    void fillOneError(unsigned int ros, unsigned int drawer, int dmu, int error);
    bool checkHasErrors(unsigned int ros, unsigned int drawer, int dmu);
    bool isModuleDCSgood(unsigned int partition, unsigned int drawer) const;
    int findDrawerErrors(const TileRawChannelCollection *coll);
    int findChannelErrors(const TileRawChannel *rch, int & gain);
    void fillBadDrawer(void);

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool
    ToolHandle<ITileDCSTool> m_tileDCS{this, "TileDCSTool", "TileDCSTool", "Tile DCS tool"};

    /*---------------------------------------------------------*/
    /* properties*/
    std::string m_rawChannelContainerName;
    std::string m_digitsContainerName;

    float m_negAmpHG{};
    float m_negAmpLG{};
    float m_jumpDeltaHG{};
    float m_jumpDeltaLG{};
    float m_pedDeltaHG{};
    float m_pedDeltaLG{};

    bool m_skipMasked{};
    bool m_skipGapCells{};
    bool m_doOnline{};
    bool m_checkDCS{};  //!< if false, do not use TileDCS at all

    /* njunior@cern.ch */
    const TileDQstatus* m_dqStatus{};

    int m_globalErrCount[4][64]; //global CRC errors

    /* njunior@cern.ch */

    TH2I_LW* m_global[2];
    TH2I_LW* m_mismatchedL1TriggerType{};

    TH2I_LW* m_badChannelJump2D[4];
    TH2I_LW* m_badChannelNeg2D[4];

    TH2I_LW* m_badChannelJump2DNotMasked[4];
    TH2I_LW* m_badChannelNeg2DNotMasked[4];

        /* tibor.zenis@cern.ch */
    TH1I_LW* m_eventinfoError_LB{};
    TH2I_LW* m_nConsecBad_LB{};
    TH1I_LW* m_nConsecBad{};

    TH2I_LW* m_errors[4][64];
    TProfile_LW* m_errorsLB[4][64];

    std::array<TH2I_LW*, 4> m_badPulseQuality;
    TH2I_LW* m_noAllDigitsInDrawer{};

    std::vector<std::string> m_errorsLabels;
    std::vector<std::string> m_partitionsLabels;
    std::vector<std::string> m_moduleLabel[NumPart]; // array of module names
    std::vector<std::string> m_cellchLabel[NumPart]; // array of cell-channels names

    bool m_isFirstEvent{};

    static const int NERROR = 17;
    static const int MASKEDERROR = NERROR - 3;
    static const int DCSERROR = NERROR - 2;
    static const int HVERROR = NERROR - 1;
    static const int NCORRUPTED = 13;
    static const int NDMU = 16;

    int m_nLumiblocks{};
    float m_qualityCut{};
    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey;
    unsigned int m_nEventsWithAllDigits{};

    // TileInfo
    std::string m_infoName;
    const TileInfo* m_tileInfo{};
    int m_i_ADCmax{};
    float m_ADCmaxMinusEps{};
    float m_ADCmaskValueMinusEps{};
    /*---------------------------------------------------------*/

};

#endif // TILEMONITORING_TILEDQFRAGLWMONTOOL_H
