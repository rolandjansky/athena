/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileClusterCellAssociationTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Associate clusters to calorimeter cells, used for E/p studies of TileCal.
 *
 * Created in September 2012
 */

#ifndef TILECLUSTERCELLASSOCIATIONTOOL_H
#define TILECLUSTERCELLASSOCIATIONTOOL_H

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

// CLASS INITIALIATIONS AND NAMESPACES
class CaloCluster;
class CaloCell;

// ADD TILEMUONMUCELLASSOCIATIONTOOL TO D3PD NAMESPACE
namespace D3PD{

//=====================================================================================
class TileClusterCellAssociationTool:public MultiAssociationTool<CaloCluster,CaloCell>{
//=====================================================================================
    public:
        typedef MultiAssociationTool<CaloCluster,CaloCell> Base;

        TileClusterCellAssociationTool(const std::string& type,const std::string& name, const IInterface* parent);
        virtual ~TileClusterCellAssociationTool();

        StatusCode initialize();
        virtual StatusCode reset(const CaloCluster& p);
        virtual const CaloCell* next();

    private:
        std::vector<const CaloCell*> m_calocellContainer;
        std::string m_calocellContainerName;         // INPUT CELL CONTAINER
        CaloCluster::cell_iterator m_cellItr;        // CELL ITERATOR
        CaloCluster::cell_iterator m_cellEnd;        // CELL ITERATOR

        std::vector<std::vector<const CaloCell*> > m_listListCells;

        const CaloCluster* m_cluster;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;
        ToolHandle<ITrackTools> m_inCalo;
        std::string m_standalone;

}; // TILECLUSTERCELLASSOCIATIONTOOL

} // NAMESPACE D3PD

#endif

