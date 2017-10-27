#include "TrkValTools/BasicValTrkParticleNtupleTool.h"
#include "../TrackInformationNtupleTool.h"
#include "../PerigeeParametersNtupleTool.h"
#include "../MeasurementVectorNtupleTool.h"
#include "../DirectTrackNtupleWriterTool.h"
#include "../TrackSummaryNtupleTool.h"
#include "TrkValTools/ResidualValidationNtupleHelper.h"
#include "TrkValTools/HitPositionNtupleHelper.h"
#include "TrkValTools/TrackPositionNtupleHelper.h"
#include "TrkValTools/DAF_ValidationNtupleHelper.h"
#include "TrkValTools/EnergyLossMonitor.h"
#include "TrkValTools/PositionMomentumWriter.h"
#include "TrkValTools/TrackDiff.h"
#include "TrkValTools/PrimaryTruthClassifier.h"
#include "TrkValTools/InDetReconstructableSelector.h"
#include "TrkValTools/InDetHaloSelector.h"
#include "TrkValTools/InDetPrimaryConversionSelector.h"
#include "TrkValTools/TruthNtupleTool.h"
#include "TrkValTools/JetTruthNtupleTool.h"
#include "TrkValTools/GenParticleJetFinder.h"
#include "TrkValTools/EventPropertyNtupleTool.h"
#include "TrkValTools/EventToTrackLinkNtupleTool.h"
#include "TrkValTools/TrkObserverTool.h"

DECLARE_COMPONENT( Trk::BasicValTrkParticleNtupleTool )
DECLARE_COMPONENT( Trk::TrackInformationNtupleTool )
DECLARE_COMPONENT( Trk::PerigeeParametersNtupleTool )
DECLARE_COMPONENT( Trk::MeasurementVectorNtupleTool )
DECLARE_COMPONENT( Trk::TrackSummaryNtupleTool )
DECLARE_COMPONENT( Trk::DirectTrackNtupleWriterTool )
DECLARE_COMPONENT( Trk::ResidualValidationNtupleHelper )
DECLARE_COMPONENT( Trk::HitPositionNtupleHelper )
DECLARE_COMPONENT( Trk::TrackPositionNtupleHelper )
DECLARE_COMPONENT( Trk::DAF_ValidationNtupleHelper )
DECLARE_COMPONENT( Trk::EnergyLossMonitor )
DECLARE_COMPONENT( Trk::PositionMomentumWriter )
DECLARE_COMPONENT( Trk::TrackDiff )
DECLARE_COMPONENT( Trk::PrimaryTruthClassifier )
DECLARE_COMPONENT( Trk::InDetReconstructableSelector )
DECLARE_COMPONENT( Trk::InDetHaloSelector )
DECLARE_COMPONENT( Trk::InDetPrimaryConversionSelector )
DECLARE_COMPONENT( Trk::TruthNtupleTool )
DECLARE_COMPONENT( Trk::GenParticleJetFinder )
DECLARE_COMPONENT( Trk::JetTruthNtupleTool )
DECLARE_COMPONENT( Trk::EventPropertyNtupleTool )
DECLARE_COMPONENT( Trk::EventToTrackLinkNtupleTool )
DECLARE_COMPONENT( Trk::TrkObserverTool )

