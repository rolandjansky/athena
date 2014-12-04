#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkInDet/TrackToVertexWrapper.h"
#include "DerivationFrameworkInDet/TrackParticleThinning.h"
#include "DerivationFrameworkInDet/MuonTrackParticleThinning.h"
#include "DerivationFrameworkInDet/EgammaTrackParticleThinning.h"
#include "DerivationFrameworkInDet/JetTrackParticleThinning.h"
#include "DerivationFrameworkInDet/TauTrackParticleThinning.h"
#include "DerivationFrameworkInDet/TrackParametersAtPV.h"
#include "DerivationFrameworkInDet/TrackStateOnSurfaceDecorator.h"
#include "DerivationFrameworkInDet/TrackParametersForTruthParticles.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( TrackToVertexWrapper )
DECLARE_TOOL_FACTORY( TrackParticleThinning )
DECLARE_TOOL_FACTORY( MuonTrackParticleThinning )
DECLARE_TOOL_FACTORY( EgammaTrackParticleThinning )
DECLARE_TOOL_FACTORY( TauTrackParticleThinning )
DECLARE_TOOL_FACTORY( JetTrackParticleThinning )
DECLARE_TOOL_FACTORY( TrackParametersAtPV )
DECLARE_TOOL_FACTORY( TrackStateOnSurfaceDecorator )
DECLARE_TOOL_FACTORY( TrackParametersForTruthParticles )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkInDet ) {
   DECLARE_TOOL( TrackToVertexWrapper )
   DECLARE_TOOL( TrackParticleThinning )
   DECLARE_TOOL( MuonTrackParticleThinning )
   DECLARE_TOOL( EgammaTrackParticleThinning )
   DECLARE_TOOL( TauTrackParticleThinning )
   DECLARE_TOOL( JetTrackParticleThinning )
   DECLARE_TOOL( TrackParametersAtPV )
	 DECLARE_TOOL( TrackStateOnSurfaceDecorator )
	 DECLARE_TOOL( TrackParametersForTruthParticles )
}

