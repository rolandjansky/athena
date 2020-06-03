/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

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

// Macros for TileTMDBMonTool
#define TMDB_ROS               5   // Stands for how many ROS we have in the detector 
                                   // (one auxiliar and the 4 TileCal partitions
#define TMDB_DRAWERS          64   // Stands for the (maximum) number of drawers in each
                                   // partition
#define TMDB_CHANNELS          8   // Stands for the maximum amount of TMDB channels in
                                   // each drawer
#define TMDB_SAMPLES           7   // Number of samples acquired by the TMDB
#define TMDB_CALIB_COEF        2   // Number of calibration coefficients used by the TMDB


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
        
        bool m_hasDsp;

        int m_summaryUpdateFrequency;
        int m_nEventsProcessed;
        ToolHandle<TileCondToolTMDB> m_tileToolTMDB; 

        std::string m_digitsContainerName;
        std::string m_dspContainerName;
        std::string m_energyContainerName;

        std::vector<std::string> m_TMDB_names[TMDB_ROS];

        TH1F* m_mPulseDig[TMDB_ROS][TMDB_DRAWERS][TMDB_CHANNELS][TMDB_SAMPLES];
        std::vector<TH1F*> m_mPulse[TMDB_ROS][TMDB_DRAWERS];
        std::vector<TH1F*> m_chanNoiseEn[TMDB_ROS][TMDB_DRAWERS];
        std::vector<TH1F*> m_calibError[TMDB_ROS][TMDB_DRAWERS];

        int m_aux_Nevents[TMDB_ROS][TMDB_DRAWERS][TMDB_CHANNELS];
        std::vector<float> m_aux_eTMDB[TMDB_ROS][TMDB_DRAWERS][TMDB_CHANNELS];
        std::vector<float> m_aux_eDsp[TMDB_ROS][TMDB_DRAWERS][TMDB_CHANNELS];
        float m_aux_coef[TMDB_ROS][TMDB_DRAWERS][TMDB_CHANNELS][TMDB_CALIB_COEF];
        TGraph* m_calibPlots[TMDB_ROS][TMDB_DRAWERS][TMDB_CHANNELS];

        TH1F* m_noiseEnergy_summ[TMDB_ROS];
        
        TProfile2D* m_peakPos_map[TMDB_ROS];
        TProfile2D* m_coefCalib_map[TMDB_ROS];
        TProfile2D* m_noiseEnergy_map[TMDB_ROS];

};

#endif
