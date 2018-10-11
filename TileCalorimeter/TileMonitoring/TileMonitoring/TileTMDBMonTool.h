
// ********************************************************************
//
// NAME:     TileTMDBMonTool.h
// PACKAGE:
//
// AUTHOR:   Amanda Camacho
//
//
// ********************************************************************

#ifndef TILEMONITORING_TILETMDBMONTOOL_H
#define TILEMONITORING_TILETMDBMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

class TileBeamInfoProvider;
class ITileBadChanTool;
class TileDQstatus;
class TileCondToolNoiseSample;
class TileDigitsCollection;
class TileCondToolTMDB;


#include <vector>
#include <string>

/** @class TileTMDBMonTool
 *  @brief Class for Tile TMDB general performance monitoring
 */

class TileTMDBMonTool : public TileFatherMonTool {
    public:
        TileTMDBMonTool(const std::string & type, const std::string & name, const IInterface* parent);

        ~TileTMDBMonTool();

        virtual StatusCode initialize();
        virtual StatusCode finalize();

        //pure virtual methods
        virtual StatusCode bookHistograms();
        virtual StatusCode fillHistograms();
        virtual StatusCode procHistograms();

    private:
        StatusCode bookTMDBHistograms(const TileDigitsCollection* digitsCollection);
        StatusCode bookTMDBSummaryHistograms(unsigned int ros);
        StatusCode updateSummaryHistograms();

        // Flags
        bool m_doAllPlots;
        // bool m_isPhysics;
        // bool m_isPedestal;

        int m_summaryUpdateFrequency;
        int m_nEventsProcessed;
        ToolHandle<TileCondToolTMDB> m_tileToolTMDB; 

        std::string m_digitsContainerName;
        std::string m_dspContainerName;
        std::string m_energyContainerName;

        std::vector<std::string> m_TMDB_names[5];

        TH1F* m_mPulseDig[5][64][8][7];
        std::vector<TH1F*> m_mPulse[5][64];
        std::vector<TH1F*> m_chanNoiseEn[5][64];
        std::vector<TH1F*> m_calibError[5][64];

        int m_aux_Nevents[5][64][8];
        std::vector<float> m_aux_eTMDB[5][64][8];
        std::vector<float> m_aux_eDsp[5][64][8];
        float m_aux_coef[5][64][8][2];
        TGraph* m_calibPlots[5][64][8];

        TH1F* m_noiseEnergy_summ[5];
        
        TProfile2D* m_peakPos_map[5];
        TProfile2D* m_coefCalib_map[5];
        TProfile2D* m_noiseEnergy_map[5];

};

#endif
