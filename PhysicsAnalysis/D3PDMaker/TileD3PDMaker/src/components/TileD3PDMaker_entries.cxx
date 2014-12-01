// $Id$
/**
 * @file components/TileD3PDMaker_entries.cxx
 * @author Marco van Woerden <mvanwoer@cern.ch> , Archil Durglishvili <archil.durglishvili@cern.ch>
 * @date November 2014
 * @brief List Gaudi components.
 */

// FILLER TOOLS 
#include "../TileCellFillerTool.h"
#include "../TileTrackFillerTool.h"
#include "../TileClusterFillerTool.h"
#include "../TileEventFillerTool.h"
#include "../TileMuonFillerTool.h"
#include "../TileAssocFillerTool.h"

// FILTER ALGORITHMS
#include "../TileTrackFilterAlg.h"
#include "../TileClusterFilterAlg.h"
#include "../TileEopFilterAlg.h"
#include "../TileMuonFilterAlg.h"
#include "../TileCosmicMuonFilterAlg.h"


// TOOLS
#include "../TrackTools.h"
//#include "../TrackParametersIdentificationHelper.h"

// GAUDI FACTORY
#include "GaudiKernel/DeclareFactoryEntries.h"

// NAMESPACE DECLARATIONS
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileCellFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileTrackFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileClusterFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileMuonFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileEventFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TileAssocFillerTool)

DECLARE_ALGORITHM_FACTORY        (TileTrackFilterAlg)
DECLARE_ALGORITHM_FACTORY        (TileClusterFilterAlg) 
DECLARE_ALGORITHM_FACTORY        (TileEopFilterAlg)
DECLARE_ALGORITHM_FACTORY        (TileMuonFilterAlg)
DECLARE_ALGORITHM_FACTORY        (TileCosmicMuonFilterAlg)

DECLARE_TOOL_FACTORY             (TrackTools)
//DECLARE_TOOL_FACTORY             (TrackParametersIdentificationHelper)


// GAUDI FACTORY ENTRIES
DECLARE_FACTORY_ENTRIES(TileD3PDMaker) {
DECLARE_NAMESPACE_TOOL   (D3PD, TileCellFillerTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileTrackFillerTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileClusterFillerTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileMuonFillerTool)
DECLARE_NAMESPACE_TOOL   (D3PD, TileEventFillerTool)

DECLARE_ALGORITHM        (TileTrackFilterAlg)
} // DECLARE_FACTORY_ENTRIES

DECLARE_FACTORY_ENTRIES( TrackTools) {
  DECLARE_TOOL( TrackTools )
} // DECLARE_FACTORY_ENTRIES
