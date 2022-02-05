#include "../G4SimTimerTool.h"
#include "../G4TrackCounterTool.h"
#include "../LooperKillerTool.h"
// This is a weird one... Geant4 declares 'times' as 'otimes' in some of its
// headers. That can have a catastrophic interference with how Boost uses the
// `times` variable itself. This next line just gets rid of one such
// catastrophic interference. Though this is super fragile...
#undef times
#include "../CosmicPerigeeActionTool.h"
#include "../MomentumConservationTool.h"
#include "../FastIDKillerTool.h"
#include "../HIPKillerTool.h"
#include "../LengthIntegratorTool.h"
#include "../HIPLArVolumeAcceptTool.h"
#include "../HitWrapperTool.h"
#include "../PhotonKillerTool.h"
#include "../FixG4CreatorProcessTool.h"
#include "../ScoringVolumeTrackKillerTool.h"
#include "../StoppedParticleActionTool.h"
#include "../FluxRecorderTool.h"
#include "../ScoringPlaneTool.h"
#include "../RadiationMapsMakerTool.h"
#include "../RadLengthActionTool.h"
#include "../LooperThresholdSetTool.h"
#include "../VolumeDumperTool.h"
#include "../TestActionTool.h"
#include "../AthenaStackingActionTool.h"
#include "../AthenaTrackingActionTool.h"

DECLARE_COMPONENT( G4UA::G4SimTimerTool )
DECLARE_COMPONENT( G4UA::G4TrackCounterTool )
DECLARE_COMPONENT( G4UA::LooperKillerTool )
DECLARE_COMPONENT( G4UA::CosmicPerigeeActionTool )
DECLARE_COMPONENT( G4UA::MomentumConservationTool )
DECLARE_COMPONENT( G4UA::FastIDKillerTool )
DECLARE_COMPONENT( G4UA::HIPKillerTool )
DECLARE_COMPONENT( G4UA::LengthIntegratorTool )
DECLARE_COMPONENT( G4UA::HIPLArVolumeAcceptTool )
DECLARE_COMPONENT( G4UA::HitWrapperTool )
DECLARE_COMPONENT( G4UA::PhotonKillerTool )
DECLARE_COMPONENT( G4UA::FixG4CreatorProcessTool )
DECLARE_COMPONENT( G4UA::ScoringVolumeTrackKillerTool )
DECLARE_COMPONENT( G4UA::StoppedParticleActionTool )
DECLARE_COMPONENT( G4UA::FluxRecorderTool )
DECLARE_COMPONENT( G4UA::ScoringPlaneTool )
DECLARE_COMPONENT( G4UA::RadiationMapsMakerTool )
DECLARE_COMPONENT( G4UA::RadLengthActionTool )
DECLARE_COMPONENT( G4UA::LooperThresholdSetTool )
DECLARE_COMPONENT( G4UA::TestActionTool )
DECLARE_COMPONENT( G4UA::VolumeDumperTool )
DECLARE_COMPONENT( G4UA::AthenaStackingActionTool )
DECLARE_COMPONENT( G4UA::AthenaTrackingActionTool )
