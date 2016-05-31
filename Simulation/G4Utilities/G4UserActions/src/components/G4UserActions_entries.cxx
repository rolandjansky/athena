#include "GaudiKernel/DeclareFactoryEntries.h"

// New tools
#include "G4UserActions/G4SimTimerTool.h"
#include "G4UserActions/G4TrackCounterTool.h"
#include "G4UserActions/LooperKillerTool.h"
#include "G4UserActions/CosmicPerigeeActionTool.h"
#include "G4UserActions/MomentumConservationTool.h"
#include "G4UserActions/FastIDKillerTool.h"
#include "G4UserActions/HIPKillerTool.h"
#include "G4UserActions/LengthIntegratorTool.h"
#include "G4UserActions/HIPLArVolumeAcceptTool.h"
#include "G4UserActions/HitWrapperTool.h"
#include "G4UserActions/PhotonKillerTool.h"
#include "G4UserActions/ScoringVolumeTrackKillerTool.h"
#include "G4UserActions/StoppedParticleActionTool.h"
#include "G4UserActions/FluxRecorderTool.h"
#include "G4UserActions/ScoringPlaneTool.h"

// Old tools
#include "G4UserActions/G4SimTimer.h"
#include "G4UserActions/G4TrackCounter.h"
#include "G4UserActions/CosmicPerigeeAction.h"
#include "G4UserActions/PhotonKiller.h"
#include "G4UserActions/HitWrapper.h"
#include "G4UserActions/StoppedParticleAction.h"
#include "G4UserActions/LooperKiller.h"
#include "G4UserActions/HIPKiller.h"
#include "G4UserActions/LengthIntegrator.h"
#include "G4UserActions/FastIDKiller.h"
#include "G4UserActions/G4SimMem.h"
#include "G4UserActions/HIPLArVolumeAccept.h"
#include "G4UserActions/MomentumConservation.h"
#include "G4UserActions/FluxRecorder.h"
#include "G4UserActions/ScoringPlane.h"
#include "G4UserActions/ScoringVolumeTrackKiller.h"

// New tools
DECLARE_TOOL_FACTORY( G4UA::G4SimTimerTool )
DECLARE_TOOL_FACTORY( G4UA::G4TrackCounterTool )
DECLARE_TOOL_FACTORY( G4UA::LooperKillerTool )
DECLARE_TOOL_FACTORY( G4UA::CosmicPerigeeActionTool )
DECLARE_TOOL_FACTORY( G4UA::MomentumConservationTool )
DECLARE_TOOL_FACTORY( G4UA::FastIDKillerTool )
DECLARE_TOOL_FACTORY( G4UA::HIPKillerTool )
DECLARE_TOOL_FACTORY( G4UA::LengthIntegratorTool )
DECLARE_TOOL_FACTORY( G4UA::HIPLArVolumeAcceptTool )
DECLARE_TOOL_FACTORY( G4UA::HitWrapperTool )
DECLARE_TOOL_FACTORY( G4UA::PhotonKillerTool )
DECLARE_TOOL_FACTORY( G4UA::ScoringVolumeTrackKillerTool )
DECLARE_TOOL_FACTORY( G4UA::StoppedParticleActionTool )
DECLARE_TOOL_FACTORY( G4UA::FluxRecorderTool )
DECLARE_TOOL_FACTORY( G4UA::ScoringPlaneTool )

// Old tools
DECLARE_TOOL_FACTORY( G4SimTimer )
DECLARE_TOOL_FACTORY( G4TrackCounter )
DECLARE_TOOL_FACTORY( CosmicPerigeeAction )
DECLARE_TOOL_FACTORY( PhotonKiller )
DECLARE_TOOL_FACTORY( HitWrapper )
DECLARE_TOOL_FACTORY( StoppedParticleAction )
DECLARE_TOOL_FACTORY( LooperKiller )
DECLARE_TOOL_FACTORY( HIPKiller )
DECLARE_TOOL_FACTORY( LengthIntegrator )
DECLARE_TOOL_FACTORY( FastIDKiller )
DECLARE_TOOL_FACTORY( G4SimMem )
DECLARE_TOOL_FACTORY( HIPLArVolumeAccept )
DECLARE_TOOL_FACTORY( MomentumConservation )
DECLARE_TOOL_FACTORY( FluxRecorder )
DECLARE_TOOL_FACTORY( ScoringPlane )
DECLARE_TOOL_FACTORY( ScoringVolumeTrackKiller )

DECLARE_FACTORY_ENTRIES( G4UserActions ) {
  // New tools
  DECLARE_TOOL( G4UA::G4SimTimerTool )
  DECLARE_TOOL( G4UA::G4TrackCounterTool )
  DECLARE_TOOL( G4UA::LooperKillerTool )
  DECLARE_TOOL( G4UA::CosmicPerigeeActionTool )
  DECLARE_TOOL( G4UA::MomentumConservationTool )
  DECLARE_TOOL( G4UA::FastIDKillerTool )
  DECLARE_TOOL( G4UA::HIPKillerTool )
  DECLARE_TOOL( G4UA::LengthIntegratorTool )
  DECLARE_TOOL( G4UA::HIPLArVolumeAcceptTool )
  DECLARE_TOOL( G4UA::HitWrapperTool )
  DECLARE_TOOL( G4UA::PhotonKillerTool )
  DECLARE_TOOL( G4UA::ScoringVolumeTrackKillerTool )
  DECLARE_TOOL( G4UA::StoppedParticleActionTool )
  DECLARE_TOOL( G4UA::FluxRecorderTool )
  DECLARE_TOOL( G4UA::ScoringPlaneTool )

  // Old tools
  DECLARE_TOOL( G4SimTimer )
  DECLARE_TOOL( G4TrackCounter )
  DECLARE_TOOL( CosmicPerigeeAction )
  DECLARE_TOOL( PhotonKiller )
  DECLARE_TOOL( HitWrapper )
  DECLARE_TOOL( StoppedParticleAction )
  DECLARE_TOOL( LooperKiller )
  DECLARE_TOOL( HIPKiller )
  DECLARE_TOOL( LengthIntegrator )
  DECLARE_TOOL( FastIDKiller )
  DECLARE_TOOL( G4SimMem )
  DECLARE_TOOL( HIPLArVolumeAccept )
  DECLARE_TOOL( MomentumConservation )
  DECLARE_TOOL( FluxRecorder )
  DECLARE_TOOL( ScoringPlane )
  DECLARE_TOOL( ScoringVolumeTrackKiller )
}
