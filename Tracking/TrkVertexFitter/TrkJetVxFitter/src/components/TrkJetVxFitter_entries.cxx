#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkJetVxFitter/JetFitterHelper.h"
#include "TrkJetVxFitter/JetFitterInitializationHelper.h"
#include "TrkJetVxFitter/JetFitterRoutines.h"
#include "TrkJetVxFitter/KalmanVertexOnJetAxisSmoother.h"
#include "TrkJetVxFitter/KalmanVertexOnJetAxisUpdator.h"
#include "TrkJetVxFitter/TrkDistanceFinderNeutralCharged.h"
#include "TrkJetVxFitter/TrkDistanceFinderNeutralNeutral.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( JetFitterHelper )
DECLARE_TOOL_FACTORY( JetFitterInitializationHelper )
DECLARE_TOOL_FACTORY( JetFitterRoutines )
DECLARE_TOOL_FACTORY( KalmanVertexOnJetAxisSmoother )
DECLARE_TOOL_FACTORY( KalmanVertexOnJetAxisUpdator )
DECLARE_TOOL_FACTORY( TrkDistanceFinderNeutralCharged )
DECLARE_TOOL_FACTORY( TrkDistanceFinderNeutralNeutral )


DECLARE_FACTORY_ENTRIES( TrkJetVxFitter )
{
  DECLARE_TOOL( JetFitterHelper )
  DECLARE_TOOL( JetFitterInitializationHelper )
  DECLARE_TOOL( JetFitterRoutines )
  DECLARE_TOOL( KalmanVertexOnJetAxisSmoother )
  DECLARE_TOOL( KalmanVertexOnJetAxisUpdator )
  DECLARE_TOOL( TrkDistanceFinderNeutralCharged )
  DECLARE_TOOL( TrkDistanceFinderNeutralNeutral )
}
