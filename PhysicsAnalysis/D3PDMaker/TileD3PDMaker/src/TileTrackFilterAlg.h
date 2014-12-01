/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTrackFilterAlg.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 *
 * Description: Algorithm to filter TrackParticles in TrackParticleContainer,
 *              used in E/p analysis.
 *
 * Created in May 2013
 */

// IDENTIFICATION
#ifndef TILETRACKFILTERALG_H
#define TILETRACKFILTERALG_H

//// INCLUDE HEADER FILES
// D3PD INCLUDES
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "Identifier/HWIdentifier.h"

/// TRACK TOOL INCLUDES
#include "TrackTools.h"


// MONTE CARLO (TRUTH) INFORMATION
#include "GeneratorObjects/McEventCollection.h"

/// C++ INCLUDES
#include <vector>
#include <algorithm>
#include <math.h>
#include <string>
#include <cmath>

// FORWARD DECLARATIONS
class ITrackTools;


//xAOD tools
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

//Type definitions
typedef xAOD::TrackParticle TRACK;
typedef xAOD::TrackParticleContainer TRACKCONTAINER;
typedef xAOD::TrackParticleAuxContainer TRACKAUXCONTAINER;


//================================================
class TileTrackFilterAlg : public AthAlgorithm{
//================================================

    public:
        // REGULAR GAUDI ALGORITHM CONSTRUCTOR
        TileTrackFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
        ~TileTrackFilterAlg(){};

        virtual StatusCode initialize();             // EXECUTED BEFORE EVENT LOOP 
        virtual StatusCode execute();                // EXECUTED FOR EACH EVENT
        virtual StatusCode finalize();               // EXECUTED AFTER EVENT LOOP
        
        bool accept(const TRACK* p);         // CHECK WHETHER TRACKPARTICLE IS OK

    private:
        StoreGateSvc* m_storeGate;
        TRACKCONTAINER* m_outputCont;

        // CONTAINER NAMES
        std::string m_trackContainerName;            // INPUT CELL CONTAINER
        std::string m_outputTracksName;              // OUTPUT CELL CONTAINER
        
        // CUTS
        float m_cutP;
        float m_cutEta;
        float m_cutSizeOfCone;

        // TOOLHANDLES
        ToolHandle<xAOD::ITrackIsolationTool> m_track_iso_tool;

}; // class TileTrackFilterAlg

#endif // TILED3PDMAKER_TILEMUONTRACKFILTERALG_H
