#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DerivationFrameworkSUSY/Truth3CollectionMaker.h"
#include "DerivationFrameworkSUSY/SUSYSignalTagger.h"
#include "DerivationFrameworkSUSY/SUSYIDWeight.h"
#include "DerivationFrameworkSUSY/TrackParticleCaloCellDecorator.h"
#include "DerivationFrameworkSUSY/trackIsolationDecorator.h"
#include "DerivationFrameworkSUSY/CaloIsolationDecorator.h"
#include "DerivationFrameworkSUSY/TrackParametersKVU.h"
#include "DerivationFrameworkSUSY/LongLivedTruthJetKinematics.h"
#include "DerivationFrameworkSUSY/TauTracksThinning.h"
#include "DerivationFrameworkSUSY/TrackCaloClusterDecorator_LowPtE.h" 

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( Truth3CollectionMaker )
DECLARE_TOOL_FACTORY( SUSYSignalTagger )
DECLARE_TOOL_FACTORY( SUSYIDWeight )
DECLARE_TOOL_FACTORY( TrackParticleCaloCellDecorator )
DECLARE_TOOL_FACTORY( trackIsolationDecorator )
DECLARE_TOOL_FACTORY( CaloIsolationDecorator )
DECLARE_TOOL_FACTORY( TrackParametersKVU )
DECLARE_TOOL_FACTORY( LongLivedTruthJetKinematics )
DECLARE_TOOL_FACTORY( TauTracksThinning )
DECLARE_TOOL_FACTORY( TrackCaloClusterDecorator_LowPtE )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkSUSY ) {
   DECLARE_TOOL( Truth3CollectionMaker )
   DECLARE_TOOL( SUSYSignalTagger )
   DECLARE_TOOL( SUSYIDWeight )
   DECLARE_TOOL( TrackParticleCaloCellDecorator )
   DECLARE_TOOL( trackIsolationDecorator )
   DECLARE_TOOL( CaloIsolationDecorator )
   DECLARE_TOOL( TrackParametersKVU )
   DECLARE_TOOL( LongLivedTruthJetKinematics )
   DECLARE_TOOL( TauTracksThinning )
   DECLARE_TOOL( TrackCaloClusterDecorator_LowPtE )
}
