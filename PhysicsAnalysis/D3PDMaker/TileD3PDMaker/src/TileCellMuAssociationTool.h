/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellMuAssociationTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Associate muons to calocells, used in calibration studies of TileCal using muons.
 *
 * Created in September 2012
 */

#ifndef TILEMUONCELLMUASSOCIATIONTOOL_H
#define TILEMUONCELLMUASSOCIATIONTOOL_H

/// D3PD INCLUDES
#include "D3PDMakerUtils/MultiAssociationTool.h"

/// GENERIC INCLUDES
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

/// PHYSICS INCLUDES
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

/// TRACK INCLUDES
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"

/// CALORIMETER INCLUDES
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "TileEvent/TileCell.h"

/// C++ INCLUDES
#include <vector>
#include <string>

/// TRACK TOOL INCLUDES
#include "ITrackTools.h"

/// C++ INCLUDES
#include <vector>
#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include <string>

// INITIALIZING CLASSES
namespace Analysis{
class Muon;
class MuonContainer;
}
class CaloCell;
namespace Rec{
class TrackParticle;
}
namespace Trk{
class Track;
}

// NAMESPACES REQUIRED
// ADD TILEMUONCELLMUASSOCIATIONTOOL TO D3PD NAMESPACE
namespace D3PD{

//=======================================================================================
class TileCellMuAssociationTool:public MultiAssociationTool<CaloCell,Analysis::Muon>{
//=======================================================================================
    public:
        typedef MultiAssociationTool<CaloCell,Analysis::Muon> Base;

        TileCellMuAssociationTool(const std::string& type,const std::string& name, const IInterface* parent);
        virtual ~TileCellMuAssociationTool();

        StatusCode initialize();
        virtual StatusCode reset(const CaloCell& p);
        virtual const Analysis::Muon* next();

    private:
        const CaloCell* m_cell;
        const Analysis::Muon* m_muon;

        std::vector<const Analysis::Muon*> m_allMuons;
        std::vector<const Trk::Track*> m_allTracks;
        int m_muonIndex;
        short m_trackType;

        const Analysis::MuonContainer* m_muons;

        // CONTAINER NAMES
        std::string m_muonContainerName;             // MUON CONTAINER
        StoreGateSvc* m_storeGate;
        ToolHandle<ITrackTools> m_inCalo;
        std::string m_standalone;

}; // TILEMUONCELLMUASSOCIATIONTOOL:MULTIASSOCIATIONTOOL

} // NAMESPACE D3PD

#endif  // TILEMUONCELLMUASSOCIATIONTOOL_H