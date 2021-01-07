#include "DerivationFrameworkBPhys/Reco_Vertex.h"
#include "DerivationFrameworkBPhys/Reco_4mu.h"
#include "DerivationFrameworkBPhys/Select_onia2mumu.h"
#include "DerivationFrameworkBPhys/Thin_vtxTrk.h"
#include "DerivationFrameworkBPhys/Thin_vtxDuplicates.h"
#include "DerivationFrameworkBPhys/AugOriginalCounts.h"
#include "DerivationFrameworkBPhys/BPhysPVThinningTool.h"
#include "DerivationFrameworkBPhys/VertexCaloIsolation.h"
#include "DerivationFrameworkBPhys/VertexTrackIsolation.h"
#include "DerivationFrameworkBPhys/BPhysMetadataBase.h"
#include "DerivationFrameworkBPhys/Bmumu_metadata.h"
//#include "DerivationFrameworkBPhys/CfAthAlgTool.h"
#include "DerivationFrameworkBPhys/Bmumu_reco_mumu.h"
#include "DerivationFrameworkBPhys/FourMuonTool.h"
//#include "DerivationFrameworkBPhys/BPhysAddMuonBasedInvMass.h"
//#include "DerivationFrameworkBPhys/BPhysVertexTrackBase.h"
//#include "DerivationFrameworkBPhys/BVertexTrackIsoTool.h"
//#include "DerivationFrameworkBPhys/BMuonTrackIsoTool.h"
//#include "DerivationFrameworkBPhys/BVertexClosestTrackTool.h"
#include "DerivationFrameworkBPhys/BTrackVertexMapLogger.h"
//#include "DerivationFrameworkBPhys/Select_Bmumu.h"
//#include "DerivationFrameworkBPhys/BPhysVarBlinder.h"
//#include "DerivationFrameworkBPhys/BmumuThinningTool.h"
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
#include "DerivationFrameworkBPhys/Cascade3Plus1.h"

using namespace DerivationFramework;

DECLARE_COMPONENT( Reco_4mu )
DECLARE_COMPONENT( Reco_Vertex )
DECLARE_COMPONENT( Select_onia2mumu )
DECLARE_COMPONENT( Thin_vtxTrk )
DECLARE_COMPONENT( Thin_vtxDuplicates )
DECLARE_COMPONENT( AugOriginalCounts )
DECLARE_COMPONENT( BPhysPVThinningTool )
DECLARE_COMPONENT( VertexCaloIsolation )
DECLARE_COMPONENT( VertexTrackIsolation )
DECLARE_COMPONENT( BPhysMetadataBase )
DECLARE_COMPONENT( Bmumu_metadata )
//DECLARE_COMPONENT( CfAthAlgTool )
//DECLARE_COMPONENT( Bmumu_reco_mumu )
DECLARE_COMPONENT( FourMuonTool )
//DECLARE_COMPONENT( BPhysAddMuonBasedInvMass )
//DECLARE_COMPONENT( BPhysVertexTrackBase )
//DECLARE_COMPONENT( BVertexTrackIsoTool )
//DECLARE_COMPONENT( BMuonTrackIsoTool )
//DECLARE_COMPONENT( BVertexClosestTrackTool )
DECLARE_COMPONENT( BTrackVertexMapLogger )
//DECLARE_COMPONENT( Select_Bmumu )
//DECLARE_COMPONENT( BPhysVarBlinder )
//DECLARE_COMPONENT( BmumuThinningTool )
DECLARE_COMPONENT( VertexPlus1TrackCascade )
DECLARE_COMPONENT( TriggerCountToMetadata )
DECLARE_COMPONENT( MuonExtrapolationTool )
DECLARE_COMPONENT( CascadeTools )
DECLARE_COMPONENT( Reco_V0Finder )
DECLARE_COMPONENT( JpsiPlusV0Cascade )
DECLARE_COMPONENT( JpsiPlusDsCascade )
DECLARE_COMPONENT( JpsiPlusDpstCascade )
DECLARE_COMPONENT( JpsiPlusDs1Cascade )
DECLARE_COMPONENT( ReVertex )
DECLARE_COMPONENT( BPhysConversionFinder )
DECLARE_COMPONENT( Cascade3Plus1 )

