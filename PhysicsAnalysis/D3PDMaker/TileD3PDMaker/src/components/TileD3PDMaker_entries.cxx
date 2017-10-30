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

DECLARE_COMPONENT( D3PD::TileCellFillerTool )
DECLARE_COMPONENT( D3PD::TileTrackFillerTool )
DECLARE_COMPONENT( D3PD::TileClusterFillerTool )
DECLARE_COMPONENT( D3PD::TileMuonFillerTool )
DECLARE_COMPONENT( D3PD::TileEventFillerTool )
DECLARE_COMPONENT( D3PD::TileAssocFillerTool )

DECLARE_COMPONENT( TileTrackFilterAlg )
DECLARE_COMPONENT( TileClusterFilterAlg ) 
DECLARE_COMPONENT( TileEopFilterAlg )
DECLARE_COMPONENT( TileMuonFilterAlg )
DECLARE_COMPONENT( TileCosmicMuonFilterAlg )

DECLARE_COMPONENT( TrackTools )
//DECLARE_COMPONENT( TrackParametersIdentificationHelper )

