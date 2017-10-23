#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkInDet/InDetTrackSelectionToolWrapper.h"
#include "DerivationFrameworkInDet/TrackToVertexWrapper.h"
#include "DerivationFrameworkInDet/TrackParticleThinning.h"
#include "DerivationFrameworkInDet/MuonTrackParticleThinning.h"
#include "DerivationFrameworkInDet/EgammaTrackParticleThinning.h"
#include "DerivationFrameworkInDet/JetTrackParticleThinning.h"
#include "DerivationFrameworkInDet/TauTrackParticleThinning.h"
#include "DerivationFrameworkInDet/DiTauTrackParticleThinning.h"
#include "DerivationFrameworkInDet/TrackParametersAtPV.h"
#include "DerivationFrameworkInDet/TrackStateOnSurfaceDecorator.h"
#include "DerivationFrameworkInDet/TrackParametersForTruthParticles.h"
#include "DerivationFrameworkInDet/EventInfoBSErrDecorator.h"
#include "DerivationFrameworkInDet/UnassociatedHitsDecorator.h"
#include "DerivationFrameworkInDet/UnassociatedHitsGetterTool.h"
#include "DerivationFrameworkInDet/LArCollisionTimeDecorator.h"
#include "DerivationFrameworkInDet/EGammaTracksThinning.h"
#include "DerivationFrameworkInDet/EGammaPrepRawDataThinning.h"
#include "DerivationFrameworkInDet/TrackMeasurementThinning.h"
#include "DerivationFrameworkInDet/EventInfoPixelDecorator.h"
#include "DerivationFrameworkInDet/TCCTrackParticleThinning.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( InDetTrackSelectionToolWrapper )
DECLARE_TOOL_FACTORY( TrackToVertexWrapper )
DECLARE_TOOL_FACTORY( TrackParticleThinning )
DECLARE_TOOL_FACTORY( MuonTrackParticleThinning )
DECLARE_TOOL_FACTORY( EgammaTrackParticleThinning )
DECLARE_TOOL_FACTORY( TauTrackParticleThinning )
DECLARE_TOOL_FACTORY( DiTauTrackParticleThinning )
DECLARE_TOOL_FACTORY( JetTrackParticleThinning )
DECLARE_TOOL_FACTORY( TrackParametersAtPV )
DECLARE_TOOL_FACTORY( TrackStateOnSurfaceDecorator )
DECLARE_TOOL_FACTORY( TrackParametersForTruthParticles )
DECLARE_TOOL_FACTORY( EventInfoBSErrDecorator )
DECLARE_TOOL_FACTORY( UnassociatedHitsDecorator )
DECLARE_NAMESPACE_TOOL_FACTORY(DerivationFramework, UnassociatedHitsGetterTool)
DECLARE_TOOL_FACTORY( LArCollisionTimeDecorator )
DECLARE_TOOL_FACTORY( EGammaTracksThinning )
DECLARE_TOOL_FACTORY( EGammaPrepRawDataThinning )
DECLARE_TOOL_FACTORY( TrackMeasurementThinning )
DECLARE_TOOL_FACTORY( EventInfoPixelDecorator )
DECLARE_TOOL_FACTORY( TCCTrackParticleThinning )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkInDet ) {
   DECLARE_TOOL( InDetTrackSelectionToolWrapper )
   DECLARE_TOOL( TrackToVertexWrapper )
   DECLARE_TOOL( TrackParticleThinning )
   DECLARE_TOOL( MuonTrackParticleThinning )
   DECLARE_TOOL( EgammaTrackParticleThinning )
   DECLARE_TOOL( TauTrackParticleThinning )
   DECLARE_TOOL( DiTauTrackParticleThinning )
   DECLARE_TOOL( JetTrackParticleThinning )
   DECLARE_TOOL( TrackParametersAtPV )
   DECLARE_TOOL( TrackStateOnSurfaceDecorator )
   DECLARE_TOOL( TrackParametersForTruthParticles )
   DECLARE_TOOL( EventInfoBSErrDecorator )
   DECLARE_TOOL( UnassociatedHitsDecorator )
   DECLARE_NAMESPACE_TOOL(DerivationFramework, UnassociatedHitsGetterTool)
   DECLARE_TOOL( LArCollisionTimeDecorator )
   DECLARE_TOOL( EGammaTracksThinning )
   DECLARE_TOOL( EGammaPrepRawDataThinning )
   DECLARE_TOOL( TrackMeasurementThinning )
   DECLARE_TOOL( EventInfoPixelDecorator )
   DECLARE_TOOL( TCCTrackParticleThinning )
}

