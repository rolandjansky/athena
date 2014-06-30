/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTrackCellAssociationTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Associate calocells to muons, used for calibration sudies of TileCal using muons.
 *
 * Created in September 2012
 */

#ifndef TILETRACKCELLASSOCIATIONTOOL_H
#define TILETRACKCELLASSOCIATIONTOOL_H

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

/// TRACK TOOL INCLUDES
#include "ITrackTools.h"

/// C++ INCLUDES
#include <vector>
#include <string>

/// TRACK TOOL INCLUDES
#include "ITrackTools.h"

// CLASS INITIALIATIONS AND NAMESPACES
class CaloCluster;
class CaloCell;


//typedef CaloCellContainer CONTAINER;

// ADD TILEMUONMUCELLASSOCIATIONTOOL TO D3PD NAMESPACE
namespace D3PD{

//=======================================================================================
class TileTrackCellAssociationTool:public MultiAssociationTool<Rec::TrackParticle,CaloCell>{
//=======================================================================================
    public:
        typedef MultiAssociationTool<Rec::TrackParticle,CaloCell> Base;

        TileTrackCellAssociationTool(const std::string& type,const std::string& name, const IInterface* parent);
        virtual ~TileTrackCellAssociationTool();

        StatusCode initialize();
        virtual StatusCode reset(const Rec::TrackParticle& p);
        virtual const CaloCell* next();
	typedef std::vector<const CaloCell*> ListOfCells;
	typedef std::vector<ListOfCells> ListOfListsOfCells;

    private:
        std::vector<const CaloCell*> m_calocellContainer;
        std::string m_calocellContainerName;         // INPUT CELL CONTAINER
        std::vector<const CaloCell*>::const_iterator m_cellItr;
        std::vector<const CaloCell*>::const_iterator m_cellEnd;
        std::vector<std::vector<const CaloCell*> > m_listListCells;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;
        ToolHandle<ITrackTools> m_inCalo;
        std::string m_standalone;

}; // TILEMUONMUCELLASSOCIATIONTOOL

} // NAMESPACE D3PD

#endif	/* TILEMUONMUCELLASSOCIATIONTOOL_H */


