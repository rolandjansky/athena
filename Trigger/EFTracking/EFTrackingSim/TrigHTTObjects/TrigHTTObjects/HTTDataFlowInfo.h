// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef TRIGHTTOBJECTS_HTTDATAFLOWINFO_H
#define TRIGHTTOBJECTS_HTTDATAFLOWINFO_H

/**
 * @file HTTDataFlowInfo.h
 * @author Yumeng Cao - yumeng.cao@cern.ch
 * @date July 17th, 2021
 * @brief Structs that store the data flow information per event
 */

 //#include "TrigHTTUtils/HTTTypes.h"
#include <iostream>
#include <vector>

struct HTTDataFlowInfo
{
    std::vector<size_t> nMappedHits_1st_layer; // Number of 1st stage mapped hits for each layer. Size = Number of 1st stage layers
    size_t nMappedHits_1st_total;              // Number of 1st stage mapped hits for all layers

    std::vector<size_t> nClusters_1st_layer; // Number of 1st stage clusters for each layer. Size = Number of 1st stage layers
    size_t nClusters_1st_total;              // Number of 1st stage clusters for all layers

    size_t nRoads_1st_total; // Number of 1st stage roads in total
    size_t nRoads_1st_7hits; // Number of 1st stage roads with 7 hits
    size_t nRoads_1st_8hits; // Number of 1st stage roads with 8 hits

    size_t nTracks_1st_total;    // Number of 1st stage tracks in total
    size_t nTracks_1st_passChi2; // Number of 1st stage tracks passing Chi2 cut
    size_t nTracks_1st_afterOR;  // Number of 1st stage tracks after overlap removal

    std::vector<size_t> nTracks_1st_per_patt_total;    // Number of 1st stage tracks per pattern in total. Size = Number of patterns
    std::vector<size_t> nTracks_1st_per_patt_passChi2; // Number of 1st stage tracks per pattern passing Chi2 cut. Size = Number of patterns
    std::vector<size_t> nTracks_1st_per_patt_afterOR;  // Number of 1st stage tracks per pattern after overlap removal. Size = Number of patterns

    size_t nConstants_1st_fitter;   // Number of 1st stage constants used in the fitter stage
    size_t nConstants_1st_passChi2; // Number of 1st stage constants used passing Chi2 cut
    size_t nConstants_1st_afterOR;  // Number of 1st stage constants used after overlap removal

    size_t nRoads_2nd_total; // Number of 2nd stage tracks in total

    size_t nTracks_2nd_total;    // Number of 2nd stage tracks in total
    size_t nTracks_2nd_passChi2; // Number of 2nd stage tracks passing Chi2 cut
    size_t nTracks_2nd_afterOR;  // Number of 2nd stage tracks after overlap removal

    size_t nConstants_2nd_extrapolate; // Number of 2nd stage constants used in the extrapolator stage
    size_t nConstants_2nd_fitter;      // Number of 2nd stage constants used in the fitter stage
    size_t nConstants_2nd_passChi2;    // Number of 2nd stage constants used passing Chi2 cut
    size_t nConstants_2nd_afterOR;     // Number of 2nd stage constants used after overlap removal

    size_t nTruthTracks;   // Number of truth tracks
    size_t nOfflineTracks; // Number of offline reconstructed tracks

    float nTracks_1st_over_nOfflineTracks; // Ratio between number of 1st stage tracks and number of offline reconstructed tracks
    float nTracks_2nd_over_nOfflineTracks; // Ratio between number of 2nd stage tracks and number of offline reconstructed tracks

    HTTDataFlowInfo() { reset(); }

    void reset();

    friend std::ostream& operator<<(std::ostream& s, HTTDataFlowInfo const& info);
};

#endif // HTTDATAFLOWINFO_H
