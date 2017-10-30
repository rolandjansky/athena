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

using namespace DerivationFramework;

DECLARE_COMPONENT( TrackToVertexWrapper )
DECLARE_COMPONENT( TrackParticleThinning )
DECLARE_COMPONENT( MuonTrackParticleThinning )
DECLARE_COMPONENT( EgammaTrackParticleThinning )
DECLARE_COMPONENT( TauTrackParticleThinning )
DECLARE_COMPONENT( DiTauTrackParticleThinning )
DECLARE_COMPONENT( JetTrackParticleThinning )
DECLARE_COMPONENT( TrackParametersAtPV )
DECLARE_COMPONENT( TrackStateOnSurfaceDecorator )
DECLARE_COMPONENT( TrackParametersForTruthParticles )
DECLARE_COMPONENT( EventInfoBSErrDecorator )
DECLARE_COMPONENT( UnassociatedHitsDecorator )
DECLARE_COMPONENT( DerivationFramework::UnassociatedHitsGetterTool )
DECLARE_COMPONENT( LArCollisionTimeDecorator )
DECLARE_COMPONENT( EGammaTracksThinning )
DECLARE_COMPONENT( EGammaPrepRawDataThinning )
DECLARE_COMPONENT( TrackMeasurementThinning )
DECLARE_COMPONENT( EventInfoPixelDecorator )

