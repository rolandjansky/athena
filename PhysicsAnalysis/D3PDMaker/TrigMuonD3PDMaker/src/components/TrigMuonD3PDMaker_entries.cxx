// $Id: TrigMuonD3PDMaker_entries.cxx 633618 2014-12-04 10:12:10Z ssnyder $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Filler tools:
#include "../MuonFeature4MomFillerTool.h"
#include "../MuonFeatureFillerTool.h"
#include "../MuonFeatureDetailsFillerTool.h"
#include "../CombinedMuonFeatureFillerTool.h"
#include "../IsoMuonFeatureFillerTool.h"
#include "../TileMuFeatureFillerTool.h"
#include "../TileTrackMuFeatureFillerTool.h"
#include "../TrigMuonEFInfoFillerTool.h"
#include "../TrigMuonEFInfoTrackFillerTool.h"
#include "../TrigMuonEFTrackFillerTool.h"
#include "../TrigMuonEFCbTrackFillerTool.h"
#include "../TgcCoinDataFillerTool.h"
#include "../RpcCoinDataFillerTool.h"
#include "../RpcPadFillerTool.h"
#include "../TrigMuonEFIsolationFillerTool.h"
#include "../TrigMuonEFIsolationEFInfoTrackIndexFillerTool.h"

// Decision tools:
#include "../MuonFeatureObjectDecisionFillerTool.h"
#include "../CombinedMuonFeatureObjectDecisionFillerTool.h"
#include "../IsoMuonFeatureObjectDecisionFillerTool.h"
#include "../TileMuFeatureObjectDecisionFillerTool.h"
#include "../TileTrackMuFeatureObjectDecisionFillerTool.h"
#include "../TrigMuonEFInfoObjectDecisionFillerTool.h"
#include "../TrigMuonEFInfoObjectSourceFillerTool.h"

// Getter tools:
#include "../MuonFeatureGetterTool.h"
#include "../MuonFeatureDetailsGetterTool.h"
#include "../CombinedMuonFeatureGetterTool.h"
#include "../IsoMuonFeatureGetterTool.h"
#include "../TileMuFeatureGetterTool.h"
#include "../TileTrackMuFeatureGetterTool.h"
#include "../TgcCoinDataGetterTool.h"
#include "../TgcCoinHierarchyGetterTool.h"

// Associator tools:
#include "../CombinedMuonFeatureMuonFeatureAssociationTool.h"
#include "../CombinedMuonFeatureIDTrackAssociationTool.h"
#include "../TileTrackMuFeatureTileMuFeatureAssociationTool.h"
#include "../TileTrackMuFeatureIDTrackAssociationTool.h"
#include "../TrigMuonEFInfoTrackAssociationTool.h"
#include "../TrigMuonEFInfoTrackSpectrometerTrackAssociationTool.h"
#include "../TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool.h"
#include "../TrigMuonEFInfoTrackCombinedTrackAssociationTool.h"
#include "../TgcCoinDataContainerAssociationTool.h"
#include "../RpcCoinDataContainerAssociationTool.h"
#include "../RpcPadContainerAssociationTool.h"
#include "../TgcCoinHierarchyCoinDataAssociationTool.h"
#include "../TrigMuonEFIsolationEFInfoAssocTool.h"
#include "../TrigMuonEFIsolationEFInfoTrackAssocTool.h"


//
// Declare the components of the package:
//

// Filler tools:
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, MuonFeature4MomFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, MuonFeatureFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, MuonFeatureDetailsFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, CombinedMuonFeatureFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, IsoMuonFeatureFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TileMuFeatureFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TileTrackMuFeatureFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFInfoFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFInfoTrackFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFTrackFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFCbTrackFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TgcCoinDataFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, RpcCoinDataFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, RpcPadFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFIsolationFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFIsolationEFInfoTrackIndexFillerTool )

// Decision tools:
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, MuonFeatureObjectDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, CombinedMuonFeatureObjectDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, IsoMuonFeatureObjectDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TileMuFeatureObjectDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TileTrackMuFeatureObjectDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFInfoObjectDecisionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFInfoObjectSourceFillerTool )

// Getter tools:
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, MuonFeatureGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, MuonFeatureDetailsGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, CombinedMuonFeatureGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, IsoMuonFeatureGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TileMuFeatureGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TileTrackMuFeatureGetterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TgcCoinDataGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TgcCoinHierarchyGetterTool)

// Associator tools:
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, CombinedMuonFeatureMuonFeatureAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, CombinedMuonFeatureIDTrackAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TileTrackMuFeatureTileMuFeatureAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TileTrackMuFeatureIDTrackAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFInfoTrackAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFInfoTrackSpectrometerTrackAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFInfoTrackCombinedTrackAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TgcCoinDataContainerAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, RpcCoinDataContainerAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, RpcPadContainerAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TgcCoinHierarchyCoinDataAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFIsolationEFInfoAssocTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigMuonEFIsolationEFInfoTrackAssocTool )

DECLARE_FACTORY_ENTRIES( TrigMuonD3PDMaker ) {

   // Filler tools:
   DECLARE_NAMESPACE_TOOL( D3PD, MuonFeature4MomFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, MuonFeatureFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, MuonFeatureDetailsFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, CombinedMuonFeatureFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, IsoMuonFeatureFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TileMuFeatureFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TileTrackMuFeatureFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFInfoFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFInfoTrackFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFTrackFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFCbTrackFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TgcCoinDataFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, RpcCoinDataFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, RpcPadFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFIsolationFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFIsolationEFInfoTrackIndexFillerTool )

   // Decision tools:
   DECLARE_NAMESPACE_TOOL( D3PD, MuonFeatureObjectDecisionFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, CombinedMuonFeatureObjectDecisionFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, IsoMuonFeatureObjectDecisionFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TileMuFeatureObjectDecisionFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TileTrackMuFeatureObjectDecisionFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFInfoObjectDecisionFillerTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFInfoObjectSourceFillerTool )

   // Getter tools:
   DECLARE_NAMESPACE_TOOL( D3PD, MuonFeatureGetterTool )
   DECLARE_NAMESPACE_TOOL( D3PD, MuonFeatureDetailsGetterTool )
   DECLARE_NAMESPACE_TOOL( D3PD, CombinedMuonFeatureGetterTool )
   DECLARE_NAMESPACE_TOOL( D3PD, IsoMuonFeatureGetterTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TileMuFeatureGetterTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TileTrackMuFeatureGetterTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TgcCoinDataGetterTool)
   DECLARE_NAMESPACE_TOOL( D3PD, TgcCoinHierarchyGetterTool)

   // Associator tools:
   DECLARE_NAMESPACE_TOOL( D3PD, CombinedMuonFeatureMuonFeatureAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, CombinedMuonFeatureIDTrackAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TileTrackMuFeatureTileMuFeatureAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TileTrackMuFeatureIDTrackAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFInfoTrackAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFInfoTrackSpectrometerTrackAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFInfoTrackCombinedTrackAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TgcCoinDataContainerAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, RpcCoinDataContainerAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, RpcPadContainerAssociationTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TgcCoinHierarchyCoinDataAssociationTool)
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFIsolationEFInfoAssocTool )
   DECLARE_NAMESPACE_TOOL( D3PD, TrigMuonEFIsolationEFInfoTrackAssocTool )

}
