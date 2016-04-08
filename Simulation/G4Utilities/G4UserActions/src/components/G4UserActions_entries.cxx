#include "GaudiKernel/DeclareFactoryEntries.h"

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
