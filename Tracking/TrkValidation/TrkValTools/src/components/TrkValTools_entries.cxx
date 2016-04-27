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
#include "GaudiKernel/DeclareFactoryEntries.h"

// using namespace Trk;

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, BasicValTrkParticleNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrackInformationNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, PerigeeParametersNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MeasurementVectorNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrackSummaryNtupleTool)
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, DirectTrackNtupleWriterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, ResidualValidationNtupleHelper )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, HitPositionNtupleHelper )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrackPositionNtupleHelper )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, DAF_ValidationNtupleHelper )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, EnergyLossMonitor )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, PositionMomentumWriter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrackDiff )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, PrimaryTruthClassifier )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, InDetReconstructableSelector )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, InDetHaloSelector )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, InDetPrimaryConversionSelector )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TruthNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GenParticleJetFinder )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, JetTruthNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, EventPropertyNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, EventToTrackLinkNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrkObserverTool )

DECLARE_FACTORY_ENTRIES(TrkValTools)
{ // in this package name these methods are registered
  DECLARE_NAMESPACE_TOOL(Trk, BasicValTrkParticleNtupleTool)
  DECLARE_NAMESPACE_TOOL(Trk, TrackInformationNtupleTool)
  DECLARE_NAMESPACE_TOOL(Trk, PerigeeParametersNtupleTool)
  DECLARE_NAMESPACE_TOOL(Trk, MeasurementVectorNtupleTool)
  DECLARE_NAMESPACE_TOOL(Trk, TrackSummaryNtupleTool)
  DECLARE_NAMESPACE_TOOL(Trk, DirectTrackNtupleWriterTool)
  DECLARE_NAMESPACE_TOOL(Trk, ResidualValidationNtupleHelper)
  DECLARE_NAMESPACE_TOOL(Trk, HitPositionNtupleHelper)
  DECLARE_NAMESPACE_TOOL(Trk, TrackPositionNtupleHelper)
  DECLARE_NAMESPACE_TOOL(Trk, DAF_ValidationNtupleHelper)
  DECLARE_NAMESPACE_TOOL(Trk, EnergyLossMonitor)
  DECLARE_NAMESPACE_TOOL(Trk, PositionMomentumWriter)
  DECLARE_NAMESPACE_TOOL(Trk, TrackDiff)
  DECLARE_NAMESPACE_TOOL(Trk, PrimaryTruthClassifier )
  DECLARE_NAMESPACE_TOOL(Trk, InDetReconstructableSelector )
  DECLARE_NAMESPACE_TOOL(Trk, InDetHaloSelector )
  DECLARE_NAMESPACE_TOOL(Trk, InDetPrimaryConversionSelector )
  DECLARE_NAMESPACE_TOOL(Trk, TruthNtupleTool )
  DECLARE_NAMESPACE_TOOL(Trk, GenParticleJetFinder )
  DECLARE_NAMESPACE_TOOL(Trk, JetTruthNtupleTool )
  DECLARE_NAMESPACE_TOOL(Trk, EventPropertyNtupleTool )
  DECLARE_NAMESPACE_TOOL(Trk, EventToTrackLinkNtupleTool )
  DECLARE_NAMESPACE_TOOL(Trk, TrkObserverTool)
}

