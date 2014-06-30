/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileMuCellAssociationTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Associate calocells to muons, used for calibration sudies of TileCal using muons.
 *
 * Created in September 2012
 */

#ifndef TILEMUONMUCELLASSOCIATIONTOOL_H
#define	TILEMUONMUCELLASSOCIATIONTOOL_H

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
namespace Analysis{
class Muon;
}
class CaloCell;

// ADD TILEMUONMUCELLASSOCIATIONTOOL TO D3PD NAMESPACE
namespace D3PD{

//=======================================================================================
class TileMuCellAssociationTool:public MultiAssociationTool<Analysis::Muon,CaloCell>{
//=======================================================================================
    public:
        typedef MultiAssociationTool<Analysis::Muon,CaloCell> Base;

        TileMuCellAssociationTool(const std::string& type,const std::string& name, const IInterface* parent);
        virtual ~TileMuCellAssociationTool();

        StatusCode initialize();
        virtual StatusCode reset(const Analysis::Muon& p);
        virtual const CaloCell* next();

    private:
        std::vector<const CaloCell*> m_calocellContainer;
        std::string m_calocellContainerName;         // INPUT CELL CONTAINER
        std::vector<const CaloCell*>::const_iterator m_cellItr;
        std::vector<const CaloCell*>::const_iterator m_cellEnd;

        std::vector<std::vector<const CaloCell*> > m_listListCells;

        const Analysis::Muon* m_muon;
        const Trk::Track* m_muon_CB_trk;
        short m_trackType;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;
        ToolHandle<ITrackTools> m_inCalo;
        std::string m_standalone;

}; // TILEMUONMUCELLASSOCIATIONTOOL

} // NAMESPACE D3PD

#endif	/* TILEMUONMUCELLASSOCIATIONTOOL_H */

