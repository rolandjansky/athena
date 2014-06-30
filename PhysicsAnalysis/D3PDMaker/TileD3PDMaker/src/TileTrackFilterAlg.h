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

/// GENERIC INCLUDES
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

/// PHYSICS INCLUDES
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
//#include "TrackIsolationTools/TrackIsolationTool.h"
#include "IsolationTool/ITrackIsolationTool.h"

/// TRACK INCLUDES
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "ITrackToVertex/ITrackToVertex.h"

/// TRACK TOOL INCLUDES
#include "ITrackTools.h"
#include "TrackTools.h"
#include "ITrackInCaloTools/ITrackExtrapolatorToCalo.h"
#include "TrackToVertex/TrackToVertex.h"


/// CALORIMETER INCLUDES
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "TileEvent/TileCell.h"
#include "CaloEvent/CaloCell.h"

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

namespace Rec{
class TrackParticle;
}
namespace Trk{
class Track;
}
//class TrackParticle;

// TYPE DEFINITIONS
typedef Rec::TrackParticleContainer TRACKCONTAINER;
typedef Rec::TrackParticle TRACKPARTICLE;

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
        
        bool accept(const TRACKPARTICLE* p);         // CHECK WHETHER TRACKPARTICLE IS OK

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
