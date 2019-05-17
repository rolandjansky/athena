#include "GaudiKernel/DeclareFactoryEntries.h"

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
#include "G4UserActions/RadiationMapsMakerTool.h"
#include "G4UserActions/RadLengthActionTool.h"
#include "G4UserActions/VolumeDumperTool.h"

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
DECLARE_TOOL_FACTORY( G4UA::RadiationMapsMakerTool )
DECLARE_TOOL_FACTORY( G4UA::RadLengthActionTool )
DECLARE_TOOL_FACTORY( G4UA::VolumeDumperTool )


DECLARE_FACTORY_ENTRIES( G4UserActions ) {
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
  DECLARE_TOOL( G4UA::RadiationMapsMakerTool )
  DECLARE_TOOL( G4UA::RadLengthActionTool )
  DECLARE_TOOL( G4UA::VolumeDumperTool )

}
