// $Id$
/**
 * @file components/TileD3PDMaker_entries.cxx
 * @author Marco van Woerden <mvanwoer@cern.ch>
 * @date September 2012
 * @brief List Gaudi components.
 */

// FILLER TOOLS 
#include "../TileMBTSFillerTool.h"
#include "../TileCellFillerTool.h"
#include "../TileTrackFillerTool.h"
#include "../TileCaloClusterFillerTool.h"
//#include "../TileMuonFillerTool.h"
#include "../TileEventFillerTool.h"

// ASSOCIATION TOOLS
//#include "../TileMuCellAssociationTool.h"
//#include "../TileCellMuAssociationTool.h"
#include "../TileClusterCellAssociationTool.h"
#include "../TileTrackCellAssociationTool.h"
#include "../TileTrackClusterAssociationTool.h"

// ALGORITHMS
#include "../TileCellFilterAlg.h"
#include "../TileTrackFilterAlg.h"

// TOOLS
#include "../TrackTools.h"
#include "../Extrapolator.h"
// #include "../FindVolAction.h"
// #include "../FindScintillatorAction.h"

// GAUDI FACTORY
#include "GaudiKernel/DeclareFactoryEntries.h"

// NAMESPACE DECLARATIONS
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileMBTSFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileCellFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileTrackFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileCaloClusterFillerTool)
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileMuonFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileEventFillerTool)

//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileMuCellAssociationTool)
//DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileCellMuAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileClusterCellAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileTrackCellAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileTrackClusterAssociationTool)

DECLARE_ALGORITHM_FACTORY        (TileCellFilterAlg)
DECLARE_ALGORITHM_FACTORY        (TileTrackFilterAlg)

DECLARE_TOOL_FACTORY             (TrackTools)
DECLARE_TOOL_FACTORY             (Extrapolator)
// DECLARE_TOOL_FACTORY             (FindScintillatorAction)


// GAUDI FACTORY ENTRIES
DECLARE_FACTORY_ENTRIES(TileD3PDMaker) {
DECLARE_NAMESPACE_TOOL   (D3PD, TileMBTSFillerTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileCellFillerTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileTrackFillerTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileCaloClusterFillerTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileMuonFillerTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileEventFillerTool)

DECLARE_NAMESPACE_TOOL   (D3PD, TileMuCellAssociationTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileCellMuAssociationTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileClusterCellAssociationTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileTrackCellAssociationTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileTrackClusterAssociationTool)

DECLARE_ALGORITHM        (TileCellFilterAlg)
DECLARE_ALGORITHM        (TileTrackFilterAlg)
} // DECLARE_FACTORY_ENTRIES

DECLARE_FACTORY_ENTRIES( TrackTools) {
  DECLARE_TOOL( TrackTools )
} // DECLARE_FACTORY_ENTRIES

DECLARE_FACTORY_ENTRIES( Extrapolator ) {
  DECLARE_TOOL( Extrapolator )
} // DECLARE_FACTORY_ENTRIES
/*
DECLARE_FACTORY_ENTRIES( FindScintillatorAction ) {
  DECLARE_TOOL( FindScintillatorAction )
} // DECLARE_FACTORY_ENTRIES*/
