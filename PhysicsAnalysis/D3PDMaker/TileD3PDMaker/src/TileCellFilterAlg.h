/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellFilterAlg.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 *
 * Description: Algorithm to filter CaloCells in CaloCellContainer,
 *              used to study TileCal's calibration with muons.
 *
 * Created in September 2012
 */

// IDENTIFICATION
#ifndef CALOD3PDMAKER_TILEMUONCELLFILTERALG_H
#define CALOD3PDMAKER_TILEMUONCELLFILTERALG_H

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

/// TRACK INCLUDES
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"

/// TRACK TOOL INCLUDES
#include "ITrackTools.h"

/// CALORIMETER INCLUDES
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "TileEvent/TileCell.h"

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
namespace Analysis{
class Muon;
}
namespace Trk{
class Track;
}
class CaloCell;

// TYPE DEFINITIONS
typedef CaloCellContainer CONTAINER; 
typedef std::vector<const CaloCell*> ListOfCells;
typedef std::vector<ListOfCells> ListOfListsOfCells;
typedef Analysis::MuonContainer MUONCONTAINER;
typedef Rec::TrackParticleContainer TRACKCONTAINER;
typedef Rec::TrackParticle TRACKPARTICLE;

//================================================
class TileCellFilterAlg : public AthAlgorithm{
//================================================

    public:
        // REGULAR GAUDI ALGORITHM CONSTRUCTOR
        TileCellFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
        ~TileCellFilterAlg(){};

        virtual StatusCode initialize();             // EXECUTED BEFORE EVENT LOOP 
        virtual StatusCode execute();                // EXECUTED FOR EACH EVENT
        virtual StatusCode finalize();               // EXECUTED AFTER EVENT LOOP

        typedef std::vector<const CaloCell*> ListOfCells;

    private:
        // INCLUDE LIQUID ARGON (yes/no)
        std::string m_includeLAr;
        std::string m_standalone;
        short m_trackType;
        
        // FILTER CONFIGURATION
        int m_filterConfiguration;
        float m_coneSize;

        // TOOLHANDLES
        ToolHandle<ITrackTools> m_inCalo;

        StoreGateSvc* m_storeGate;
        ConstDataVector<CONTAINER>* m_outputCont;
	const TRACKCONTAINER* trackcoll;
        // CONTAINER NAMES
        std::string m_inputContainerName;            // MUON CONTAINER
        std::string m_calocellContainerName;         // INPUT CELL CONTAINER
        std::string m_outputCellsName;               // OUTPUT CELL CONTAINER

}; // class TileCellFilterAlg

#endif // TILED3PDMAKER_TILEMUONCELLFILTERALG_H
