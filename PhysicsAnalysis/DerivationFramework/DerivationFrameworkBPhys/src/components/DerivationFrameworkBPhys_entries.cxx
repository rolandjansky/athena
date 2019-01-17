#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkBPhys/Reco_mumu.h"
#include "DerivationFrameworkBPhys/Reco_4mu.h"
#include "DerivationFrameworkBPhys/Reco_dimuTrk.h"
#include "DerivationFrameworkBPhys/Reco_dimuTrkTrk.h"
#include "DerivationFrameworkBPhys/Select_onia2mumu.h"
#include "DerivationFrameworkBPhys/SelectEvent.h"
#include "DerivationFrameworkBPhys/Thin_vtxTrk.h"
#include "DerivationFrameworkBPhys/Thin_vtxDuplicates.h"
#include "DerivationFrameworkBPhys/AugOriginalCounts.h"
#include "DerivationFrameworkBPhys/BPhysPVThinningTool.h"
#include "DerivationFrameworkBPhys/VertexCaloIsolation.h"
#include "DerivationFrameworkBPhys/VertexTrackIsolation.h"
#include "DerivationFrameworkBPhys/BPhysMetadataBase.h"
#include "DerivationFrameworkBPhys/Bmumu_metadata.h"
#include "DerivationFrameworkBPhys/CfAthAlgTool.h"
#include "DerivationFrameworkBPhys/Bmumu_reco_mumu.h"
#include "DerivationFrameworkBPhys/FourMuonTool.h"
#include "DerivationFrameworkBPhys/BPhysAddMuonBasedInvMass.h"
#include "DerivationFrameworkBPhys/BPhysVertexTrackBase.h"
#include "DerivationFrameworkBPhys/BVertexTrackIsoTool.h"
#include "DerivationFrameworkBPhys/BMuonTrackIsoTool.h"
#include "DerivationFrameworkBPhys/BVertexClosestTrackTool.h"
#include "DerivationFrameworkBPhys/BTrackVertexMapLogger.h"
#include "DerivationFrameworkBPhys/Select_Bmumu.h"
#include "DerivationFrameworkBPhys/BPhysVarBlinder.h"
#include "DerivationFrameworkBPhys/BmumuThinningTool.h"
#include "DerivationFrameworkBPhys/VertexPlus1TrackCascade.h"
#include "DerivationFrameworkBPhys/TriggerCountToMetadata.h"
#include "DerivationFrameworkBPhys/MuonExtrapolationTool.h"
#include "DerivationFrameworkBPhys/CascadeTools.h"
#include "DerivationFrameworkBPhys/Reco_V0Finder.h"
#include "DerivationFrameworkBPhys/JpsiPlusV0Cascade.h"
#include "DerivationFrameworkBPhys/JpsiPlusDsCascade.h"
#include "DerivationFrameworkBPhys/JpsiPlusDpstCascade.h"
#include "DerivationFrameworkBPhys/JpsiPlusDs1Cascade.h"
#include "DerivationFrameworkBPhys/ReVertex.h"
#include "DerivationFrameworkBPhys/BPhysConversionFinder.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( Reco_mumu )
DECLARE_TOOL_FACTORY( Reco_4mu )
DECLARE_TOOL_FACTORY( Reco_dimuTrk )
DECLARE_TOOL_FACTORY( Reco_dimuTrkTrk )
DECLARE_TOOL_FACTORY( Select_onia2mumu )
DECLARE_TOOL_FACTORY( SelectEvent )
DECLARE_TOOL_FACTORY( Thin_vtxTrk )
DECLARE_TOOL_FACTORY( Thin_vtxDuplicates )
DECLARE_TOOL_FACTORY( AugOriginalCounts )
DECLARE_TOOL_FACTORY( BPhysPVThinningTool )
DECLARE_TOOL_FACTORY( VertexCaloIsolation )
DECLARE_TOOL_FACTORY( VertexTrackIsolation )
DECLARE_TOOL_FACTORY( BPhysMetadataBase )
DECLARE_TOOL_FACTORY( Bmumu_metadata )
DECLARE_TOOL_FACTORY( CfAthAlgTool )
DECLARE_TOOL_FACTORY( Bmumu_reco_mumu )
DECLARE_TOOL_FACTORY( FourMuonTool )
DECLARE_TOOL_FACTORY( BPhysAddMuonBasedInvMass )
DECLARE_TOOL_FACTORY( BPhysVertexTrackBase )
DECLARE_TOOL_FACTORY( BVertexTrackIsoTool )
DECLARE_TOOL_FACTORY( BMuonTrackIsoTool )
DECLARE_TOOL_FACTORY( BVertexClosestTrackTool )
DECLARE_TOOL_FACTORY( BTrackVertexMapLogger )
DECLARE_TOOL_FACTORY( Select_Bmumu )
DECLARE_TOOL_FACTORY( BPhysVarBlinder )
DECLARE_TOOL_FACTORY( BmumuThinningTool )
DECLARE_TOOL_FACTORY( VertexPlus1TrackCascade )
DECLARE_TOOL_FACTORY( TriggerCountToMetadata )
DECLARE_TOOL_FACTORY( MuonExtrapolationTool )
DECLARE_TOOL_FACTORY( CascadeTools )
DECLARE_TOOL_FACTORY( Reco_V0Finder )
DECLARE_TOOL_FACTORY( JpsiPlusV0Cascade )
DECLARE_TOOL_FACTORY( JpsiPlusDsCascade )
DECLARE_TOOL_FACTORY( JpsiPlusDpstCascade )
DECLARE_TOOL_FACTORY( JpsiPlusDs1Cascade )
DECLARE_TOOL_FACTORY( ReVertex )
DECLARE_TOOL_FACTORY( BPhysConversionFinder )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkBPhys ) {
   DECLARE_TOOL( Reco_mumu )
   DECLARE_TOOL( Reco_4mu )
   DECLARE_TOOL( Reco_dimuTrk )
   DECLARE_TOOL( Reco_dimuTrkTrk )
   DECLARE_TOOL( Select_onia2mumu )
   DECLARE_TOOL( SelectEvent )
   DECLARE_TOOL( Thin_vtxTrk )
   DECLARE_TOOL( Thin_vtxDuplicates )
   DECLARE_TOOL( AugOriginalCounts )
   DECLARE_TOOL( BPhysPVThinningTool )
   DECLARE_TOOL( VertexCaloIsolation )
   DECLARE_TOOL( VertexTrackIsolation )
   DECLARE_TOOL( BPhysMetaDataBase)
   DECLARE_TOOL( Bmumu_metadata )
   DECLARE_TOOL( CfAthAlgTool )
   DECLARE_TOOL( Bmumu_reco_mumu )
   DECLARE_TOOL( FourMuonTool )
   DECLARE_TOOL( BPhysAddMuonBasedInvMass )
   DECLARE_TOOL( BPhysVertexTrackBase )
   DECLARE_TOOL( BVertexTrackIsoTool )
   DECLARE_TOOL( BMuonTrackIsoTool )
   DECLARE_TOOL( BVertexClosestTrackTool )
   DECLARE_TOOL( BTrackVertexMapLogger )
   DECLARE_TOOL( Select_Bmumu )
   DECLARE_TOOL( BPhysVarBlinder )
   DECLARE_TOOL( BmumuThinningTool )
   DECLARE_TOOL( VertexPlus1TrackCascade )
   DECLARE_TOOL( TriggerCountToMetadata )
   DECLARE_TOOL( MuonExtrapolationTool )
   DECLARE_TOOL( CascadeTools )
   DECLARE_TOOL( Reco_V0Finder )
   DECLARE_TOOL( JpsiPlusV0Cascade )
   DECLARE_TOOL( JpsiPlusDsCascade )
   DECLARE_TOOL( JpsiPlusDpstCascade )
   DECLARE_TOOL( JpsiPlusDs1Cascade )
   DECLARE_TOOL( ReVertex )
   DECLARE_TOOL( BPhysConversionFinder )
}
